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

            void AddParam(const Parameter& param);
            void AddParam(std::string name, std::string value, bool isOptional, std::string defaultValue = "");
            const Parameter* SetParamValue(std::string name, std::string value);
            const Parameter* GetParam(const std::string& name) const;

            std::vector<std::string>      GetParameterNames();

            ~Parameters();

        private:
            std::map<std::string, std::shared_ptr<Parameter>> _params;
        };

        // Todo: check whether templates can be used here?
        class Parameter {
        public:
            Parameter(std::string fieldName, bool isOptional, std::string defaultValue = "");
            Parameter();

            void SetValue(std::string value);

            const std::string& GetValueAsString() const;

            int GetValueAsInt() const;

            bool GetValueAsBool() const;

            const std::string& GetName() const;
            const std::string& GetDefaultValue() const;

            bool IsOptional() const;

        private:
            std::string _paramName;
            std::string _paramDefaultValue;
            std::string _paramValue;
            bool        _isOptional;
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
