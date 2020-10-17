# README #

## What is this repository for?

Tenalytics DB data structure
* [Learn Markdown](https://bitbucket.org/tutorials/markdowndemo)

## How do I get set up?
### Clone this repo
git clone https://hkv@bitbucket.org/hkv/dbaistuff.git

### Build Structure Setup

For local machine setup, following environment is used.

* Ubuntu Linux 18.04.04 LTS
* C++-17 standards
* g++-9, gcc-9 compiler

### Spark 3.0

Download and setup Spark 3.0 https://spark.apache.org/news/spark-3.0.0-preview2.html
Use jupyter with pyspark set the following variables & run pyspark. jupyter should be installed as well
```
sudo apt-get install jupyter
export PYSPARK_DRIVER_PYTHON=jupyter
export PYSPARK_DRIVER_PYTHON_OPTS='notebook'
```
You can now run pyspark from spark bin directory
```
./pyspark
```
To see the directory, go to port 8888
http://127.0.0.1/8888

To see run time visit -
http://127.0.0.1:4040


### C++ Build Environment

Use C++-17 standard. For compilation, g++/gcc version 9 is used.

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


For local machine setup, following environment is used.

* Ubuntu Linux 18.04.04 LTS
* C++-17 standards
* g++-9, gcc-9 compiler

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
 $ gunzip boost_1_73_0.tar.gz
 $ tar xvf gunzip boost_1_73_0.tar
 $ sudo ./build.sh [debug|release]
 ```

#### POCO

POCO is network library for C++ See here https://pocoproject.org/ for more information.

It is built as a submodule in tendb. Follow these steps to build

```
$ cd ${TENDB_ROOT_DIR}/cpp/external_libs
$ git submodule update --init --recursive
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
#### Ray

Ray is an open source task scheduler. It is a submodule within external_libs.

It requires that following are installed
* python3
* bazel - see https://docs.bazel.build/versions/master/install-ubuntu.html for installation

Run the tutorial from https://github.com/ray-project/tutorial
Follow the tutorial to install all the components and run ray.

Follow the following steps to build it locally. 

```
$ cd ${TENDB_ROOT_DIR}/cpp/external_libs
$ git submodule --update --recursive
$ cd ray
$ sudo ./build.sh
```

It was registered as a submodule using steps shown below. It is currently fixed as release ray-0.8.5 This is a branch with release 0.8.5
First submodule was added. It clones the data into the ray as shown below.
```
$ cd ${TENDB_ROOT_DIR}/cpp/external_libs/ray
$ git submodule add https://github.com/apache/ray
$ cd ray
$ git checkout -b ray-0.8.5
$ git submodule init
$ git submodule update
```

#### Google glog

Google glog is a C++ logging library. 
Follow the following steps to build it.

```
$ cd ${TENDB_ROOT_DIR}/cpp/external_libs
$ git submodule --update --recursive
$ cd glog
$ sudo ./build.sh
```
It was registered as a submodule using steps shown below. It is currently fixed as latest release commit 0a2e5931bd5ff22fd3bf8999eb8ce776f159cda6 May 12, 2020

```
$ cd ${TENDB_ROOT_DIR}/cpp/external_libs/glog
$ git submodule add https://github.com/google/glog
...
$ cd glog
$ git submodule init
$ git submodule update
```

#### Googletest

Googletest is a C++ unit test library. Follow the following steps to build it.

```
$ cd ${TENDB_ROOT_DIR}/cpp/external_libs
$ git submodule --update --recursive
$ cd googletest
$ sudo ./build.sh
```
It was registered as a submodule using steps shown below. It is currently fixed as latest release commit as aee0f9d9b5b87796ee8a0ab26b7587ec30e8858e June 26, 2020

```
$ cd ${TENDB_ROOT_DIR}/cpp/external_libs/googletest
$ git submodule add https://github.com/google/glog
...
$ cd glog
$ git submodule init
$ git submodule update
```

### Development Guidelines
To check status without submodules the following can be done.
```
git status --ignore-submodules=all
```
* Configuration
* Dependencies
* Database configuration
* How to run tests
* Deployment instructions

### TPC-DS benchmarks
TPC-DS queries will be used for measuring the performance against other systems for queries.

If needed to generate get TPC-DS from git hub. You may use commands shown below to generate the data and queries. TPC-DS also has specification documents in the repo.
```
git clone https://github.com/gregrahn/tpcds-kit
```
run the follwing commands from tools subdirectory after building them.
Generate query

```
./dsqgen -DIRECTORY ../query_templates -INPUT ../query_templates/templates.lst -VERBOSE Y -QUALIFY Y -SCALE 1 -DIALECT ansi -OUTPUT_DIR /mnt/c/Users/hkver/Documents/dbai/tpc-ds/queries/
```

Generate data

```
./dsdgen -DIR ../../data -VERBOSE Y -SCALE 1
```
#### Types and Queries
These are the types of queries.

Interactive (1-3 months of data scanned)  Simple star-join queries
  19, 42, 52, 55
 
Reporting (1 year of data scanned)  Simple star-join queries
  3, 7, 53, 89
 
Analytic (Multiple years, customer patterns)  Customer extracts, star joins
  34, 34, 59
 
Complex  Fact-to-fact joins, windows, extensive subqueries
  23, 36, 64, 94

### Under Development

#### Concepts -
* map       - like zone-map
* transform - like data-tensor
* hierarchy - for hierarchical data
* aggregates- for feaures & aggregates
* distributed - for distributes data
* incremental - build incrementally
* cache       - keep LRU or LRU (T) type of tensor

* index - multi-dimensional, hierarchical access to a single point

* DSLs on DAGs - lazy evaluation

#### Tasks - TODO

* Query 5 Analysis on debug build
* Join time taken - all rows processed
 Rows = 50000 Elapsed ms=15636
 Rows = 100000 Elapsed ms=59247
 Rows = 300000 Elapsed ms=91408 

* Join time taken - with min-max maps (~50% reduction)
  Rows = 50000 Elapsed ms=13625
  Rows = 100000 Elapsed ms=30746

* Join time taken - with min-max maps (~50% reduction)
  Rows = 50000 Elapsed ms=7535
  Rows = 100000 Elapsed ms=16887
  Rows = 200000 Elapsed ms=33534

* Join time taken - with min-max maps (~50% reduction) rest time taken in serial lookup for rowId
  Rows = 50000 Elapsed ms=9079 RowId Time ms= 8959 ValId Time ms= 14
  Rows = 100000 Elapsed ms=20772 RowId Time ms= 20517 ValId Time ms= 57
  Rows = 200000 Elapsed ms=41176 RowId Time ms= 40616 ValId Time ms= 170

* Join time taken - with inverse-index for the data ~100x reduction
  Rows = 100000 Elapsed ms=261 RowId Time us= 60 ValId Time ms= 2
  Rows = 200000 Elapsed ms=578 RowId Time ms= 170 ValId Time ms= 3

* Query Analysis - 10/10/2020 Release build VM 8 core 8GB  (3x reduction from debug build)
  Single thread implementaion
  TenDB - single thread
    Query 6 -  Revenue=1.56378e+08 Time = 176848us = 177ms
    Query 5 -  Revenue=6.33263e+09 Time = 32016802us = 32s
    Rows = 100000 Elapsed ms=76 Rows = 200000 Elapsed ms=167
  Spark3.0 - single worker
    Query 6 - |1.5659409560959977E8| Time = 11-13 sec
    Query 5 - 31s to 40s
    n_name|             revenue|
+--------------+--------------------+
|        RUSSIA| 5.606302283899996E7|
|       ROMANIA|5.4994648594799995E7|
|UNITED KINGDOM| 5.468614748900003E7|
|        FRANCE| 5.194113723339999E7|
|       GERMANY| 5.153649818379998E7|
+--------------+--------------------+

* Query Analysis - 10/12/2020 (Most of work spent in going through orders list
   Creating reverse map for min-max values.
 Rows = 200000 Elapsed ms=712
 Orders RowId Time ms= 218
 Orders ValId Time ms= 2
 Supplier RowId Time ms= 28
 Supplier ValId Time ms= 0
 Nation RowId Time ms= 0
 Nation ValId Time ms= 0
 Restart Time ms= 25
 Rows = 300000 Elapsed ms=1162
 Orders RowId Time ms= 427
 Orders ValId Time ms= 4
 Supplier RowId Time ms= 43
 Supplier ValId Time ms= 0
 Nation RowId Time ms= 0
 Nation ValId Time ms= 0
 Restart Time ms= 38
 Rows = 400000 Elapsed ms=1672
 Orders RowId Time ms= 706
 Orders ValId Time ms= 5
 Supplier RowId Time ms= 56
 Supplier ValId Time ms= 0
 Nation RowId Time ms= 1
 Nation ValId Time ms= 0
 Restart Time ms= 51
  Query5 Revenue=6.33263e+09 Time = 133047690us

* Reverse map for min-max is not efficient. It ends up scanning lot more data and perf does not imrpove.

* TODO do reverse map for all column in one shot

* Add event IDs in the message. Also add context for each user.

* Inverse map - get the arrays where zones exist instead of traversing

* Hash look up into the values

* Query6 and Query5 comparisons with Spark

* Add logging and clean up code

* Multi-threaded and thread-pool implementation analysis

* Use Ray & Dask type clusters with Python bindings for a single VM node 

* Async multi-tasking using Ray - explore
* CMakefile changes to put binarues in bin
* DSL - which ML algorithm to encode & which app

*  Read TPCH data and print table
*  Run Query 6
* Compare Query 6 against Spark on VM
*  Do pre-join to create the tables
*  Put zone-map for column chunks

* Run Query 5
* Compare Query 5 against Spark on VM

* Write DSLs - groupby scan filter etc.
* Write Query 6 & Query 5 using DSL
* Do TPCH queries using the DSL using jupyter

* Use arrow::Result to return Status along with the value

### Contribution guidelines ###

* Writing tests
* Code review
* Other guidelines

### Who do I talk to? ###

* Repo owner or admin
* Other community or team contact