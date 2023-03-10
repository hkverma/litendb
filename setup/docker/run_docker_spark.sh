#!/bin/bash
sudo -v
MASTER=$(docker run --name="master" -h master --add-host master:192.168.1.10 --add-host spark1:192.168.1.11 --add-host spark2:192.168.1.12 --add-host spark3:192.168.1.13 --add-host spark4:192.168.1.14 --expose=1-65535 --env SPARK_MASTER_IP=192.168.1.10 -d drpaulbrewer/spark-master:latest)
sudo pipework eth0 $MASTER 192.168.1.10/24@192.168.1.1
SPARK1=$(docker run --name="spark1" -h spark1 --add-host home:192.168.1.8 --add-host master:192.168.1.10 --add-host spark1:192.168.1.11 --add-host spark2:192.168.1.12 --add-host spark3:192.168.1.13 --add-host spark4:192.168.1.14 --expose=1-65535 --env mem=10G --env master=spark://192.168.1.10:7077 -v /data:/data -v /tmp:/tmp -d drpaulbrewer/spark-worker:latest)
sudo pipework eth0 $SPARK1 192.168.1.11/24@192.168.1.1
