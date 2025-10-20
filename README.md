# README #

## Liten Database Tensor Indexing Overview

### Concept
The Liten database constructs a tensor-based data structure that uses indices to link dimensional data with corresponding metrics. This architecture transforms traditional tabular relationships into highly efficient index lookups, resulting in dramatically faster query execution.

### Implementation Detail
A GitHub module provides a method that automatically creates data tensors during data persistence. By generating these tensors as data is stored, the system ensures query acceleration at the source, minimizing runtime computation overhead.

### Performance Benefits

Many query operations become direct index lookups rather than complex joins. Multi-dimensional tensors simplify query logic and boost performance. The approach enables high-speed analytical queries across large datasets.

### Acceleration Technology

* Novel distributed tensor-based engine with multi-dimensional hierarchical data models
* Faster execution compared to traditional relational models
* No joins, fast aggregates, and simpler query plans
* Further acceleration with GPU and FPGA hardware tensors
* Open lakehouse storage, enabling interoperability with other analytics tools

Transactional guarantees maintained over the data lake layer

### Query Performance Examples

Test Environment:
Ran on Azure Standard DS2 v3 (6 GiB RAM, 2 cores)

#### TPCH Query 6

This query quantifies the amount of revenue increase that would have resulted from eliminating company-wide discounts.

| Engine      | Execution Time |
| :---------- | :------------- |
| Spark 3.2.0 | 16 sec         |
| **Liten**   | **0.06 sec**   |

#### TPCH Query 5

This query lists the revenue volume generated through local suppliers.
| Engine      | Execution Time |
| :---------- | :------------- |
| Spark 3.2.0 | 31 sec         |
| **Liten**   | **0.64 sec**   |

## Development Directions

**[Build directions](docs/Build.md)**:  It has directions on how to build and install Liten locally.

## Try It Now

### Notebooks Summary

1. **[Liten TPC-H Queries](py/notebooks/LitenTpchQ5Q6.ipynb)**: This notebook demonstrates how to use the Liten library for data processing and querying with TPC-H datasets, including setting up a cache, reading tables, and creating relationships between them. This also explains how it can achieve the query acceleration.

2. **[Liten Basic Sentiment Analysis](py/notebooks/LitenBasicSentimentAnalysis.ipynb)**: This notebook demonstrates how to use the Liten cache to download a dataset, process reviews, and analyze sentiment using the TextBlob library.

3. **[Liten NY Taxi XGBoost](py/notebooks/LitenNYTaxiXGBoost.ipynb)**: This notebook focuses on predicting New York City taxi fares using XGBoost, demonstrating data cleaning, feature engineering, model training, and prediction. It keeps the data in cache.

4. **[Spark TPC-H Queries](py/notebooks/SparkTpchQueries.ipynb)**: This notebook shows how to run TPC-H queries using Spark, including loading datasets, defining schemas, and executing SQL queries to analyze data. This can be used to compare against Liten Runs.

### Demos
- **[Liten TPC-H Queries Demo](py/notebooks/demos/LitenTpchQ5Q6.ipynb)**
- **[Liten Basic Sentiment Analysis Demo](py/notebooks/demos/LitenBasicSentimentAnalysis.ipynb)**
- **[Liten NY Taxi XGBoost Demo](py/notebooks/demos/LitenNYTaxiXGBoost.ipynb)**

### Summary:
Liten’s tensor indexing model and distributed engine demonstrate over 100× faster query performance on common analytical workloads compared to Spark 3.2.0.

## License
This project is licensed under the [Apache License 2.0](py/liten/LICENSE).

© 2025 HK Verma and contributors


