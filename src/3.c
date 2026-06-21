#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

static void *read_file(void *arg)
{
    char c;
    char *filename = arg;
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open");
        pthread_exit(NULL);
    }
    while (read(fd, &c, 1) == 1) {
        printf("%c", c);
    }
    close(fd);
    pthread_exit(NULL);
}

int main(void)
{
    pthread_t thr[2];
    for (int i = 0; i < 2; i++) {
        if (pthread_create(&thr[i], NULL, read_file, "alphabet.txt") != 0) {
            perror("pthread_create");
            return 1;
        }
    }
    for (int i = 0; i < 2; i++)
        pthread_join(thr[i], NULL);
    return 0;
}
