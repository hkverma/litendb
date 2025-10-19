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


## Try it out now

TODO: python notebook and how to try it on colab

For building 

### Summary:
Liten’s tensor indexing model and distributed engine demonstrate over 100× faster query performance on common analytical workloads compared to Spark 3.2.0.

