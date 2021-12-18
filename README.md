# README #

## What is this repository for?

Liten Data - Build and Run 
* [Learn Markdown](https://bitbucket.org/tutorials/markdowndemo)

## How do I get set up?

### Clone this repo

git clone https://hkv@bitbucket.org/hkv/dbaistuff.git

### Build Structure Setup

For local machine setup, following environment is used.

* Ubuntu Linux 18.04.04 LTS
* C++-17 standards
* g++-9, gcc-9 compiler

### C++ Build Environment

Use C++-17 standard. For compilation, g++/gcc version 9 is used.

Install cmake 3.17 minimum. Download source code & build. Or download the binary from cmake website.

https://cmake.org/download/

Set LITEN_ROOT_DIR to the root of the repository.
```
$ export LITEN_ROOT_DIR=<root-dir>
```

From the root following directories exist.

* cpp - Hsas C++ code. Use CMake to build and maintain.
* jvm - Has java & scala code. Use maven to build and maintain

Within cpp these directories exist.

* external_libs - All external C++ libs. These include boost, Poco,Apache-arrow
* Other directories in C++ are liten specific code.


For local machine setup, following environment is used.

* Ubuntu Linux 18.04.04 LTS
* C++-17 standards
* g++-9, gcc-9 compiler

### Required packages
SSL should be installed
```
apt-get install openssl libssl-dev
```
### Submodule packages
These are the packages in external_libs that Tenalytics uses.
You can build all the packages using the following command
```
 $ cd ${LITEN_ROOT_DIR}/cpp/external_libs
 $ sudo ./buildall.sh [debug|release]
```
#### Boost

Download 1.73.0 from https://www.boost.org/users/download to the following directory ${LITEN_ROOT_DIR}/cpp/external_libs/boost

 Run the following commands to compile boost.
 
 ```
 $ cd ${LITEN_ROOT_DIR}/cpp/external_libs/boost
 $ gunzip boost_1_73_0.tar.gz
 $ tar xvf gunzip boost_1_73_0.tar
 $ sudo ./build.sh [debug|release]
 ```

#### Apache Arrow

Apache Arrow is in-memory representation of data formats. It can be used to read csv, json, parquet etc. Liten will read from arrow API to create its own Tensor structure.

Apache Arrow is a submodule within external_libs.

Follow the following steps to build it.

```
$ cd ${LITEN_ROOT_DIR}/cpp/external_libs
$ git submodule update --recursive
$ cd arrow
$ sudo ./build.sh [Debug|Release]
```
It was registered as a submodule using steps shown below. 

First submodule was added. It clones the data into the arrow subdir.
It is currently fixed as to release 2.0.0. You can get it by first updating the git and then checkout the tag

```
$ cd ${LITEN_ROOT_DIR}/cpp/external_libs/arrow
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
Arrow also has gandiva in it. Liten uses gandiva as an expression evaluator. This needs these libs as well
```
sudo apt-get install clang-tools
sudo apt-get install clang-format
sudo apt-get install clang-tidy
```
Install llvm 12 as well.
```
printf "deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial-12 main" |sudo tee /etc/apt/sources.list.d/llvm-toolchain-xenial-12.list
wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key |sudo apt-key add -
sudo apt update
sudo apt install llvm-12
Update to 12
```
sudo apt install clang-12 --install-suggest
```
Install zlib as well
```
sudo apt install zlib1g
sudo apt install zlib1g-dev
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
$ cd ${LITEN_ROOT_DIR}/cpp/external_libs
$ git submodule update --recursive
$ cd ray
$ sudo ./build.sh
```

It was registered as a submodule using steps shown below. It is currently fixed as release ray-0.8.5 This is a branch with release 0.8.5
First submodule was added. It clones the data into the ray as shown below.
```
$ cd ${LITEN_ROOT_DIR}/cpp/external_libs/ray
$ git submodule add https://github.com/apache/ray
$ cd ray
$ git checkout -b ray-0.8.5
$ git submodule init
$ git submodule update
```

#### TBB - Intel Thread Building Blocks

TBB provides task manager and various other parallel algorithms to build parallel applications. 

TBB is a submodule within external_libs.

Follow the following steps to build it.

```
$ cd ${LITEN_ROOT_DIR}/cpp/external_libs/tbb
$ git submodule update --init
$ ./build.sh [debug|release]
```
It was registered as a submodule using steps shown below.
```
$ cd ${LITEN_ROOT_DIR}/cpp/external_libs/tbb
$ git submodule add https://github.com/apache/arrow
remote: Enumerating objects: 100065, done.        
remote: Total 100065 (delta 0), reused 0 (delta 0), pack-reused 100065        
Receiving objects: 100% (100065/100065), 53.00 MiB | 10.23 MiB/s, done.
Resolving deltas: 100% (68755/68755), done.
$ cd tbb
$ git submodule add
```

#### Google glog

Google glog is a C++ logging library. 
Follow the following steps to build it.

```
$ cd ${LITEN_ROOT_DIR}/cpp/external_libs
$ git submodule update --recursive
$ cd glog
$ sudo ./build.sh [Debug|Release]
```
It was registered as a submodule using steps shown below. It is currently fixed as latest release commit from Oct 3, 2020. It can be updated and new submodule version checked in if needed.

```
$ cd ${LITEN_ROOT_DIR}/cpp/external_libs/glog
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

#### POCO

POCO is network library for C++ See here https://pocoproject.org/ for more information.

It is built as a submodule in liten. Follow these steps to build. Tenalytics does not currently use Poco library.

```
$ cd ${LITEN_ROOT_DIR}/cpp/external_libs
$ git submodule update --init --recursive
$ cd poco
$ ./build.sh
```
It was registered as a submodule using steps shown below. master branch points to the latest stable release. Latest release is 1.10.1

```
$ cd ${LITEN_ROOT_DIR}/cpp/external_libs/poco
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

#### Googletest

Googletest is a C++ unit test library. Follow the following steps to build it.

```
$ cd ${LITEN_ROOT_DIR}/cpp/external_libs
$ git submodule update --recursive
$ cd googletest
$ sudo ./build.sh
```
It was registered as a submodule using steps shown below. It is currently fixed as latest release commit as aee0f9d9b5b87796ee8a0ab26b7587ec30e8858e June 26, 2020

```
$ cd ${LITEN_ROOT_DIR}/cpp/external_libs/googletest
$ git submodule add https://github.com/google/glog
```

#### gperftools

gperftools is google perftool library. This can be used in conjunction with valgrind.

```
$ cd ${LITEN_ROOT_DIR}/cpp/external_libs
$ git submodule add https://github.com/gperftools/gperftools
```
For profiling following tools can be used
* gprof - old gnu profiler.
* valgrind - callgrind does call checks. Use kcahcegrind to view call stacks
* gperfools - can be used to profile code sections. Use kcachegrind to see call stacks and costs.

### Jupyter Notebook and Python lib Installations
I use conda and create a new env liten. Use this env and install new environment. Python is used for a real notbook environment.

#### Conda environment
Install miniconda with python3. Follow instructions from here
https://docs.conda.io/projects/conda/en/latest/user-guide/install/linux.html
Set environement variabel MINICONDA_ROOT_DIR to ~/miniconda3 or other directory here conda was installed.
```
MINICONDA_ROOT_DIR="/home/azureuser/miniconda3"
```
Use python 3.8.5 version. Ray needs version 3.8
```
conda install python=3.8.5
```
Create a new environment liten for tenanlytics tests
```
conda create --name liten python=3.8.5
```
Change env to liten everytime for work
```
conda activate liten
```
If needed env can be removed.
```
conda env remove --name liten
```
Update pip and setuptools. This is typically not needed.
```
curl https://bootstrap.pypa.io/get-pip.py | python
pip install --upgrade setuptools
```

#### Install Jupyter
Install jupyter notebook and then start the notebook.
```
conda install jupyter
```

#### Install required packges
You can create a new environment called liten using the following commands
```
conda env create -f py/environment.yml
conda activate liten
```
The existing environment can be updated using the following environments.
```
conda env update --file environment.yml --prune
```
For C++ build to pick the correct arrow lib, add following to LD_LIBRARY_PATH
```
export LD_LIBRARY_PATH=${MINICONDA_ROOT_DIR}/envs/liten/lib:${LD_LIBRARY_PATH}
```
Liten uses graphviz to show query plans. It is installed using sudo command.
```
sudo apt install graphviz
```

#### Check Arrow Installation
Open ArrowTutorial.ipynb in notebook and run to check that arrow is ok.
```
cd py/notebooks
jupyter notebook
```

#### Check Arrow Installation
Open RayTutorial.ipynb in notebook and run to check that Ray is ok.
```
cd py/notebooks
jupyter notebook
```

#### Check Liten Installation
Open LitenIntro.ipynb in notebook and run to check that arrow is ok.
```
cd py/notebooks
jupyter notebook
```

### Python Setup Builds
Python setup.py is in py/liten subdirectory.

Always develop in liten environment.
You can check cmake command separately. Not needed but useful if cmake canges are made.
```
cmake -DPYTHON_EXECUTABLE=/home/hkverma/miniconda3/envs/liten/bin/python -DPython3_EXECUTABLE=/home/hkverma/miniconda3/envs/liten/bin/python  -DCMAKE_BUILD_TYPE=debug /mnt/c/Users/hkver/Documents/dbai/dbaistuff/py/liten
cmake --build . --config _liten
```
#### setup.py
First get to the liten conda environment. This should have all the library components.
```
conda activate liten
```
Run setup.py to build from py/liten directory.
       
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

This wheel can be testeted locally by using pip install. Uninstall liten first if installed earlier.
```
pip uninstall liten
..
pip install dist/liten-0.0.1-cp38-cp38-linux_x86_64.whl
```
Test the code with TenalyticsIntro notebook.

Upload it for pip install commands from testpy repository.
```
python3 -m twine upload --repository dist/*
```
This package can be installed using conda like.
```bash
python3 -m pip install --index-url https://test.pypi.org/simple/ --no-deps liten-pkg-liten
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

### TPC benchmarks

#### TPC-DS
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

##### Types and Queries
These are the types of queries.

Interactive (1-3 months of data scanned)  Simple star-join queries
  19, 42, 52, 55
 
Reporting (1 year of data scanned)  Simple star-join queries
  3, 7, 53, 89
 
Analytic (Multiple years, customer patterns)  Customer extracts, star joins
  34, 34, 59
 
Complex  Fact-to-fact joins, windows, extensive subqueries
  23, 36, 64, 94

#### TPC-H
TPC-H kit can be downloaded from official TPC-H benchmark site
Official TPC-H benchmark - [http://www.tpc.org/tpch](http://www.tpc.org/tpch)
Follow the README file to create data files and queries.

The results of the TPC runs are in benchmarks/README.md

Use x2goclient and server
ssh -i ~/dev_key.pem azureuser@xx.xx.xx.
x2goclient
https://wiki.x2go.org/doku.php

### Software Architecture

#### TCache
Thread safe simple Key-Value storage for buffer management
Get a sha512 on schema json. Sha512 is the name of the schema.
array_uuid is added in ascending version
```
{[table_name, field_name]-> map<version, array_uuid>
{schema_sha512}
```
If in DDR-RAM, there is a map from array_uuid to Array pointer.
```
unordered_map<array_uuid, array_ptr>
```
There is also a map to know version for a given array_uuid
```
unordered_map<array_uuid, version>
```
List of all the tables and schema is stored separately
```
unordered_map<table_name, schema_sha512>
```
We also maintain the latest version of the data. version starts from 0 and increased every time a change is made.
```
uint64_t current_version
```
CacheManager (LRU or NFU or Time-region-based), evict if cache size exceeds max

#### TMetaPersist
Buffer-Manager to read and write TCache components like – {UUID.array, table_name, field_name, version}

#### DataViews

Dataviews are from cache structure. 
They can to be rebuilt lazily or eagerly once commit has been called for a new version.

Dataviews are created for a given table name. Given a memory structure TTable can be partial and full table. 
Howevr TRowBlock is always served, and one should be able to hold it in memory.

|View      | Information          |
| -------  | --------------       |
|TRowBlock | vector [array_ptr]   |
|TTable    | vector [TRowBlock]   |



#### DataTensorView
TColumnMap has Reverse Index, Zone-map (min-max)

#### Liten Cache Operations
Exposed through GQL or REST
Cache-ops
```
Download-table (table_name, table URI) 
Remove-table(table_name)
```
Table URI is most likely in S3/ABFS etc.
For streaming operations.
```
Add-rowblock([start_time,end_time], table_name, rowblock URI) – Used for streaming
```
Streaming data must be presented in order of time
Buidling tensor views
```
build-data-tensor(table_name)
```

#### Hierarchy
Extra dataviews to create the hierarchy


#### Liten Transactional
Liten array is immutable. Each transaction creates a new array with a new version. 
Use delta table to keep a transaction storage views.
```
update(key,value)
delete(key)
```

#### Liten datatensor operations
Cube is a view of the arrow data. It is generated based on the user query.
The indices can be point, range or set
```
point - one value such as 12-12-21 or 'Adam' etc
range - between min and max ['12-12-20':'12-12;-21'], [0:4]
set - list of values [2,4,8]
```
Multidimensional indices will be viewed as indices as shown below
```
ordertable[orderitem][region][city]
```
One can apply aggregate operations on chosen cubes
```
aggregate(cell)
```

#### ROLAP - OLAP using Relational SQL
Use Liten to OLAP operations
Use Velox for all vector operations, compare with Gandiva
Use Liten for data store and managemement


#### Concepts

These are some of the techniques being used in Liten code.

* map       - keep zone map
* transform - create data-tensor
* hierarchy - For hierarchical data
* aggregates- For feaures & aggregates
* distributed - for distributes data
* incremental - build incrementally
* cache       - keep LRU or LRU (T) type of tensor
* index - multi-dimensional, hierarchical access to a single point
* DSLs on DAGs - lazy evaluation


### Tasks

#### In Progress

  Create Liten tensor for query processing
  Modife schema to add dimension hierarchy. For example in TPCH it looks like 
  Child     Parent
  ORDERS -> CUSTOMER
  PART -> PARTSUPP
  SUPP -> NATION -> REGION

  Add these hierarchies into Schema.

  Add the following APIs and execute query6 and query5 using these APIs.
  
  API -
  lineitem = vector<region>  region contains set<nations>
  lineitem = vector<orders>  orders contain set<customer>

  l1[set<lineitem-ids>] = lineitem.slice(region == "europe")
  l2[set<lineitem-ids>] = l1[set<lineitem-ids>].filter(for col == "orders" do pred = "orderdate == 1996")
  l2.aggregate(revenue as ep*(1-dis) by nation)
  map????
  
  Modify Liten to enable these two operations
  
  Change ML training example and add features as table in Liten as well
  Add pcap reading and parsing for network security work with streaming actions
  
  Use RecordBatches instead of Table in C++ code, make type names consistent across python and cpp

#### TODO

  Use property instead of get_* functions in python  
  Enhance make_dtensor to replace only dimensional columns and replace fact table completely
  Clean and rerun all the other demo python codes

  Explore Splunk addition
  
  Reading logs from multiple database systems and doing performance analysis (say between Oracle and SQL-Server)
  
* Clean LitenIntro_1.ipynb for print outputs
* Design LitenIntro_2.ipynb with multiple remote cache

* Create demo with optimized timing 
* Select between all TXXX name or Lxxx names. Make it consistent across python and cpp
* Write cache into SSD or storage for persistence
* In TTable.h remove multiple maps_ copies, keep only one. Do an analysis.
* Use ctest to run tests (from cmake)
* What to do if same table is read again & again
* Register Liten library with pytest 
* 

#### Ideas

* Added LCache as class in package. Works with local as well as Ray remote code. Build the demo here.

* Use Ray to add clusters - Use Ray and Object Library for a scaleout demo.

* Add event IDs in the message. Also add context for each user.

* Inverse map - get the arrays where zones exist instead of traversing

* Use Ray & Dask type clusters with Python bindings for a single VM node 

* Async multi-tasking using Ray - explore
* CMakefile changes to put binaries in bin
* DSL - which ML algorithm to encode & which app

* Write DSLs - groupby scan filter etc. using decorator to pipeline these operations
* Write Query 6 & Query 5 using DSL
* Do TPCH queries using the DSL using jupyter

* Use arrow::Result to return Status along with the value

#### To Read

[Spark data source example code](https://github.com/spirom/spark-data-sources)

[IPC benchmark good one to run to check perf](https://github.com/goldsborough/ipc-bench)

[IPC perf named pipe vs socket](https://stackoverflow.com/questions/1235958/ipc-performance-named-pipe-vs-socket)

[Simple IPC benchmark](https://github.com/rigtorp/ipc-bench)

[Write small data io with mmap etc in C++ lib](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1031r1.pdf)
[LLFIO C++ lib](https://ned14.github.io/llfio/)
[metall lib for memory mapped files](https://metall.readthedocs.io/en/latest/)
[metall paper](https://www.osti.gov/servlets/purl/1576900)

[Brendan perf tools](https://github.com/brendangregg/perf-tools)

[shmem-blog](https://github.com/goldshtn/shmemq-blog)

[PCA for network analytics](http://conferences.sigcomm.org/sigcomm/2004/papers/p405-lakhina111.pdf)

[PCA eigen values](https://towardsdatascience.com/pca-eigenvectors-and-eigenvalues-1f968bc6777a)

[Arrow Extended columnar processing in spark](https://issues.apache.org/jira/browse/SPARK-27396)

[Synapse ML Services](https://github.com/microsoft/SynapseML)

#### Completed
* Added LitenIntro_0.ipynb - Added dimention and fact tabes
* Added LitenIntro_1.ipynb Works with Ray remote calls

#### To Write

* Writing tests
* Code review
* Other guidelines

### Who do I talk to? ###

* Repo owner or admin
* Other community or team contact
