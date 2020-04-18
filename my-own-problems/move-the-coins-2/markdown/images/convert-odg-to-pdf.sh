#! /bin/bash

ODG_FILENAME=$1
PERCENTAGE_SIZE=18
FILE_BASENAME=$(basename ${ODG_FILENAME} .odg)

soffice --headless --convert-to pdf ${ODG_FILENAME}
convert -density 400 ${FILE_BASENAME}.pdf -resize 20% ${FILE_BASENAME}.png
rm ${FILE_BASENAME}.pdf

