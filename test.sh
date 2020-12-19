#!/bin/bash

pow_max=13
base=2
out_file_name=test_out.txt
rm -f ./$out_file_name
# Test how data encrypts/decrypts with different block size
# Input files size are approximately the same as block size (-1..0..+1)
result=0
for i in $(seq 4 $pow_max); # start from 2^4 == 16
do
  let block_size=( $base**$i )
  for j in $(seq -1 1); # create file's with (block_size + -1..0..+1) bytes to ensure that program handles different size of input files
  do
    dd if=/dev/zero of=./input.bin bs=$((block_size + j)) count=1 > /dev/null 2>&1

    ./bin/ED-tool -e -i ./input.bin -o ./output.bin -k 01234567890123456789012345678901 -s $block_size > /dev/null 2>&1
    if (( "$?" > 0)); then
      echo "Encryption failed ! Block size: $block_size input file size : $((block_size+j))" >> ./$out_file_name
      result=-1
    fi

    ./bin/ED-tool -d -i ./output.bin -o ./output_decrypted.bin -k 01234567890123456789012345678901 -s $block_size > /dev/null 2>&1

    if (( "$?" > 0)); then
      echo "Decryption failed ! Block size: $block_size input file size : $((block_size+j))" >> ./$out_file_name
      result=-1
    fi
  done
done

# Encrypting / Decrypting one file_size using different block_size
for i in $(seq 4 $pow_max); # start from 2^4 == 16
do
  let file_size=( $base**$i )
  for j in $(seq 4 $pow_max); # increment through block size but with stable file size
  do
    let block_size=( $base**$j )
    dd if=/dev/zero of=./input.bin bs=$file_size count=1 > /dev/null 2>&1

    ./bin/ED-tool -e -i ./input.bin -o ./output.bin -k 01234567890123456789012345678901 -s $block_size > /dev/null 2>&1
    if (( "$?" > 0)); then
      echo "Encryption failed ! Block size: $block_size input file size : $file_size" >> ./$out_file_name
      result=-1
    fi

    ./bin/ED-tool -d -i ./output.bin -o ./output_decrypted.bin -k 01234567890123456789012345678901 -s $block_size > /dev/null 2>&1

    if (( "$?" > 0)); then
      echo "Decryption failed ! Block size: $block_size input file size : $file_size" >> ./$out_file_name
      result=-1
    fi
  done
done

# encrypting/decrypting 10mb file using different block size

  let file_size=10000000
  dd if=/dev/zero of=./input.bin bs=$file_size count=1 > /dev/null 2>&1
  for j in $(seq 4 $pow_max); # increment through block size but with stable file size
  do
    let block_size=( $base**$j )

    ./bin/ED-tool -e -i ./input.bin -o ./output.bin -k 01234567890123456789012345678901 -s $block_size > /dev/null 2>&1
    if (( "$?" > 0)); then
      echo "Encryption failed ! Block size: $block_size input file size : $file_size" >> ./$out_file_name
      result=-1
    fi

    ./bin/ED-tool -d -i ./output.bin -o ./output_decrypted.bin -k 01234567890123456789012345678901 -s $block_size > /dev/null 2>&1

    if (( "$?" > 0)); then
      echo "Decryption failed ! Block size: $block_size input file size : $file_size" >> ./$out_file_name
      result=-1
    fi
  done


rm -f input.bin output.bin output_decrypted.bin
if(("$result" == -1)); then
  echo "Test failed! Check $out_file_name for details"
fi