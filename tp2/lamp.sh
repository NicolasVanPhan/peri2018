#!/bin/bash

while true
do
	toto=`dd bs=1 count=1 < /dev/ledbp 2> /dev/null`;
	if [ $toto -eq '1' ]
	then
		echo '1' > /dev/ledbp;
	else
		echo '0' > /dev/ledbp;
	fi
done
