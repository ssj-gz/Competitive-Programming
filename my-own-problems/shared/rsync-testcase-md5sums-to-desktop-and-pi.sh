#! /bin/bash

GIT_ROOT='/home/simon/tmp/DONTSYNC/hackerrank/'
CURRENT_DIR="$(pwd)"
GIT_ROOT_RELATIVE_DIR=${CURRENT_DIR#${GIT_ROOT}}
echo "GIT_ROOT_RELATIVE_DIR: ${GIT_ROOT_RELATIVE_DIR}"

rsync -avi --progress md5sums.txt desktop:/home/simon/devel/hackerrank/${GIT_ROOT_RELATIVE_DIR}
rsync -avi --progress md5sums.txt raspberrypi:/home/pi/devel/hackerrank/${GIT_ROOT_RELATIVE_DIR}
