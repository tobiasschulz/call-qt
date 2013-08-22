#!/bin/bash

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
EOT
touch src/version.cpp

# compile
make -j4
