cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
cd ..
gdb bin/logger