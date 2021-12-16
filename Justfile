# Justfile

ota_step_1:
	PLATFORMIO_UPLOAD_FLAGS="--auth=$(cat ota_password.txt)" pio run -e d1_mini_lite_otaonly -t upload
ota_step_2:
	PLATFORMIO_UPLOAD_FLAGS="--auth=$(cat ota_password.txt)" pio run -e d1_mini_lite_ota     -t upload
full_ota:
	just ota_step_1
	sleep 2
	# wait for it to connect to the network again
	while ! ping -c 1 -w 1 mug.local; do sleep 2; done
	sleep 2
	just ota_step_2

get_ota_flags:
	#!/bin/bash
	echo -n "--auth=$(cat ota_password.txt)"
