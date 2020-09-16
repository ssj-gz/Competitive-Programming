#! /bin/bash

for i in 1 2 3 4 5 6 7; do 
    mkdir $i 
    cp *_${i}_*.png $i
    cd $i
    ../../../../../convert-pngs-to-gif.sh
    cd -
    mv $i/final/final.gif MOVCOIN2_ED_${i}.gif
done 
mv MOV*.gif ../../../../

