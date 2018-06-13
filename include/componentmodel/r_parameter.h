//
// Created by istvan on 3/1/17.
//

#ifndef RIAPS_CORE_R_PARAMETER_H
#define RIAPS_CORE_R_PARAMETER_H

#include <string>
#include <map>
#include <memory>
#include <vector>

namespace riaps {

    namespace componentmodel {

        class Parameter;

        class Parameters {
        public:
            Parameters();

            void addParam(const Parameter &param);
            void addParam(std::string name, std::string value, bool isOptional, std::string defaultValue = "");
            const Parameter* setParamValue(std::string name, std::string value);
            const Parameter* getParam(const std::string &name) const;

            std::vector<std::string>      getParameterNames();

            ~Parameters();

        private:
            std::map<std::string, std::shared_ptr<Parameter>> m_params;
        };

        // Todo: check whether templates can be used here?
        class Parameter {
        public:
            Parameter(std::string fieldName, bool isOptional, std::string defaultValue = "");
            Parameter();

            void setValue(std::string value);

            const std::string& getValueAsString() const;

            bool getValueAsInt(int *intValue) const;

            bool getValueAsBool() const;

            const std::string& getName() const;
            const std::string& getDefaultValue() const;

            bool isOptional() const;

        private:
            std::string m_paramName;
            std::string m_paramDefaultValue;
            std::string m_paramValue;
            bool        m;
        };

        class ComponentActual{

        public:
            ComponentActual(const std::string& paramName,
                            const std::string& paramValue,
                            bool  hasReferred);



            const std::string& GetParamName()        const;
            const std::string& GetParamValue()       const;
            const std::string& GetReferredParamName() const;

            bool HasValue();
            bool HasParamReferred();

            ~ComponentActual();

        private:
            std::string _paramName;
            std::string _paramValue;
            bool        _hasParamReferred;

        };
    }
}
#endif //RIAPS_CORE_R_PARAMETER_H
