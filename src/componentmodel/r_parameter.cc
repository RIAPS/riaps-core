//
// Created by istvan on 3/1/17.
//
#include <componentmodel/r_parameter.h>

namespace riaps{
    namespace componentmodel{
        Parameters::Parameters() {

        }

        void Parameters::AddParam(std::string name, std::string value, std::string defaultValue) {
            auto newItem = std::unique_ptr<Parameter>(new Parameter(name, defaultValue));
            if (value!=""){
                newItem->SetValue(value);
            } else{
                newItem->SetValue(defaultValue);
            }

            _params[name] = std::move(newItem);
        }

        const Parameter* Parameters::GetParam(std::string name) {
            if (_params.find(name) == _params.end()){
                return NULL;
            }

            return _params[name].get();
        }

        Parameter::Parameter(std::string fieldName, std::string defaultValue) {
            _paramName = fieldName;
            _paramDefaultValue = defaultValue;
        }

        Parameter::Parameter() {

        }

        void Parameter::SetValue(std::string value) {
            _paramValue = value;
        }

        std::string Parameter::GetValueAsString() {
            return _paramValue;
        }
    }
}