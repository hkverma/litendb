"""
Liten Service
"""
import litendb.lib as cliten

class Service:
    """
    Service Class
    """
    tservice = cliten.TService()
    
    def __init__(self):
        """
        Create and initialize Liten Serice
        """
        pass
    
    def start(self):
        """
        Start Liten Service
        """
        return Service.tservice.start()

    def shutdown(self):
        """
        Shutdown Liten Service
        """
        return Service.tservice.shutdown()
    
