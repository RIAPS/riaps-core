

Handling Timer Events
=====================
**Model definition:**

.. code-block:: bash

   app DistributedEstimator {
     component Sensor {
       timer clock 1000; // Periodic timer, calls the clock callback in every second
     }
   }

**C++ Implementation:**

.. code-block:: cpp

   namespace distributedestimator {
      namespace components {
         void Sensor::OnClock(riaps::ports::PortBase *port) {

         }
      }
   }

Send Messages on Publish Port
=============================
**Model definition:**

.. code-block:: bash

   app distributedestimator {
     component Sensor {
       timer clock 1000;
       pub ready : SensorReady;
     }
   }

**C++ Implementation:**

.. code-block:: cpp

   namespace distributedestimator {
      namespace components {
         void Sensor::OnClock(riaps::ports::PortBase *port) {
            capnp::MallocMessageBuilder messageBuilder;
            auto msgSensorReady = messageBuilder.initRoot<messages::SensorReady>();
            msgReady.setMsg("data_ready");
            SendReady(messageBuilder, msgReady);
         }
      }
   }

Receive Messages on Subscribe Port
==================================
**Model definition:**

.. code-block:: bash

   app distributedestimator {
     component LocalEstimator {
       sub ready : SensorReady;
     }
   }

**C++ Implementation:**

.. code-block:: cpp

   namespace distributedestimator {
      namespace components {
         void LocalEstimator::OnReady(const messages::SensorReady::Reader &message,
                                      riaps::ports::PortBase *port) {
            component_logger()->info("{}: {} {}", __func__, message.getMsg().cStr(), ::getpid());
         }
      }
   }

Define Message Types
====================

RIAPS is using `Cap'n Proto <https://capnproto.org/>`_ for defining messages.

**Cap'n Proto Schema for SensorReady**

.. code-block:: capnp

   using Cxx = import "/capnp/c++.capnp";
   $Cxx.namespace("distributedestimator::messages");

   struct SensorReady {
      msg @0 : Text;
   }

**Generate C++ Code from Capn'p Schema**

.. code-block:: shell

   capnp compile ./filename.capnp -oc++:../