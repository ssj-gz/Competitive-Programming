#! /bin/bash

# Unfortunately, imagemagick takes tons of time and RAM to convert
# (say) 1000 pngs into a gif; split into smaller batches and combine
# at the end instead.

export BATCH_SIZE=100

new_batch_countdown=0
dir_number=0
dir_name=""

for i in $(seq -f "%05g" 0 99999); do
  echo $i
  if [ ! -e "MoveCoins2Editorial_4_collect_and_propagate_along_node_chain_left_to_right_optimised_$i.png" ]; then
    break
  fi
  if [ ${new_batch_countdown} -eq 0 ]; then
      printf -v dir_name "%05d" $dir_number
      mkdir "${dir_name}"
      new_batch_countdown=${BATCH_SIZE}
      dir_number=$(($dir_number + 1))
  fi
  cp -v "MoveCoins2Editorial_4_collect_and_propagate_along_node_chain_left_to_right_optimised_$i.png" ${dir_name}
  new_batch_countdown=$(($new_batch_countdown - 1))
  echo "new_batch_countdown: ${new_batch_countdown}"
done

for dir_name in $(seq -f "%05g" 0 99999); do
  if [ ! -d ${dir_name} ]; then
    echo "Dir ${dir_name} not found"
    break
  fi

  cd ${dir_name}
  convert *.png -layers OptimizeFrame -monitor "../${dir_name}.gif"
  cd -
done

mkdir final
convert *.gif final/final.gif


