#!/bin/bash

cc="g++"
options="-I. -m32 -shared -fPIC -Wall"
constants=""

mkdir -p bin
mkdir -p objects

echo "Compiling /src_lib..."
$cc $options $constants -c src_lib/sys_libloader.cpp -o objects/sys_libloader.o
$cc $options $constants -c src_lib/sys_patch.cpp -o objects/sys_patch.o

echo "Linking objects..."
objects="$(ls objects/*.o)"
$cc -m32 -shared -L/lib32 -o bin/libcod2rev.so -ldl $objects $pthread_link

echo "Cleaning up..."
rm objects -r
echo "Done!"
