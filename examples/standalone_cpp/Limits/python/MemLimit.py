#MemLimit.py
from riaps.run.comp import Component
import os
import logging

class MemLimit(Component):
    def __init__(self):
        super(MemLimit, self).__init__()	        
        self.pid = os.getpid()
        self.logger.info("(PID %s) - starting MemLimit, %s",str(self.pid),str(now))
        
    def on_ticker(self):
        now = self.ticker.recv_pyobj()
        self.logger.info('PID(%s) - on_ticker(): %s',str(self.pid),str(now))
    
	def __destroy__(self):			
		self.logger.info("(PID %s) - stopping MemLimit, %s",str(self.pid),now)   	        	        
