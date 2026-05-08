#!/bin/bash

rm -f foo

uname -srvo # Print information about the Kernel and Linux distribution
echo "bar" > foo
ls -l foo
chmod u-r foo # turn off user-read permission
ls -l foo
cat foo # this should fail as we don't have permission to read
echo "baz" >> foo # we still have permission to write
ls -l foo # verify if the file size increased
chmod u+r foo # turn on user-read permission
cat foo # now we should be able to read the file contents
