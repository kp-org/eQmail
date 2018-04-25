# eQmail

Initially eQmail was forked from netqmail by applying some patches and saving
the sources separatily. The goal was to have a source code package available,
which compiles out of the box on actual systems. Additional it should provide
important functionality like TLS and authentication to make *qmail a suitable
package. Not to forget full IPv6 support.

Since the first realease there were made some internal changes too.  The most
important one is the consolidation of 'qlibs'. It's an update of the original
environment from djb, including changes he mentioned in some TODO files.  The
'qlibs' are not part of the git repository, but included in the public eQmail
releases (available as tgz' at ftp.dyndn.es). The qlibs are available through
https://gitlab.com/kaili/qlibs as separate repository.

eQmail is positioned to be a modern but plain MTA.  Advanced features will be
provided by modules of the openqmail package (e.g. DKIM).  Some more features
are on the TODO list  after all internals are consolidated to an equal level.
As of its design it is simply to add additional functionality through plugins
and/or wrappers. All the stuff - eQmail, qlibs and openqmail - is under heavy
development so things will change.


See CHANGLOG* files for details. Additional info can be found at
  - https://openqmail.org
  - https://blog.dyndn.es
