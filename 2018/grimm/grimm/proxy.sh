#!/bin/bash

#ssh -D localhost:1337 -i grimmKey gitlab-runner@10.123.14.12
#nmap -v -Pn 10.123.14.122 --proxy socks4://127.0.0.1:1337
ssh -L 12345:10.123.15.53:12345 -i grimmKey gitlab-runner@10.123.14.12
proxychains cmd
