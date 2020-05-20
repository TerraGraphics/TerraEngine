#!/usr/bin/env sh

for directory in platforms core middleware editor ""
do
	number=$(scc --ci --no-cocomo --no-complexity --include-ext=cpp,c,h,hpp ./$directory | grep Total | awk '{print $NF}')
	name=$directory
	if test -z "$directory"
	then
		name="TOTAL"
	fi

	printf '%10s: %d\n' $name $number
done
