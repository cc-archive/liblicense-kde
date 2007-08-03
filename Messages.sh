#! /bin/sh
#$EXTRACTRC `find . -name \*.rc` >> rc.cpp || exit 11
$EXTRACTRC `find . -name \*.ui` >> rc.cpp || exit 11
$XGETTEXT `find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C` -o $podir/liblicense.pot
rm -f rc.cpp

