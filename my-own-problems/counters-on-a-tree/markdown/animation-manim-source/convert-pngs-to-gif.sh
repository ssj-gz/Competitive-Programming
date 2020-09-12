#! /bin/bash

# Unfortunately, imagemagick takes tons of time and RAM to convert
# (say) 1000 pngs into a gif; split into smaller batches and combine
# at the end instead.

export BATCH_SIZE=100

new_batch_countdown=0
dir_number=0
dir_name=""

first_png_filename=$(ls -1 *00000*.png)

for i in $(seq -f "%05g" 0 99999); do
  echo $i
  png_filename="${first_png_filename/00000/$i}"
  echo "png_filename: ${png_filename}"
  if [ ! -e "${png_filename}" ]; then
    break
  fi
  if [ ${new_batch_countdown} -eq 0 ]; then
      printf -v dir_name "%05d" $dir_number
      mkdir "${dir_name}"
      new_batch_countdown=${BATCH_SIZE}
      dir_number=$(($dir_number + 1))
  fi
  cp -v "${png_filename}" ${dir_name}
  new_batch_countdown=$(($new_batch_countdown - 1))
  echo "new_batch_countdown: ${new_batch_countdown}"
done

for dir_name in $(seq -f "%05g" 0 99999); do
  if [ ! -d ${dir_name} ]; then
    echo "Dir ${dir_name} not found"
    break
  fi

  cd ${dir_name}
  convert *.png -layers OptimizeFrame -monitor -delay 10 "../${dir_name}.gif" # 10 fps.
  cd -
done

mkdir final
convert *.gif final/final.gif


