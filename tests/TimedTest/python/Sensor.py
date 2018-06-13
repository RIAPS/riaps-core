#Sensor.py
from riaps.run.comp import Component
import os
import logging

class Sensor(Component):
    def __init__(self):
        super(Sensor, self).__init__()	        
        self.pid = os.getpid()
        self.logger.info("(PID %s) - starting Sensor",str(self.pid))
        

    def on_clock(self):
        now = self.clock.recv_pyobj()
        self.logger.info('PID(%s) - on_clock(): %s',str(self.pid),str(now))

    def on_request(self):
        msg = self.request.recv_pyobj()
        self.logger.info("PID (%s) - on_query():%s",str(self.pid),str(msg))
    
    def __destroy__(self):			
        self.logger.info("(PID %s) - stopping Sensor",str(self.pid))   	        	        
