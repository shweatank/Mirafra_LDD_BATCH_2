#!/bin/bash

SYSFS_FILE="/sys/kernel/sys_control/volume_cmd"
LAST_CMD=""

# Function to get current volume percentage
get_volume() {
    amixer get Master | grep -o "[0-9]*%" | head -1 | tr -d '%'
}

# Function to set volume and update sysfs
set_volume() {
    local cmd=$1
    if [ "$cmd" = "up" ]; then
        amixer set Master 5%+ > /dev/null
    elif [ "$cmd" = "down" ]; then
        amixer set Master 5%- > /dev/null
    fi
    echo "" > "$SYSFS_FILE" # Clear command
}

# Main loop
while true; do
    CMD=$(cat "$SYSFS_FILE")
    if [ "$CMD" != "$LAST_CMD" ] && [ -n "$CMD" ]; then
        set_volume "$CMD"
        LAST_CMD="$CMD"
    fi
    sleep 0.1
done
