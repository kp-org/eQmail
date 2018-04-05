
/* excluded from qmail-send.c */
/* this file is too long ------------------------------------------- BOUNCES */

char *stripvdomprepend(char *recip)
//char *recip;
{
  int i;
  char *domain;
  int domainlen;
  char *prepend;

  i = str_rchr(recip,'@');
  if (!recip[i]) return recip;
  domain = recip + i + 1;
  domainlen = str_len(domain);

  for (i = 0;i <= domainlen;++i)
    if ((i == 0) || (i == domainlen) || (domain[i] == '.'))
      if ((prepend = constmap(&mapvdoms,domain + i,domainlen - i)))
      {
        if (!*prepend) break;
        i = str_len(prepend);
        if (str_diffn(recip,prepend,i)) break;
        if (recip[i] != '-') break;
        return recip + i + 1;
      }
  return recip;
}

stralloc bouncetext = {0};

void addbounce(unsigned long id,char *recip,char *report)
//unsigned long id;
//char *recip;
//char *report;
{
  int fd;
  int pos;
  int w;
  while (!stralloc_copys(&bouncetext,"<")) nomem();
  while (!stralloc_cats(&bouncetext,stripvdomprepend(recip))) nomem();
  for (pos = 0;pos < bouncetext.len;++pos)
    if (bouncetext.s[pos] == '\n')
       bouncetext.s[pos] = '_';
  while (!stralloc_cats(&bouncetext,">:\n")) nomem();
  while (!stralloc_cats(&bouncetext,report)) nomem();
  if (report[0])
    if (report[str_len(report) - 1] != '\n')
      while (!stralloc_cats(&bouncetext,"\n")) nomem();
  for (pos = bouncetext.len - 2;pos > 0;--pos)
    if (bouncetext.s[pos] == '\n')
      if (bouncetext.s[pos - 1] == '\n')
        bouncetext.s[pos] = '/';
  while (!stralloc_cats(&bouncetext,"\n")) nomem();
  fnmake2_bounce(id);
  for (;;) {
    fd = open_append(fn2.s);
    if (fd != -1) break;
    log("alert: unable to append to bounce message; HELP! sleeping...\n");
    sleep(10);
  }
  pos = 0;
  while (pos < bouncetext.len)
  {
    w = write(fd,bouncetext.s + pos,bouncetext.len - pos);
    if (w <= 0)
    {
      log("alert: unable to append to bounce message; HELP! sleeping...\n");
      sleep(10);
    }
    else
      pos += w;
  }
  close(fd);
}

int injectbounce(unsigned long id) {
  struct qmail qqt;
  struct stat st;
  char *bouncesender;
  char *bouncerecip;
  int r;
  int fd;
//  substdio ssread;
  buffer bread;
  char buf[128];
  char inbuf[128];
  static stralloc sender = {0};
  static stralloc quoted = {0};
  datetime_sec birth;
  unsigned long qp;

  if (!getinfo(&sender,&birth,id)) return 0; /* XXX: print warning */

 /* owner-@host-@[] -> owner-@host */

  if (sender.len >= 5)
    if (str_equal(sender.s + sender.len - 5,"-@[]"))
    {
      sender.len -= 4;
      sender.s[sender.len - 1] = 0;
    }

  fnmake2_bounce(id);
  fnmake_mess(id);
  if (stat(fn2.s,&st) == -1) {
//   if (errno == error_noent)
    if (errno == ENOENT)
      return 1;
    log(B("warning: unable to stat ",fn2.s,"\n"));
    return 0;
  }
  if (str_equal(sender.s,"#@[]"))
    log(B("triple bounce: discarding ",fn2.s,"\n"));
  else
  {
    if (qmail_open(&qqt) == -1)
    { log("warning: unable to start qmail-queue, will try later\n"); return 0; }
    qp = qmail_qp(&qqt);

    if (*sender.s) { bouncesender = ""; bouncerecip = sender.s; }
    else { bouncesender = "#@[]"; bouncerecip = doublebounceto.s; }

    while (!newfield_datemake(now())) nomem();
    qmail_put(&qqt,newfield_date.s,newfield_date.len);
    qmail_puts(&qqt,"From: ");
    while (!quote(&quoted,&bouncefrom)) nomem();
    qmail_put(&qqt,quoted.s,quoted.len);
    qmail_puts(&qqt,"@");
    qmail_put(&qqt,bouncehost.s,bouncehost.len);
    qmail_puts(&qqt,"\nTo: ");
    while (!quote2(&quoted,bouncerecip)) nomem();
    qmail_put(&qqt,quoted.s,quoted.len);
    qmail_puts(&qqt,"\n\
Subject: failure notice\n\
\n\
Hi. This is the qmail-send program at ");
   qmail_put(&qqt,bouncehost.s,bouncehost.len);
   qmail_puts(&qqt,*sender.s ? ".\n\
I'm afraid I wasn't able to deliver your message to the following addresses.\n\
This is a permanent error; I've given up. Sorry it didn't work out.\n\
\n\
" : ".\n\
I tried to deliver a bounce message to this address, but the bounce bounced!\n\
\n\
");

    fd = open_read(fn2.s);
    if (fd == -1)
      qmail_fail(&qqt);
    else
    {
      buffer_init(&bread,read,fd,inbuf,sizeof(inbuf));
      while ((r = buffer_get(&bread,buf,sizeof(buf))) > 0)
        qmail_put(&qqt,buf,r);
      close(fd);
      if (r == -1)
        qmail_fail(&qqt);
    }

    qmail_puts(&qqt,*sender.s ? "--- Below this line is a copy of the message.\n\n" : "--- Below this line is the original bounce.\n\n");
    qmail_puts(&qqt,"Return-Path: <");
    while (!quote2(&quoted,sender.s)) nomem();
    qmail_put(&qqt,quoted.s,quoted.len);
    qmail_puts(&qqt,">\n");

    fd = open_read(fn.s);
    if (fd == -1)
      qmail_fail(&qqt);
    else
    {
      buffer_init(&bread,read,fd,inbuf,sizeof(inbuf));
      while ((r = buffer_get(&bread,buf,sizeof(buf))) > 0)
        qmail_put(&qqt,buf,r);
      close(fd);
      if (r == -1)
        qmail_fail(&qqt);
    }

    qmail_from(&qqt,bouncesender);
    qmail_to(&qqt,bouncerecip);
    if (*qmail_close(&qqt))
      { log("warning: trouble injecting bounce message, will try later\n"); return 0; }

#include "qstrings.h"
//   strnum2[fmt_ulong(strnum2,id)] = 0;
//   o_log2("bounce msg ",strnum2);
    log(B("bounce msg ",fmtnum(id)));
//   strnum2[fmt_ulong(strnum2,qp)] = 0;
    log(B(" qp ",fmtnum(qp),"\n"));
    }
   if (unlink(fn2.s) == -1) {
     log(B("warning: unable to unlink ",fn2.s,"\n"));
     return 0;
   }
   return 1;
}
