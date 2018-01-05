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

        void Parameters::AddParam(const Parameter &param) {
            AddParam(param.GetName(), param.GetValueAsString(), param.IsOptional(), param.GetDefaultValue());
        }



        const Parameter* Parameters::GetParam(const std::string& name) const {
            if (_params.find(name) == _params.end()){
                return nullptr;
            }
            auto ptr = (_params.find(name))->second;

            return ptr.get();
        }

        const Parameter* Parameters::SetParamValue(std::string name, std::string value) {
            if (_params.find(name) == _params.end()){
                return nullptr;
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

        const std::string& Parameter::GetDefaultValue() const {
            return _paramDefaultValue;
        }

        const std::string& Parameter::GetName() const {
            return _paramName;
        }

        void Parameter::SetValue(std::string value) {
            _paramValue = value;
        }

        const std::string& Parameter::GetValueAsString() const{
            return _paramValue;
        }

        bool Parameter::IsOptional() const {
            return _isOptional;
        }

        ComponentActual::ComponentActual(const std::string &paramName, const std::string &paramValue, bool hasReferred)
            : _paramName(paramName),
              _paramValue(paramValue),
              _hasParamReferred(hasReferred){
        }


        const std::string& ComponentActual::GetParamValue() const {
            if (_hasParamReferred){
                throw std::runtime_error("Cannot query param value of referred parameter here.");
            }
            return _paramValue;
        }

        const std::string& ComponentActual::GetReferredParamName() const {
            if (_hasParamReferred) return _paramValue;
            throw std::runtime_error("Cannot query referred param here. It is value type param.");
        }

        bool ComponentActual::HasValue() {
            return !_hasParamReferred;
        }

        bool ComponentActual::HasParamReferred() {
            return _hasParamReferred;
        }

        const std::string& ComponentActual::GetParamName() const {
            return _paramName;
        }

        ComponentActual::~ComponentActual() {

        }
    }
}