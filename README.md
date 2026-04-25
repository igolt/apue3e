# APUE Notes and Exercises

Notes and exercises resolutions for the third edition of the book [_Advanced
Programming in Unix Environments_](http://www.apuebook.com/apue3e.html), which
focuses on describing the services provided by various versions of the UNIX
operating system.

## IMPORTANT
###
To prevent uploading files that are generated as side effects, you should
install the pre-commit script:
```bash
$ cp scripts/pre-commit .git/hooks/pre-commit
```

Set the `SKIP` env var to 1 if you want to skip the pre-commit hook.
```bash
$ SKIP=1 git commit
```
