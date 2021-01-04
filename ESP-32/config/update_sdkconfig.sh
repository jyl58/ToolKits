#!/bin/sh
current_path=$(pwd)

if [ ! -f "$current_path"/../sdkconfig ]; then
	echo "cp a new sdkconfig to current path"
	cp "$ADF_PATH"/../../config/sdkconfig "$current_path"/../

else
	diff=$(diff "$current_path"/../sdkconfig  "$ADF_PATH"/../../config/sdkconfig)
	if [ ! -n "$diff" ]; then
		echo "sdkconfig is same with default."
	else
		echo "sdkconfig is different with default,so cp a new one."
		cp "$ADF_PATH"/../../config/sdkconfig "$current_path"/../
	fi	
fi
