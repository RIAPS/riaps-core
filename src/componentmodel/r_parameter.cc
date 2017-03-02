//
// Created by istvan on 3/1/17.
//
#include <componentmodel/r_parameter.h>


namespace riaps{
    namespace componentmodel{
        Parameters::Parameters() {

        }

        void Parameters::AddParam(std::string name, std::string value, bool isOptional, std::string defaultValue) {

            auto newItem = std::shared_ptr<Parameter>(new Parameter(name, isOptional, defaultValue));
            if (value!=""){
                newItem->SetValue(value);
            } else{
                newItem->SetValue(defaultValue);
            }

            _params[name] = newItem;
        }

        const Parameter* Parameters::GetParam(std::string name) {
            if (_params.find(name) == _params.end()){
                return NULL;
            }

            return _params[name].get();
        }

        const Parameter* Parameters::SetParamValue(std::string name, std::string value) {
            if (_params.find(name) == _params.end()){
                return NULL;
            }

            _params[name]->SetValue(value);
            return _params[name].get();
        }

        std::vector<std::string> Parameters::GetParameterNames() {
            std::vector<std::string> results;
            for (auto it = _params.begin(); it!=_params.end(); it++){
                results.push_back(it->first);
            }
            return results;
        }

        Parameters::~Parameters() {

        }

        Parameter::Parameter(std::string fieldName, bool isOptional, std::string defaultValue) {
            _paramName = fieldName;
            _paramDefaultValue = defaultValue;
            _isOptional = isOptional;
        }

        Parameter::Parameter() {

        }

        void Parameter::SetValue(std::string value) {
            _paramValue = value;
        }

        std::string Parameter::GetValueAsString() {
            return _paramValue;
        }

        bool Parameter::IsOptional() {
            return _isOptional;
        }
    }
}