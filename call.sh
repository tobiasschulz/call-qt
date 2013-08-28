#!/bin/bash

alias ll='ls -lh'

function newversion() {
	# version stuff
	export $(grep "VERSION =" call-qt.pro | sed 's@ = @=@gm' | sed 's@#.*@@gm')
	export VERSION=$[$(echo $VERSION | sed 's@0\.@@gm')+1]
	cat call-qt.pro > .call-qt.pro
	grep -v VERSION .call-qt.pro > call-qt.pro
	cat >> call-qt.pro << EOT
# The application version                                                          ## VERSION
VERSION = 0.$VERSION                                                                      ## VERSION
# Define the preprocessor macro to get the application version in our application. ## VERSION
DEFINES += APP_VERSION=\\\\\"\$\$VERSION\\\\\"                                           ## VERSION
DEFINES += APP_BUILD=\\\\\"$VERSION\\\\\"                                             ## VERSION
EOT
	touch src/version.cpp
}

function compile() {
	# compile
	qmake CONFIG+=debug
	make -j4
	return $?
}

function winecompile() {
	export QT=/data/local/qt-5.1.0-windows/5.1.0/mingw48_32/bin/
	export MINGW=/data/local/qt-5.1.0-windows/Tools/mingw48_32/bin/
	export W_QT=$(echo -n "Z:"; echo $QT | sed 's@/@\\@gm')
	export W_MINGW=$(echo -n "Z:"; echo $MINGW | sed 's@/@\\@gm')
	(mkdir ../call-qt-win 2>/dev/null; cd ../call-qt-win && (
		echo -n > make.bat
		echo "set PATH=%PATH%;$W_QT;$W_MINGW" > make.bat
		echo "$QT/qmake.exe ../call-qt/call-qt.pro" >> make.bat
		echo "$MINGW/mingw32-make.exe" >> make.bat
		wine cmd /c make.bat
		cp bin/call-qt.exe ../call-qt/ 2>/dev/null
		for dll in Qt5Core.dll Qt5Gui.dll Qt5Network.dll Qt5Widgets.dll \
				libgcc_s_dw2-1.dll libstdc++-6.dll icuin51.dll icuuc51.dll icudt51.dll libwinpthread-1.dll
		do
			test -f ../call-qt/$dll || cp $QT/$dll ../call-qt/
		done
		cd ../call-qt
		upx *.exe *.dll 2>/dev/null
		ll *.exe *.dll 2>/dev/null
		zip -rq compiled.zip *.exe *.dll img/ ui/ src/
	))
}

function debug() {
	gdb -ex run ./call-qt
}

function profile() {
	valgrind --leak-check=full ./call-qt
}

function run() {
	./call-qt
}

function commit() {
	find src/ -exec git add '{}' \;
	git commit -a
	git push origin master
}


if [ "x$1" = "xvalgrind" ] || [ "x$1" = "xprofile" ]
then
	newversion
	compile && profile
elif [ "x$1" = "xgdb" ] || [ "x$1" = "xdebug" ]
then
	newversion
	compile && debug
elif [ "x$1" = "xcommit" ]
then
	commit
elif [ "x$1" = "xwine" ]
then
	winecompile
else
	newversion
	compile && run
fi
