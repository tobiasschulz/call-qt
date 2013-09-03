/*
 * version.cpp
 *
 *  Created on: 22.08.2013
 *      Author: tobias
 */

#include "config.h"

#ifndef APP_VERSION
#define APP_VERSION "unknown"
#endif

#ifndef APP_BUILD
#define APP_BUILD "unknown"
#endif

QString Config::version()
{
	return APP_VERSION;
}

QString Config::build()
{
	return APP_BUILD;
}

QString CONFIG__version()
{
	return APP_VERSION;
}

QString CONFIG__build()
{
	return APP_BUILD;
}
