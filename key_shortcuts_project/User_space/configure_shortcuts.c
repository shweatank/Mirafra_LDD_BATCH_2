#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <linux/input.h>
#include <time.h>
#include <errno.h>
#include <dirent.h>

#define DEVICE_PATH "/dev/key_shortcuts"
#define CONFIG_FILE "/etc/key_shortcuts.conf"
#define LOG_FILE "/var/log/key_shortcuts.log"
#define INPUT_DIR "/dev/input"
#define IOC_MAGIC 'k'
#define IOC_SET_CONFIG _IOW(IOC_MAGIC, 1, struct shortcut_config)
#define CONFIG_VERSION 1

struct shortcut_config {
    unsigned int version;
    unsigned int camera_key;
    unsigned int fileman_key;
};

static void log_to_file(const char *message) {
    FILE *log = fopen(LOG_FILE, "a");
    if (!log) {
        fprintf(stderr, "Failed to open log file: %s\n", LOG_FILE);
        return;
    }
    char timestamp[64];
    time_t now = time(NULL);
    strftime(timestamp, sizeof(timestamp), "[%Y-%m-%d %H:%M:%S]", localtime(&now));
    fprintf(log, "%s %s\n", timestamp, message);
    fclose(log);
}

static char *find_keyboard_device(void) {
    DIR *dir = opendir(INPUT_DIR);
    if (!dir) {
        fprintf(stderr, "Failed to open %s: %s\n", INPUT_DIR, strerror(errno));
        return NULL;
    }

    struct dirent *entry;
    char *device_path = NULL;
    while ((entry = readdir(dir))) {
        if (strncmp(entry->d_name, "event", 5) != 0) continue;
        char path[256];
        snprintf(path, sizeof(path), "%s/%s", INPUT_DIR, entry->d_name);
        int fd = open(path, O_RDONLY);
        if (fd < 0) continue;
        char name[256] = "Unknown";
        ioctl(fd, EVIOCGNAME(sizeof(name)), name);
        close(fd);
        if (strstr(name, "AT Translated Set 2 keyboard")) {
            device_path = strdup(path);
            break;
        }
    }
    closedir(dir);
    return device_path;
}

static unsigned int read_keycode(const char *prompt) {
    char *input_device = find_keyboard_device();
    if (!input_device) {
        fprintf(stderr, "No keyboard device found\n");
        return 0;
    }
    printf("Using input device: %s\n", input_device);

    int fd = open(input_device, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Failed to open input device %s: %s\n", input_device, strerror(errno));
        free(input_device);
        return 0;
    }

    printf("%s (press the key and release it): ", prompt);
    fflush(stdout);

    struct input_event ev;
    unsigned int keycode = 0;
    int state = 0; // 0: waiting for press, 1: waiting for release

    while (1) {
        ssize_t n = read(fd, &ev, sizeof(ev));
        if (n != sizeof(ev)) continue;

        if (ev.type == EV_KEY) {
            // Ignore modifier keys except Ctrl for shortcut keys
            if (ev.code == KEY_RIGHTCTRL || ev.code == KEY_LEFTSHIFT ||
                ev.code == KEY_RIGHTSHIFT || ev.code == KEY_LEFTALT ||
                ev.code == KEY_RIGHTALT ||
                (ev.code == KEY_ENTER && strcmp(prompt, "Press Ctrl key") != 0)) {
                continue;
            }

            if (state == 0 && ev.value == 1) {
                keycode = ev.code;
                printf("\nDetected keycode: %u\n", keycode);
                state = 1; // Wait for release
            } else if (state == 1 && ev.value == 0 && ev.code == keycode) {
                printf("Press Enter to confirm, or any other key to retry: ");
                fflush(stdout);
                char input[16];
                if (fgets(input, sizeof(input), stdin) && input[0] == '\n') {
                    break;
                }
                state = 0; // Retry
                printf("%s (press the key and release it): ", prompt);
                fflush(stdout);
            }
        }
    }

    close(fd);
    free(input_device);
    return keycode;
}

static char keycode_to_char(unsigned int keycode) {
    if (keycode == 46) return 'C';
    if (keycode == 18) return 'E';
    return '?';
}

static int read_config(struct shortcut_config *config) {
    FILE *file = fopen(CONFIG_FILE, "r");
    if (!file) return -1;
    if (fscanf(file, "camera_key=%u,fileman_key=%u", &config->camera_key, &config->fileman_key) != 2) {
        fclose(file);
        return -1;
    }
    fclose(file);
    config->version = CONFIG_VERSION;
    return 0;
}

static int save_config(const struct shortcut_config *config) {
    FILE *file = fopen(CONFIG_FILE, "w");
    if (!file) {
        fprintf(stderr, "Failed to open config file: %s: %s\n", CONFIG_FILE, strerror(errno));
        return -1;
    }
    fprintf(file, "camera_key=%u,fileman_key=%u\n", config->camera_key, config->fileman_key);
    fclose(file);
    return 0;
}

static int set_kernel_config(const struct shortcut_config *config) {
    int fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "Failed to open device: %s: %s\n", DEVICE_PATH, strerror(errno));
        return -1;
    }
    printf("Sending config: version=%u, camera_key=%u, fileman_key=%u, size=%zu\n",
           config->version, config->camera_key, config->fileman_key, sizeof(struct shortcut_config));
    if (ioctl(fd, IOC_SET_CONFIG, config) < 0) {
        fprintf(stderr, "Failed to set config via ioctl: %s\n", strerror(errno));
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
}

int main(int argc, char *argv[]) {
    struct shortcut_config config = { .version = CONFIG_VERSION, .camera_key = 0, .fileman_key = 0 };
    char log_msg[512];

    // Read existing configuration
    if (read_config(&config) == 0) {
        printf("\nCurrent Shortcuts:\n");
        printf("  Camera: Ctrl + %c\n", keycode_to_char(config.camera_key));
        printf("  File Manager: Ctrl + %c\n", keycode_to_char(config.camera_key));
    } else {
        log_to_file("Starting initial configuration setup");
    }

    // Prompt for changes
    printf("\nDo you want to change settings? (y/yes): ");
    char input[16];
    if (!fgets(input, sizeof(input), stdin) || (input[0] != 'y' && input[0] != 'Y')) {
        return 0;
    }

    // Get Ctrl key
    unsigned int ctrl_key;
    do {
        ctrl_key = read_keycode("Press Ctrl key");
        if (ctrl_key != 29) {
            printf("Error: Keycode %u is not left Ctrl key (expected 29). Try again.\n", ctrl_key);
        }
    } while (ctrl_key != 29);
    snprintf(log_msg, sizeof(log_msg), "Detected Ctrl key (keycode=29)");
    log_to_file(log_msg);

    // Get camera shortcut
    do {
        config.camera_key = read_keycode("Press key for camera shortcut");
        if (config.camera_key == 0 || config.camera_key == 29) {
            printf("Error: Invalid keycode %u. Please press a valid key.\n", config.camera_key);
        }
    } while (config.camera_key == 0 || config.camera_key == 29);
    snprintf(log_msg, sizeof(log_msg), "Set camera shortcut to Ctrl + keycode %u", config.camera_key);
    log_to_file(log_msg);
    printf("%s\n", log_msg);

    // Get file manager shortcut
    do {
        config.fileman_key = read_keycode("Press key for file manager shortcut");
        if (config.fileman_key == 0 || config.fileman_key == 29 || config.fileman_key == config.camera_key) {
            printf("Error: Invalid keycode %u or already used. Please press a different key.\n", config.fileman_key);
        }
    } while (config.fileman_key == 0 || config.fileman_key == 29 || config.fileman_key == config.camera_key);
    snprintf(log_msg, sizeof(log_msg), "Set file manager shortcut to Ctrl + keycode %u", config.fileman_key);
    log_to_file(log_msg);
    printf("%s\n", log_msg);

    // Save and apply configuration
    if (save_config(&config) < 0) {
        log_to_file("Failed to save configuration");
        return 1;
    }
    if (set_kernel_config(&config) < 0) {
        log_to_file("Failed to apply configuration to kernel");
        return 1;
    }

    printf("\nCurrent Shortcuts:\n");
    printf("  Camera: Ctrl + %c\n", keycode_to_char(config.camera_key));
    printf("  File Manager: Ctrl + %c\n", keycode_to_char(config.fileman_key));
    log_to_file("Configuration saved successfully");
    printf("Configuration saved successfully\n");

    return 0;
}
