# README #

## What is this repository for?

Tenalytics Information
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

Starting Spark in a standalone cluster mode on a single node. More information can be found here -
   https://spark.apache.org/docs/3.0.1/spark-standalone.html
First start the master. Log file of master gives out the web address for starting a slave. Do that using the slave.
Change conf/spark-env.sh with correct cores & python version
SPARK_WORKER_CORES, to set the number of cores to use on this machine
SPARK_WORKER_CORES=6
SPARK_WORKER_MEMORY, to set how much total memory workers have to give executors (e.g. 1000m, 2g)
SPARK_WORKER_MEMORY=6g
PYSPARK_PYTHON=python3
PYSPARK_DRIVER_PYTHON=python3

Once master and slaves are running, start pyspark notebook with master node. The code is shown below.

```
$ sbin/start-master.sh 
$ more ~/spark/spark-3.0.0-bin-hadoop2.7/logs/spark-azureuser-org.apache.spark.deploy.master.Master-1-dev.out
$  sbin/start-slave.sh spark://dev.udukqz5jp4je5ng1bvoq2ijmdd.xx.internal.cloudapp.net:7077
$  vi ~/spark/spark-3.0.0-bin-hadoop2.7/logs/spark-azureuser-org.apache.spark.deploy.worker.Worker-1-dev.out
$  ~/spark/spark-3.0.0-bin-hadoop2.7/bin/pyspark --master spark://dev.udukqz5jp4je5ng1bvoq2ijmdd.xx.internal.cloudapp.net:7077
```
Use the following web lcations
localhost:8080 for spark master and slave informations
localhost:4040 for job and stages status
Once all done kill jupyter notebook. Stop slave and master after that.
```
$ sbin/stop-slave.sh
$ sbin/stop-master.sh
```

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
### Jupyter Notebook and Python lib Installations
I use conda and create a new env tendb. Use this env and install new environment.

#### Conda environment
Install miniconda with python3. Follow instructions from here
https://docs.conda.io/projects/conda/en/latest/user-guide/install/linux.html
Set environement variabel MINICONDA_ROOT_DIR to ~/miniconda3 or other directory here conda was installed.
```
MINICONDA_ROOT_DIR="/home/azureuser/miniconda3"
```

Create a new environment tendb for tenanlytics tests
```
conda create --name tendb
```
Change env to tendb everytime for tendb work
```
conda activate tendb
```
#### Install Arrow
Follow directions from
https://arrow.apache.org/docs/python/install.html
Install arrow
```
conda install -c conda-forge pyarrow
```
#### Install Jupyter
Install jupyter notebook and then start the notebook
```
conda install jupyter
...
jupyter notebook
```
Open ArrowTutorial from py/notebooks/ and run to check that arrow is ok

#### Install Cython
Cython is used for C++ bindings and wrappers. Install conda as shown below.
```
conda install -c anaconda cython
```
For C++ build to pick the correct arrow lib, add following to LD_LIBRARY_PATH
```
export LD_LIBRARY_PATH=${MINICONDA_ROOT_DIR}/envs/tendb/lib:${LD_LIBRARY_PATH}
```
#### Install graphviz
TenDB uses graphviz to show query plans.Install using the following commands
```
sudo apt install graphviz
pip install graphviz
```

#### Other python pacakges
Install these python packages as well.
```
conda install numpy
```

### Python Setup Builds
Python setup.py is in py/tendb subdirectory.

Always develop in tendb environment.
You can check cmake command separately. Not needed but useful if cmake canges are made.
```
cmake -DPYTHON_EXECUTABLE=/home/hkverma/miniconda3/envs/tendb/bin/python -DPython3_EXECUTABLE=/home/hkverma/miniconda3/envs/tendb/bin/python  -DCMAKE_BUILD_TYPE=debug /mnt/c/Users/hkver/Documents/dbai/dbaistuff/py/tendb
cmake --build . --config _tendb
```
#### setup.py
Run setup.py to build from py/tendb directory.
       
```
python setup.py build
```
To check for a dist do the following
```
python3 setup.py sdist
```
Create a wheel (zip file with all the install libs, files etc.) do the following.
```
python3 setup.py bdist_wheel
```

This wheel can be testeted locally by using pip install. Uninstall tendb first if installed erarlier.
```
pip uninstall tendb
..
pip install dist/tendb-0.0.2-cp39-cp39-linux_x86_64.whl
```
Test the code with TenalyticsIntro notebook.

Upload it for pip install commands from testpy repository.
```
python3 -m twine upload --repository dist/*
```
This package can be installed using conda like.
```bash
python3 -m pip install --index-url https://test.pypi.org/simple/ --no-deps tendb-pkg-tendb
```

### Submodule packages
These are the packages in external_libs that Tenalytics uses.

#### Boost

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

It is built as a submodule in tendb. Follow these steps to build. Tenalytics does not currently use Poco library.

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
$ git submodule update --recursive
$ cd arrow
$ sudo ./build.sh [Debug|Release]
```
It was registered as a submodule using steps shown below. 

First submodule was added. It clones the data into the arrow subdir.
It is currently fixed as to release 2.0.0. You can get it by first updating the git and then checkout the tag

```
$ cd ${TENDB_ROOT_DIR}/cpp/external_libs/arrow
$ git submodule add https://github.com/apache/arrow
remote: Enumerating objects: 100065, done.        
remote: Total 100065 (delta 0), reused 0 (delta 0), pack-reused 100065        
Receiving objects: 100% (100065/100065), 53.00 MiB | 10.23 MiB/s, done.
Resolving deltas: 100% (68755/68755), done.
$ cd arrow
$ git pull
$ git pull
$ git checkout tags/apache-arrow-2.0.0
$ git commit -a -m "Updated to arrow 2.0.0"
```
When cloning these can be done from the arrow submodule directory to get arrow files.
```
$ git submodule init
$ git submodule update
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
$ git submodule update --recursive
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
$ git submodule update --recursive
$ cd glog
$ sudo ./build.sh [Debug|Release]
```
It was registered as a submodule using steps shown below. It is currently fixed as latest release commit from Oct 3, 2020. It can be updated and new submodule version checked in if needed.

```
$ cd ${TENDB_ROOT_DIR}/cpp/external_libs/glog
$ git submodule add https://github.com/google/glog
$ cd glog
$ git pull
$ git commit -a -m "Updated to latest glog version"
```

Following will also get the latest commit glog in an empty submodule
```
$ git submodule init
$ git submodule update
```

#### Googletest

Googletest is a C++ unit test library. Follow the following steps to build it.

```
$ cd ${TENDB_ROOT_DIR}/cpp/external_libs
$ git submodule update --recursive
$ cd googletest
$ sudo ./build.sh
```
It was registered as a submodule using steps shown below. It is currently fixed as latest release commit as aee0f9d9b5b87796ee8a0ab26b7587ec30e8858e June 26, 2020

```
$ cd ${TENDB_ROOT_DIR}/cpp/external_libs/googletest
$ git submodule add https://github.com/google/glog
```

#### gperftools

gperftools is google perftool library. This can be used in conjunction with valgrind.

```
$ cd ${TENDB_ROOT_DIR}/cpp/external_libs
$ git submodule add https://github.com/gperftools/gperftools
```
For profiling following tools can be used
* gprof - old gnu profiler.
* valgrind - callgrind does call checks. Use kcahcegrind to view call stacks
* gperfools - can be used to profile code sections. Use kcachegrind to see call stacks and costs.

#### TBB - Intel Thread Building Blocks

TBB provides task manager and various other parallel algorithms to build parallel applications. 

TBB is a submodule within external_libs.

Follow the following steps to build it.

```
$ cd ${TENDB_ROOT_DIR}/cpp/external_libs/tbb
$ git submodule update --init
$ ./build.sh [debug|release]
```
It was registered as a submodule using steps shown below.
```
$ cd ${TENDB_ROOT_DIR}/cpp/external_libs/tbb
$ git submodule add https://github.com/apache/arrow
remote: Enumerating objects: 100065, done.        
remote: Total 100065 (delta 0), reused 0 (delta 0), pack-reused 100065        
Receiving objects: 100% (100065/100065), 53.00 MiB | 10.23 MiB/s, done.
Resolving deltas: 100% (68755/68755), done.
$ cd tbb
$ git submodule add
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

#### Results

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
  Query5 Revenue=6.33263e+09 Time = 133047690us = 133 sec

* Reverse map for min-max is not efficient. It ends up scanning lot more data and perf does not imrpove.

* Reverse map for all column in one shot - >6x improvement
I20201017 17:06:13.388578  2307 TpchQueries.cpp:189] Rows = 200000 Elapsed ms=669
I20201017 17:06:13.388593  2307 TpchQueries.cpp:190] Orders RowId Time ms= 204
I20201017 17:06:13.388595  2307 TpchQueries.cpp:191] Orders ValId Time ms= 2
I20201017 17:06:13.388597  2307 TpchQueries.cpp:192] Supplier RowId Time ms= 30
I20201017 17:06:13.388599  2307 TpchQueries.cpp:193] Supplier ValId Time ms= 0
I20201017 17:06:13.388602  2307 TpchQueries.cpp:194] Nation RowId Time ms= 0
I20201017 17:06:13.388602  2307 TpchQueries.cpp:195] Nation ValId Time ms= 0
I20201017 17:06:13.388604  2307 TpchQueries.cpp:196] Restart Time ms= 25
I20201017 17:06:13.733212  2307 TpchQueries.cpp:189] Rows = 300000 Elapsed ms=1014
I20201017 17:06:13.733227  2307 TpchQueries.cpp:190] Orders RowId Time ms= 308
I20201017 17:06:13.733229  2307 TpchQueries.cpp:191] Orders ValId Time ms= 5
I20201017 17:06:13.733232  2307 TpchQueries.cpp:192] Supplier RowId Time ms= 46
I20201017 17:06:13.733233  2307 TpchQueries.cpp:193] Supplier ValId Time ms= 0
I20201017 17:06:13.733235  2307 TpchQueries.cpp:194] Nation RowId Time ms= 1
I20201017 17:06:13.733237  2307 TpchQueries.cpp:195] Nation ValId Time ms= 0
I20201017 17:06:13.733238  2307 TpchQueries.cpp:196] Restart Time ms= 38
I20201017 17:06:14.078064  2307 TpchQueries.cpp:189] Rows = 400000 Elapsed ms=1358
I20201017 17:06:14.078078  2307 TpchQueries.cpp:190] Orders RowId Time ms= 411
I20201017 17:06:14.078109  2307 TpchQueries.cpp:191] Orders ValId Time ms= 6
I20201017 17:06:14.078110  2307 TpchQueries.cpp:192] Supplier RowId Time ms= 61
I20201017 17:06:14.078112  2307 TpchQueries.cpp:193] Supplier ValId Time ms= 1
I20201017 17:06:14.078114  2307 TpchQueries.cpp:194] Nation RowId Time ms= 2
I20201017 17:06:14.078115  2307 TpchQueries.cpp:195] Nation ValId Time ms= 0
I20201017 17:06:14.078117  2307 TpchQueries.cpp:196] Restart Time ms= 50
I20201017 17:06:14.411913  2307 TpchQueries.cpp:189] Rows = 500000 Elapsed ms=1692
I20201017 17:06:14.411928  2307 TpchQueries.cpp:190] Orders RowId Time ms= 513
I20201017 17:06:14.411931  2307 TpchQueries.cpp:191] Orders ValId Time ms= 7
I20201017 17:06:14.411932  2307 TpchQueries.cpp:192] Supplier RowId Time ms= 76
I20201017 17:06:14.411934  2307 TpchQueries.cpp:193] Supplier ValId Time ms= 1
I20201017 17:06:14.411936  2307 TpchQueries.cpp:194] Nation RowId Time ms= 2
I20201017 17:06:14.411937  2307 TpchQueries.cpp:195] Nation ValId Time ms= 0
I20201017 17:06:14.411939  2307 TpchQueries.cpp:196] Restart Time ms= 63

Query5 Revenue=6.33263e+09
Time =  20259536us = 20 sec

* Query Result - 10/10/2020 Release build VM 8 core 8GB  (3x reduction from debug build)
  Single thread implementaion
  TenDB - single thread
    Query 6 -  Revenue=1.56378e+08 Time = 176848us = 177ms
    Query 5 -  Revenue=6.33263e+09 Time = 32016802us = 32s
    Rows = 100000 Elapsed ms=76 Rows = 200000 Elapsed ms=167
  Spark3.0 - single worker multiple cores
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

* Query Result - 10/18/2020 Release build VM 8 core 8GB  (3-4x reduction from debug build)
  Single thread implementaion
  TenDB - single thread
sqlDf = spark.sql("select sum(l_extendedprice * l_discount) as revenue from lineitem "
                  "where l_shipdate >= date '1997-01-01' "
                  "and l_shipdate < date '1997-01-01' + interval '1' year "
                  " and l_discount between 0.07 - 0.01 and 0.07 + 0.01 "
                  " and l_quantity < 25;")
		  
    Query 6 -  Revenue=1.56378e+08 Time = 152505s = 152ms
                                          149871us = 149ms
    Query 5 -
sql5Df = spark.sql("select n_name, sum(l_extendedprice * (1 - l_discount)) as revenue "
    "from customer, orders, lineitem, supplier, nation, region "
    "where c_custkey = o_custkey "
    "and l_orderkey = o_orderkey "
    "and l_suppkey = s_suppkey "
    "and c_nationkey = s_nationkey "
    "and s_nationkey = n_nationkey "
    "and n_regionkey = r_regionkey "
    "and r_name = 'EUROPE' "
    "and o_orderdate >= date '1995-01-01' "
    "and o_orderdate < date '1995-01-01' + interval '1' year "
    "group by n_name order by revenue desc")
    
Revenue=6.33263e+09 Time = 5516979us = 5.5s
                           5584644us = 5.5s
    
  Spark3.0 - two workers
    Query 6 - |1.5659409560959977E8| Time = 11-13 sec
    Query 5 - 35 to 40 sec
    n_name|             revenue|
+--------------+--------------------+
|        RUSSIA| 5.606302283899996E7|
|       ROMANIA|5.4994648594799995E7|
|UNITED KINGDOM| 5.468614748900003E7|
|        FRANCE| 5.194113723339999E7|
|       GERMANY| 5.153649818379998E7|
+--------------+--------------------+

11/08/2020
Added TBB - 8 workers with parallel job distribution VM 8 node, 8GB SF=1
Query 6 - 
Serial Query6 Revenue=1.56378e+08  Time= 82841us = 82ms
Parallel Query6 Revenue=1.56594e+08 Time= 23161us = 23ms
Query 5
Serial
 Query 5 Elapsed ms=5599
 Query5 Revenue=
 RUSSIA=1.28826e+09
 ROMANIA=1.1785e+09
 UNITED KINGDOM=1.2382e+09
 FRANCE=1.30446e+09
 GERMANY=1.31384e+09
Parallel Elapsed ms=3428
 Result same as Serial

11/14/2020
Spark setup 8 core Azure Standard DS2 v3 ( 8 GiB memory) - 4 core on one CPU socket
Single node Spark Cluster Setup
Spark Cluster Setup 0 1 Master 1 Slave with 6 cores and 6 GB RAM.
Query 6
Time = 10 sec
Query 5
Time = 26 sec (scan, broadcast, join, hash-aggregate)

TenDB 6 Thread results -
Query 6 - 42ms
Query 5 - 3.2s (numMaps = 6 or 3 or 1)

11/15/2020 6 worker results
Query 6 - 32ms
Query 5 - 2.4s (numMaps = 6 or 3 or 1)

12/23/2020
Completed end-2-end demo with Arrow and Tenalytics demo.

#### Discoveries
* Broadcast of inverse maps (Can we reduce 3.2 sec further for 6 parallel threads). Broadcast on same VM should have no impact. It is only across nodes that this matters.

#### Tasks TODO

* Use Ray to add clusters - Use Ray and Object Library for a scaleout demo.

* Add event IDs in the message. Also add context for each user.

* Inverse map - get the arrays where zones exist instead of traversing

* Use Ray & Dask type clusters with Python bindings for a single VM node 

* Async multi-tasking using Ray - explore
* CMakefile changes to put binarues in bin
* DSL - which ML algorithm to encode & which app

* Write DSLs - groupby scan filter etc. using decorator to pipeline these operations
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