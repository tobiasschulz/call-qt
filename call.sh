#!/bin/bash

alias ll='ls -lh'

export QT_VERSION=5.1.1
export MINGW_VERSION=mingw48_32
export QT_MAIN=/data/local/qt-5.1.1-windows

export QT_BIN=$QT_MAIN/$QT_VERSION/$MINGW_VERSION/bin/
export QTCREATOR_BIN=$QT_MAIN/Tools/QtCreator/bin/
export MINGW_BIN=$QT_MAIN/Tools/$MINGW_VERSION/bin/
export QT_PLUGINS=$QT_MAIN/$QT_VERSION/$MINGW_VERSION/plugins/
export W_QT_BIN=$(echo -n "Z:"; echo $QT_BIN | sed 's@/@\\@gm')
export W_MINGW_BIN=$(echo -n "Z:"; echo $MINGW_BIN | sed 's@/@\\@gm')

export CODEDIR=../call-qt
export WIN32DIR=../call-qt-win
export WINRELEASE=../call-qt-win32-release
export WINDEBUG=../call-qt-win32-debug

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
	rm -rf $WIN32DIR 2>/dev/null
	rm -f *.exe *.dll compiled.zip
}

function winecompile() {
	(mkdir $WIN32DIR $WINDEBUG $WINRELEASE 2>/dev/null; cd $WIN32DIR && (
		echo -n > make.bat
		echo "set PATH=%PATH%;$W_QT_BIN;$W_MINGW_BIN" > make.bat
		echo "$QT_BIN/qmake.exe $CODEDIR/call-qt.pro" >> make.bat
		echo "$MINGW_BIN/mingw32-make.exe" >> make.bat
		nice -n 19 wine cmd /c make.bat
		rm -f $WINRELEASE/call-qt.exe $WINDEBUG/call-qt.exe
		cp bin/call-qt.exe $WINRELEASE 2>/dev/null
		cp bin/call-qt.exe $WINDEBUG 2>/dev/null
		cp bin/call-qt.exe $WINDEBUG
	))
	test -f $WINDEBUG/call-qt.exe
}

function winezip() {
	(mkdir $WIN32DIR $WINDEBUG $WINRELEASE 2>/dev/null; cd $WIN32DIR && (
		for dll in Qt5Core.dll Qt5Gui.dll Qt5Network.dll Qt5Widgets.dll Qt5Multimedia.dll Qt5MultimediaWidgets.dll Qt5Svg.dll Qt5OpenGL.dll \
				libgcc_s_dw2-1.dll libstdc++-6.dll icuin51.dll icuuc51.dll icudt51.dll libwinpthread-1.dll
		do
			test -f $WINRELEASE/$dll || cp $QT_BIN/$dll $WINRELEASE
			export debugdll=$(echo $dll | sed 's@.dll@d.dll@gm')
			test -f $WINDEBUG/$debugdll || (test -f $WINDEBUG/$dll || (cp $QT_BIN/$debugdll $WINDEBUG || cp $QT_BIN/$dll $WINDEBUG))
		done
		for dll in libEGL.dll libGLESv2.dll D3DCompiler_43.dll
		do
			true
#			test -f ../call-qt/$dll || cp $QTCREATOR_BIN/$dll ../call-qt/
		done
		cp -rf $QT_PLUGINS/platforms/ $WINRELEASE
		cp -rf $QT_PLUGINS/mediaservice/ $WINRELEASE
		cp -rf $QT_PLUGINS/imageformats/ $WINRELEASE
		cp -rf $QT_PLUGINS/platforms/ $WINDEBUG
		cp -rf $QT_PLUGINS/mediaservice/ $WINDEBUG
		cp -rf $QT_PLUGINS/imageformats/ $WINDEBUG
	))

	(mkdir $CODEDIR 2>/dev/null; cd $CODEDIR && (
		cp -rf img/ ui/ src/ $WINRELEASE
		cp -rf img/ ui/ src/ $WINDEBUG
	))

	#upx *.exe *.dll 2>/dev/null
	(mkdir $WINRELEASE 2>/dev/null; cd $WINRELEASE && (
		ll *.exe *.dll 2>/dev/null
		rm -f $CODEDIR/compiled.zip $CODEDIR/dependencies.zip 2>/dev/null
		nice -n 19 zip -rq $CODEDIR/compiled.zip *.exe img/ ui/ src/
		nice -n 19 zip -rq $CODEDIR/dependencies.zip *.dll platforms/ imageformats/ mediaservice/
	))

	rm -rf /data/share/test/*
	echo A | nice -n 19 unzip -d /data/share/test/ $CODEDIR/dependencies.zip >/dev/null 2>&1
	echo A | nice -n 19 unzip -d /data/share/test/ $CODEDIR/compiled.zip >/dev/null 2>&1
	chmod -R 0777 /data/share/test/
}

function winerun() {
	nice -n 19 wine call-qt.exe
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
