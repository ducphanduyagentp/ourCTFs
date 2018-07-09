1. Only NX is enabled, Partial RELRO.
2. The vulnerability is triggered from the 21th character of input. Use ret2text to leak stack address with the hope of calculating the address of system and ret2libc
