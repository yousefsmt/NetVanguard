#!/bin/bash

rm -rf build
autoreconf -fi
rm -rf autom4*

echo ""
echo "********************************************"
echo "***** Finish auto generate config file *****"
echo "********************************************"
echo ""

mkdir build
cd build
../configure

echo ""
echo "********************************************"
echo "***** Finish auto generate Makefile ********"
echo "********************************************"
echo ""

make
make check

echo ""
echo "********************************************"
echo "********** Successfully build **************"
echo "********************************************"
echo ""