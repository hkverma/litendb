# Build arrow
cd arrow/cpp
mkdir release
cd release
cmake ..
make arrow
make install
