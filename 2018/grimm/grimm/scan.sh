#!/bin/bash
HOST=192.168.14.30

for port in {1..65535}; do
    timeout 1 nc -vv $HOST $port 2>&1 | grep open &
done
