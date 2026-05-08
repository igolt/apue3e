#!/bin/bash

set -e

SCRIPT_DIR="$(dirname "$0")"

cd $SCRIPT_DIR

rm -f foo bar
[ -f "a.out" ] || cc figure-4.9.c -o figure-4.9

echo "this is foo" > foo
echo "this is bar" > bar
ls -lh foo bar
echo "=== foo content ==="; cat foo
echo "=== bar content ==="; cat bar
./figure-4.9
ls -lh foo bar
echo "=== foo content ==="; cat foo
echo "=== bar content ==="; cat bar
