#!/bin/bash
SPARK_ACTION=$1
SPARK_HOME=/opt/spark
case $SPARK_ACTION in
    start)
	pushd $SPARK_HOME
	sbin/start-all.sh
	# check localhost:8080 on web
	popd
        ;;
    stop)
        pushd $SPARK_HOME
        sbin/stop-all.sh
        popd
        ;;
    *)
        echo "Usage: spark.sh [start|stop]"
esac
