# riaps:keep_import:begin
from riaps.run.comp import Component
import logging
import capnp
import timersync_capnp
from enum import Enum


class AppState(Enum):
    INIT = 0        #Just started up, isn't yet trying to synchronize
    SYNC = 1        #Currently waiting until the synchronization point
    RUNNING = 2     #Synchronization point passed, all 'periodic' ports roughly in sync 

# riaps:keep_import:end

class CompA(Component):

# riaps:keep_constr:begin
    def __init__(self):
        super(CompA, self).__init__()
        

# riaps:keep_constr:end

# riaps:keep_periodic:begin
    def on_clock(self):
        pass
# riaps:keep_periodic:end


# riaps:keep_impl:begin

# riaps:keep_impl:end