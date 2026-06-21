[English](README.md) · [Русский](README.ru.md)

# Buffered and unbuffered I/O in Linux

Five C programs demonstrate the relation between a file descriptor, the `struct file` object, and the index node (`inode`) under different ways of opening one file. The output of each program shows which kernel objects are shared and which are independent. The structure relations are given in the figures.

The work targets Linux kernel 6.8. The programs use POSIX calls and also build on macOS; the output below is from real runs.

## Contents

- [Kernel structures](#kernel-structures)
- [Program 1. Buffered I/O, reading](#program-1-buffered-io-reading)
- [Program 2. Unbuffered I/O, reading](#program-2-unbuffered-io-reading)
- [Program 3. Reading in two threads](#program-3-reading-in-two-threads)
- [Program 4. Buffered I/O, writing](#program-4-buffered-io-writing)
- [Program 5. Unbuffered I/O, writing](#program-5-unbuffered-io-writing)
- [Build and run](#build-and-run)
- [Files](#files)

## Kernel structures

A file descriptor is the index of an element in the `fd_array` array of the `files_struct` structure, reachable from the process structure `task_struct`. Indices 0, 1, 2 are held by the standard streams `stdin`, `stdout`, `stderr`, so the first `open()` call returns descriptor 3.

A descriptor refers to a `struct file`. The `struct file` holds the current offset `f_pos` and a pointer to the index node `inode`. The index node describes the file and holds its size (the `st_size` field of `struct stat`). Several `struct file` objects may refer to one `inode`.

In user space a standard library stream (`FILE *`) sits on top of a descriptor and adds its own buffer. One open file can therefore be shared at several levels. Each level is shown in a separate program.

## Program 1. Buffered I/O, reading

The file `alphabet.txt` is opened once (`O_RDONLY`), and the descriptor corresponds to one `struct file`. The `fdopen()` function is called on this descriptor twice and returns the pointers `fs1` and `fs2`. The `setvbuf()` function sets a 20-byte buffer for each stream.

![structure relations, program 1](img/prog_1.png)

The pointers `fs1` and `fs2` refer to different `FILE` structures with different buffers, but their `_fileno` field is the same. Both streams therefore reach one `struct file` and the `f_pos` field is shared.

```
aubvcwdxeyfzg
hijklmnopqrst
```

The first `fscanf()` call through `fs1` reads the first 20 characters (`a`..`t`) into the buffer and moves the shared offset `f_pos` to 20. The next call through `fs2` starts reading at position 20 and gets the remaining characters (`u`..`z` and the newline). The buffer contents are then printed in turn by `fprintf()`.

## Program 2. Unbuffered I/O, reading

The file `alphabet.txt` is opened twice, returning descriptors 3 and 4. The two `open()` calls correspond to two `struct file` objects with independent `f_pos` fields; both refer to one `inode`.

![structure relations, program 2](img/prog_2.png)

```
aabbccddeeffgghhiijjkkllmmnnooppqqrrssttuuvvwwxxyyzz
```

The offsets change independently and both start at zero. Reading is done one character at a time through each descriptor in turn, so every character is read and printed twice.

## Program 3. Reading in two threads

The main thread creates two extra threads and waits for them; it does not read the file itself. Each extra thread opens `alphabet.txt`, gets its own descriptor, reads the file, and prints the characters with `printf()`. The two `open()` calls create two `struct file` objects with independent `f_pos` fields, so the structure relations match program 2.

```
ababcdecdefghijfklgmhnijklmonpoqprqsrtsutvuwvxwyxzyz
```

Each thread goes through the file from its own position, that is, reads it in full. The scheduler interleaves the threads, so the two alphabets are mixed and the character order changes from run to run.

## Program 4. Buffered I/O, writing

The file `q.txt` is opened twice with `fopen()` in mode `"w"`, and each stream is given a 20-byte buffer. The odd letters are written through `fs1`, the even letters through `fs2`. After opening, each write, and closing, the `inode` and the file size from `struct stat` are printed.

![structure relations, program 4](img/prog_4.png)

The data first goes into the standard library buffers (`buff1`, `buff2`) in user space.

```
fopen fs1: inode = 66698169, size = 0 bytes
fopen fs2: inode = 66698169, size = 0 bytes
fprintf: inode = 66698169, size = 0 bytes
fprintf: inode = 66698169, size = 0 bytes
fprintf: inode = 66698169, size = 0 bytes
fprintf: inode = 66698169, size = 0 bytes
fprintf: inode = 66698169, size = 0 bytes
fprintf: inode = 66698169, size = 0 bytes
fprintf: inode = 66698169, size = 0 bytes
fprintf: inode = 66698169, size = 0 bytes
fprintf: inode = 66698169, size = 0 bytes
fprintf: inode = 66698169, size = 0 bytes
fprintf: inode = 66698169, size = 0 bytes
fprintf: inode = 66698169, size = 0 bytes
fprintf: inode = 66698169, size = 0 bytes
fprintf: inode = 66698169, size = 0 bytes
fprintf: inode = 66698169, size = 0 bytes
fprintf: inode = 66698169, size = 0 bytes
fprintf: inode = 66698169, size = 0 bytes
fprintf: inode = 66698169, size = 0 bytes
fprintf: inode = 66698169, size = 0 bytes
fprintf: inode = 66698169, size = 0 bytes
fprintf: inode = 66698169, size = 0 bytes
fprintf: inode = 66698169, size = 0 bytes
fprintf: inode = 66698169, size = 0 bytes
fprintf: inode = 66698169, size = 0 bytes
fprintf: inode = 66698169, size = 0 bytes
fprintf: inode = 66698169, size = 0 bytes
fclose fs1: inode = 66698169, size = 13 bytes
fclose fs2: inode = 66698169, size = 13 bytes
```

A write to the file happens in three cases: the buffer is full, `fflush()` is called, or the file is closed with `fclose()`. The 13 characters fit in the 20-byte buffer, so `fprintf()` does not change the file and its size is 0. The size changes only on `fclose()`. The index node is constant, because the file is one. Both streams are opened in mode `"w"`, the file is truncated, and the data of the last flush is kept (the even letters `bdfhjlnprtvxz`). To avoid losing data, the second stream should be opened in mode `"a"`.

The `inode` number belongs to this machine; on Linux the value differs and the behavior is the same.

## Program 5. Unbuffered I/O, writing

The file `q.txt` is opened twice (`O_RDWR`). The odd letters are written through descriptor `fd1`, the even letters through `fd2` with the `write()` system call. The file size is printed after each write.

```
open: inode = 66695168, size = 0 bytes
write: inode = 66695168, size = 1 bytes
write: inode = 66695168, size = 1 bytes
write: inode = 66695168, size = 2 bytes
write: inode = 66695168, size = 2 bytes
write: inode = 66695168, size = 3 bytes
write: inode = 66695168, size = 3 bytes
write: inode = 66695168, size = 4 bytes
write: inode = 66695168, size = 4 bytes
write: inode = 66695168, size = 5 bytes
write: inode = 66695168, size = 5 bytes
write: inode = 66695168, size = 6 bytes
write: inode = 66695168, size = 6 bytes
write: inode = 66695168, size = 7 bytes
write: inode = 66695168, size = 7 bytes
write: inode = 66695168, size = 8 bytes
write: inode = 66695168, size = 8 bytes
write: inode = 66695168, size = 9 bytes
write: inode = 66695168, size = 9 bytes
write: inode = 66695168, size = 10 bytes
write: inode = 66695168, size = 10 bytes
write: inode = 66695168, size = 11 bytes
write: inode = 66695168, size = 11 bytes
write: inode = 66695168, size = 12 bytes
write: inode = 66695168, size = 12 bytes
write: inode = 66695168, size = 13 bytes
write: inode = 66695168, size = 13 bytes
```

The `write()` call uses no user-space buffer, so each byte reaches the file at once and the size changes immediately (unlike program 4). The two descriptors have independent `f_pos` fields (structure relations as in program 2) and both start at zero, so the writes overwrite each other. The size grows only when a write goes past the current end of the file. The even letters `bdfhjlnprtvxz` are kept. To avoid losing data, the file should be opened with the `O_APPEND` flag.

## Build and run

```bash
make        # builds src/1 .. src/5
make run    # builds and runs all five
```

A single program (run from the `src` directory, because the files are read and written next to it):

```bash
cc -Wall -std=c11 src/2.c -o src/2 && (cd src && ./2)
cc -Wall -std=c11 -pthread src/3.c -o src/3 && (cd src && ./3)
```

## Files

| file | purpose |
|---|---|
| `src/1.c` | buffered reading, `fdopen` twice, shared `struct file` (shared `f_pos`) |
| `src/2.c` | unbuffered reading, two `struct file` (independent `f_pos`), one `inode` |
| `src/3.c` | reading in two threads, each with its own `open` (independent `f_pos`) |
| `src/4.c` | buffered writing, size changes only on buffer flush |
| `src/5.c` | unbuffered writing, size changes immediately |
| `src/alphabet.txt` | input data for programs 1, 2, 3 |
| `img/prog_*.png` | kernel structure relations |
