### Test and Results

#### TPC-H Query 5 and Query 6 restuls and analysis

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
  Liten - single thread
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
  Liten - single thread
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

Liten 6 Thread results -
Query 6 - 42ms
Query 5 - 3.2s (numMaps = 6 or 3 or 1)

11/15/2020 6 worker results
Query 6 - 32ms
Query 5 - 2.4s (numMaps = 6 or 3 or 1)

12/23/2020
Completed end-2-end demo with Arrow and Tenalytics demo.

#### Discoveries
Broadcast of inverse maps (Can we reduce 3.2 sec further for 6 parallel threads). Broadcast on same VM should have no impact. It is only across nodes that this matters.
