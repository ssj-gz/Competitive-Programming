#! /bin/bash

if [ "$#" -ne "1" ] && [ "$#" -ne "2" ] ; then
    echo "Blah"
    exit 1
fi

SOURCE_FILENAME=$1
if [ "$#" -eq "2" ]; then
    shift
    DEST_FILENAME=$1
else
    DEST_FILENAME=$SOURCE_FILENAME
fi

echo "SOURCE_FILENAME: $SOURCE_FILENAME DEST_FILENAME: $DEST_FILENAME"

rsync -avi --progress "$SOURCE_FILENAME"  vps2.etotheipiplusone.com:/home/simon/public_html/codechef/$DEST_FILENAME

echo "http://vps2.etotheipiplusone.com:30176/public_html/codechef/$DEST_FILENAME"

