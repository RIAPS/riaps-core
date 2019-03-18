


#include <ModbusUART.h>
// riaps:keep_header:begin

// riaps:keep_header:end

namespace distributedvoltage {
    namespace components {

        // riaps:keep_construct:begin
        ModbusUART::ModbusUART(const py::object*  parent_actor     ,
                      const py::dict     actor_spec       ,
                      const py::dict     type_spec        ,
                      const std::string& name             ,
                      const std::string& type_name        ,
                      const py::dict     args             ,
                      const std::string& application_name ,
                      const std::string& actor_name       )
            : ModbusUARTBase(parent_actor, actor_spec, type_spec, name, type_name, args, application_name, actor_name) {
            auto pid = ::getpid();
            current_status_ = ModbusUART::Status::INIT;
            ctx_ = nullptr;

            sched_param x{85};
            auto ret = sched_setscheduler(0, SCHED_FIFO, &x);
            if (ret!=0) {
                component_logger()->warn("Failed to set scheduler");
            } else {
                component_logger()->info("Component is using FIFO scheduler, priority: {}", x.sched_priority);
            }

            // Configure Modbus UART port parameters
            auto iport = component_config().component_parameters.getParam("port");
            if (iport != nullptr) {
                std::string iportStr = iport->getValueAsString();
                if (iportStr == "UART1") {
                    port_config_.portname = "/dev/ttyO1";
                } else if (iportStr == "UART2") {
                    port_config_.portname = "/dev/ttyO2";
                } else if (iportStr == "UART3") {
                    port_config_.portname = "/dev/ttyO3";
                } else if (iportStr == "UART4") {
                    port_config_.portname = "/dev/ttyO4";
                } else if (iportStr == "UART5") {
                    port_config_.portname = "/dev/ttyO5";
                } else {
                    component_logger()->error("{}: Invalid UART argument (port={}), use port = UART1..5", pid,
                                   iport->getValueAsString());
                }
            } else {
                // Declare a default value
                port_config_.portname = "/dev/ttyO2";
            }

            auto ibaudrate = component_config().component_parameters.getParam("baudrate");
            if (ibaudrate != nullptr) {
                if(!ibaudrate->getValueAsInt(&(port_config_.baudrate))) {
                    component_logger()->error("{}: Invalid Baud Rate argument - {}, should be integer value", pid, port_config_.baudrate);
                }
            } else {
                // Declare a default value
                port_config_.baudrate = 115200;
            }

            auto ibytesize = component_config().component_parameters.getParam("bytesize");
            if (ibytesize != nullptr) {
                if(!ibytesize->getValueAsInt(&(port_config_.bytesize))) {
                    component_logger()->error("{}: Invalid Byte Size argument - {}, should be integer value", pid, port_config_.bytesize);
                }
            } else {
                // Declare a default value
                port_config_.bytesize = 8;
            }

            auto iparity = component_config().component_parameters.getParam("parity");
            if (iparity != nullptr) {
                port_config_.parity = iparity->getValueAsString()[0];
            } else {
                // Declare a default value
                port_config_.parity = 'N';
            }

            auto istopbits = component_config().component_parameters.getParam("stopbits");
            if (istopbits != nullptr) {
                if(!istopbits->getValueAsInt(&(port_config_.stopbits))) {
                    component_logger()->error("{}: Invalid Stop Bits argument - {}, should be integer value", pid, port_config_.stopbits);
                }
            } else {
                // Declare a default value
                port_config_.stopbits = 1;
            }

            auto islaveaddress = component_config().component_parameters.getParam("slaveaddress");
            if (islaveaddress != nullptr) {
                if(!islaveaddress->getValueAsInt(&(port_slave_address_))) {
                    component_logger()->error("{}: Invalid Port Address argument - {}, should be integer value", pid, port_slave_address_);
                }
            } else {
                // Declare a default value
                port_slave_address_ = 10;
            }

            // Configure number of Modbus data registers used
            auto inumholdreg = component_config().component_parameters.getParam("numholdreg");
            if (inumholdreg != nullptr) {
                if(!inumholdreg->getValueAsInt(&(nb_holding_regs_))) {
                    component_logger()->error("{}: Invalid Number of Holding Registers argument - {}, should be integer value", pid, nb_holding_regs_);
                }
            } else {
                // Set a default of 3 (an arbritary number based on the first example created)
                nb_holding_regs_ = 3;
            }

            auto inuminputreg = component_config().component_parameters.getParam("numinputreg");
            if (inuminputreg != nullptr) {
                if(!inuminputreg->getValueAsInt(&(nb_input_regs_))) {
                    component_logger()->error("{}: Invalid Number of Input Registers argument - {}, should be integer value", pid, nb_input_regs_);
                }
            } else {
                // Set a default of 4 (an arbritary number based on the first example created)
                nb_input_regs_ = 4;
            }

            auto inumcoilbits = component_config().component_parameters.getParam("numcoilbits");
            if (inumcoilbits != nullptr) {
                if(!inumcoilbits->getValueAsInt(&(nb_coil_bits_))) {
                    component_logger()->error("{}: Invalid Number of Coil Bits argument - {}, should be integer value", pid, nb_coil_bits_);
                }
            } else {
                // Set a default of 8 (an arbritary number based on the first example created)
                nb_coil_bits_ = 8;
            }

            auto inumdiscretebits = component_config().component_parameters.getParam("numdiscretebits");
            if (inumdiscretebits != nullptr) {
                if(!inumdiscretebits->getValueAsInt(&(nb_discrete_bits_))) {
                    component_logger()->error("{}: Invalid Number of Discreet Bits argument - {}, should be integer value", pid, nb_discrete_bits_);
                }
            } else {
                // Set a default of 8 (an arbritary number based on the first example created)
                nb_discrete_bits_ = 8;
            }

            // Modbus port initially closed
            port_open_ = false;
            port_serial_mode_ = MODBUS_RTU_RS232;

            // Setup Modbus Data Registers
            holding_regs_  = std::unique_ptr<uint16_t[]>(new uint16_t[nb_holding_regs_]);
            input_regs_   = std::unique_ptr<uint16_t[]>(new uint16_t[nb_input_regs_]);
            coil_bits_     = std::unique_ptr<uint8_t[]>(new uint8_t[nb_coil_bits_]);
            discrete_bits_ = std::unique_ptr<uint8_t[]>(new uint8_t[nb_discrete_bits_]);

            component_logger()->info("{}: Modbus UART settings {} @{}:{} {}{}{}", pid, port_slave_address_, port_config_.portname,
                          port_config_.baudrate, port_config_.bytesize, port_config_.parity, port_config_.stopbits);
            component_logger()->info(
                    "{}: Modbus Reg settings: numCoilBits={}, numDiscreteBits={}, numInputRegs={}, numHoldingRegs={}",
                    pid, nb_coil_bits_, nb_discrete_bits_, nb_input_regs_, nb_holding_regs_);

            /* Initialize the libmodbus context
            * Returns a pointer to a modbus_t structure if successful. Otherwise it shall return NULL.
            *   where, baudrate: 9600, 19200, 57600, 115200, etc
            *          Parity: 'N', 'O', 'E'
            *          Slave Address is a decimal value
            *          Serial Mode is either RS232 (0) or RS485 (1) -  RS232 mode is default
            */
            ctx_ = modbus_new_rtu(port_config_.portname.c_str(), port_config_.baudrate, port_config_.parity,
                                 port_config_.bytesize, port_config_.stopbits);
            if (ctx_ == nullptr) {
                component_logger()->error("Unable to create the libmodbus context");
            }

            //_logger->debug("Init vars");
            //int resResult = clock_getres(CLOCK_MONOTONIC, &resolution);

            //if (resResult != 0) {
            //    _logger->debug("Error occurred {}", strerror(errno));
            //    exit(errno);
            //}
        }
        // riaps:keep_construct:end

        void ModbusUART::OnClock() {
            // riaps:keep_onclock:begin
            auto msg = RecvClock();

            // Init the modbus
            if (current_status_ == ModbusUART::Status::INIT && ctx_ != nullptr) {
                if (StartRTUModbus(port_serial_mode_)) {
                    component_logger()->debug("{}: Started Modbus: port={}, slaveAddress={}", ::getpid(), port_config_.portname, port_slave_address_);
                    current_status_ = ModbusUART::Status::MODBUS_READY;
                }
            }
            // Modbus is ready, sync the clocks
            else if (current_status_ == ModbusUART::Status::MODBUS_READY) {
                timespec current_time{0,0};
                clock_gettime(CLOCK_REALTIME, &current_time);
                timespec wake_up = {
                        ((current_time.tv_sec + 2) / 10 + 1) * 10,
                        0
                };
                auto timer_port = GetPortAs<riaps::ports::PeriodicTimer>(PORT_TIMER_CLOCK);
                timer_port->Halt();
                while (wake_up.tv_sec>current_time.tv_sec) {
                    clock_gettime(CLOCK_REALTIME, &current_time);
                }
                timer_port->Start();
                current_status_ = ModbusUART::Status::RUNNING;
            } else if (current_status_ == ModbusUART::Status::RUNNING) {
                int addr = 0;
                int nb   = 4;

                timespec now{0, 0};
                clock_gettime(CLOCK_REALTIME, &now);
                auto regs_read = modbus_read_input_registers(ctx_, addr, nb, input_regs_.get());
                if (regs_read == -1) {
                    component_logger()->error("Failed to read input registers: {}.  Address={}, #Reg={}", modbus_strerror(errno), addr, nb);
                } else {
                    MessageBuilder<messages::Voltage> message;
                    auto values    = message->initValues(nb);
                    auto timestamp = message->initTime();
                    timestamp.setTvSpec(now.tv_sec);
                    timestamp.setTvNspec(now.tv_nsec);
                    for (uint i = 0; i<nb; i++) {
                        values.set(i, input_regs_[i]);
                    }
                    SendCurrentvoltage(message);
                }

            }

            // riaps:keep_onclock:end
        }

        // riaps:keep_impl:begin

        /* Close Modbus */
        void ModbusUART::CloseModbus() {
            if (ctx_!=nullptr) {
                modbus_close(ctx_);
                modbus_free(ctx_);
                ctx_ = nullptr;
            }
            port_open_ = false;
            component_logger()->info("Modbus closed");
        }

        bool ModbusUART::StartRTUModbus(int serial_mode) {

            // Enable debugging of libmodbus
            modbus_set_debug(ctx_, false);

            modbus_set_slave(ctx_, port_slave_address_);

            if (serial_mode == MODBUS_RTU_RS485) {
                if (modbus_rtu_set_serial_mode(ctx_, serial_mode) == -1) {
                    component_logger()->error("Invalid Serial Mode: {}.  SerialMode={}", modbus_strerror(errno), serial_mode);
                    modbus_free(ctx_);
                    return false;
                } else {
                    component_logger()->debug("Modbus serial mode set to RS485");
                }
            }

            if (modbus_connect(ctx_) == -1) {
                component_logger()->error("Connection failed: {}", modbus_strerror(errno));
                modbus_free(ctx_);
                return false;
            } else {
                port_open_ = true;
                component_logger()->info("Modbus port open. Port={}, Baud={}, {}{}{}", port_config_.portname, port_config_.baudrate,
                              port_config_.bytesize, port_config_.parity, port_config_.stopbits);
            }

            return port_open_;
        }

        // riaps:keep_impl:end

        // riaps:keep_destruct:begin
        ModbusUART::~ModbusUART() {
            CloseModbus();
        }
        // riaps:keep_destruct:end

    }
}

std::unique_ptr<distributedvoltage::components::ModbusUART>
create_component_py(const py::object *parent_actor,
                    const py::dict actor_spec,
                    const py::dict type_spec,
                    const std::string &name,
                    const std::string &type_name,
                    const py::dict args,
                    const std::string &application_name,
                    const std::string &actor_name) {
    auto ptr = new distributedvoltage::components::ModbusUART(parent_actor, actor_spec, type_spec, name, type_name, args,
                                                                     application_name,
                                                                     actor_name);
    return std::move(std::unique_ptr<distributedvoltage::components::ModbusUART>(ptr));
}

PYBIND11_MODULE(libmodbusuart, m) {
    py::class_<distributedvoltage::components::ModbusUART> testClass(m, "ModbusUART");
    testClass.def(py::init<const py::object*, const py::dict, const py::dict, const std::string&, const std::string&, const py::dict, const std::string&, const std::string&>());

    testClass.def("setup"                 , &distributedvoltage::components::ModbusUART::Setup);
    testClass.def("activate"              , &distributedvoltage::components::ModbusUART::Activate);
    testClass.def("terminate"             , &distributedvoltage::components::ModbusUART::Terminate);
    testClass.def("handlePortUpdate"      , &distributedvoltage::components::ModbusUART::HandlePortUpdate);
    testClass.def("handleCPULimit"        , &distributedvoltage::components::ModbusUART::HandleCPULimit);
    testClass.def("handleMemLimit"        , &distributedvoltage::components::ModbusUART::HandleMemLimit);
    testClass.def("handleSpcLimit"        , &distributedvoltage::components::ModbusUART::HandleSpcLimit);
    testClass.def("handleNetLimit"        , &distributedvoltage::components::ModbusUART::HandleNetLimit);
    testClass.def("handleNICStateChange"  , &distributedvoltage::components::ModbusUART::HandleNICStateChange);
    testClass.def("handlePeerStateChange" , &distributedvoltage::components::ModbusUART::HandlePeerStateChange);
    testClass.def("handleReinstate"       , &distributedvoltage::components::ModbusUART::HandleReinstate);

    m.def("create_component_py", &create_component_py, "Instantiates the component from python configuration");
}

