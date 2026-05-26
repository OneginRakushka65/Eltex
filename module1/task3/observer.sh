#!/bin/bash

CONF="observer.conf"
LOG="observer.log"

for script in $(grep -v '^#' "$CONF" | grep -v '^$'); do
    name=$(basename "$script")
    found=0

    for pid in /proc/[0-9]*; do
        cmd=$(tr '\0' ' ' < "$pid/cmdline" 2>/dev/null || true)
        [[ "$cmd" == *"$name"* ]] && { found=1; break; }
    done

    if [[ $found -eq 0 ]]; then
        nohup $script >/dev/null 2>&1 &
        echo "$(date '+%Y-%m-%d %H:%M:%S') restarted $script" >> "$LOG"
    fi
done