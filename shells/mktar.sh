#!/bin/sh
#-----------------------------------------------------------------------------
#
#	mktar.sh
#
#-----------------------------------------------------------------------------

PRODUCT=""
VERSION=""
PROC=DoTarDenis

Usage()
{
	exec >&2

	echo "Usage: mktar.sh -p product"
	exit 1
}

Error()
{
	echo "mktar.sh: $1" >&2
}

ErrorExit()
{
	echo "mktar.sh: $1" >&2
	exit 1
}

DoTar()
{
       echo " tar --exclude-vcs -hcf $ROOTACT/archives/${PRODUCT}-${VERSION}.tar $PRODUCT"
       tar --exclude-vcs -hcf $ROOTACT/archives/${PRODUCT}-${VERSION}.tar $PRODUCT && \
       gzip $ROOTACT/archives/${PRODUCT}-${VERSION}.tar
}

DoTarDenis()
{
        TGZ=$ROOTACT/archives/${PRODUCT}-${VERSION}.tgz
	what="tar --exclude=FDB_lora -czhf $TGZ $PRODUCT"
	echo ${what}
        ${what}
	#cp $TGZ $HOME/Denis/osx/Dropbox/archives_ACT
	echo "  cp $TGZ $HOME/Denis/osx/Google Drive/archives_ACT"
	cp $TGZ "$HOME/Denis/osx/Google Drive/archives_ACT"
	rm $TGZ
}


while [ $# -gt 0 ]
do
	case $1 in
		-p)	shift
			if [ $# = 0 ]
			then
				Usage
			fi
			PRODUCT=$1
			shift
		;;
	esac
done

if [ -z "$ROOTACT" ]
then
	Error "ROOTACT not specified"
	Usage
fi
if [ -z "$PRODUCT" ]
then
	Error "product name not specified"
	Usage
fi

mkdir -p $ROOTACT/archives > /dev/null 2>&1

# no cd otherwise it is impossible to do a "MAKE src" from another directory
#cd $ROOTACT/$PRODUCT || ErrorExit "ROOTACT/$PRODUCT does not exist"

if [ ! -f "./Version" ]
then
	ErrorExit "./Version file not found"
fi
VERSION=`cat ./Version`
DORM="0"
cd ..
if	[ ! -d $PRODUCT ]
then
	where=$(pwd)
	echo	"$PRODUCT does not exist in $where"
	if	[ ! -d ${PRODUCT}-${VERSION} ]
	then
		echo	"${PRODUCT}-${VERSION} does not exist in $where"
		exit 0
	fi
	echo "temporarily link ${PRODUCT}-${VERSION} to ${PRODUCT}"
	ln -s ${PRODUCT}-${VERSION} ${PRODUCT} || exit 0
	DORM="1"
fi


$PROC
if	[ "$DORM" != "0" ]
then
	rm ${PRODUCT}
fi


