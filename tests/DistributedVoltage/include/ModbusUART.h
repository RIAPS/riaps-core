

#ifndef MODBUSUART_H
#define MODBUSUART_H
#include <base/ModbusUARTBase.h>
// riaps:keep_header:begin
#include <modbus/modbus.h>
#include <modbus/modbus-rtu.h>
// riaps:keep_header:end>>

namespace distributedvoltage {
    namespace components {
        class ModbusUART : public ModbusUARTBase {
        public:
            ModbusUART(const py::object*  parent_actor     ,
                          const py::dict     actor_spec       ,
                          const py::dict     type_spec        ,
                          const std::string& name             ,
                          const std::string& type_name        ,
                          const py::dict     args             ,
                          const std::string& application_name ,
                          const std::string& actor_name       );


            virtual void OnClock() override;

            virtual ~ModbusUART();

            // riaps:keep_decl:begin
        private:

            enum Status {
                INIT,
                MODBUS_READY,
                RUNNING
            };

            struct serial_port_config {
                std::string portname;
                int baudrate;
                int bytesize;
                char parity;
                int stopbits;
            };

            serial_port_config port_config_;
            modbus_t* ctx_;

            int nb_holding_regs_;
            int nb_input_regs_;
            int nb_coil_bits_;
            int nb_discrete_bits_;
            std::unique_ptr<uint16_t[]> holding_regs_;
            std::unique_ptr<uint16_t[]> input_regs_;
            std::unique_ptr<uint8_t[]> coil_bits_;
            std::unique_ptr<uint8_t[]> discrete_bits_;
            bool port_open_;

            int port_slave_address_;
            int port_serial_mode_;

            Status current_status_;

            bool StartRTUModbus(int serial_mode);
            void CloseModbus();
            // riaps:keep_decl:end
        };
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
const std::string &actor_name);

#endif // MODBUSUART_H
