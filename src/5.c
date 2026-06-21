#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

int main()
{
    struct stat statbuf;
    int fd1 = open("q.txt", O_RDWR | O_CREAT | O_TRUNC, 0644);
    int fd2 = open("q.txt", O_RDWR);

    if (fd1 == -1 || fd2 == -1) {
        perror("open");
        return 1;
    }

    if (stat("q.txt", &statbuf) == -1) {
        perror("stat");
        return 1;
    }
    fprintf(stdout, "open: inode = %ld, size = %ld bytes\n",
            (long)statbuf.st_ino, (long)statbuf.st_size);

    for (char c = 'a'; c <= 'z'; c++)
    {
        if (c % 2)
            write(fd1, &c, 1);
        else
            write(fd2, &c, 1);

        if (stat("q.txt", &statbuf) == -1) {
            perror("stat");
            return 1;
        }
        fprintf(stdout, "write: inode = %ld, size = %ld bytes\n",
                (long)statbuf.st_ino, (long)statbuf.st_size);
    }

    close(fd1);
    close(fd2);

    return 0;
}
