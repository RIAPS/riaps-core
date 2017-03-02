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

            void AddParam(std::string name, std::string value, bool isOptional, std::string defaultValue = "");
            const Parameter* SetParamValue(std::string name, std::string value);
            const Parameter* GetParam(std::string name);

            std::vector<std::string> GetParameterNames();

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

            std::string GetValueAsString();

            int GetValueAsInt();

            bool GetValueAsBool();

            bool IsOptional();

        private:
            std::string _paramName;
            std::string _paramDefaultValue;
            std::string _paramValue;
            bool        _isOptional;
        };
    }
}
#endif //RIAPS_CORE_R_PARAMETER_H
