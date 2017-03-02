//
// Created by istvan on 3/1/17.
//

#ifndef RIAPS_CORE_R_PARAMETER_H
#define RIAPS_CORE_R_PARAMETER_H

#include <string>
#include <map>
#include <memory>

namespace riaps {

    namespace componentmodel {

        class Parameter;

        class Parameters {
        public:
            Parameters();

            void AddParam(std::string name, std::string value, std::string defaultValue = "");

            const Parameter *GetParam(std::string name);

        private:
            std::map<std::string, std::unique_ptr<Parameter>> _params;
        };

        // Todo: with templates
        class Parameter {
        public:
            Parameter(std::string fieldName, std::string defaultValue = "");
            Parameter();

            void SetValue(std::string value);

            std::string GetValueAsString();

            int GetValueAsInt();

            bool GetValueAsBool();

        private:
            std::string _paramName;
            std::string _paramDefaultValue;
            std::string _paramValue;
        };
    }
}
#endif //RIAPS_CORE_R_PARAMETER_H
