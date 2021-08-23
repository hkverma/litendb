"""
Utility functions used across different files
"""
import codecs

def to_bytes(s):
    """
    to_bytes(s)
    Convert various string types to utf-8 bytes
    Parameters
      s: input string type
    Returns
      utf-8 byte string
    """
    if type(s) is bytes:
        return s
    elif type(s) is str or (sys.version_info[0] < 3 and type(s) is unicode):
        return codecs.encode(s, 'utf-8')
    else:
        raise TypeError("Expected bytes or string, but got %s." % type(s))
