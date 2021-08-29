# Notes - not_so_tiger

- Bug is overflow in strcpy
- There is an instance cat on the stack. strcpy will overflow the stack
- The issue is the null terminating byte.

## Leak

## Write

- The object is stored at 0xdd40
- The index is stored at 0xdd40+0x28 = 0xdd68
- It will read the index to interpret the variant type. We can confuse the type
- AAR
    - Make and set a cat type 2
        - age
        - name will overflow 1 byte into the index to type 0
    - Get cat. It will interpret with type 0
        - char *name => actually age will be interpreted as an address
        - age
- Then we can read GOT table to leak libc
- We can leak stack by reading environ
- Scan the stack for the canary
- Then overwrite the stack when cin. Use a strdup cat at this step to avoid running onto its own buffer on stack
