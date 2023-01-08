#!/bin/bash

rm -R build_rel
mkdir build_rel
(
	cd build_rel
	cmake .. -GNinja
	ninja
)

