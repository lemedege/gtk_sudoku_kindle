#!/bin/bash
count=0
FILE=0
difficulty=intermediate

for i in $(seq -f "%02g" 1 99)
do
	file="${difficulty}$i.txt"
	qqwing --generate 1 --difficulty $difficulty --symmetry random --one-line >> $file
done

