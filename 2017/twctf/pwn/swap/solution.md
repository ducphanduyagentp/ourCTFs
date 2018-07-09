1. The program allows us to swap the content of any 2 memory address, so we need to find addresses that are writable.
2. RELRO is only enabled partially so we can swap the GOT entry of a function used in the program with system address and then input the address of "/bin/sh" to get a shell. But first we need to leak an address to bypass aslr.
