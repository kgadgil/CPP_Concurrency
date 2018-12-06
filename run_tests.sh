#!/bin/bash

declare -a delay_pairs=("1024 1024" "512 1024" "256 1024" "128 1024" "64 1024" "32 1024" "16 1024" "8 1024" "4 1024" "2 1024" "1 1024")

filename=lockfree_75.txt
rm $filename

make clean
make
for delay in "${delay_pairs[@]}"
do
	#echo "$delay" &>> circ_25.txt
	./tut 1024 $delay &>> $filename
done

echo "Done"
