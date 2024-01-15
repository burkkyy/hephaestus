#!/bin/bash

ADD_PATH="export LD_LIBRARY_PATH="$PWD/engine/build/lib/:$LD_LIBRARY_PATH""
REMOVE_PATH="export LD_LIBRARY_PATH="""

echo $ADD_PATH
eval "$ADD_PATH"

# Yes or no func 
yon(){
	read -p "$* (Y/n): " -e res
	res=${res,,}
	case $res in
		'' | [Yy]*) return 0 ;;
		*) return 1 ;;
	esac
}

yon "Clear LD_LIBRARY_PATH?" && {
	echo $REMOVE_PATH
	eval "$REMOVE_PATH"
}

