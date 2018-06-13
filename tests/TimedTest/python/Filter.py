#Filter.py
from riaps.run.comp import Component
import os
import logging

class Filter(Component):
    def __init__(self):
        super(Filter, self).__init__()	        
        self.pid = os.getpid()
        self.logger.info("(PID %s) - starting Filter",str(self.pid))
        

    def on_ready(self):
        msg = self.ready.recv_pyobj()
        self.logger.info("PID (%s) - on_ready():%s",str(self.pid),str(msg))

    def on_query(self):
        req = self.query.recv_pyobj()
        self.logger.info("PID (%s) - on_query():%s",str(self.pid),str(req))
    
    def __destroy__(self):			
        self.logger.info("(PID %s) - stopping Filter",str(self.pid))   	        	        
