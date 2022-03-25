tsc ../src_web/load_map.ts

if [[ -z ${1+x} ]]; then
	rm -R ./webpage
	mkdir webpage
fi

cp ../src_web/load_map.js webpage
cp ../src_web/load_map.html webpage

if [[ -z ${1+x} || "${1}" == "2" ]]; then
	mkdir build_map
	mkdir build_demo

	function build_features() {
		(
			cd build_"$1"
			emcmake cmake ../.. -DCMAKE_BUILD_TYPE=Release -DVIDEORECORDER=OFF -DVULKAN=OFF -DSERVER=OFF -DTOOLS=OFF -DPREFER_BUNDLED_LIBS=ON -DMAP_RENDERER=${2} -DDEMO_RENDERER=${3} -DDEV=ON
			cmake --build . --parallel
		)
	}

	build_features map ON OFF &
	build_features demo OFF ON &

	wait

	function copy_build() {
		cp build_"$1"/DDNet_"$1".worker.js webpage
		cp build_"$1"/DDNet_"$1".js webpage
		cp build_"$1"/DDNet_"$1".wasm webpage
		cp build_"$1"/DDNet_"$1".data webpage
	}

	copy_build demo &
	copy_build map &

	wait
fi

echo "done"
