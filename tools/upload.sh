#!/bin/bash
PI_HOST=$1
PI_PORT=$2
PI_PASS=$3
PI_FILE=$4

if [ $PI_PORT -eq 8266 ]; then
	PI_PORT=22
fi

if [ $PI_PASS == "no_pwd_found_in_code" ]; then
	scp -P $PI_PORT $PI_FILE root@$PI_HOST:/tmp/sketch.bin
	ssh -p $PI_PORT root@$PI_HOST '/usr/local/bin/merge-sketch-with-bootloader.lua'
	ssh -p $PI_PORT root@$PI_HOST '/usr/local/bin/run-avrdude'
else
	expect -c "spawn scp -P $PI_PORT $PI_FILE root@$PI_HOST:/tmp/sketch.bin; expect \"password:\"; send \"$PI_PASS\\r\"; interact"
	expect -c "spawn ssh -p $PI_PORT root@$PI_HOST /usr/local/bin/merge-sketch-with-bootloader.lua; expect \"password:\"; send \"$PI_PASS\\r\"; interact"
	expect -c "spawn ssh -p $PI_PORT root@$PI_HOST /usr/local/bin/run-avrdude; expect \"password:\"; send \"$PI_PASS\\r\"; interact"
fi
