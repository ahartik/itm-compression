#pragma once

//int call4(int a, int b, int c, int d);
static inline int call4(int a, int b, int c, int d) {
    int res;
    asm volatile("int $128": "=a"(res): "a"(a), "b"(b), "c"(c), "d"(d));
    return res;
}
#define mkcall4(a,b,c,d) call4((int)a, (int)b, (int)c, (int)d)

#define readdata(fd,buf,len) mkcall4(3,fd,buf,len)
#define writedata(fd,buf,len) mkcall4(4,fd,buf,len)
#define openfile(name,flags,mode) mkcall4(5,name,flags,mode)
