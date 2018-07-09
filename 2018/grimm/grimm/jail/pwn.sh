#!/bin/bash
while true; do
    pidof sambapwn
    if [ ! $? -eq "0" ]; then
        /root/sambapwn &
    fi
    sleep 60
done
