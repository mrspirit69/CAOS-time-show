#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>

int main() {
    struct timeval start, end;
    char buffer[128];

    int fd = open("/dev/time_dev", O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    gettimeofday(&start, NULL);

    read(fd, buffer, sizeof(buffer));

    gettimeofday(&end, NULL);
    close(fd);

    long delay_us = (end.tv_sec - start.tv_sec) * 1000000L +
                    (end.tv_usec - start.tv_usec);

    printf("Raw module response: %s", buffer);
    printf("Estimated message delay: %ld us\n", delay_us);

    return 0;
}
