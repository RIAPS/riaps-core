//
// Created by istvan on 2/22/17.
//

#ifndef RIAPS_CORE_PORTMESSAGEFIELD_H
#define RIAPS_CORE_PORTMESSAGEFIELD_H

enum FieldTypes {R_INT, R_DOUBLE, R_STRING};

class PortMessageField{

    std::string _fieldValue;
    std::string _fieldName;

public:
    PortMessageField(FieldTypes fieldType);

    const int*         GetInt();
    const double*      GetDouble();
    const std::string* GetString();


    ~PortMessageField();



protected:
    FieldTypes  _fieldType;
};

#endif //RIAPS_CORE_PORTMESSAGEFIELD_H
