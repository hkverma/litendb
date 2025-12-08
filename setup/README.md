## Build using docker
Build a local docker image first
```
docker build . -t myubuntu22
```
Get a bash shell. Map root dir to work.
```
docker run -v /home/hkverma/github/litendb:/work -it myubuntu22:latest
```
After that follow the build direction from root README