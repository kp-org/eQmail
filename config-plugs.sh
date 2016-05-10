
# create control/smtpplugins
[ -f QMAIL/control/smtpplugins ] || \
( echo "Creating empty smtpplugins config file..."
printf "
# config file for qmail-spp\n#\n
# pathes are relativ to QMAIL\n#\n" > QMAIL/control/smtpplugins
printf "[connection]\n\n[helo]\n\n[mail]\n\n[rcpt]\n\n[data]\n\n#[auth]\n" \
       >> QMAIL/control/smtpplugins
chown root:qmail QMAIL/control/smtpplugins
chmod 644 QMAIL/control/smtpplugins	)

mkdir -p QMAIL/bin/plugins
chgrp qmail QMAIL/bin/plugins

# create config file for qmail-bfque
[ -f QMAIL/control/beforequeue ] || \
( echo "Creating config file beforequeue..."
printf "#!/bin/sh
# config file for qmail-bfqueue\n#\n
# Enter commands here, like

export QUEUE=\"\$BINDIR/qmail-queue\"
np=\"\$BINDIR/qmail-queue\" ; exec \$np \"\$@\"
"       >> QMAIL/control/beforequeue
chown root:qmail QMAIL/control/beforequeue
chmod 644 QMAIL/control/beforequeue     )

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
