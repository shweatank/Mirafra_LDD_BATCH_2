// #include <stdio.h>
// #include <stdlib.h>
// #include <dirent.h>
// #include <string.h>
// #include <sys/stat.h>
// #include <pwd.h>
// #include <grp.h>
// #include <time.h>
// #include <unistd.h>

// // Function to print file permissions like -rw-r--r--
// void print_permissions(mode_t mode) {
//     char perms[11] = "----------";
//     if (S_ISDIR(mode)) perms[0] = 'd';
//     if (S_ISLNK(mode)) perms[0] = 'l';
//     if (mode & S_IRUSR) perms[1] = 'r';
//     if (mode & S_IWUSR) perms[2] = 'w';
//     if (mode & S_IXUSR) perms[3] = 'x';
//     if (mode & S_IRGRP) perms[4] = 'r';
//     if (mode & S_IWGRP) perms[5] = 'w';
//     if (mode & S_IXGRP) perms[6] = 'x';
//     if (mode & S_IROTH) perms[7] = 'r';
//     if (mode & S_IWOTH) perms[8] = 'w';
//     if (mode & S_IXOTH) perms[9] = 'x';
//     printf("%s ", perms);
// }

// int main(int argc, char *argv[]) {
//     DIR *dp;
//     struct dirent *entry;
//     struct stat fileStat;
//     int show_all = 0;
//     int long_list = 0;
//     const char *path = ".";

//     // Argument parsing
//     for (int i = 1; i < argc; i++) {
//         if (argv[i][0] == '-') {
//             if (strchr(argv[i], 'a')) show_all = 1;
//             if (strchr(argv[i], 'l')) long_list = 1;
//         } else {
//             path = argv[i];
//         }
//     }

//     dp = opendir(path);
//     if (dp == NULL) {
//         perror("opendir");
//         return 1;
//     }

//     while ((entry = readdir(dp)) != NULL) {
//         if (!show_all && entry->d_name[0] == '.')
//             continue; // skip hidden files unless -a is used

//         if (long_list) {
//             char fullpath[1024];
//             snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

//             if (stat(fullpath, &fileStat) == -1) {
//                 perror("stat");
//                 continue;
//             }

//             print_permissions(fileStat.st_mode);
//             printf("%ld ", fileStat.st_nlink);
//             printf("%s ", getpwuid(fileStat.st_uid)->pw_name);
//             printf("%s ", getgrgid(fileStat.st_gid)->gr_name);
//             printf("%5ld ", fileStat.st_size);

//             char timebuf[80];
//             strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", localtime(&fileStat.st_mtime));
//             printf("%s ", timebuf);

//             printf("%s\n", entry->d_name);
//         } else {
//             printf("%s  ", entry->d_name);
//         }
//     }

//     if (!long_list) printf("\n");
//     closedir(dp);
//     return 0;
// }




#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>

#define BUF_SIZE 1024*8

struct linux_dirent64 {
    ino64_t        d_ino;
    off64_t        d_off;
    unsigned short d_reclen;
    unsigned char  d_type;
    char           d_name[];
};

const char *file_type(unsigned char d_type) {
    switch (d_type) {
        case DT_REG: return "regular";
        case DT_DIR: return "directory";
        case DT_LNK: return "symlink";
        default:     return "other";
    }
}

int main(int argc, char *argv[]) {
    char buf[BUF_SIZE];
    int fd, nread;
    struct linux_dirent64 *d;
    int bpos;
    const char *path = ".";

    if (argc > 1) path = argv[1];

    fd = open(path, O_RDONLY | O_DIRECTORY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    for (;;) {
        nread = syscall(SYS_getdents64, fd, buf, BUF_SIZE);
        if (nread == -1) {
            perror("getdents64");
            exit(EXIT_FAILURE);
        }

        if (nread == 0) break; // End of directory

        for (bpos = 0; bpos < nread;) {
            d = (struct linux_dirent64 *) (buf + bpos);
            if (d->d_name[0] != '.')  // Skip hidden files
                printf("%s (%s)\n", d->d_name, file_type(d->d_type));
            bpos += d->d_reclen;
        }
    }

    close(fd);
    return 0;
}
