#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <sys/sysmacros.h>

#define MAX_FILES 1024

struct file_info {
    char name[256];
    struct stat st;
};

// Compare function for sorting by time (oldest to newest)
int cmp_time(const void *a, const void *b) {
    const struct file_info *fa = (const struct file_info *)a;
    const struct file_info *fb = (const struct file_info *)b;
    return fa->st.st_mtime - fb->st.st_mtime;
}

void print_file_info(struct file_info *file) {
    char perms[11] = "----------";
    mode_t mode = file->st.st_mode;

    if (S_ISDIR(mode)) perms[0] = 'd';
    if (mode & S_IRUSR) perms[1] = 'r';
    if (mode & S_IWUSR) perms[2] = 'w';
    if (mode & S_IXUSR) perms[3] = 'x';
    if (mode & S_IRGRP) perms[4] = 'r';
    if (mode & S_IWGRP) perms[5] = 'w';
    if (mode & S_IXGRP) perms[6] = 'x';
    if (mode & S_IROTH) perms[7] = 'r';
    if (mode & S_IWOTH) perms[8] = 'w';
    if (mode & S_IXOTH) perms[9] = 'x';

    struct passwd *pw = getpwuid(file->st.st_uid);
    struct group  *gr = getgrgid(file->st.st_gid);

    char timebuf[64];
    strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", localtime(&file->st.st_mtime));

    printf("%s %ld %s %s %8ld %s %s\n",
           perms,
           (long)file->st.st_nlink,
           pw ? pw->pw_name : "?",
           gr ? gr->gr_name : "?",
           (long)file->st.st_size,
           timebuf,
           file->name);
}

int main(int argc, char *argv[]) {
    const char *dirpath = ".";
    if (argc > 1)
        dirpath = argv[1];

    DIR *dir = opendir(dirpath);
    if (!dir) {
        perror("opendir");
        return 1;
    }

    struct file_info files[MAX_FILES];
    int count = 0;
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL && count < MAX_FILES) {
        snprintf(files[count].name, sizeof(files[count].name), "%s", entry->d_name);

        char path[512];
        snprintf(path, sizeof(path), "%s/%s", dirpath, entry->d_name);

        if (stat(path, &files[count].st) == 0) {
            count++;
        }
    }

    closedir(dir);

    // Sort by time
    qsort(files, count, sizeof(struct file_info), cmp_time);

    // Print in reverse order (newest first, like -r)
    for (int i = count - 1; i >= 0; i--) {
        print_file_info(&files[i]);
    }

    return 0;
}

