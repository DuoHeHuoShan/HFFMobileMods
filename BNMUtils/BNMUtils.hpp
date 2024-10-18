#pragma once

#include <BNM/PropertyBase.hpp>
#include <BNM/MethodBase.hpp>
#include <BNM/FieldBase.hpp>
#include <BNM/Class.hpp>

/*
BNMU_BeginDefineClass(ExampleNamespace::ExampleClass, ExampleNamespace, ExampleClass)
    BNMU_DefineField(BNM::Field<int>, exampleField)
    BNMU_DefineMethod(BNM::MethodBase, exampleMethod, -1)
    BNMU_DefineProperty(BNM::PropertyBase, exampleProperty)
BNMU_EndDefineClass()
*/


#define BNMU_BeginDefineClass(classname, namezpace, name) namespace classname { \
    static BNMUClassDefinition classDefinition(namezpace, #name); \

#define BNMU_DefineField(type, fieldName) static type fieldName; \
static BNMUFieldDefinition _##fieldName(classDefinition, fieldName, #fieldName);

#define BNMU_DefineMethod(type, methodName, parameters) static type methodName; \
static BNMUMethodDefinition _##methodName(classDefinition, methodName, #methodName, parameters);

#define BNMU_DefineProperty(type, propertyName) static type propertyName; \
static BNMUPropertyDefinition _##propertyName(classDefinition, propertyName, #propertyName);

#define BNMU_EndDefineClass() };

class BNMUFieldDefinition;
class BNMUMethodDefinition;
class BNMUPropertyDefinition;
class BNMUClassDefinition {
public:
    static std::vector<BNMUClassDefinition *> classDefinitions;
    std::vector<BNMUFieldDefinition> fieldDefinitions;
    std::vector<BNMUMethodDefinition> methodDefinitions;
    std::vector<BNMUPropertyDefinition> propertyDefinitions;
    std::string_view namezpace;
    std::string_view name;
    BNM::Class clazz;

    BNMUClassDefinition(std::string_view namezpace, std::string_view name) : namezpace(namezpace), name(name) {
        classDefinitions.emplace_back(this);
    }
};
std::vector<BNMUClassDefinition *> BNMUClassDefinition::classDefinitions = std::vector<BNMUClassDefinition *>();

class BNMUFieldDefinition {
public:
    std::string_view fieldName;
    BNM::FieldBase &field;
    BNMUClassDefinition &classDefinition;
    BNMUFieldDefinition(BNMUClassDefinition &classDefinition, BNM::FieldBase &field, std::string_view fieldName) : field(field), fieldName(fieldName), classDefinition(classDefinition) {
        classDefinition.fieldDefinitions.emplace_back(*this);
    }
};

class BNMUMethodDefinition {
public:
    std::string_view methodName;
    BNM::MethodBase &method;
    BNMUClassDefinition &classDefinition;
    int parameters = -1;
    BNMUMethodDefinition(BNMUClassDefinition &classDefinition, BNM::MethodBase &method, std::string_view methodName, int parameters = -1) : classDefinition(classDefinition), method(method), methodName(methodName), parameters(parameters) {
        classDefinition.methodDefinitions.emplace_back(*this);
    }
};

class BNMUPropertyDefinition {
public:
    std::string_view propertyName;
    BNM::PropertyBase &property;
    BNMUClassDefinition &classDefinition;
    BNMUPropertyDefinition(BNMUClassDefinition &classDefinition, BNM::PropertyBase &property, std::string_view propertyName) : property(property), propertyName(propertyName), classDefinition(classDefinition) {
        classDefinition.propertyDefinitions.emplace_back(*this);
        BNM_LOG_ERR("%d %d", &BNMUClassDefinition::classDefinitions[0], &classDefinition);
    }
};

void BNMU_OnLoaded() {
    for(BNMUClassDefinition *classDefinition : BNMUClassDefinition::classDefinitions) {
        classDefinition->clazz = BNM::Class(classDefinition->namezpace, classDefinition->name);
        for(BNMUFieldDefinition &fieldDefinition : classDefinition->fieldDefinitions) {
            fieldDefinition.field = classDefinition->clazz.GetField(fieldDefinition.fieldName);
        }
        for(BNMUMethodDefinition &methodDefinition : classDefinition->methodDefinitions) {
            methodDefinition.method = classDefinition->clazz.GetMethod(methodDefinition.methodName);
        }
        for(BNMUPropertyDefinition &propertyDefinition : classDefinition->propertyDefinitions) {
            propertyDefinition.property = classDefinition->clazz.GetProperty(propertyDefinition.propertyName);
        }
    }
}