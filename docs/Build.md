## Build and Debug Steps

Following sections describe development tools and software needed to build Liten.

### C++, gdb versions

* C++-20 standards
* g++-11, gcc-11 compiler
* Minimum cmake version 3.22

### Git repo and code

Clone Liten code from github.
```bash
git clone https://github.com/hkverma/tendb.git
```
Set LITEN_ROOT_DIR to the root of the repository.
```bash
export LITEN_ROOT_DIR=<root-dir>
```
Set LITEN_BUILD_TYPE to debug or release for C++ and python wheel builds.
```bash
export LITEN_BUILD_TYPE=<debug|release>
```
These are the packages in external_libs that Liten uses. Build debug or release based on LITEN_BUILD_TYPE settings.

Update all the submodules as shown below.
```bash
cd ${LITEN_ROOT_DIR}/cpp/opensource
git submodule init
git submodule update --recursive
```

From the root following directories exist.

* cpp - Has C++ code. Use CMake to build and maintain.
* jvm - Has java & scala code. Use maven to build and maintain.
* py  - Has python code.

Within cpp these directories exist.

* opensource - All external C++ libs. These include arrow, fizz, folly and wangle. Various other libraries are installed using apt-get install while building the VM.
* Other directories in C++ are liten specific code.

### Build Machine setup

We use Ubuntu 24.01 LTS Linux OS for development

First you must create a venv in python to start working.
```bash
python -m venv tendb
source tendb/bin/activate
```
Update and get the required python libs.
```bash
pip install -r ./setup/requirements.txt
```
Set LITEN_VENV_DIR it is currently used in CMakefile.
```bash
export LITEN_VENV_DIR=<tendb-venv-dir>
```
Run setup/buildvm.sh to install relevant build tools and libraries on the VM

```bash
sudo ./setup/buildvm.sh
```

### Compile C++ code

Build the Liten code.
```bash
cd ${LITEN_ROOT_DIR}/cpp
./buildall.sh
```

One can build all the opensource packages using the following command. It is not needed unless one wants to have a local copy for debug.
```bash
cd ${LITEN_ROOT_DIR}/cpp/opensource
sudo ./buildall.sh
```
To generate document, do the following.
```bash
cd ${LITEN_ROOT_DIR}/cpp
doxygen
```
You can run the following test and check the log file as well. By default, log files are in /tmp/Liten.INFO The dataset should have all the Tpch data files.

```
build/debug/bin/cache_test ../../data/tpch/sfdot001g
```

### Build Liten python wheel

There is a build.sh script in py/liten to build the wheels. Do the following.

```bash
./build.sh -b
```
You can also run setup.py to build from py/liten directory.
       
```bash
python3 setup.py build
```
To check for a dist do the following
```bash
python3 setup.py sdist
```
Create a wheel (zip file with all the install libs, files etc.) do the following.
```bash
python3 setup.py bdist_wheel
```

This wheel can be tested locally by using pip install. Uninstall liten first if installed earlier.
```bash
pip uninstall litendb
```
```bash
pip install dist/litendb-0.0.1-cp38-cp38-linux_x86_64.whl
```

You can check cmake command separately. Not needed but useful if cmake canges are made.
```bash
cmake -DPYTHON_EXECUTABLE=/home/hkverma/miniconda3/envs/liten/bin/python -DPython3_EXECUTABLE=/home/hkverma/miniconda3/envs/liten/bin/python  -DCMAKE_BUILD_TYPE=debug /mnt/c/Users/hkver/Documents/dbai/dbaistuff/py/liten
cmake --build . --config _liten
```
#### Upload to pip website
It is not needed for local runs. However, once uploaded anyone can use it. You can use the build.sh script to upload the wheels.
```
#To repair
./build.sh -r
#To upload to testpypi
./build.sh -t
#To upload to pypi
./build.sh -u
```
You may need to repair the wheel to manylinux distro. Use the following commands
```
auditwheel repair litendb-0.0.11-cp310-cp310-linux_x86_64.whl -w . --plat manylinux_2_35_x86_64
```
Upload it for pip install commands from testpy repository.
```bash
python3 -m twine upload --repository dist/*
```
This package can be installed using conda like.
```bash
python3 -m pip install --index-url https://test.pypi.org/simple/ --no-deps liten-pkg-liten
```

It can also be uploaded to main pip.
```bash
twine upload dist/*
```
Now the package can be installed directly like.
```bash
#From testpy
pip install -i https://test.pypi.org/simple/ litendb
#From pypi
pip install litendb
```
