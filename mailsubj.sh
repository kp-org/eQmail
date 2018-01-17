subject="$1"
shift
( echo Subject: "$subject"
  echo To: ${1+"$@"}
  echo ''
  cat
) | QPRFX/bin/qmail-inject
