#!/bin/sh

# nullmailer
ssh razor sendmail kp1@gorre.aldox.de << EOF
from:qmail-test@razor.aldox.de

test from razor
EOF
