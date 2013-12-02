#!/usr/bin/env bash

which bxquery > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "ERROR: Missing Bayeux's bxquery !" >&2
    exit 1
fi
opwd=$(pwd)

function my_exit()
{
    cd ${opwd}
    exit $1
}

build_dir=$(pwd)/__build
test -d ${build_dir} && rm -fr ${build_dir}

test ! -d ${build_dir} && mkdir ${build_dir}
cd ${build_dir}

cmake \
    -DCMAKE_INSTALL_PREFIX=.. \
    -DCMAKE_FIND_ROOT_PATH:PATH=$(bxquery --prefix) \
    ..
if [ $? -ne 0 ]; then
    echo "ERROR: Configuration failed !" 1>&2
    my_exit 1
fi
make
if [ $? -ne 0 ]; then
    echo "ERROR: Build failed !" 1>&2
    my_exit 1
fi
make install
if [ $? -ne 0 ]; then
    echo "ERROR: Installation failed !" 1>&2
    my_exit 1
fi

cd ${opwd}
ls -l

#pandoc -r rst -w html README.rst -o geomtools-ex01_README.html
geomtools_inspector_bin="bxgeomtools_inspector"

echo "Run the 'ex01' program..." 1>&2
./ex01
if [ $? -ne 0 ]; then
    echo "ERROR: Example program ex01 failed !" 1>&2
    my_exit 1
fi

echo "Browse the geometry setup via the '${geomtools_inspector_bin}' program..." 1>&2
${geomtools_inspector_bin} \
   --manager-config config/manager.conf \
   --with-visu --visu-view-3d

rm -f ./ex01
rm -f ./geomtools-ex01.C
rm -f ./geomtools-ex01.gdml
rm -f ./geomtools-ex01_README.html
rm -fr ${build_dir}
find . -name "*~" -exec rm -f \{\} \;


cd ${opwd}

my_exit 0

# end
