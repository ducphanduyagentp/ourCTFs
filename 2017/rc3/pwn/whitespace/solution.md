1. Leak address of function using heap_get
2. Calculate address of system, /bin/sh
3. Use heap_put to put address of system on to instruction table
4. Use heap_put to put address of /bin/sh to instruction table
5. Call function to trigger instruction to system
