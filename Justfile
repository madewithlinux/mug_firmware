# Justfile

generate_html_tempaltes:
	python3 web/transform_template.py web/*.html > src/generated/html_templates.inc

wait_for_connection:
	while ! ping -c 1 -w 1 mug.local; do sleep 2; done

do_ota ENV: wait_for_connection
	PLATFORMIO_UPLOAD_FLAGS="--auth=$(cat ota_password.txt)" pio run -e {{ENV}} -t upload

ota: generate_html_tempaltes
	just do_ota d1_mini_ota

full_ota:
	just do_ota d1_mini_otaonly
	just do_ota d1_mini_ota

get_ota_flags:
	#!/bin/bash
	echo -n "--auth=$(cat ota_password.txt)"


build_and_copy_data_files:
	cd webui && rm -rf dist/
	cd webui && npm run build
	rm -rf data/www
	mkdir -p data/www
	cp -r webui/dist/* data/www
	rm -f data/www/*.map
	# TZ=GMT date '+%a, %d %b %Y %H:%M:%S GMT' |tr -d '\n' > data/last-modified

gzip_data_files:
	#!/bin/bash
	set -euxo pipefail
	for localpath in $(find data/www/ -maxdepth 1 -type f); do
		gzip -9 -f < "${localpath}" > "${localpath}.gz"
	done

upload_data_files:
	#!/bin/bash
	set -euxo pipefail
	upload_data_files_curl_args() {
		set +x
		for localpath in $(find data/www/ -maxdepth 1 -type f); do
			destpath=$(echo ${localpath} |sed -n 's|^data/www/||p')
			echo "-F" "${destpath}=@${localpath}"
		done
	}
	upload_data_files_curl_args
	set -x
	curl -vvv -w '\n' \
		$(upload_data_files_curl_args) \
		http://192.168.1.173/api/replace_static_files

data_files: build_and_copy_data_files gzip_data_files upload_data_files
