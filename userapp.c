#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define DEVICE_NAME "/dev/shortcut_dev"
#define IOCTL_MAGIC 'k'
#define IOCTL_CMD _IO(IOCTL_MAGIC, 1)

int main() {
    int fd = open(DEVICE_NAME, O_RDWR);
    if (fd < 0) {
        perror("Failed to open the device");
        return EXIT_FAILURE;
    }

    if (ioctl(fd, IOCTL_CMD) < 0) {
        perror("Failed to send IOCTL command");
        close(fd);
        return EXIT_FAILURE;
    }

    printf("IOCTL command sent successfully\n");

    // Open YouTube using xdg-open
    if (system("/usr/bin/xdg-open https://www.youtube.com") < 0) {
        perror("Failed to open YouTube");
        close(fd);
        return EXIT_FAILURE;
    }

    close(fd);
    return EXIT_SUCCESS;
}