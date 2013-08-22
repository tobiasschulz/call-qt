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

QString Config::version() {
	return APP_VERSION;
}
