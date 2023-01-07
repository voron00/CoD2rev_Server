#!/bin/bash

name="cod2rev_lnxded"
constants="-D TESTING_LIBRARY=0"
static="-static"
shared=""
pthread_link="-lpthread"

if [ "$1" = "lib" ]; then
	name="libcod2rev.so"
	constants="-D TESTING_LIBRARY=1"
	static=""
	shared="-shared"
fi

cc="g++"
options="-I. -m32 $shared -fPIC -Wall"

cc_zlib="gcc"
options_zlib="-I. -m32 $shared -fPIC"

mkdir -p bin
mkdir -p objects/lib
mkdir -p objects/src
mkdir -p objects/zlib

objects_lib=""
objects_src=""
objects_zlib=""

if [ "$1" = "lib" ]; then
	echo "Compiling /src_lib..."
	$cc $options $constants -c src_lib/sys_libloader.cpp -o objects/lib/sys_libloader.o
	$cc $options $constants -c src_lib/sys_patch.cpp -o objects/lib/sys_patch.o
	objects_lib="$(ls objects/lib/*.o)"
fi

echo "Compiling /src..."
for f in src/*.cpp ; do $cc $options $constants -c $f -o objects/${f%.cpp}.o ; done

echo "Compiling /src/huffman..."
$cc $options $constants -c src/huffman/huffman.cpp -o objects/src/huffman.o

echo "Compiling /src/zlib..."
$cc_zlib $options_zlib -c src/zlib/adler32.c -o objects/zlib/adler32.o
$cc_zlib $options_zlib -c src/zlib/crc32.c -o objects/zlib/crc32.o
$cc_zlib $options_zlib -c src/zlib/deflate.c -o objects/zlib/deflate.o
$cc_zlib $options_zlib -c src/zlib/inffast.c -o objects/zlib/inffast.o
$cc_zlib $options_zlib -c src/zlib/inflate.c -o objects/zlib/inflate.o
$cc_zlib $options_zlib -c src/zlib/inftrees.c -o objects/zlib/inftrees.o
$cc_zlib $options_zlib -c src/zlib/unzip.c -o objects/zlib/unzip.o
$cc_zlib $options_zlib -c src/zlib/trees.c -o objects/zlib/trees.o
$cc_zlib $options_zlib -c src/zlib/utils.c -o objects/zlib/utils.o

echo "Linking objects..."
objects_src="$(ls objects/src/*.o)"
objects_zlib="$(ls objects/zlib/*.o)"
$cc -m32 $static $shared -L/lib32 -o bin/$name -ldl $objects_lib $objects_src $objects_zlib $pthread_link

echo "Cleaning up..."
rm objects/zlib -r
rm objects -r
echo "Done!"