/* This file has been generated by the Hex-Rays decompiler.
   Copyright (c) 2007-2015 Hex-Rays <info@hex-rays.com>

   Detected compiler: GNU C++
*/

#include <defs.h>


//-------------------------------------------------------------------------
// Function declarations

int init_proc();
// void __noreturn _exit(int status);
// int puts(const char *s);
// DIR *opendir(const char *name);
// int system(const char *command);
// char *strchr(const char *s, int c);
// int close(int fd);
// int closedir(DIR *dirp);
// ssize_t read(int fd, void *buf, size_t nbytes);
// int __gmon_start__(void); weak
// struct dirent *readdir(DIR *dirp);
// int setvbuf(FILE *stream, char *buf, int modes, size_t n);
// int open(const char *file, int oflag, ...);
// char *strdup(const char *s);
// int __fastcall __cxa_finalize(_QWORD); weak
void __fastcall __noreturn main(__int64 a1, char **a2, char **a3);
int sub_AC0();
int sub_AF0();
int sub_B30();
int sub_B70();
int sub_BA5();
signed __int64 __fastcall sub_BE6(__int64 a1, __int64 a2);
__int64 __fastcall sub_C00(int fd, __int64 a2, __int64 a3);
signed __int64 __fastcall sub_C5E(__int64 a1);
void __fastcall __noreturn sub_CB7(__int64 a1);
char *__fastcall sub_D42(char *file);
char *sub_DA1();
void __fastcall init(unsigned int a1, __int64 a2, __int64 a3);
void term_proc();
// int ITM_deregisterTMCloneTable(void); weak
// int __fastcall Jv_RegisterClasses(_QWORD); weak

//-------------------------------------------------------------------------
// Data declarations

__int64 (__fastcall *off_201DE0[2])() = { &sub_B70, &sub_B30 }; // weak
__int64 (__fastcall *off_201DE8)() = &sub_B30; // weak
__int64 qword_201DF0 = 0LL; // weak
void *off_202098 = &off_202098; // weak
__int64 (__fastcall *off_2020A0)() = &sub_C5E; // weak
char _bss_start; // weak
_UNKNOWN unk_2020BF; // weak
// extern struct _IO_FILE *stdout;
// extern struct _IO_FILE *stdin;
// extern _UNKNOWN _cxa_finalize; weak
// extern _UNKNOWN _gmon_start__; weak


//----- (0000000000000920) ----------------------------------------------------
int init_proc()
{
  void *v0; // rax@1

  v0 = &_gmon_start__;
  if ( &_gmon_start__ )
    LODWORD(v0) = __gmon_start__();
  return v0;
}
// 9E0: using guessed type int __gmon_start__(void);

//----- (0000000000000A40) ----------------------------------------------------
void __fastcall __noreturn main(__int64 a1, char **a2, char **a3)
{
  __int64 (__fastcall **v3)(); // rsi@1
  char *v4; // rdi@1
  signed __int64 i; // rcx@1
  char v6; // [sp+8h] [bp-20h]@1

  v3 = &off_2020A0;
  v4 = &v6;
  for ( i = 6LL; i; --i )
  {
    *v4 = *v3;
    v3 = (v3 + 4);
    v4 += 4;
  }
  setvbuf(stdin, 0LL, 2, 0LL);
  setvbuf(stdout, 0LL, 2, 0LL);
  sub_CB7(&v6);
}
// 2020A0: using guessed type __int64 (__fastcall *off_2020A0)();

//----- (0000000000000A8F) ----------------------------------------------------
#error "A95: positive sp value has been found (funcsize=3)"

//----- (0000000000000AC0) ----------------------------------------------------
int sub_AC0()
{
  int (**v0)(void); // rax@1

  v0 = (&unk_2020BF - &_bss_start);
  if ( (&unk_2020BF - &_bss_start) > 0xE )
  {
    v0 = &ITM_deregisterTMCloneTable;
    if ( &ITM_deregisterTMCloneTable )
      LODWORD(v0) = ITM_deregisterTMCloneTable();
  }
  return v0;
}
// 2020B8: using guessed type char _bss_start;
// 202100: using guessed type int ITM_deregisterTMCloneTable(void);

//----- (0000000000000AF0) ----------------------------------------------------
int sub_AF0()
{
  return 0;
}

//----- (0000000000000B30) ----------------------------------------------------
int sub_B30()
{
  int result; // eax@4

  if ( !_bss_start )
  {
    if ( &_cxa_finalize )
      __cxa_finalize(off_202098);
    result = sub_AC0();
    _bss_start = 1;
  }
  return result;
}
// A30: using guessed type int __fastcall __cxa_finalize(_QWORD);
// 202098: using guessed type void *off_202098;
// 2020B8: using guessed type char _bss_start;

//----- (0000000000000B70) ----------------------------------------------------
int sub_B70()
{
  int result; // eax@3

  if ( qword_201DF0 && &Jv_RegisterClasses )
  {
    Jv_RegisterClasses(&qword_201DF0);
    result = sub_AF0();
  }
  else
  {
    result = sub_AF0();
  }
  return result;
}
// 201DF0: using guessed type __int64 qword_201DF0;
// 202108: using guessed type int __fastcall Jv_RegisterClasses(_QWORD);

//----- (0000000000000BA5) ----------------------------------------------------
int sub_BA5()
{
  DIR *v0; // rbx@1
  struct dirent *v1; // rax@5

  v0 = opendir("db");
  if ( !v0 )
    _exit(0);
  while ( 1 )
  {
    v1 = readdir(v0);
    if ( !v1 )
      break;
    if ( v1->d_name[0] != 46 )
      puts(v1->d_name);
  }
  return closedir(v0);
}

//----- (0000000000000BE6) ----------------------------------------------------
signed __int64 __fastcall sub_BE6(__int64 a1, __int64 a2)
{
  __int64 v2; // rax@1
  char v3; // dl@2

  v2 = 0LL;
  while ( 1 )
  {
    v3 = *(a1 + v2);
    if ( v3 != *(a2 + v2) )
      break;
    ++v2;
    if ( !v3 )
      return 1LL;
  }
  return 0LL;
}

//----- (0000000000000C00) ----------------------------------------------------
__int64 __fastcall sub_C00(int fd, __int64 a2, __int64 a3)
{
  __int64 v3; // rbp@1
  __int64 i; // rbx@1
  char buf; // [sp+Fh] [bp-29h]@3

  v3 = a3;
  for ( i = 0LL; i != v3; ++i )
  {
    if ( read(fd, &buf, 1uLL) <= 0 )
      _exit(0);
    if ( buf == 10 )
      break;
    *(a2 + i) = buf;
  }
  return i;
}

//----- (0000000000000C5E) ----------------------------------------------------
signed __int64 __fastcall sub_C5E(__int64 a1)
{
  __int64 v1; // rbp@1
  signed __int64 v2; // rcx@1
  __int64 *v3; // rdi@1
  int v4; // edi@4
  signed __int64 result; // rax@5
  __int64 v6; // [sp+0h] [bp-98h]@1

  v1 = a1;
  v2 = 32LL;
  v3 = &v6;
  while ( v2 )
  {
    *v3 = 0;
    v3 = (v3 + 4);
    --v2;
  }
  v4 = 0;
  if ( sub_C00(0, &v6, 4096LL) & 7 )
LABEL_7:
    _exit(v4);
  result = sub_BE6(&v6, v1);
  if ( result )
  {
    v4 = system("cat flag.txt");
    goto LABEL_7;
  }
  return result;
}

//----- (0000000000000CB7) ----------------------------------------------------
void __fastcall __noreturn sub_CB7(__int64 a1)
{
  __int64 v1; // r12@1
  __int64 v2; // rax@5
  char v3; // [sp+Fh] [bp-99h]@2

  v1 = 0LL;
  while ( 1 )
  {
    while ( 1 )
    {
      memset(&v3, 0, 0x81uLL);
      sub_C00(0, &v3, 128LL);
      if ( !sub_BE6(&v3, "USER") )
        break;
      if ( v1 )
        _exit(0);
      LODWORD(v2) = (*(a1 + 8))(&v3, "USER");
      v1 = v2;
    }
    if ( sub_BE6(&v3, "PASS") )
    {
      (*a1)(v1, "PASS");
    }
    else if ( sub_BE6(&v3, "LIST") )
    {
      (*(a1 + 16))(&v3, "LIST");
    }
  }
}

//----- (0000000000000D42) ----------------------------------------------------
char *__fastcall sub_D42(char *file)
{
  int v1; // eax@1
  int v2; // ebx@1
  char s; // [sp+Fh] [bp-1019h]@1

  memset(&s, 0, 0x1001uLL);
  v1 = open(file, 0);
  v2 = v1;
  if ( v1 == -1 )
    _exit(0);
  sub_C00(v1, &s, 4096LL);
  close(v2);
  return strdup(&s);
}

//----- (0000000000000DA1) ----------------------------------------------------
char *sub_DA1()
{
  signed __int64 v0; // rcx@1
  char *v1; // rdi@1
  char file; // [sp+Ch] [bp-9Ch]@1
  char v4; // [sp+Dh] [bp-9Bh]@4
  char v5; // [sp+Eh] [bp-9Ah]@4
  char v6; // [sp+Fh] [bp-99h]@4

  v0 = 33LL;
  v1 = &file;
  while ( v0 )
  {
    *v1 = 0;
    v1 += 4;
    --v0;
  }
  file = 100;
  v4 = 98;
  v5 = 47;
  sub_C00(0, &v6, 128LL);
  if ( strchr(&v6, 47) )
    _exit(0);
  return sub_D42(&file);
}

//----- (0000000000000E10) ----------------------------------------------------
void __fastcall init(unsigned int a1, __int64 a2, __int64 a3)
{
  __int64 v3; // r13@1
  __int64 v4; // rbx@1
  signed __int64 v5; // rbp@1

  v3 = a3;
  v4 = 0LL;
  v5 = &off_201DE8 - off_201DE0;
  init_proc();
  if ( v5 )
  {
    do
      (off_201DE0[v4++])(a1, a2, v3);
    while ( v4 != v5 );
  }
}
// 201DE0: using guessed type __int64 (__fastcall *off_201DE0[2])();
// 201DE8: using guessed type __int64 (__fastcall *off_201DE8)();

//----- (0000000000000E84) ----------------------------------------------------
void term_proc()
{
  ;
}

#error "There were 1 decompilation failure(s) on 16 function(s)"
