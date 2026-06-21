#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>

struct stat statbuf;
int main(void)
{
    char buf1[20];
    char buf2[20];

    FILE *fs1 = fopen("q.txt", "w");
    setvbuf(fs1, buf1, _IOFBF, sizeof(buf1));
    stat("q.txt", &statbuf);
    fprintf(stdout, "fopen fs1: inode = %ld, size = %ld bytes\n", (long)statbuf.st_ino, (long)statbuf.st_size);
    
    FILE *fs2 = fopen("q.txt", "w");
    setvbuf(fs2, buf2, _IOFBF, sizeof(buf2));
    stat("q.txt", &statbuf);
    fprintf(stdout, "fopen fs2: inode = %ld, size = %ld bytes\n", (long)statbuf.st_ino, (long)statbuf.st_size);
    
    for (char c = 'a'; c <= 'z'; c++) {
        if (c % 2)
            fprintf(fs1, "%c", c);
        else
            fprintf(fs2, "%c", c);
        stat("q.txt", &statbuf);
        fprintf(stdout, "fprintf: inode = %ld, size = %ld bytes\n", (long)statbuf.st_ino, (long)statbuf.st_size);
    }
    fclose(fs1);
    stat("q.txt", &statbuf);
    fprintf(stdout, "fclose fs1: inode = %ld, size = %ld bytes\n", (long)statbuf.st_ino, (long)statbuf.st_size);
    fclose(fs2);
    stat("q.txt", &statbuf);
    fprintf(stdout, "fclose fs2: inode = %ld, size = %ld bytes\n", (long)statbuf.st_ino, (long)statbuf.st_size);
    
    return 0;
}
