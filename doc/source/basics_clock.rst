Basics
######

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
            std::cout << "LocalEstimator::OnReady(): " << message.getMsg().cStr() << " " << ::getpid() << std::endl;
            //TODO: NO cout, just spdlog
         }
      }
   }

Define Message Types
====================

RIAPS is using `Cap'n Proto <https://capnproto.org/>`_ for defining messages.
