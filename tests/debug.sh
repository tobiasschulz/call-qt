#!/bin/bash

make -j4 && gdb -ex run ./call-qt
