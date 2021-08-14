import liten as ten

cdef class TSchema:

    nameToTSchema = { }
    
    def __cinit__(self, name, ttype, schema):
        """
        Add arrow schema in cache by name
        Parameters
           name: name of schema
           ttype: type of table must be DimTable or FactTable
           schema: arrow schema to be added in liten cache
        """
        sp_schema = pyarrow_unwrap_schema(schema)
        if ttype != ten.TTable.Dimension and ttype != ten.TTable.Fact:
            print("Error: Schema type must be Dimension or Fact")
            return None
        tc_ttype = <TableType>ttype
        sp_tschema_result = self.tcache.AddSchema(ten.litenutils.to_bytes(name), tc_ttype, sp_schema)
        if (not sp_tschema_result.ok()):
            print ("Failed to add schema=", name)
            return None
        sp_tschema = sp_tschema_result.ValueOrDie()
        p_tschema = sp_tschema.get()
        if (NULL == p_tschema):
            print ("Failed to add schema=", name)
            return None
        nameToTSchema[name] = self

    def get_arrow_schema(self):
        """
        Get pyarrow schema from Liten schema
        """
        pa_schema = pyarrow_wrap_schema(sp_schema)
        return pa_schema
    
    def get_name(self):
        """
        Returns
          unique name of the table
        """
        name = p_tschema.GetName()
        return ten.litenutils.to_bytes(name)

    def get_type(self):
        """
        Returns
          Dimension or Fact Table
        """
        if (ten.TableType.Dimension == tc_ttype):
            return ten.TableType.Dimension
        else:
            return ten.TableType.Fact

    def get_schema(self, name):
        schema = self.nameToTSchema[name]
        return schema
        
