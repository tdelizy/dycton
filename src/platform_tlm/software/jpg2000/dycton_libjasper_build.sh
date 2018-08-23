#!/bin/bash
echo "building libjasper (jpg2000) for dycton simulator (mips32)"
echo ""
echo ""
echo ""
cd libjasper
export CPPFLAGS='-I../../../../../.. -I../../../../../jpg2000'
./configure --host=mipsel-unknown-elf --disable-libjpeg --disable-opengl
make -B
echo ""
echo ""
echo ""
echo "compilation have failed in the application build (this is ok)"
echo "retrieving library file (libjasper.a)"
mv src/libjasper/.libs/libjasper.a ../libjasper.a
