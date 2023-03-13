val sparkSession = SparkSession.builder
  .master("k8s://https://kubernetes.default.svc:32768")
  .appName("test")
  .config("spark.driver.host", "sparkrunner-0")
  .config("spark.driver.port", "7077")
  .config("spark.driver.blockManager.port", "7078")
  .config("spark.kubernetes.container.image","spark-alluxio")
  .config("fs.alluxio.impl", "alluxio.hadoop.FileSystem")
  .config("fs.alluxio-ft.impl", "alluxio.hadoop.FaultTolerantFileSystem")
  .getOrCreate