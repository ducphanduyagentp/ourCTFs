#!/bin/bash
while true; do
    pidof sambapwn
    if [ ! $? -eq "0" ]; then
        /home/lister/sambapwn &
    fi
    sleep 60
done
