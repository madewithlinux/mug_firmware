# Justfile

wait_for_connection:
	while ! ping -c 1 -w 1 mug.local; do sleep 2; done

do_ota ENV: wait_for_connection
	PLATFORMIO_UPLOAD_FLAGS="--auth=$(cat ota_password.txt)" pio run -e {{ENV}} -t upload

ota:
	just do_ota d1_mini_ota

full_ota:
	just do_ota d1_mini_otaonly
	just do_ota d1_mini_ota

get_ota_flags:
	#!/bin/bash
	echo -n "--auth=$(cat ota_password.txt)"
