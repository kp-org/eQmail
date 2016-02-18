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

#rm -R QMAIL/boot
#rm -R QMAIL/doc
#rm -R QMAIL/cat
