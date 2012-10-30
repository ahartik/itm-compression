#pragma once

void call4(int a, int b, int c, int d);
#define mkcall4(a,b,c,d) call4((int)a, (int)b, (int)c, (int)d)

#define readdata(fd,buf,len) mkcall4(3,fd,buf,len)
#define writedata(fd,buf,len) mkcall4(4,fd,buf,len)
#define openfile(name,flags,mode) mkcall4(5,name,flags,mode)
