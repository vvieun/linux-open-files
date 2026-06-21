#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
int main(void)
{
    char c;
    int fd1 = open("alphabet.txt", O_RDONLY);
    int fd2 = open("alphabet.txt", O_RDONLY);
    if (fd1 == -1 || fd2 == -1) {
        perror("open");
        return 1;
    }
    int flag1 = 1, flag2 = 1;
    while (flag1 == 1 && flag2 == 1) {
        flag1 = read(fd1, &c, 1);
        printf("%c", c);
        flag2 = read(fd2, &c, 1);
        printf("%c", c);
    }
    close(fd1);
    close(fd2);
    return 0;
}
