# Chapter 1 - Unix System Overview

## 1.3 Logging In
- **Pasword file**: usually the file `/etc/passwd`. Every entry in the password
  is composed of seven colon separated fields:
    1. The login name (`igolt`)
    2. Encrypted password (all contemporary systems moved the password to a
    different file)
    3. Numeric user ID (the first 1000)
    4. Numeric group ID (the second 1000)
    5. A comment field (`Igor Correa Rodrigues`)
    6. The user's home directory (`/home/igolt`)
    7. The shell program (`/usr/bin/zsh`)
```bash
igolt:x:1000:1000:Igor Correa Rodrigues:/home/igolt:/usr/bin/zsh
```

## 1.4 Files and directories
- **Directory**: is a file that contains **directory entries**.
- **Directory entry**: contains a filename along with a structure of information
  describing the attributes of the file.
- **Notes on filenames**: the only two characters that cannot appear in a
filename are the slash character (_/_) and the null character (`\0`). The slash
separates the filenames that form a pathname and the null character terminates a
pathname.

## 1.7 Error handling
When an error occurs in one of the UNIX System functions, a negative value (or
`NULL`) is often returned, and `errno` is usually set to a value that tells
why.
Historically `errno` use to be defined as:
```c
extern int errno;
```
But in a environment that supports threads, each thread needs its own copy of
`errno` to prevent one thread from interfering with another. Linux, for example,
supports multithreaded access to `errno` by defining it as
```c
extern int *__errno_location(void);
#define errno (*__errno_location())
```

### Rules to be aware of with respect to `errno`
There are **two** rules to be aware of with respect to `errno`.
1. It's value is never cleared by a routine if and error does not occur.
2. The value of `errno` is never set to 0 by any of the functions, and none of
   the constants defined in `<errno.h>` has a value of 0.

### Error Recovery
Errors can be divided into two categories: **fatal** and **nonfatal**. A fatal
error has no recovery action. The best we can do is print an error message on
the user's screen or to a log file, and then exit. Nonfatal errors can sometimes
be dealt with more robustly. Most nonfatal errors are temporary, such as a
resource shortage, and might not occur when there is less activity on the
system. The typical recovery action for resource related nonfatal error is to
delay and retry later.
