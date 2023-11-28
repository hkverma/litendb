#!/bin/bash
mvn clean package
# Run it like - /opt/spark/bin/spark-submit --class com.liten.datasourcev2.batchreader.DataSourceV2Example --master local[2] target/litendb-0.0.1.jar
