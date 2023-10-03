
import sys
import codecs

class Table:
    """
    Table Class
    """
    def __init__(self, ttable):
        """
        Create and initialize Liten Cache
        """
        if (None == ttable):
            raise ValueError("Invalid value for Liten table")
        self.ttable = ttable
    
    def get_pyarrow_table(self):
        """
        Returns
          Arrow table
        """
        return self.ttable.get_pyarrow_table()

    def get_name(self):
        """
        Returns
          unique name of the table
        """
        return self.ttable.get_name()
    
    def get_type(self):
        """
        Returns
          Dimension or Fact Table
        """
        return self.ttable.get_type()
