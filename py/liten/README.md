## Liten Database

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
