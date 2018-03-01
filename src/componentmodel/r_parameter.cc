//
// Created by istvan on 3/1/17.
//
#include <componentmodel/r_parameter.h>
#include <ios>
#include <sstream>


namespace riaps{
    namespace componentmodel{
        Parameters::Parameters() {

        }

        void Parameters::addParam(std::string name, std::string value, bool isOptional, std::string defaultValue) {

            auto newItem = std::shared_ptr<Parameter>(new Parameter(name, isOptional, defaultValue));
            if (value!=""){
                newItem->setValue(value);
            } else{
                newItem->setValue(defaultValue);
            }

            m_params[name] = newItem;
        }

        void Parameters::addParam(const Parameter &param) {
            addParam(param.getName(), param.getValueAsString(), param.isOptional(), param.getDefaultValue());
        }



        const Parameter* Parameters::getParam(const std::string &name) const {
            if (m_params.find(name) == m_params.end()){
                return nullptr;
            }
            auto ptr = (m_params.find(name))->second;

            return ptr.get();
        }

        const Parameter* Parameters::setParamValue(std::string name, std::string value) {
            if (m_params.find(name) == m_params.end()){
                return nullptr;
            }

            m_params[name]->setValue(value);
            return m_params[name].get();
        }

        std::vector<std::string> Parameters::getParameterNames() {
            std::vector<std::string> results;
            for (auto it = m_params.begin(); it!=m_params.end(); it++){
                results.push_back(it->first);
            }
            return results;
        }



        Parameters::~Parameters() {

        }

        Parameter::Parameter(std::string fieldName, bool isOptional, std::string defaultValue) {
            m_paramName = fieldName;
            m_paramDefaultValue = defaultValue;
            m = isOptional;
        }

        Parameter::Parameter() {

        }

        const std::string& Parameter::getDefaultValue() const {
            return m_paramDefaultValue;
        }

        const std::string& Parameter::getName() const {
            return m_paramName;
        }

        void Parameter::setValue(std::string value) {
            m_paramValue = value;
        }

        const std::string& Parameter::getValueAsString() const{
            return m_paramValue;
        }

        bool Parameter::getValueAsInt(int *intValue) const {
            try{
                *intValue = std::stoi(m_paramValue);
            }catch(std::invalid_argument& e){
                return false;
            } catch(std::out_of_range& e){
                return false;
            }
            return true;
        }

        bool Parameter::getValueAsBool() const {
            bool b;
            std::stringstream(m_paramValue) >> std::boolalpha >> b;
            return b;
        }

        bool Parameter::isOptional() const {
            return m;
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