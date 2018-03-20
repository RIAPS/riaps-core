#GroupComp.py
from riaps.run.comp import Component
import os
import logging

class GroupComp(Component):
    def __init__(self):
        super(GroupComp, self).__init__()	        
        self.pid = os.getpid()
        self.logger.info("(PID %s) - starting GroupComp",str(self.pid))
        

    def on_clock(self):
        now = self.clock.recv_pyobj()
        self.logger.info('PID(%s) - on_clock(): %s',str(self.pid),str(now))
    
    def __destroy__(self):			
        self.logger.info("(PID %s) - stopping GroupComp",str(self.pid))   	        	        
