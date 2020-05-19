#! /bin/bash

ODG_FILENAME=$1
PERCENTAGE_SIZE=20
FILE_BASENAME=$(basename ${ODG_FILENAME} .odg)

soffice --headless --convert-to pdf ${ODG_FILENAME}
convert -density 400 ${FILE_BASENAME}.pdf -resize ${PERCENTAGE_SIZE}% ${FILE_BASENAME}.png
rm ${FILE_BASENAME}.pdf

