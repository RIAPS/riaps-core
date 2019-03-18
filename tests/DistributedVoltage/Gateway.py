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
        bytes = self.currentvoltage.recv()
        msg = distributedvoltage_capnp.Voltage.from_bytes(bytes)

        timestamp = msg.time
        values    = msg.values

        s = ""
        for i in values:
            s = f'{s} {i} at {timestamp.tvSpec}.{timestamp.tvNspec}'
        self.logger.info("values arrived: %s" % s)
        pass
# riaps:keep_currentvoltage:end

# riaps:keep_impl:begin

# riaps:keep_impl:end