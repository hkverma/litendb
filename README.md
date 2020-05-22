# README #

## What is this repository for?

Tenalytics DB data structure
* [Learn Markdown](https://bitbucket.org/tutorials/markdowndemo)

## How do I get set up?

### Build Structure Setup

For local machine setup, following environment is used.

* Ubuntu Linux 18.04.04 LTS
* C++-17 standards
* g++-9, gcc-9 compiler

### Spark 3.0

Download and setup Spark 3.0 https://spark.apache.org/news/spark-3.0.0-preview2.html

### C++ Build Environment

Use C++-17 standard. For compilation, g++/gcc verion 9 is used.

Install cmake 3.17 minimum. Download source code & build. Or download the binary from cmake website.

https://cmake.org/download/

Set TENDB_ROOT_DIR to the root of the repository.
```
$ export TENDB_ROOT_DIR=<root-dir>
```

From the root following directories exist.

* cpp - Hsas C++ code. Use CMake to build and maintain.
* jvm - Has java & scala code. Use maven to build and maintain

Within cpp these directories exist.

* external_libs - All external C++ libs. These include boost, Poco,Apache-arrow
* Other directories in C++ are tendb specific code.

### Required packages
SSL should be installed
```
apt-get install openssl libssl-dev
```

### Boost

Download 1.73.0 from https://www.boost.org/users/download to the following directory ${TENDB_ROOT_DIR}/cpp/external_libs/boost

 Run the following commands to compile boost.
 
 ```
 $ cd ${TENDB_ROOT_DIR}/cpp/external_libs/boost
 $ gunzip gunzip boost_1_73_0.tar.gz
 $ tar xvf gunzip boost_1_73_0.tar
 $ sudo ./build.sh
 ```

#### POCO

POCO is network library for C++ See here https://pocoproject.org/ for more information.

It is built as a submodule in tendb. Follow these steps to build

```
$ cd ${TENDB_ROOT_DIR}/cpp/external_libs
$ git submodule --update --recursive
$ cd poco
$ ./build.sh
```
It was registered as a submodule using steps shown below. master branch points to the latest stable release. Latest release is 1.10.1

```
$ cd ${TENDB_ROOT_DIR}/cpp/external_libs/poco
$ git submodule add https://github.com/pocoproject/poco
remote: Enumerating objects: 5, done.        
remote: Counting objects: 100% (5/5), done.        
remote: Compressing objects: 100% (5/5), done.        
remote: Total 134044 (delta 1), reused 0 (delta 0), pack-reused 134039        
Receiving objects: 100% (134044/134044), 134.80 MiB | 6.26 MiB/s, done.
Resolving deltas: 100% (109076/109076), done.
$ cd poco
$ git checkout master
Already on 'master'
Your branch is up to date with 'origin/master'.
$ git checkout 3fc3e5f5b8462f7666952b43381383a79b8b5d92
....
HEAD is now at 3fc3e5f5b Merge branch 'poco-1.10.1'
$ git submodule init
$ git submodule update
```

#### Apache Arrow

Apache Arrow is in-memory representation of data formats. It can be used to read csv, json, parquet etc. TenDB will read from arrow API to create its own Tensor structure.

Apache Arrow is a submodule within external_libs.

Follow the following steps to build it.

```
$ cd ${TENDB_ROOT_DIR}/cpp/external_libs
$ git submodule --update --recursive
$ cd arrow
$ sudo ./build.sh
```
It was registered as a submodule using steps shown below. It is currently fixed as release  3cbcb7b62c2f2d02851bff837758637eb592a64b arrow (apache-arrow-0.17.0)

First submodule was added. It clones the data into the arrow subdir.
```
$ cd ${TENDB_ROOT_DIR}/cpp/external_libs/arrow
$ git submodule add https://github.com/apache/arrow
remote: Enumerating objects: 100065, done.        
remote: Total 100065 (delta 0), reused 0 (delta 0), pack-reused 100065        
Receiving objects: 100% (100065/100065), 53.00 MiB | 10.23 MiB/s, done.
Resolving deltas: 100% (68755/68755), done.
$ cd arrow
$ git checkout 3cbcb7b62c2f2d02851bff837758637eb592a64b
$ git submodule init
$ git submodule update
Submodule path 'arrow': checked out '7194f94e0ed65d9429180eb96e35fcb4eca38848'
```

## TODO  - Add later

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