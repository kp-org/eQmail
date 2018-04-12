/*
 *  Revision 20180123, Kai Peter
 *  - prevented 'misleading-indentation' warning
*/
#include "fmtqfn.h"
#include "fmt.h"
#include "auto_split.h"

/* @qfn: seems to be q(ueue)f(ile)n(ame)
   This is NOT a standard lib (doesn't belong to fmt.c! Used by
   qmail-clean, qmail-qread, qmail-queue and qmail-send.
*/

unsigned int fmt_qfn(char *s,char *dirslash,unsigned long id,int flagsplit)
{
 unsigned int len;
 unsigned int i;

 len = 0;
 i = fmt_str(s,dirslash); len += i; if (s) s += i;
 if (flagsplit)
  {
   i = fmt_ulong(s,id % auto_split); len += i; if (s) s += i;
   i = fmt_str(s,"/"); len += i; if (s) s += i;
  }
 i = fmt_ulong(s,id); len += i; if (s) s += i;
 if (s) { *s++ = 0; } ++len;
 return len;
}
