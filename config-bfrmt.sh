
# create config file for qmail-bfrmt

[ -f QMAIL/control/beforemote ] || \
( echo "Creating config file beforemote..."
printf "#!/bin/sh
# config file for qmail-bfrmt\n#\n
# Enter commands here, like

export DKREMOTE=\"\$BINDIR/qmail-remote\"
np=\"\$BINDIR/qmail-remote\" ; exec \$np \"\$@\"
"       >> QMAIL/control/beforemote
chown root:qmail QMAIL/control/beforemote
chmod 644 QMAIL/control/beforemote     )
