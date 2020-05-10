# README #

## What is this repository for?

Tenalytics DB data structure
* [Learn Markdown](https://bitbucket.org/tutorials/markdowndemo)

## How do I get set up?

### Build structure setup
For local machine setup, do the following things

#### Spark 3.0
Download and setup Spark 3.0 https://spark.apache.org/news/spark-3.0.0-preview2.html

### cpp build

Use C++-17 standard. For compilation, g++/gcc verion 9 is used.
Install cmake 3.17 minimum. Download source code & build. Or download the binary from cmake website.
https://cmake.org/download/

Set TENDB_ROOT_DIR to the root of the repository. From the root following directories are visible -
* cpp - has C++ code. Use CMake to build and maintain.
* jvm - Has java & scala code. Use maven to build and maintain
Within cpp the directories are -
* external_libs - All external C++ libs. These include boost, Poco,Apache-arrow
* Other directories in C++ are tendb specific code.

Do the following for C++ library setups
#### install required packages
SSL should be installed
```
apt-get install openssl libssl-dev
```
####

#### Boost library download 1.73.0 from https://www.boost.org/users/download to the following directory ${TENDB_ROOT_DIR}/cpp/external_libs/boost
 Run the following commands
 ```
 $ cd ${TENDB_ROOT_DIR}/cpp/external_libs/boost
 $ gunzip gunzip boost_1_73_0.tar.gz
 $ tar xvf gunzip boost_1_73_0.tar
 $ sudo ./buildall.sh
 ```
#### Apache Arrow
Apache Arrow is in-memory representation of data formats. It can be used to read csv, json, parquet etc. TenDB will read from arrow API to create its own Tensor structure.
Apache Arrow is a submodule within external_libs. It was registered as a submodule as shown below. It is currently fixed as release 0.17
```
git submodule add https://github.com/apache/arrow
```
Do the following to get to the right release.

#### Poco library setups
TODO

## TODO - These need modifications
* Configuration
* Dependencies
* Database configuration
* How to run tests
* Deployment instructions

### Contribution guidelines ###

* Writing tests
* Code review
* Other guidelines

### Who do I talk to? ###

* Repo owner or admin
* Other community or team contact