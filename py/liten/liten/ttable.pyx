cdef class TTable:
     """
     Liten Table Class
     """
    DimTable=0
    FactTable=1
    
    def __cinit__(self, ttable):
        """
        Create and initialize Liten Cache
        """
        sp_ttable = ttable
        p_ttable = sp_ttable.get()
        sp_ctable = p_ttable.GetTable()

    def get_arrow_table(self):
        pa_table = pyarrow_wrap_table(sp_ctable)
        return pa_table
