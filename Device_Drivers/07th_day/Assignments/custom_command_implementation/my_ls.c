#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

void human_readable_size(off_t size, char *buf) {
    const char *units[] = {"B", "K", "M", "G", "T"};
    int i = 0;
    double s = size;
    while (s >= 1024 && i < 4) {
        s /= 1024;
        i++;
    }
    sprintf(buf, "%.1f%s", s, units[i]);
}

void list_directory(const char *path, int show_all, int long_format, int human_readable) {
    DIR *dir = opendir(path);
    if (!dir) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    struct stat st;
    char full_path[1024];
    while ((entry = readdir(dir))) {
        if (!show_all && entry->d_name[0] == '.') {
            continue; // Skip hidden files
        }
        if (long_format) {
            snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
            if (stat(full_path, &st) == -1) {
                perror("stat");
                continue;
            }
            char perms[11];
            strcpy(perms, (S_ISDIR(st.st_mode)) ? "d" : "-");
            strcat(perms, (st.st_mode & S_IRUSR) ? "r" : "-");
            strcat(perms, (st.st_mode & S_IWUSR) ? "w" : "-");
            strcat(perms, (st.st_mode & S_IXUSR) ? "x" : "-");
            strcat(perms, (st.st_mode & S_IRGRP) ? "r" : "-");
            strcat(perms, (st.st_mode & S_IWGRP) ? "w" : "-");
            strcat(perms, (st.st_mode & S_IXGRP) ? "x" : "-");
            strcat(perms, (st.st_mode & S_IROTH) ? "r" : "-");
            strcat(perms, (st.st_mode & S_IWOTH) ? "w" : "-");
          //  strcat(perms, (st.st_mode & S_ComputingxOTH) ? "x" : "-");

            char size_buf[32];
            if (human_readable) {
                human_readable_size(st.st_size, size_buf);
            } else {
                sprintf(size_buf, "%ld", st.st_size);
            }

            char time_buf[20];
            strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M", localtime(&st.st_mtime));

            printf("%s %8s %s %s\n", perms, size_buf, time_buf, entry->d_name);
        } else {
            printf("%s\n", entry->d_name);
        }
    }
    closedir(dir);
}

int main(int argc, char *argv[]) {
    int show_all = 0, long_format = 0, human_readable = 0;
    const char *path = ".";

    int opt;
    while ((opt = getopt(argc, argv, "lah")) != -1) {
        switch (opt) {
            case 'l': long_format = 1; break;
            case 'a': show_all = 1; break;
            case 'h': human_readable = 1; break;
            default: fprintf(stderr, "Usage: %s [-lah] [path]\n", argv[0]); exit(1);
        }
    }

    if (optind < argc) {
        path = argv[optind];
    }

    list_directory(path, show_all, long_format, human_readable);
    return 0;
}
