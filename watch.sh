#!/usr/bin/env bash

inotifywait \
    -mr \
    --format "%w%f" \
    -e close_write . |
        while read f; do
            filename=$(basename $f)
            extension="${filename##*.}"
            if [[ "c h" =~ $extension ]]; then
                ./build.sh
            fi
        done
