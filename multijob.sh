#!/bin/bash

for j 
	do
	# $j has file 
	while IFS= read -r line
		do
       	 	./jobcommander issuejob $line
	done <"$j"
	done
echo
