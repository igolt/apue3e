#!/bin/bash

set -e

SCRIPT_DIR="$(dirname $(realpath "$0"))"
EXECUTABLE="$SCRIPT_DIR/a.out"
SRC="$SCRIPT_DIR/date.c"

[ -x "$EXECUTABLE" ] || gcc "$SRC" -o "$EXECUTABLE"

"$EXECUTABLE"
env TZ=America/Sao_Paulo "$EXECUTABLE"
env TZ=Europe/London "$EXECUTABLE"
