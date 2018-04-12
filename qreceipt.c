/*
 *  Revision 20180409, Kai Peter
 *  - switched to errmsg, code clean-up
 *  Revision 20160713, Kai Peter
 *  - switched to 'buffer', added 'unistd.h'
 *  Revision 20160503, Kai Peter
 *  - added braces to inner if-else block
 *  - changed return type of main to int
 */
#include <unistd.h>
#include "sig.h"
#include "env.h"
#include "buffer.h"
#include "stralloc.h"
#include "getln.h"
#include "alloc.h"
#include "str.h"
#include "hfield.h"
#include "token822.h"
#include "errmsg.h"
#include "headerbody.h"
#include "open.h"
#include "quote.h"
#include "qmail.h"

#define WHO "qreceipt"

void die_usage() { errint(EHARD,"usage: qreceipt deliveryaddress"); }
void die_read() {
  if (errno == ENOMEM) errmem;
  errint(ESOFT,"read error"); }
void doordie(stralloc *sa, int r) {  // 'do or die'
  if (r == 1) { return; }
  if (r == -1) errmem;
  errint(EHARD,B("unable to parse this: ",sa->s));
}

char *target;

int flagreceipt = 0;

char *returnpath;
stralloc messageid = {0};
stralloc sanotice = {0};

//int rwnotice(addr) token822_alloc *addr; { token822_reverse(addr);
int rwnotice(token822_alloc *addr) {
  token822_reverse(addr);
  if (token822_unquote(&sanotice,addr) != 1) errmem;
  if (sanotice.len == str_len(target))
    if (!str_diffn(sanotice.s,target,sanotice.len))
      flagreceipt = 1;
  token822_reverse(addr); return 1; }

struct qmail qqt;

stralloc quoted = {0};

void finishheader() {
  char *qqx;

  if (!flagreceipt) _exit(0);  //die_noreceipt();
  if (str_equal(returnpath,"")) _exit(0); //die_noreceipt();
  if (str_equal(returnpath,"#@[]")) _exit(0); //die_noreceipt();

  if (!quote2(&quoted,returnpath)) errmem;

  if (qmail_open(&qqt) == -1) errint(errno,"unable to fork");;

  qmail_puts(&qqt,"From: DELIVERY NOTICE SYSTEM <");
  qmail_put(&qqt,quoted.s,quoted.len);
  qmail_puts(&qqt,">\n");
  qmail_puts(&qqt,"To: <");
  qmail_put(&qqt,quoted.s,quoted.len);
  qmail_puts(&qqt,">\n");
  qmail_puts(&qqt,"Subject: success notice\n\
\n\
Hi! This is the qreceipt program. Your message was delivered to the\n\
following address: ");
  qmail_puts(&qqt,target);
  qmail_puts(&qqt,". Thanks for asking.\n");
  if (messageid.s) {
    qmail_puts(&qqt,"Your ");
    qmail_put(&qqt,messageid.s,messageid.len);
  }

  qmail_from(&qqt,"");
  qmail_to(&qqt,returnpath);
  qqx = qmail_close(&qqt);

  if (*qqx) {
   if (*qqx == 'D') errint(errno,"permanent qmail-queue error");
   else errint(errno,"temporary qmail-queue error");
  }
}

stralloc hfbuf = {0};
token822_alloc hfin = {0};
token822_alloc hfrewrite = {0};
token822_alloc hfaddr = {0};

void doheaderfield(stralloc *h) {
  switch(hfield_known(h->s,h->len)) {
  case H_MESSAGEID:
    if (!stralloc_copy(&messageid,h)) errmem;
      break;
  case H_NOTICEREQUESTEDUPONDELIVERYTO:
    doordie(h,token822_parse(&hfin,h,&hfbuf));
    doordie(h,token822_addrlist(&hfrewrite,&hfaddr,&hfin,rwnotice));
    break;
  }
}

void dobody(stralloc *h) { ; }

int main(int argc,char **argv) {
  sig_pipeignore();
  if (!(target = argv[1])) die_usage();
  if (!(returnpath = env_get("SENDER"))) die_usage();
  if (headerbody(buffer_0,doheaderfield,finishheader,dobody) == -1) die_read();
  _exit(0);
}
