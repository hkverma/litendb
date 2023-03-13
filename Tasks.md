# Tasks

## Demo in progress
  pyspark weblog analytics with
  Query 5 TPCH query using Liten
  K8s setup with Spark
  K8s setup with Liten data layer and Spark
  Liten Data on AWS
  
## Programming In Progress

  Modify all C++ names to follow google naming conventions

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

## Pipeline

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

* Clean notebooks. Reproduce on Azure web.
* Draw query plan, print query and result for each query.
* Complete ML and NLP training data predictions

#### Example Result logs

###### before tensor
I20211002 09:54:48.303267   133 TpchDemo.cpp:367] Rows = 5500000 Elapsed ms=31495
I20211002 09:54:48.303342   133 TpchDemo.cpp:368] Orders RowId Time ms= 6551
I20211002 09:54:48.303347   133 TpchDemo.cpp:369] Orders ValId Time ms= 803
I20211002 09:54:48.303350   133 TpchDemo.cpp:370] Supplier RowId Time ms= 1330
I20211002 09:54:48.303354   133 TpchDemo.cpp:371] Supplier ValId Time ms= 260
I20211002 09:54:48.303356   133 TpchDemo.cpp:372] Nation RowId Time ms= 282
I20211002 09:54:48.303359   133 TpchDemo.cpp:373] Nation ValId Time ms= 143
I20211002 09:54:48.303362   133 TpchDemo.cpp:374] Scan Time ms= 783
I20211002 09:54:48.303365   133 TpchDemo.cpp:375] Extra Time ms= 21179
E20211002 09:54:51.381997   133 TpchDemo.cpp:382] Missing order key
I20211002 09:54:51.388489   133 TestMain.cpp:138] Query5 Revenue=
I20211002 09:54:51.388456   133 TpchDemo.cpp:462] Query 5 Elapsed ms=34580
I20211002 09:54:51.388495   133 TestMain.cpp:140] RUSSIA=1.33841e+09
I20211002 09:54:51.388517   133 TestMain.cpp:140] UNITED KINGDOM=1.29815e+09
I20211002 09:54:51.388525   133 TestMain.cpp:140] ROMANIA=1.31158e+09
I20211002 09:54:51.388530   133 TestMain.cpp:140] FRANCE=1.32222e+09
I20211002 09:54:51.388536   133 TestMain.cpp:140] GERMANY=1.32511e+09
I20211002 09:54:51.388542   133 TestMain.cpp:154] 34580453us

###### after tensor
I20211004 17:43:50.564781  2684 TpchDemo.cpp:367] Rows = 5500000 Elapsed ms=17213
I20211004 17:43:50.564800  2684 TpchDemo.cpp:368] Orders RowId Time ms= 951
I20211004 17:43:50.564802  2684 TpchDemo.cpp:369] Orders ValId Time ms= 34
I20211004 17:43:50.564805  2684 TpchDemo.cpp:370] Supplier RowId Time ms= 215
I20211004 17:43:50.564807  2684 TpchDemo.cpp:371] Supplier ValId Time ms= 7
I20211004 17:43:50.564810  2684 TpchDemo.cpp:372] Nation RowId Time ms= 8
I20211004 17:43:50.564811  2684 TpchDemo.cpp:373] Nation ValId Time ms= 5
I20211004 17:43:50.564911  2684 TpchDemo.cpp:374] Scan Time ms= 37
I20211004 17:43:50.564918  2684 TpchDemo.cpp:375] Extra Time ms= 9696
E20211004 17:43:52.233779  2684 TpchDemo.cpp:382] Missing order key
I20211004 17:43:52.234112  2684 TpchDemo.cpp:462] Query 5 Elapsed ms=18882
I20211004 17:43:52.234131  2684 TestMain.cpp:144] Query5 Revenue=
I20211004 17:43:52.234134  2684 TestMain.cpp:146] RUSSIA=1.33841e+09
I20211004 17:43:52.234146  2684 TestMain.cpp:146] UNITED KINGDOM=1.29815e+09
I20211004 17:43:52.234149  2684 TestMain.cpp:146] ROMANIA=1.31158e+09
I20211004 17:43:52.234153  2684 TestMain.cpp:146] FRANCE=1.32222e+09
I20211004 17:43:52.234155  2684 TestMain.cpp:146] GERMANY=1.32511e+09
I20211004 17:43:52.234159  2684 TestMain.cpp:160] 18882415us

##### after lookup
I20211005 20:57:46.152598  1974 TpchDemo.cpp:367] Rows = 5500000 Elapsed ms=16346
I20211005 20:57:46.152614  1974 TpchDemo.cpp:368] Orders RowId Time ms= 608
I20211005 20:57:46.152616  1974 TpchDemo.cpp:369] Orders ValId Time ms= 50
I20211005 20:57:46.152618  1974 TpchDemo.cpp:370] Supplier RowId Time ms= 271
I20211005 20:57:46.152621  1974 TpchDemo.cpp:371] Supplier ValId Time ms= 9
I20211005 20:57:46.152622  1974 TpchDemo.cpp:372] Nation RowId Time ms= 9
I20211005 20:57:46.152624  1974 TpchDemo.cpp:373] Nation ValId Time ms= 8
I20211005 20:57:46.152626  1974 TpchDemo.cpp:374] Scan Time ms= 52
I20211005 20:57:46.152628  1974 TpchDemo.cpp:375] Extra Time ms= 9361
E20211005 20:57:47.699096  1974 TpchDemo.cpp:382] Missing order key
I20211005 20:57:47.699347  1974 TpchDemo.cpp:462] Query 5 Elapsed ms=17893
I20211005 20:57:47.699364  1974 TestMain.cpp:144] Query5 Revenue=
I20211005 20:57:47.699368  1974 TestMain.cpp:146] RUSSIA=1.33841e+09
I20211005 20:57:47.699378  1974 TestMain.cpp:146] UNITED KINGDOM=1.29815e+09
I20211005 20:57:47.699380  1974 TestMain.cpp:146] ROMANIA=1.31158e+09
I20211005 20:57:47.699384  1974 TestMain.cpp:146] FRANCE=1.32222e+09
I20211005 20:57:47.699388  1974 TestMain.cpp:146] GERMANY=1.32511e+09
I20211005 20:57:47.699390  1974 TestMain.cpp:160] 17893166us

