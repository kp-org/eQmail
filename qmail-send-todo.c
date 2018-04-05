
/* excluded from qmail-send.c */
/* this file is too long ---------------------------------------------- TODO */

datetime_sec nexttodorun;
int flagtododir = 0; /* if 0, have to readsubdir_init again */
readsubdir todosubdir;
stralloc todoline = {0};
char todobuf[BUFFER_INSIZE];
char todobufinfo[512];
char todobufchan[CHANNELS][1024];

void todo_init()
{
 flagtododir = 0;
 nexttodorun = now();
 trigger_set();
}

void todo_selprep(nfds,rfds,wakeup)
int *nfds;
fd_set *rfds;
datetime_sec *wakeup;
{
 if (flagexitasap) return;
 trigger_selprep(nfds,rfds);
 if (flagtododir) *wakeup = 0;
 if (*wakeup > nexttodorun) *wakeup = nexttodorun;
}

void todo_do(fd_set *rfds)
{
  struct stat st;
  buffer ss;
  int fd;
  buffer ssinfo; int fdinfo;
  buffer sschan[CHANNELS];
  int fdchan[CHANNELS];
  int flagchan[CHANNELS];
  struct prioq_elt pe;
  char ch;
  int match;
  unsigned long id;
// int z;
  int c;
  unsigned long uid;
  unsigned long pid;

 fd = -1;
 fdinfo = -1;
 for (c = 0;c < CHANNELS;++c) fdchan[c] = -1;

 if (flagexitasap) return;

 if (!flagtododir)
  {
   if (!trigger_pulled(rfds))
     if (recent < nexttodorun)
       return;
   trigger_set();
   readsubdir_init(&todosubdir, "todo", pausedir);
   flagtododir = 1;
   nexttodorun = recent + SLEEP_TODO;
  }

 switch(readsubdir_next(&todosubdir, &id))
  {
    case 1:
      break;
    case 0:
      flagtododir = 0;
    default:
      return;
  }

 fnmake_todo(id);

 fd = open_read(fn.s);
 if (fd == -1) { log(B("warning: unable to open ",fn.s,"\n")); return; }

 fnmake_mess(id);
 /* just for the statistics */
 if (stat(fn.s,&st) == -1)
  { log(B("warning: unable to stat ",fn.s,"\n")); goto fail; }

 for (c = 0;c < CHANNELS;++c)
  {
   fnmake_chanaddr(id,c);
   if (unlink(fn.s) == -1) if (errno != error_noent)
    { log(B("warning: unable to unlink ",fn.s,"\n")); goto fail; }
  }

 fnmake_info(id);
 if (unlink(fn.s) == -1) if (errno != error_noent)
  { log(B("warning: unable to unlink ",fn.s,"\n")); goto fail; }

 fdinfo = open_excl(fn.s);
 if (fdinfo == -1)
  { log(B("warning: unable to create ",fn.s,"\n")); goto fail; }

 strnum3[fmt_ulong(strnum3,id)] = 0;
 log(B("new msg ",strnum3,"\n"));

 for (c = 0;c < CHANNELS;++c) flagchan[c] = 0;

  buffer_init(&ss,read,fd,todobuf,sizeof(todobuf));
  buffer_init(&ssinfo,write,fdinfo,todobufinfo,sizeof(todobufinfo));

 uid = 0;
 pid = 0;

 for (;;)
  {
   if (getln(&ss,&todoline,&match,'\0') == -1)
    {
     /* perhaps we're out of memory, perhaps an I/O error */
     fnmake_todo(id);
     log(B("warning: trouble reading ",fn.s,"\n")); goto fail;
    }
   if (!match) break;

   switch(todoline.s[0])
    {
     case 'u':
       scan_ulong(todoline.s + 1,&uid);
       break;
     case 'p':
       scan_ulong(todoline.s + 1,&pid);
       break;
     case 'F':
       if (buffer_putflush(&ssinfo,todoline.s,todoline.len) == -1)
	{
	 fnmake_info(id);
         log(B("warning: trouble writing to ",fn.s,"\n")); goto fail;
	}
       log(B("info msg ",strnum3));
       strnum2[fmt_ulong(strnum2,(unsigned long) st.st_size)] = 0;
       log(B(": bytes ",strnum2));
       log(" from <"); logsafe(todoline.s + 1);
       strnum2[fmt_ulong(strnum2,pid)] = 0;
       log(B("> qp ",strnum2));
       strnum2[fmt_ulong(strnum2,uid)] = 0;
       log(B(" uid ",strnum2));
       log("\n");
       break;
     case 'T':
       switch(rewrite(todoline.s + 1))
    {
	 case 0: nomem(); goto fail;
	 case 2: c = 1; break;
	 default: c = 0; break;
        }
       if (fdchan[c] == -1)
    {
	 fnmake_chanaddr(id,c);
	 fdchan[c] = open_excl(fn.s);
	 if (fdchan[c] == -1)
          { log(B("warning: unable to create ",fn.s,"\n")); goto fail; }
	 buffer_init(&sschan[c]
	   ,write,fdchan[c],todobufchan[c],sizeof(todobufchan[c]));
	 flagchan[c] = 1;
	}
       if (buffer_put(&sschan[c],rwline.s,rwline.len) == -1)
        {
     fnmake_chanaddr(id,c);
         log(B("warning: trouble writing to ",fn.s,"\n")); goto fail;
        }
       break;
     default:
       fnmake_todo(id);
       log(B("warning: unknown record type in ",fn.s,"\n")); goto fail;
    }
  }

 close(fd); fd = -1;

  fnmake_info(id);
  if (buffer_flush(&ssinfo) == -1)
    { log(B("warning: trouble writing to ",fn.s,"\n")); goto fail; }
  if (fsync(fdinfo) == -1)
    { log(B("warning: trouble fsyncing ",fn.s,"\n")); goto fail; }
  close(fdinfo); fdinfo = -1;

 for (c = 0;c < CHANNELS;++c)
   if (fdchan[c] != -1)
    {
     fnmake_chanaddr(id,c);
     if (buffer_flush(&sschan[c]) == -1)
      { log(B("warning: trouble writing to ",fn.s,"\n")); goto fail; }
     if (fsync(fdchan[c]) == -1)
      { log(B("warning: trouble fsyncing ",fn.s,"\n")); goto fail; }
     close(fdchan[c]); fdchan[c] = -1;
    }

 fnmake_todo(id);
 if (buffer_putflush(&sstoqc,fn.s,fn.len) == -1) { cleandied(); return; }
 if (buffer_get(&ssfromqc,&ch,1) != 1) { cleandied(); return; }
 if (ch != '+')
  {
   log(B("warning: qmail-clean unable to clean up ",fn.s,"\n"));
   return;
  }

 pe.id = id; pe.dt = now();
 for (c = 0;c < CHANNELS;++c)
   if (flagchan[c])
     while (!prioq_insert(&pqchan[c],&pe)) nomem();

 for (c = 0;c < CHANNELS;++c) if (flagchan[c]) break;
 if (c == CHANNELS)
   while (!prioq_insert(&pqdone,&pe)) nomem();

 return;

 fail:
 if (fd != -1) close(fd);
 if (fdinfo != -1) close(fdinfo);
 for (c = 0;c < CHANNELS;++c)
   if (fdchan[c] != -1) close(fdchan[c]);
}
