import liten as ten

cdef class TTable:
     """
     Liten Table Class
     """
    Dimension=0
    Fact=1
    nameToTTable = { }
    
    def __cinit__(self, name, table, ttype, schema_name=""):
        """
        Create arrow table in cache by name
        Parameters
           name: name of table
           table: arrow table to be added in liten cache
           ttype: type of table must be DimTable or FactTable
        Returns
           Added Liten TTable
        """
        sp_ctable = pyarrow_unwrap_table(table)
        if ttype != self.DimTable and ttype != self.FactTable:
            print("Error: Table must be DimTable or FactTable")
            return None       
        tc_ttype = <TableType>ttype
        sp_ttable_result = self.tcache.AddTable(ten.litenutils.to_bytes(name), tc_ttype, sp_ctable, schema_name)
        if (not sp_ttable_result.ok()):
            print ("Failed to add table=", name)
            return None
        sp_ttable = sp_ttable_result.ValueOrDie()
        p_ttable = sp_ttable.get()
        if (NULL == p_ttable):
            print ("Failed to build table")
            return None        
        sp_ctable = p_ttable.GetTable()
        self.nameToTTable[name] = self

    def get_arrow_table(self):
        """
        Returns
          Arrow table
        """
        pa_table = pyarrow_wrap_table(sp_ctable)
        return pa_table

    def get_name(self):
        """
        Returns
          unique name of the table
        """
        name = p_ttable.GetName()
        return ten.litenutils.to_bytes(name)

    def get_type(self):
        """
        Returns
          Dimension or Fact Table
        """
        cdef:
           TableType ttype
        ttype = p_ttable.GetType()
        if (TableType.DimensionTable == ttype):
            return Dimension
        else:
            return Fact

    def get_table(self, name):
        table = self.nameToTTable[name]
        return table
