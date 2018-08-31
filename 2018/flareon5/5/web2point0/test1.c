#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "test1.h"
#define UNLIKELY(x) __builtin_expect(!!(x), 0)
#define LIKELY(x) __builtin_expect(!!(x), 1)

#define TRAP(x) (wasm_rt_trap(WASM_RT_TRAP_##x), 0)

#define FUNC_PROLOGUE                                            \
  if (++wasm_rt_call_stack_depth > WASM_RT_MAX_CALL_STACK_DEPTH) \
    TRAP(EXHAUSTION)

#define FUNC_EPILOGUE --wasm_rt_call_stack_depth

#define UNREACHABLE TRAP(UNREACHABLE)

#define CALL_INDIRECT(table, t, ft, x, ...)          \
  (LIKELY((x) < table.size && table.data[x].func &&  \
          table.data[x].func_type == func_types[ft]) \
       ? ((t)table.data[x].func)(__VA_ARGS__)        \
       : TRAP(CALL_INDIRECT))

#define MEMCHECK(mem, a, t)  \
  if (UNLIKELY((a) + sizeof(t) > mem->size)) TRAP(OOB)

#define DEFINE_LOAD(name, t1, t2, t3)              \
  static inline t3 name(wasm_rt_memory_t* mem, u64 addr) {   \
    MEMCHECK(mem, addr, t1);                       \
    t1 result;                                     \
    memcpy(&result, &mem->data[addr], sizeof(t1)); \
    return (t3)(t2)result;                         \
  }

#define DEFINE_STORE(name, t1, t2)                           \
  static inline void name(wasm_rt_memory_t* mem, u64 addr, t2 value) { \
    MEMCHECK(mem, addr, t1);                                 \
    t1 wrapped = (t1)value;                                  \
    memcpy(&mem->data[addr], &wrapped, sizeof(t1));          \
  }

DEFINE_LOAD(i32_load, u32, u32, u32);
DEFINE_LOAD(i64_load, u64, u64, u64);
DEFINE_LOAD(f32_load, f32, f32, f32);
DEFINE_LOAD(f64_load, f64, f64, f64);
DEFINE_LOAD(i32_load8_s, s8, s32, u32);
DEFINE_LOAD(i64_load8_s, s8, s64, u64);
DEFINE_LOAD(i32_load8_u, u8, u32, u32);
DEFINE_LOAD(i64_load8_u, u8, u64, u64);
DEFINE_LOAD(i32_load16_s, s16, s32, u32);
DEFINE_LOAD(i64_load16_s, s16, s64, u64);
DEFINE_LOAD(i32_load16_u, u16, u32, u32);
DEFINE_LOAD(i64_load16_u, u16, u64, u64);
DEFINE_LOAD(i64_load32_s, s32, s64, u64);
DEFINE_LOAD(i64_load32_u, u32, u64, u64);
DEFINE_STORE(i32_store, u32, u32);
DEFINE_STORE(i64_store, u64, u64);
DEFINE_STORE(f32_store, f32, f32);
DEFINE_STORE(f64_store, f64, f64);
DEFINE_STORE(i32_store8, u8, u32);
DEFINE_STORE(i32_store16, u16, u32);
DEFINE_STORE(i64_store8, u8, u64);
DEFINE_STORE(i64_store16, u16, u64);
DEFINE_STORE(i64_store32, u32, u64);

#define I32_CLZ(x) ((x) ? __builtin_clz(x) : 32)
#define I64_CLZ(x) ((x) ? __builtin_clzll(x) : 64)
#define I32_CTZ(x) ((x) ? __builtin_ctz(x) : 32)
#define I64_CTZ(x) ((x) ? __builtin_ctzll(x) : 64)
#define I32_POPCNT(x) (__builtin_popcount(x))
#define I64_POPCNT(x) (__builtin_popcountll(x))

#define DIV_S(ut, min, x, y)                                 \
   ((UNLIKELY((y) == 0)) ?                TRAP(DIV_BY_ZERO)  \
  : (UNLIKELY((x) == min && (y) == -1)) ? TRAP(INT_OVERFLOW) \
  : (ut)((x) / (y)))

#define REM_S(ut, min, x, y)                                \
   ((UNLIKELY((y) == 0)) ?                TRAP(DIV_BY_ZERO) \
  : (UNLIKELY((x) == min && (y) == -1)) ? 0                 \
  : (ut)((x) % (y)))

#define I32_DIV_S(x, y) DIV_S(u32, INT32_MIN, (s32)x, (s32)y)
#define I64_DIV_S(x, y) DIV_S(u64, INT64_MIN, (s64)x, (s64)y)
#define I32_REM_S(x, y) REM_S(u32, INT32_MIN, (s32)x, (s32)y)
#define I64_REM_S(x, y) REM_S(u64, INT64_MIN, (s64)x, (s64)y)

#define DIVREM_U(op, x, y) \
  ((UNLIKELY((y) == 0)) ? TRAP(DIV_BY_ZERO) : ((x) op (y)))

#define DIV_U(x, y) DIVREM_U(/, x, y)
#define REM_U(x, y) DIVREM_U(%, x, y)

#define ROTL(x, y, mask) \
  (((x) << ((y) & (mask))) | ((x) >> (((mask) - (y) + 1) & (mask))))
#define ROTR(x, y, mask) \
  (((x) >> ((y) & (mask))) | ((x) << (((mask) - (y) + 1) & (mask))))

#define I32_ROTL(x, y) ROTL(x, y, 31)
#define I64_ROTL(x, y) ROTL(x, y, 63)
#define I32_ROTR(x, y) ROTR(x, y, 31)
#define I64_ROTR(x, y) ROTR(x, y, 63)

#define FMIN(x, y)                                          \
   ((UNLIKELY((x) != (x))) ? NAN                            \
  : (UNLIKELY((y) != (y))) ? NAN                            \
  : (UNLIKELY((x) == 0 && (y) == 0)) ? (signbit(x) ? x : y) \
  : (x < y) ? x : y)

#define FMAX(x, y)                                          \
   ((UNLIKELY((x) != (x))) ? NAN                            \
  : (UNLIKELY((y) != (y))) ? NAN                            \
  : (UNLIKELY((x) == 0 && (y) == 0)) ? (signbit(x) ? y : x) \
  : (x > y) ? x : y)

#define TRUNC_S(ut, st, ft, min, max, maxop, x)                             \
   ((UNLIKELY((x) != (x))) ? TRAP(INVALID_CONVERSION)                       \
  : (UNLIKELY((x) < (ft)(min) || (x) maxop (ft)(max))) ? TRAP(INT_OVERFLOW) \
  : (ut)(st)(x))

#define I32_TRUNC_S_F32(x) TRUNC_S(u32, s32, f32, INT32_MIN, INT32_MAX, >=, x)
#define I64_TRUNC_S_F32(x) TRUNC_S(u64, s64, f32, INT64_MIN, INT64_MAX, >=, x)
#define I32_TRUNC_S_F64(x) TRUNC_S(u32, s32, f64, INT32_MIN, INT32_MAX, >,  x)
#define I64_TRUNC_S_F64(x) TRUNC_S(u64, s64, f64, INT64_MIN, INT64_MAX, >=, x)

#define TRUNC_U(ut, ft, max, maxop, x)                                    \
   ((UNLIKELY((x) != (x))) ? TRAP(INVALID_CONVERSION)                     \
  : (UNLIKELY((x) <= (ft)-1 || (x) maxop (ft)(max))) ? TRAP(INT_OVERFLOW) \
  : (ut)(x))

#define I32_TRUNC_U_F32(x) TRUNC_U(u32, f32, UINT32_MAX, >=, x)
#define I64_TRUNC_U_F32(x) TRUNC_U(u64, f32, UINT64_MAX, >=, x)
#define I32_TRUNC_U_F64(x) TRUNC_U(u32, f64, UINT32_MAX, >,  x)
#define I64_TRUNC_U_F64(x) TRUNC_U(u64, f64, UINT64_MAX, >=, x)

#define DEFINE_REINTERPRET(name, t1, t2)  \
  static inline t2 name(t1 x) {           \
    t2 result;                            \
    memcpy(&result, &x, sizeof(result));  \
    return result;                        \
  }

DEFINE_REINTERPRET(f32_reinterpret_i32, u32, f32)
DEFINE_REINTERPRET(i32_reinterpret_f32, f32, u32)
DEFINE_REINTERPRET(f64_reinterpret_i64, u64, f64)
DEFINE_REINTERPRET(i64_reinterpret_f64, f64, u64)


static u32 func_types[4];

static void init_func_types(void) {
  func_types[0] = wasm_rt_register_func_type(4, 1, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[1] = wasm_rt_register_func_type(1, 0, WASM_RT_I32);
  func_types[2] = wasm_rt_register_func_type(5, 1, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[3] = wasm_rt_register_func_type(3, 1, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
}

static u32 f1(u32, u32, u32, u32);
static u32 f2(u32, u32, u32, u32);
static u32 f3(u32, u32, u32, u32);
static u32 f4(u32, u32, u32, u32);
static u32 f5(u32, u32, u32, u32);
static u32 f6(u32, u32, u32, u32);
static u32 f7(u32, u32, u32, u32);
static u32 f8(u32, u32, u32, u32, u32);
static u32 Match(u32, u32, u32, u32);
static u32 writev_c(u32, u32, u32);

static u32 g0;
static u32 __heap_base;
static u32 __data_end;

static void init_globals(void) {
  g0 = 66592u;
  __heap_base = 66592u;
  __data_end = 1052u;
}

static wasm_rt_memory_t memory;

static wasm_rt_table_t T0;

static u32 f1(u32 p0, u32 p1, u32 p2, u32 p3) {
  u32 l0 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1;
  i0 = g0;
  i1 = 32u;
  i0 -= i1;
  l0 = i0;
  i1 = p0;
  i32_store((&memory), (u64)(i0 + 20), i1);
  i0 = l0;
  i1 = p1;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l0;
  i1 = p2;
  i32_store((&memory), (u64)(i0 + 12), i1);
  i0 = l0;
  i1 = p3;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = 2u;
  i1 = l0;
  i1 = i32_load((&memory), (u64)(i1 + 16));
  i0 = i0 > i1;
  if (i0) {
    i0 = l0;
    i1 = 105u;
    i32_store((&memory), (u64)(i0 + 24), i1);
    goto B0;
  }
  i0 = l0;
  i1 = l0;
  i1 = i32_load((&memory), (u64)(i1 + 20));
  i1 = i32_load8_u((&memory), (u64)(i1));
  i32_store8((&memory), (u64)(i0 + 31), i1);
  i0 = l0;
  i0 = i32_load8_u((&memory), (u64)(i0 + 31));
  i1 = 15u;
  i0 &= i1;
  if (i0) {
    i0 = l0;
    i1 = 112u;
    i32_store((&memory), (u64)(i0 + 24), i1);
    goto B0;
  }
  i0 = l0;
  i0 = i32_load((&memory), (u64)(i0 + 12));
  i1 = l0;
  i1 = i32_load((&memory), (u64)(i1 + 20));
  i1 = i32_load8_u((&memory), (u64)(i1 + 1));
  i32_store8((&memory), (u64)(i0), i1);
  i0 = l0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = 2u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l0;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 24), i1);
  B0:;
  i0 = l0;
  i0 = i32_load((&memory), (u64)(i0 + 24));
  FUNC_EPILOGUE;
  return i0;
}

static u32 f2(u32 p0, u32 p1, u32 p2, u32 p3) {
  u32 l0 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2;
  i0 = g0;
  i1 = 32u;
  i0 -= i1;
  l0 = i0;
  i1 = p0;
  i32_store((&memory), (u64)(i0 + 20), i1);
  i0 = l0;
  i1 = p1;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l0;
  i1 = p2;
  i32_store((&memory), (u64)(i0 + 12), i1);
  i0 = l0;
  i1 = p3;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = 2u;
  i1 = l0;
  i1 = i32_load((&memory), (u64)(i1 + 16));
  i0 = i0 > i1;
  if (i0) {
    i0 = l0;
    i1 = 105u;
    i32_store((&memory), (u64)(i0 + 24), i1);
    goto B0;
  }
  i0 = l0;
  i1 = l0;
  i1 = i32_load((&memory), (u64)(i1 + 20));
  i1 = i32_load8_u((&memory), (u64)(i1));
  i32_store8((&memory), (u64)(i0 + 31), i1);
  i0 = l0;
  i0 = i32_load8_u((&memory), (u64)(i0 + 31));
  i1 = 15u;
  i0 &= i1;
  i1 = 1u;
  i0 = i0 != i1;
  if (i0) {
    i0 = l0;
    i1 = 112u;
    i32_store((&memory), (u64)(i0 + 24), i1);
    goto B0;
  }
  i0 = l0;
  i0 = i32_load((&memory), (u64)(i0 + 12));
  i1 = l0;
  i1 = i32_load((&memory), (u64)(i1 + 20));
  i1 = i32_load8_u((&memory), (u64)(i1 + 1));
  i2 = 4294967295u;
  i1 ^= i2;
  i32_store8((&memory), (u64)(i0), i1);
  i0 = l0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = 2u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l0;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 24), i1);
  B0:;
  i0 = l0;
  i0 = i32_load((&memory), (u64)(i0 + 24));
  FUNC_EPILOGUE;
  return i0;
}

static u32 f3(u32 p0, u32 p1, u32 p2, u32 p3) {
  u32 l0 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2;
  i0 = g0;
  i1 = 32u;
  i0 -= i1;
  l0 = i0;
  i1 = p0;
  i32_store((&memory), (u64)(i0 + 20), i1);
  i0 = l0;
  i1 = p1;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l0;
  i1 = p2;
  i32_store((&memory), (u64)(i0 + 12), i1);
  i0 = l0;
  i1 = p3;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = 3u;
  i1 = l0;
  i1 = i32_load((&memory), (u64)(i1 + 16));
  i0 = i0 > i1;
  if (i0) {
    i0 = l0;
    i1 = 105u;
    i32_store((&memory), (u64)(i0 + 24), i1);
    goto B0;
  }
  i0 = l0;
  i1 = l0;
  i1 = i32_load((&memory), (u64)(i1 + 20));
  i1 = i32_load8_u((&memory), (u64)(i1));
  i32_store8((&memory), (u64)(i0 + 31), i1);
  i0 = l0;
  i0 = i32_load8_u((&memory), (u64)(i0 + 31));
  i1 = 15u;
  i0 &= i1;
  i1 = 2u;
  i0 = i0 != i1;
  if (i0) {
    i0 = l0;
    i1 = 112u;
    i32_store((&memory), (u64)(i0 + 24), i1);
    goto B0;
  }
  i0 = l0;
  i0 = i32_load((&memory), (u64)(i0 + 12));
  i1 = l0;
  i1 = i32_load((&memory), (u64)(i1 + 20));
  i1 = i32_load8_u((&memory), (u64)(i1 + 1));
  i2 = l0;
  i2 = i32_load((&memory), (u64)(i2 + 20));
  i2 = i32_load8_u((&memory), (u64)(i2 + 2));
  i1 ^= i2;
  i32_store8((&memory), (u64)(i0), i1);
  i0 = l0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = 3u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l0;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 24), i1);
  B0:;
  i0 = l0;
  i0 = i32_load((&memory), (u64)(i0 + 24));
  FUNC_EPILOGUE;
  return i0;
}

static u32 f4(u32 p0, u32 p1, u32 p2, u32 p3) {
  u32 l0 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2;
  i0 = g0;
  i1 = 32u;
  i0 -= i1;
  l0 = i0;
  i1 = p0;
  i32_store((&memory), (u64)(i0 + 20), i1);
  i0 = l0;
  i1 = p1;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l0;
  i1 = p2;
  i32_store((&memory), (u64)(i0 + 12), i1);
  i0 = l0;
  i1 = p3;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = 3u;
  i1 = l0;
  i1 = i32_load((&memory), (u64)(i1 + 16));
  i0 = i0 > i1;
  if (i0) {
    i0 = l0;
    i1 = 105u;
    i32_store((&memory), (u64)(i0 + 24), i1);
    goto B0;
  }
  i0 = l0;
  i1 = l0;
  i1 = i32_load((&memory), (u64)(i1 + 20));
  i1 = i32_load8_u((&memory), (u64)(i1));
  i32_store8((&memory), (u64)(i0 + 31), i1);
  i0 = l0;
  i0 = i32_load8_u((&memory), (u64)(i0 + 31));
  i1 = 15u;
  i0 &= i1;
  i1 = 3u;
  i0 = i0 != i1;
  if (i0) {
    i0 = l0;
    i1 = 112u;
    i32_store((&memory), (u64)(i0 + 24), i1);
    goto B0;
  }
  i0 = l0;
  i0 = i32_load((&memory), (u64)(i0 + 12));
  i1 = l0;
  i1 = i32_load((&memory), (u64)(i1 + 20));
  i1 = i32_load8_u((&memory), (u64)(i1 + 1));
  i2 = l0;
  i2 = i32_load((&memory), (u64)(i2 + 20));
  i2 = i32_load8_u((&memory), (u64)(i2 + 2));
  i1 &= i2;
  i32_store8((&memory), (u64)(i0), i1);
  i0 = l0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = 3u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l0;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 24), i1);
  B0:;
  i0 = l0;
  i0 = i32_load((&memory), (u64)(i0 + 24));
  FUNC_EPILOGUE;
  return i0;
}

static u32 f5(u32 p0, u32 p1, u32 p2, u32 p3) {
  u32 l0 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2;
  i0 = g0;
  i1 = 32u;
  i0 -= i1;
  l0 = i0;
  i1 = p0;
  i32_store((&memory), (u64)(i0 + 20), i1);
  i0 = l0;
  i1 = p1;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l0;
  i1 = p2;
  i32_store((&memory), (u64)(i0 + 12), i1);
  i0 = l0;
  i1 = p3;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = 3u;
  i1 = l0;
  i1 = i32_load((&memory), (u64)(i1 + 16));
  i0 = i0 > i1;
  if (i0) {
    i0 = l0;
    i1 = 105u;
    i32_store((&memory), (u64)(i0 + 24), i1);
    goto B0;
  }
  i0 = l0;
  i1 = l0;
  i1 = i32_load((&memory), (u64)(i1 + 20));
  i1 = i32_load8_u((&memory), (u64)(i1));
  i32_store8((&memory), (u64)(i0 + 31), i1);
  i0 = l0;
  i0 = i32_load8_u((&memory), (u64)(i0 + 31));
  i1 = 15u;
  i0 &= i1;
  i1 = 4u;
  i0 = i0 != i1;
  if (i0) {
    i0 = l0;
    i1 = 112u;
    i32_store((&memory), (u64)(i0 + 24), i1);
    goto B0;
  }
  i0 = l0;
  i0 = i32_load((&memory), (u64)(i0 + 12));
  i1 = l0;
  i1 = i32_load((&memory), (u64)(i1 + 20));
  i1 = i32_load8_u((&memory), (u64)(i1 + 1));
  i2 = l0;
  i2 = i32_load((&memory), (u64)(i2 + 20));
  i2 = i32_load8_u((&memory), (u64)(i2 + 2));
  i1 |= i2;
  i32_store8((&memory), (u64)(i0), i1);
  i0 = l0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = 3u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l0;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 24), i1);
  B0:;
  i0 = l0;
  i0 = i32_load((&memory), (u64)(i0 + 24));
  FUNC_EPILOGUE;
  return i0;
}

static u32 f6(u32 p0, u32 p1, u32 p2, u32 p3) {
  u32 l0 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2;
  i0 = g0;
  i1 = 32u;
  i0 -= i1;
  l0 = i0;
  i1 = p0;
  i32_store((&memory), (u64)(i0 + 20), i1);
  i0 = l0;
  i1 = p1;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l0;
  i1 = p2;
  i32_store((&memory), (u64)(i0 + 12), i1);
  i0 = l0;
  i1 = p3;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = 3u;
  i1 = l0;
  i1 = i32_load((&memory), (u64)(i1 + 16));
  i0 = i0 > i1;
  if (i0) {
    i0 = l0;
    i1 = 105u;
    i32_store((&memory), (u64)(i0 + 24), i1);
    goto B0;
  }
  i0 = l0;
  i1 = l0;
  i1 = i32_load((&memory), (u64)(i1 + 20));
  i1 = i32_load8_u((&memory), (u64)(i1));
  i32_store8((&memory), (u64)(i0 + 31), i1);
  i0 = l0;
  i0 = i32_load8_u((&memory), (u64)(i0 + 31));
  i1 = 15u;
  i0 &= i1;
  i1 = 5u;
  i0 = i0 != i1;
  if (i0) {
    i0 = l0;
    i1 = 112u;
    i32_store((&memory), (u64)(i0 + 24), i1);
    goto B0;
  }
  i0 = l0;
  i0 = i32_load((&memory), (u64)(i0 + 12));
  i1 = l0;
  i1 = i32_load((&memory), (u64)(i1 + 20));
  i1 = i32_load8_u((&memory), (u64)(i1 + 1));
  i2 = l0;
  i2 = i32_load((&memory), (u64)(i2 + 20));
  i2 = i32_load8_u((&memory), (u64)(i2 + 2));
  i1 += i2;
  i32_store8((&memory), (u64)(i0), i1);
  i0 = l0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = 3u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l0;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 24), i1);
  B0:;
  i0 = l0;
  i0 = i32_load((&memory), (u64)(i0 + 24));
  FUNC_EPILOGUE;
  return i0;
}

static u32 f7(u32 p0, u32 p1, u32 p2, u32 p3) {
  u32 l0 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2;
  i0 = g0;
  i1 = 32u;
  i0 -= i1;
  l0 = i0;
  i1 = p0;
  i32_store((&memory), (u64)(i0 + 20), i1);
  i0 = l0;
  i1 = p1;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l0;
  i1 = p2;
  i32_store((&memory), (u64)(i0 + 12), i1);
  i0 = l0;
  i1 = p3;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = 3u;
  i1 = l0;
  i1 = i32_load((&memory), (u64)(i1 + 16));
  i0 = i0 > i1;
  if (i0) {
    i0 = l0;
    i1 = 105u;
    i32_store((&memory), (u64)(i0 + 24), i1);
    goto B0;
  }
  i0 = l0;
  i1 = l0;
  i1 = i32_load((&memory), (u64)(i1 + 20));
  i1 = i32_load8_u((&memory), (u64)(i1));
  i32_store8((&memory), (u64)(i0 + 31), i1);
  i0 = l0;
  i0 = i32_load8_u((&memory), (u64)(i0 + 31));
  i1 = 15u;
  i0 &= i1;
  i1 = 6u;
  i0 = i0 != i1;
  if (i0) {
    i0 = l0;
    i1 = 112u;
    i32_store((&memory), (u64)(i0 + 24), i1);
    goto B0;
  }
  i0 = l0;
  i0 = i32_load((&memory), (u64)(i0 + 12));
  i1 = l0;
  i1 = i32_load((&memory), (u64)(i1 + 20));
  i1 = i32_load8_u((&memory), (u64)(i1 + 2));
  i2 = l0;
  i2 = i32_load((&memory), (u64)(i2 + 20));
  i2 = i32_load8_u((&memory), (u64)(i2 + 1));
  i1 -= i2;
  i32_store8((&memory), (u64)(i0), i1);
  i0 = l0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = 3u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l0;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 24), i1);
  B0:;
  i0 = l0;
  i0 = i32_load((&memory), (u64)(i0 + 24));
  FUNC_EPILOGUE;
  return i0;
}

static u32 f8(u32 p0, u32 p1, u32 p2, u32 p3, u32 p4) {
  u32 l0 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  i0 = g0; // Well g0 is really like a stack pointer, like esp/rsp
  i1 = 4294967232u;
  i0 += i1; // i0 -= 64
  l0 = i0; // l0 = esp
  g0 = i0; // g0 = esp
  i0 = l0; // i0 = esp
  i1 = p0; 
  i32_store((&memory), (u64)(i0 + 52), i1); // mem[esp + 52] = p0
  i0 = l0;
  i1 = p1;
  i32_store((&memory), (u64)(i0 + 48), i1); // mem[esp + 48] = p1
  i0 = l0;
  i1 = p2;
  i32_store((&memory), (u64)(i0 + 44), i1); // mem[esp + 44] = p2
  i0 = l0;
  i1 = p3;
  i32_store((&memory), (u64)(i0 + 40), i1); // mem[esp + 40] = p3
  i0 = l0;
  i1 = p4;
  i32_store((&memory), (u64)(i0 + 36), i1); // mem[esp + 36] = p4
  i0 = l0;
  i1 = 0u;
  p0 = i1; // p0 = 0
  i32_store((&memory), (u64)(i0 + 32), i1); // mem[esp + 32] = 0
  i0 = l0;
  i1 = l0; // i1 = esp
  i1 = i32_load((&memory), (u64)(i1 + 52));
  i32_store((&memory), (u64)(i0 + 28), i1); // mem[esp + 28] = mem[esp + 52] = original p0
  i0 = l0;
  i1 = p0;
  i32_store((&memory), (u64)(i0 + 24), i1); // mem[esp + 24] = 0
  L1: 
    i0 = 0u;
    p0 = i0; // p0 = 0
    i0 = l0; // i0 = esp
    i0 = i32_load((&memory), (u64)(i0 + 28)); // i0 = mem[esp + 28]
    i1 = l0;
    i1 = i32_load((&memory), (u64)(i1 + 52)); // i1 = mem[esp + 52]
    i2 = l0;
    i2 = i32_load((&memory), (u64)(i2 + 48)); // i2 = mem[esp + 48]
    i1 += i2; 
    i0 = i0 < i1;
    if (i0) { // if (mem[esp + 28] < mem[esp + 52] + mem[esp + 48])
      i0 = l0;
      i0 = i32_load((&memory), (u64)(i0 + 24)); // i0 = mem[esp + 24]
      i1 = l0;
      i1 = i32_load((&memory), (u64)(i1 + 40)); // i1 = mem[esp + 40]
      i0 = i0 < i1;
      p0 = i0; // p0 = mem[esp + 24] < mem[esp + 40]
    }
    i0 = p0;
    i1 = 1u;
    i0 &= i1; // take last bit of current p0
    i0 = !(i0); // flip it
    if (i0) {goto B3;} // break
    i0 = l0;
    i1 = l0;
    i1 = i32_load((&memory), (u64)(i1 + 28)); // i1 = mem[esp + 28]
    i1 = i32_load8_u((&memory), (u64)(i1)); // i1 = mem[mem[esp + 28]]
    i32_store8((&memory), (u64)(i0 + 63), i1); // mem[esp + 63] = mem[mem[esp + 28]]
    i0 = l0;
    i1 = l0;
    i1 = i32_load8_u((&memory), (u64)(i1 + 63)); // i1 = mem[esp + 63]
    i2 = 15u;
    i1 &= i2; // i1 = mem[esp + 63] & 15 (last 4 bits)
    i32_store8((&memory), (u64)(i0 + 23), i1); // mem[esp + 23] = mem[esp + 63] & 15
    i0 = 7u;
    i1 = l0;
    i1 = i32_load8_u((&memory), (u64)(i1 + 23)); // i1 = mem[esp + 23]
    i0 = (u32)((s32)i0 <= (s32)i1); // i0 = 7 <= mem[esp + 23]
    if (i0) { // This means mem[esp + 23] < 7
      i0 = l0;
      i1 = 112u;
      i32_store((&memory), (u64)(i0 + 56), i1); // mem[esp + 56] = 112
      goto B0; // get the result and return, not win because retval is 112
    }
    i0 = l0;
    i1 = l0;
    i1 = i32_load8_u((&memory), (u64)(i1 + 23)); // i1 = mem[esp + 23]
    i2 = 2u;
    i1 <<= (i2 & 31); // i1 = mem[esp + 23] << (2 & 31) = mem[esp + 23] << 2
    i2 = 1024u;
    i1 += i2;
    i1 = i32_load((&memory), (u64)(i1)); // i1 = mem[(mem[esp + 23] << 2) + 1024]
    i32_store((&memory), (u64)(i0 + 16), i1); // mem[esp + 16] = i1
    i0 = l0;
    i1 = 0u;
    i32_store8((&memory), (u64)(i0 + 15), i1); // mem[esp + 15] = 0
    i0 = l0;
    i1 = 0u;
    p0 = i1; // p0 = 0
    i32_store((&memory), (u64)(i0 + 8), i1); // mem[esp + 8] = 0
    i0 = p0;
    i1 = l0;
    i1 = i32_load((&memory), (u64)(i1 + 28)); // i1 = mem[esp + 28]
    i2 = l0;
    i2 = i32_load((&memory), (u64)(i2 + 48)); // i2 = mem[esp + 48]
    i3 = l0;
    i3 = i32_load((&memory), (u64)(i3 + 28)); // i3 = mem[esp + 28]
    i4 = l0;
    i4 = i32_load((&memory), (u64)(i4 + 52)); // i4 = mem[esp + 52]
    i3 -= i4; // i3 = mem[esp + 28] - mem[esp + 52]
    i2 -= i3; // i2 = mem[esp + 48] - mem[esp + 28] + mem[esp + 52]
    i3 = l0;
    i4 = 15u;
    i3 += i4; // i3 = esp + 15
    i4 = l0;
    i5 = 8u;
    i4 += i5; // i4 = esp + 8
    i5 = l0;
    i5 = i32_load((&memory), (u64)(i5 + 16)); // i5 = mem[esp + 16]
    // i5 is the index of the function in the function table
    i1 = CALL_INDIRECT(T0, u32 (*)(u32, u32, u32, u32), 0, i5, i1, i2, i3, i4); // call some shits
    // call ftable[mem[esp + 16]](mem[esp + 28], mem[esp + 48] - mem[esp + 28] + mem[esp + 52], esp + 15, esp + 8)
    i0 = i0 != i1; // i0 = i1 != 0
    if (i0) {goto B3;} // break
    i0 = l0;
    i0 = i32_load8_u((&memory), (u64)(i0 + 15));
    i1 = l0;
    i1 = i32_load((&memory), (u64)(i1 + 44));
    i2 = l0;
    i2 = i32_load((&memory), (u64)(i2 + 24));
    i1 += i2;
    i1 = i32_load8_u((&memory), (u64)(i1));
    i2 = 24u;
    p0 = i2;
    i1 <<= (i2 & 31);
    i2 = p0;
    i1 = (u32)((s32)i1 >> (i2 & 31));
    i0 = i0 == i1;
    if (i0) {
      i0 = l0;
      i1 = l0;
      i1 = i32_load((&memory), (u64)(i1 + 32));
      i2 = 1u;
      i1 += i2;
      i32_store((&memory), (u64)(i0 + 32), i1);
    }
    i0 = l0;
    i1 = l0;
    i1 = i32_load((&memory), (u64)(i1 + 28));
    i2 = l0;
    i2 = i32_load((&memory), (u64)(i2 + 8));
    i1 += i2;
    i32_store((&memory), (u64)(i0 + 28), i1);
    i0 = l0;
    i1 = l0;
    i1 = i32_load((&memory), (u64)(i1 + 24));
    i2 = 1u;
    i1 += i2;
    i32_store((&memory), (u64)(i0 + 24), i1);
    goto L1; // loop back
    B3:;
  i0 = l0;
  i0 = i32_load((&memory), (u64)(i0 + 28)); // i0 = mem[esp + 28]
  i1 = l0;
  i1 = i32_load((&memory), (u64)(i1 + 52)); // i1 = mem[esp + 52]
  i2 = l0;
  i2 = i32_load((&memory), (u64)(i2 + 48)); // i2 = mem[esp + 48]
  i1 += i2;
  i0 = i0 != i1; // i0 = mem[esp + 28] != mem[esp + 52] + mem[esp + 48]
  if (i0) { // this means mem[esp + 28] needs to be == mem[esp + 52] + mem[esp + 48]
    i0 = l0;
    i0 = i32_load((&memory), (u64)(i0 + 36)); // i0 = mem[esp + 36]
    i1 = 0u;
    i32_store8((&memory), (u64)(i0), i1); // mem[mem[esp + 36]] = 0
    goto B6; // not win because mem[mem[esp + 36]] needs the last bit to be 1
  }
  i0 = l0;
  i0 = i32_load((&memory), (u64)(i0 + 32)); // i0 = mem[esp + 32]
  i1 = l0;
  i1 = i32_load((&memory), (u64)(i1 + 40)); // i1 = mem[esp + 40]
  i0 = i0 != i1; // i0 = mem[esp + 32] != mem[esp + 40]
  if (i0) { // this means mem[esp + 32] needs to be = mem[esp + 40]
    i0 = l0;
    i0 = i32_load((&memory), (u64)(i0 + 36)); // i0 = mem[esp + 36]
    i1 = 0u;
    i32_store8((&memory), (u64)(i0), i1); // mem[mem[esp + 36]] = 0, also not win
    goto B8; // not win
  }
  i0 = l0;
  i0 = i32_load((&memory), (u64)(i0 + 36));
  i1 = 1u;
  i32_store8((&memory), (u64)(i0), i1); // This is the winning path, mem[mem[esp + 36]] = 1
  B8:; // Fail if jump to here
  B6:; // Fail if jump to here
  i0 = l0;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 56), i1); // mem[esp + 56] = 0
  B0:; // get the result and return
  i0 = l0;
  i0 = i32_load((&memory), (u64)(i0 + 56)); // mem[esp + 56] is the result
  p0 = i0;
  i0 = l0;
  i1 = 4294967232u;
  i0 -= i1; // i0 += 64
  g0 = i0;
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static u32 Match(u32 p0, u32 p1, u32 p2, u32 p3) {
  u32 l0 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  i0 = g0;
  i1 = 32u;
  i0 -= i1; // i0 = esp
  l0 = i0; // l0 = esp
  g0 = i0; // g0 = esp
  i0 = l0;
  i1 = p0;
  i32_store((&memory), (u64)(i0 + 24), i1); // mem[esp + 24] = p0
  i0 = l0;
  i1 = p1;
  i32_store((&memory), (u64)(i0 + 20), i1); // mem[esp + 20] = p1
  i0 = l0;
  i1 = p2;
  i32_store((&memory), (u64)(i0 + 16), i1); // mem[esp + 16] = p2
  i0 = l0;
  i1 = p3;
  i32_store((&memory), (u64)(i0 + 12), i1); // mem[esp + 12] = p3
  i0 = l0;
  i1 = 0u;
  i32_store8((&memory), (u64)(i0 + 11), i1); // mem[esp + 11] = 0
  i0 = l0;
  i0 = i32_load((&memory), (u64)(i0 + 24)); // i0 = mem[esp + 24]
  i1 = l0;
  i1 = i32_load((&memory), (u64)(i1 + 20)); // i1 = mem[esp + 20]
  i2 = l0;
  i2 = i32_load((&memory), (u64)(i2 + 16)); // i2 = mem[esp + 16]
  i3 = l0;
  i3 = i32_load((&memory), (u64)(i3 + 12)); // i2 = mem[esp + 12]
  i4 = l0;
  i5 = 11u;
  i4 += i5; // i4 = esp + 11
  i0 = f8(i0, i1, i2, i3, i4); // i0 = f8(p0, p1, p2, p3, addr)
  if (i0) { // This means failed if i0 != 0
    i0 = l0;
    i1 = 0u;
    i32_store((&memory), (u64)(i0 + 28), i1);
    goto B0;
  }
  // Winning
  i0 = l0;
  i1 = l0;
  i1 = i32_load8_u((&memory), (u64)(i1 + 11)); // i1 = mem[esp + 11]
  i2 = 1u; 
  i1 &= i2; // i1 &= 1; get last bit? which has to be 1.
  i32_store((&memory), (u64)(i0 + 28), i1);
  B0:; // Get the results from mem[esp + 28] 
  i0 = l0;
  i0 = i32_load((&memory), (u64)(i0 + 28));
  p0 = i0; // p0 = mem[esp + 28]
  i0 = l0;
  i1 = 32u;
  i0 += i1;
  g0 = i0; // g0 = esp + 32 LOL
  i0 = p0; // i0 = mem[esp + 28]
  FUNC_EPILOGUE;
  return i0; // return mem[esp + 28]
}

static u32 writev_c(u32 p0, u32 p1, u32 p2) {
  u32 l0 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  i0 = g0;
  i1 = 32u;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  i0 = l0;
  i1 = p0;
  i32_store((&memory), (u64)(i0 + 28), i1);
  i0 = l0;
  i1 = p1;
  i32_store((&memory), (u64)(i0 + 24), i1);
  i0 = l0;
  i1 = p2;
  i32_store((&memory), (u64)(i0 + 20), i1);
  i0 = l0;
  i1 = 0u;
  p0 = i1;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l0;
  i1 = p0;
  i32_store((&memory), (u64)(i0 + 12), i1);
  L1: 
    i0 = l0;
    i0 = i32_load((&memory), (u64)(i0 + 12));
    i1 = l0;
    i1 = i32_load((&memory), (u64)(i1 + 20));
    i0 = (u32)((s32)i0 >= (s32)i1);
    if (i0) {goto B0;}
    i0 = l0;
    i1 = 0u;
    i32_store((&memory), (u64)(i0 + 8), i1);
    L3: 
      i0 = l0;
      i0 = i32_load((&memory), (u64)(i0 + 8));
      i1 = l0;
      i1 = i32_load((&memory), (u64)(i1 + 24));
      i2 = l0;
      i2 = i32_load((&memory), (u64)(i2 + 12));
      i3 = 3u;
      i2 <<= (i3 & 31);
      i1 += i2;
      i1 = i32_load((&memory), (u64)(i1 + 4));
      i0 = i0 >= i1;
      if (i0) {goto B2;}
      i0 = l0;
      i0 = i32_load((&memory), (u64)(i0 + 24));
      i1 = l0;
      i1 = i32_load((&memory), (u64)(i1 + 12));
      i2 = 3u;
      i1 <<= (i2 & 31);
      i0 += i1;
      i0 = i32_load((&memory), (u64)(i0));
      i1 = l0;
      i1 = i32_load((&memory), (u64)(i1 + 8));
      i0 += i1;
      i0 = i32_load8_u((&memory), (u64)(i0));
      i1 = 24u;
      p0 = i1;
      i0 <<= (i1 & 31);
      i1 = p0;
      i0 = (u32)((s32)i0 >> (i1 & 31));
      (*Z_envZ_putc_jsZ_vi)(i0);
      i0 = l0;
      i1 = l0;
      i1 = i32_load((&memory), (u64)(i1 + 8));
      i2 = 1u;
      i1 += i2;
      i32_store((&memory), (u64)(i0 + 8), i1);
      goto L3;
    UNREACHABLE;
    B2:;
    i0 = l0;
    i1 = l0;
    i1 = i32_load((&memory), (u64)(i1 + 16));
    i2 = l0;
    i2 = i32_load((&memory), (u64)(i2 + 24));
    i3 = l0;
    i3 = i32_load((&memory), (u64)(i3 + 12));
    i4 = 3u;
    i3 <<= (i4 & 31);
    i2 += i3;
    i2 = i32_load((&memory), (u64)(i2 + 4));
    i1 += i2;
    i32_store((&memory), (u64)(i0 + 16), i1);
    i0 = l0;
    i1 = l0;
    i1 = i32_load((&memory), (u64)(i1 + 12));
    i2 = 1u;
    i1 += i2;
    i32_store((&memory), (u64)(i0 + 12), i1);
    goto L1;
  UNREACHABLE;
  B0:;
  i0 = l0;
  i0 = i32_load((&memory), (u64)(i0 + 16));
  p0 = i0;
  i0 = l0;
  i1 = 32u;
  i0 += i1;
  g0 = i0;
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static const u8 data_segment_data_0[] = {
  0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 
  0x04, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 
  0x07, 
};

static void init_memory(void) {
  wasm_rt_allocate_memory((&memory), 2, 65536);
  memcpy(&(memory.data[1024u]), data_segment_data_0, 25);
}

static void init_table(void) {
  uint32_t offset;
  wasm_rt_allocate_table((&T0), 8, 8);
  offset = 1u;
  T0.data[offset + 0] = (wasm_rt_elem_t){func_types[0], (wasm_rt_anyfunc_t)(&f1)};
  T0.data[offset + 1] = (wasm_rt_elem_t){func_types[0], (wasm_rt_anyfunc_t)(&f2)};
  T0.data[offset + 2] = (wasm_rt_elem_t){func_types[0], (wasm_rt_anyfunc_t)(&f3)};
  T0.data[offset + 3] = (wasm_rt_elem_t){func_types[0], (wasm_rt_anyfunc_t)(&f4)};
  T0.data[offset + 4] = (wasm_rt_elem_t){func_types[0], (wasm_rt_anyfunc_t)(&f5)};
  T0.data[offset + 5] = (wasm_rt_elem_t){func_types[0], (wasm_rt_anyfunc_t)(&f6)};
  T0.data[offset + 6] = (wasm_rt_elem_t){func_types[0], (wasm_rt_anyfunc_t)(&f7)};
}

/* export: 'memory' */
wasm_rt_memory_t (*WASM_RT_ADD_PREFIX(Z_memory));
/* export: '__heap_base' */
u32 (*WASM_RT_ADD_PREFIX(Z___heap_baseZ_i));
/* export: '__data_end' */
u32 (*WASM_RT_ADD_PREFIX(Z___data_endZ_i));
/* export: 'Match' */
u32 (*WASM_RT_ADD_PREFIX(Z_MatchZ_iiiii))(u32, u32, u32, u32);
/* export: 'writev_c' */
u32 (*WASM_RT_ADD_PREFIX(Z_writev_cZ_iiii))(u32, u32, u32);

static void init_exports(void) {
  /* export: 'memory' */
  WASM_RT_ADD_PREFIX(Z_memory) = (&memory);
  /* export: '__heap_base' */
  WASM_RT_ADD_PREFIX(Z___heap_baseZ_i) = (&__heap_base);
  /* export: '__data_end' */
  WASM_RT_ADD_PREFIX(Z___data_endZ_i) = (&__data_end);
  /* export: 'Match' */
  WASM_RT_ADD_PREFIX(Z_MatchZ_iiiii) = (&Match);
  /* export: 'writev_c' */
  WASM_RT_ADD_PREFIX(Z_writev_cZ_iiii) = (&writev_c);
}

void WASM_RT_ADD_PREFIX(init)(void) {
  init_func_types();
  init_globals();
  init_memory();
  init_table();
  init_exports();
}
