## Build using docker
Build a local docker image first
```
docker build . -t myubuntu22
```
Get a bash shell. Map root dir to work.
```
docker run -v /path/to/litendb:/work -it myubuntu22:latest
```
If venv not activated do the following in docker shell.
```
source /work/venv/bin/activate
```
After that build C++ code.
```
cd /path/to/litendb/cpp
./buildall.sh
```
The python can be built after that.
```
cd /path/to/litendb/py/liten
./build.sh -b
```
This will build a wheel in dist directory.
