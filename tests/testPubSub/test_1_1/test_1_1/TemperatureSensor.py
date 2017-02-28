#
from riaps.run.comp import Component
import logging
import os
import sys

class TemperatureSensor(Component):
    def __init__(self, logfile):
        super(TemperatureSensor, self).__init__()

        logpath = '/tmp/' + logfile
        try:
            os.remove(logpath)
        except OSError:
            pass

        self.pid = os.getpid()
        self.pending = 0

        self.testlogger = logging.getLogger(__name__)
        self.testlogger.setLevel(logging.DEBUG)
        self.fh = logging.FileHandler(logpath)
        self.fh.setLevel(logging.DEBUG)
        self.testlogger.addHandler(self.fh)
        self.messageCounter = 0

    def on_clock(self):
        msg = self.clock.recv_pyobj()
        #self.testlogger.info("[%d] on_clock():%s [%d]", msg, self.pid)
        self.messageCounter += 1
        msg = self.messageCounter
        self.sendTemperature.send_pyobj(msg)
        self.testlogger.info("Sent messages: %d", self.messageCounter)


