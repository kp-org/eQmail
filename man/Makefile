# Don't edit Makefile! Use conf-* for configuration.

SHELL=/bin/sh
QPRFX = `head -1 ../autocf-qprfx`
BREAK = `head -1 ../autocf-break`
SPAWN = `head -1 ../autocf-spawn`

# create man pages

default: man

clean:
	@echo -n Cleaning up man pages ...
	@rm -f 5/dot-qmail.5 8/qmail-bfrmt.8 5/qmail-control.5 8/qmail-getpw.8 \
	7/qmail-limits.7 8/qmail-newmrh.8 8/qmail-pw2u.8 8/qmail-send.8 \
	8/qmail-start.8 5/qmail-users.5 5/smtpplugins.5
	@echo  done!

dot-qmail.5: dot-qmail.man
	cat dot-qmail.man | sed s}BREAK}"$(BREAK)"}g > 5/dot-qmail.5

man: dot-qmail.5 qmail-bfrmt.8 qmail-control.5 qmail-getpw.8 qmail-limits.7 \
qmail-newmrh.8 qmail-newu.8 qmail-pw2u.8 qmail-send.8 qmail-start.8 qmail-users.5 \
smtpplugins.5

qmail-bfrmt.8: qmail-bfrmt.man
	cat qmail-bfrmt.man | sed s}QPRFX}"$(QPRFX)"}g > 8/qmail-bfrmt.8

qmail-control.5: qmail-control.man
	cat qmail-control.man | sed s}QPRFX}"$(QPRFX)"}g > 5/qmail-control.5

qmail-getpw.8: qmail-getpw.man
	cat qmail-getpw.man | sed s}BREAK}"$(BREAK)"}g > 8/qmail-getpw.8

qmail-limits.7: qmail-limits.man
	cat qmail-limits.man | sed s}SPAWN}"$(SPAWN)"}g > 7/qmail-limits.7

qmail-newmrh.8: qmail-newmrh.man
	cat qmail-newmrh.man | sed s}QPRFX}"$(QPRFX)"}g > 8/qmail-newmrh.8

qmail-newu.8: qmail-newu.man
	cat qmail-newu.man | sed s}QPRFX}"$(QPRFX)"}g > 8/qmail-newu.8

qmail-pw2u.8: qmail-pw2u.man
	cat qmail-pw2u.man | sed s}QPRFX}"$(QPRFX)"}g \
	| sed s}BREAK}"$(BREAK)"}g > 8/qmail-pw2u.8

qmail-send.8: qmail-send.man
	cat qmail-send.man | sed s}BREAK}"$(BREAK)"}g \
	| sed s}SPAWN}"$(SPAWN)"}g > 8/qmail-send.8

qmail-start.8: qmail-start.man
	cat qmail-start.man | sed s}QPRFX}"$(QPRFX)"}g > 8/qmail-start.8

qmail-users.5: qmail-users.man
	cat qmail-users.man | sed s}QPRFX}"$(QPRFX)"}g > 5/qmail-users.5

smtpplugins.5: smtpplugins.man
	cat smtpplugins.man | sed s}QPRFX}"$(QPRFX)"}g > 5/smtpplugins.5
