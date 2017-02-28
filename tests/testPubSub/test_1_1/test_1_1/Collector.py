# import riaps
from riaps.run.comp import Component
import logging
import os

class Collector(Component):
    def __init__(self, logfile):
        super(Collector, self).__init__()

        logpath = '/tmp/' + logfile
        try:
            os.remove(logpath)
        except OSError:
            pass

        self.testlogger = logging.getLogger(__name__)
        self.testlogger.setLevel(logging.DEBUG)
        self.fh = logging.FileHandler(logpath)
        self.fh.setLevel(logging.DEBUG)
        self.testlogger.addHandler(self.fh)
        self.messageCounter = 0
        
    def on_getTemperature(self):
        msg = self.getTemperature.recv_pyobj()
        self.testlogger.info("Received messages: %d", msg)


    




