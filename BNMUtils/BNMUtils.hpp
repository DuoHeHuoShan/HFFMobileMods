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

#define BNMU_DefineMethod(type, methodName, ...) static type methodName; \
static BNMUMethodDefinition _##methodName(classDefinition, methodName, #methodName, __VA_ARGS__);

#define BNMU_DefineMethodName(type, name, methodName, ...) static type name; \
static BNMUMethodDefinition _##name(classDefinition, name, #methodName, __VA_ARGS__);

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
    enum class FindType {
        ByParametersCount,
        ByParametersName,
        ByParametersType
    } findType;
    union {
        int parameters = -1;
        std::initializer_list<std::string_view> parametersName;
        std::initializer_list<BNM::CompileTimeClass> parametersType;
    };
    BNMUMethodDefinition(BNMUClassDefinition &classDefinition, BNM::MethodBase &method, std::string_view methodName, int parameters = -1) : classDefinition(classDefinition), method(method), methodName(methodName), parameters(parameters) {
        findType = FindType::ByParametersCount;
        classDefinition.methodDefinitions.emplace_back(*this);
    }
    BNMUMethodDefinition(BNMUClassDefinition &classDefinition, BNM::MethodBase &method, std::string_view methodName, std::initializer_list<std::string_view> parametersName) : classDefinition(classDefinition), method(method), methodName(methodName), parametersName(parametersName) {
        findType = FindType::ByParametersName;
        classDefinition.methodDefinitions.emplace_back(*this);
    }
    BNMUMethodDefinition(BNMUClassDefinition &classDefinition, BNM::MethodBase &method, std::string_view methodName, std::initializer_list<BNM::CompileTimeClass> parametersType) : classDefinition(classDefinition), method(method), methodName(methodName), parametersType(parametersType) {
        findType = FindType::ByParametersType;
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
    }
};

void BNMU_OnLoaded() {
    static bool hasInited = false;
    if(hasInited) return;
    hasInited = true;
    for(BNMUClassDefinition *classDefinition : BNMUClassDefinition::classDefinitions) {
        classDefinition->clazz = BNM::Class(classDefinition->namezpace, classDefinition->name);
        for(BNMUFieldDefinition &fieldDefinition : classDefinition->fieldDefinitions) {
            fieldDefinition.field = classDefinition->clazz.GetField(fieldDefinition.fieldName);
        }
        for(BNMUMethodDefinition &methodDefinition : classDefinition->methodDefinitions) {
            switch (methodDefinition.findType) {
                case BNMUMethodDefinition::FindType::ByParametersCount:
                    methodDefinition.method = classDefinition->clazz.GetMethod(methodDefinition.methodName, methodDefinition.parameters);
                    break;
                case BNMUMethodDefinition::FindType::ByParametersName:
                    methodDefinition.method = classDefinition->clazz.GetMethod(methodDefinition.methodName, methodDefinition.parametersName);
                    break;
                case BNMUMethodDefinition::FindType::ByParametersType:
                    methodDefinition.method = classDefinition->clazz.GetMethod(methodDefinition.methodName, methodDefinition.parametersType);
                    break;
            }
        }
        for(BNMUPropertyDefinition &propertyDefinition : classDefinition->propertyDefinitions) {
            propertyDefinition.property = classDefinition->clazz.GetProperty(propertyDefinition.propertyName);
        }
    }
}