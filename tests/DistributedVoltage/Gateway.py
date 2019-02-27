# riaps:keep_import:begin
from riaps.run.comp import Component
import logging
import capnp
import distributedvoltage_capnp

# riaps:keep_import:end

class Gateway(Component):

# riaps:keep_constr:begin
    def __init__(self):
        super(Gateway, self).__init__()
# riaps:keep_constr:end

# riaps:keep_currentvoltage:begin
    def on_currentvoltage(self):
        pass
# riaps:keep_currentvoltage:end

# riaps:keep_impl:begin

# riaps:keep_impl:end