#!/bin/bash

# Yes or no func 
yon(){
	read -p $'::: '"$* (Y/n): " -e res
	res=${res,,}
	case $res in
		'' | [Yy]*) return 0 ;;
		*) return 1 ;;
	esac
}

yon "Build glfw?" && {
	# Build glfw
	cd lib/glfw
	cmake -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF -DGLFW_BUILD_DOCS=OFF .
	make
}

yon "Append to LD_LIBRARY_PATH?" && {
	printenv LD_LIBRARY_PATH
	export LD_LIBRARY_PATH="$PWD/engine/build/lib/:$LD_LIBRARY_PATH"
	printenv LD_LIBRARY_PATH
}

