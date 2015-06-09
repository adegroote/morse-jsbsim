#!/bin/bash -e
export _WR=$(pwd)
export _WP=$_WR/ws
export _WS=$_WP/src
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$_WP/lib
export PATH=$PATH:$_WP/bin

mkdir -p $_WS

# checkout 3.4.3 since 3.5 requires cmake 2.8.11
cd $_WS
git clone --depth=5 git://git.savannah.nongnu.org/certi.git -b CERTI-3_4_3
cd certi
# fix Unknown CMake command "target_include_directories".
patch -p0 << EOF
--- CMakeLists.txt
+++ CMakeLists.txt
@@ -453 +453 @@
-add_subdirectory( test )
+#add_subdirectory( test )
EOF

mkdir build; cd build
cmake -DCMAKE_INSTALL_PREFIX=$_WP ..
make -j; make install

cd $_WS
name=PyHLA-1.1.1-Source
wget http://download.savannah.gnu.org/releases/certi/contrib/PyHLA/${name}.tar.gz
tar xvf ${name}.tar.gz; cd ${name}
patch -p0 < $_WR/cmake-pyhla.patch

mkdir build; cd build
cmake -DPYTHON_EXECUTABLE=/usr/bin/python3.4 -DPYTHON_LIBRARY=/usr/lib/x86_64-linux-gnu/libpython3.4m.so -DPYTHON_INCLUDE_DIR=/usr/include/python3.4 -DPYTHON_INCLUDE_DIR2=/usr/include/x86_64-linux-gnu/python3.4m -DCMAKE_INSTALL_PREFIX=$_WP ..
make -j; make install

cd $_WS
git clone --depth=5 git://git.code.sf.net/p/jsbsim/code jsbsim
cd jsbsim
./autogen.sh --enable-libraries --prefix=$_WP
make install

cd $_WS
git clone --depth=5 https://github.com/morse-simulator/morse.git
cd morse
mkdir build; cd build
cmake -DPYTHON_EXECUTABLE=/usr/bin/python3.4 -DCMAKE_INSTALL_PREFIX=$_WP ..
make -j; make install

cd $_WR/cpp
mkdir build; cd build
cmake -DCERTI_DIR=$_WP/share/scripts -DJSBSIM_ROOT_DIR=$_WP ..
make -j

cd $_WR
morse import $(pwd)

exit

'''
rtig
# export JSBSIM_MORSE_HOME=./jsbsim/

./cpp/build/jsbsim_node
xvfb-run --auto-servernum --server-args="-screen 0 160x120x16" morse --noaudio run morse-jsbsim
'''
