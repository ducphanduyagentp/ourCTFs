#!/bin/bash
while true; do
    pidof sambapwn >/dev/null 2>&1
    if [ ! $? -eq "0" ]; then
        /home/lister/sambapwn &
    fi
    sleep 60
done
