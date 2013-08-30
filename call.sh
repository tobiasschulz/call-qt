#!/bin/bash

alias ll='ls -lh'

export QT_VERSION=5.1.1
export MINGW_VERSION=mingw48_32
export QT_MAIN=/data/local/qt-5.1.1-windows

export QT_BIN=$QT_MAIN/$QT_VERSION/$MINGW_VERSION/bin/
export QTCREATOR_BIN=$QT_MAIN/Tools/QtCreator/bin/
export MINGW_BIN=$QT_MAIN/Tools/$MINGW_VERSION/bin/
export MINGW_PLATFORM=$QT_MAIN/$QT_VERSION/$MINGW_VERSION/plugins/platforms/
export W_QT_BIN=$(echo -n "Z:"; echo $QT_BIN | sed 's@/@\\@gm')
export W_MINGW_BIN=$(echo -n "Z:"; echo $MINGW_BIN | sed 's@/@\\@gm')

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
	nice -n 19 qmake CONFIG+=debug
	nice -n 19 make -j4
	return $?
}

function wineclean() {
	rm -rf ../call-qt-win 2>/dev/null
	rm -f *.exe *.dll compiled.zip
}

function winecompile() {
	(mkdir ../call-qt-win 2>/dev/null; cd ../call-qt-win && (
		echo -n > make.bat
		echo "set PATH=%PATH%;$W_QT_BIN;$W_MINGW_BIN" > make.bat
		echo "$QT_BIN/qmake.exe ../call-qt/call-qt.pro" >> make.bat
		echo "$MINGW_BIN/mingw32-make.exe" >> make.bat
		wine cmd /c make.bat
	))
}

function winezip() {
	(mkdir ../call-qt-win 2>/dev/null; cd ../call-qt-win && (
		cp bin/call-qt.exe ../call-qt/ 2>/dev/null
		for dll in Qt5Core.dll Qt5Gui.dll Qt5Network.dll Qt5Widgets.dll Qt5Multimedia.dll Qt5MultimediaWidgets.dll Qt5Svg.dll Qt5OpenGL.dll \
				libgcc_s_dw2-1.dll libstdc++-6.dll icuin51.dll icuuc51.dll icudt51.dll libwinpthread-1.dll
		do
			test -f ../call-qt/$dll || cp $QT_BIN/$dll ../call-qt/
		done
		for dll in libEGL.dll libGLESv2.dll D3DCompiler_43.dll
		do
		true
#			test -f ../call-qt/$dll || cp $QTCREATOR_BIN/$dll ../call-qt/
		done
		for dll in qminimal.dll qoffscreen.dll qwindowsd.dll
		do
			test -f ../call-qt/$dll || cp $MINGW_PLATFORM/$dll ../call-qt/
		done
		cd ../call-qt
		#upx *.exe *.dll 2>/dev/null
		ll *.exe *.dll 2>/dev/null
		rm -f compiled.zip 2>/dev/null
		zip -rq compiled.zip *.exe *.dll img/ ui/ src/
	))
	echo A | unzip -d /data/share/test/ compiled.zip >/dev/null 2>&1
}

function winerun() {
	wine call-qt.exe
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
	shift
	while [ "x$1" = "xclean" ] || [ "x$1" = "xmake" ] || [ "x$1" = "xrun" ]
	do
		if [ "x$1" = "xclean" ]
		then
			wineclean
		elif [ "x$1" = "xmake" ]
		then
			winecompile && winezip
		elif [ "x$1" = "xrun" ]
		then
			winerun
		else
			winecompile && winezip
		fi
		shift
	done
else
	newversion
	compile && run
fi
