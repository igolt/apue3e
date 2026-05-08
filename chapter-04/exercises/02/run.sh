#!/bin/bash

# ensure that the files will be created by the script
rm -f foo bar


uname -srvo # Print information about the Kernel and Linux distribution
cat foo # Just to show that the file doesn't exists
umask 777
touch foo
ls -l foo
cat bar # Just to show that the file doesn't exists
umask 002
touch bar
ls -l bar
