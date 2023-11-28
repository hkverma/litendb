package com.liten.datasourcev2.batchreader

import org.apache.spark.Partition
import org.apache.spark.sql.SparkSession
import org.apache.spark.Partition
import org.apache.spark.sql.{SaveMode, SparkSession}

object DataSourceV2Example {

  def main(args: Array[String]) : Unit = {

    val batchformat : String = "com.liten.datasourcev2.batchreader"
    val csvformat : String = "com.liten.datasourcev2.csvbatchreader"

    val sparkSession = SparkSession.builder
      .master("local[2]")
      .appName("example")
      .getOrCreate()

    val simpleDf = sparkSession.read
      .format(batchformat)
      .load()

    simpleDf.show()

    println("number of partitions in simple source is " + simpleDf.rdd.getNumPartitions)


    val simpleMultiDf = sparkSession.read
      .format(batchformat)
      .load()

    simpleMultiDf.show()
    println(
      "number of partitions in simple multi source is " + simpleMultiDf.rdd.getNumPartitions)


    val simpleCsvDf = sparkSession.read
      .format(csvformat)
      .load("src/test/resources/adult.csv")

    simpleCsvDf.printSchema()
    simpleCsvDf.show()
    println(
      "number of partitions in simple csv source is " + simpleCsvDf.rdd.getNumPartitions)

/*
    val simpleMysqlDf = sparkSession.createDataFrame(Seq(
      Tuple1("test1"),
      Tuple1("test2")
    )).toDF("user")

    //write examples
    simpleMysqlDf.write
      .format(myformat)
      .mode(SaveMode.Append)
      .save()

    simpleMysqlDf.write
      .format(
        "com.madhukaraphatak.examples.sparktwo.datasourcev2.mysqlwithtransaction")
      .save()

    val simplePartitoningDf = sparkSession.read
      .format(
        "com.madhukaraphatak.examples.sparktwo.datasourcev2.partitionaffinity")
      .load()

    val dfRDD = simplePartitoningDf.rdd
    val baseRDD =
      dfRDD.dependencies.head.rdd.dependencies.head.rdd.dependencies.head.rdd

    val partition = baseRDD.partitions(0)
    val getPrefferedLocationDef = baseRDD.getClass
      .getMethod("getPreferredLocations", classOf[Partition])
    val preferredLocation = getPrefferedLocationDef
      .invoke(baseRDD, partition)
      .asInstanceOf[Seq[String]]
    println("preferred location is " + preferredLocation)

    */

    sparkSession.stop()

  }
}