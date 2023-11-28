package com.liten.datasourcev2.batchreader

import java.util

import org.apache.spark.sql.catalyst.InternalRow
import org.apache.spark.sql.connector.catalog.{SupportsRead, Table, TableCapability, TableProvider}
import org.apache.spark.sql.connector.expressions.Transform
import org.apache.spark.sql.connector.read._
import org.apache.spark.sql.types.{StringType, StructField, StructType}
import org.apache.spark.sql.util.CaseInsensitiveStringMap
import org.apache.spark.unsafe.types.UTF8String

import scala.collection.JavaConverters._

/** DefaultSource for batch reader
Spark DataSourceV2 API looks for DefaultSource class in the given package.
Creating a new DataSourceV2 with TableProvider relation provider
*/
class DefaultSource extends TableProvider{

  override def inferSchema(caseInsensitiveStringMap: CaseInsensitiveStringMap): StructType =
    getTable(null,Array.empty[Transform],caseInsensitiveStringMap.asCaseSensitiveMap()).schema()

  override def getTable(structType: StructType, transforms: Array[Transform], map: util.Map[String, String]): Table =
    new BatchTableRead()
}

/** Define read support with Table
  */
class BatchTableRead extends Table with SupportsRead {
  override def name(): String = this.getClass.toString

  override def schema(): StructType = StructType(Array(StructField("value", StringType)))

  override def capabilities(): util.Set[TableCapability] = Set(TableCapability.BATCH_READ).asJava

  override def newScanBuilder(options: CaseInsensitiveStringMap): ScanBuilder = new BatchScanBuilder()
}

/** Scan for batch
  */
class BatchScanBuilder extends ScanBuilder {
  override def build(): Scan = new BatchScan
}

/** Scan for each batch
 */
class BatchScan extends Scan with Batch{
  override def readSchema(): StructType =  StructType(Array(StructField("value", StringType)))

  override def toBatch: Batch = this

  override def planInputPartitions(): Array[InputPartition] = {
    Array(new BatchPartition())
  }
  override def createReaderFactory(): PartitionReaderFactory = new BatchPartitionReaderFactory()
}

// simple class to organise the partition
class BatchPartition extends InputPartition

// reader factory
class BatchPartitionReaderFactory extends PartitionReaderFactory {
  override def createReader(partition: InputPartition): PartitionReader[InternalRow] = new BatchPartitionReader
}


/** Read a test array
  */
class BatchPartitionReader extends PartitionReader[InternalRow] {

  val values = Array("1", "2", "3", "4", "5")

  var index = 0
  
  def next = index < values.length

  def get = {
    val stringValue = values(index)
    val stringUtf = UTF8String.fromString(stringValue)
    val row = InternalRow(stringUtf)
    index = index + 1
    row
  }

  def close() = ()

}

