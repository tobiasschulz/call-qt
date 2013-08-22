#!/bin/bash

function newversion() {
	# version stuff
	export $(grep "VERSION =" voipcall-qt.pro | sed 's@ = @=@gm' | sed 's@#.*@@gm')
	export VERSION=$[$(echo $VERSION | sed 's@0\.@@gm')+1]
	cat voipcall-qt.pro > .voipcall-qt.pro
	grep -v VERSION .voipcall-qt.pro > voipcall-qt.pro
	cat >> voipcall-qt.pro << EOT
# The application version                                                          ## VERSION
VERSION = 0.$VERSION                                                                      ## VERSION
# Define the preprocessor macro to get the application version in our application. ## VERSION
DEFINES += APP_VERSION=\\\\\"\$\$VERSION\\\\\"                                           ## VERSION
DEFINES += APP_BUILD=\\\\\"\$VERSION\\\\\"                                             ## VERSION
EOT
	touch src/version.cpp
}

function compile() {
	# compile
	make -j4
}

function debug() {
	gdb -ex run ./call-qt
}

function run() {
	./call-qt
}

function commit() {
	find src/ -exec git add '{}' \;
	git commit -a
	git push origin master
}


if [ "x$1" = "xgdb" ] || [ "x$1" = "xdebug" ]
then
	newversion
	compile
	debug
elif [ "x$1" = "xcommit" ]
then
	compile
	commit
else
	newversion
	compile
	run
fi
