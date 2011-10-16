#!/bin/bash

URL="http://www.codon.org.uk/~mjg59/libx86/downloads/libx86-1.1.tar.gz"
NAME=$(basename "$URL")
DIRNAME=$(basename "$URL" .tar.gz)
NEWNAME=libx86

if [ -e $NEWNAME ] && [ "$1" !=  "-force" ] ; then
	echo "Already fetched."
	exit 0
fi

#echo $NAME
#echo $DIRNAME

rm -rf $NEWNAME

wget -q "$URL"
tar xzf $NAME
rm -f $NAME
mv $DIRNAME $NEWNAME

cat >> $NEWNAME/x86-common.h <<EOF
#define IF_MASK X86_EFLAGS_IF
#define IOPL_MASK X86_EFLAGS_IOPL
#define VIF_MASK X86_EFLAGS_VIF
#define TF_MASK X86_EFLAGS_TF
EOF
