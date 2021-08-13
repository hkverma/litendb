cdef class TSchema:

        def __cinit__(self, tschema):
        """
        Create and initialize Liten Cache
        """
        sp_tschema = tschema
        p_tschema = sp_tschema.get()
        if (NULL == p_tschema):
            print ("Failed to get schema=", name)
            return None
        sp_schema = p_tschema.GetSchema()

    def get_arrow_schema(self):
        """
        Get pyarrow schema from Liten schema
        """
        pa_schema = pyarrow_wrap_schema(sp_schema)
        return pa_schema
