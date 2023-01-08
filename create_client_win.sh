#!/bin/bash

rm -R build_win
mkdir build_win
(
	cd build_win
	cmake .. -GNinja -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchains/mingw64.toolchain -DAUTOUPDATE=ON -DDOWNLOAD_GTEST=OFF
	ninja package_zip
)

rm -R website
mkdir website

cp build_win/config_directory.bat website
cp build_win/config_directory.sh website

function cp_and_mkdir() {
	if [ ! -d "../website/$(dirname $1)" ]; then
		mkdir -p ../website/$(dirname $1)
	fi
	cp "$1" ../website/$(dirname $1)
	DIR_FILE=$1
	echo "\"${DIR_FILE:2}\"," >> ../website/update.json
}
export -f cp_and_mkdir

function cp_dll() {
	cp "$1" ../website/
	echo "\"${1:2}\"," >> ../website/update.json
}
export -f cp_dll

echo '
[
  {
    "version": "16.2",
    "client": true,
    "server": true,
    "download": [
      "storage.cfg",
      "config_directory.bat",
      "config_directory.sh",' > website/update.json

(
	cd build_win || exit 1
	find ./data -name \* -type f -exec bash -c 'cp_and_mkdir "$0"' {} \;
	find . -maxdepth 1 -name \*.dll -type f -exec bash -c 'cp_dll "$0"' {} \;
)

echo '
    ]
  }
]' >> website/update.json

# copy the .exe
cp build_win/DDNet.exe website/DDNet-win64-x86_64.exe
cp build_win/DDNet-Server.exe website/DDNet-Server-win64-x86_64.exe

