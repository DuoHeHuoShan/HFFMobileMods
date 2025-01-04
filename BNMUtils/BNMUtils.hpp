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

#ifndef BNMU_EXTERN

#define BNMU_BeginDefineClass(classname, namezpace, name) namespace classname { \
    BNMUClassDefinition classDefinition(namezpace, #name); \
    const BNM::Class &clazz = classDefinition.clazz;
#define BNMU_DefineField(type, fieldName) type fieldName; \
BNMUFieldDefinition _##fieldName(classDefinition, fieldName, #fieldName);

#define BNMU_DefineMethod(type, methodName, ...) type methodName; \
BNMUMethodDefinition _##methodName(classDefinition, methodName, #methodName, __VA_ARGS__);

#define BNMU_DefineMethodName(type, name, methodName, ...) type name; \
BNMUMethodDefinition _##name(classDefinition, name, #methodName, __VA_ARGS__);

#define BNMU_DefineProperty(type, propertyName) type propertyName; \
BNMUPropertyDefinition _##propertyName(classDefinition, propertyName, #propertyName);

#else

#define BNMU_BeginDefineClass(classname, namezpace, name) namespace classname { \
    extern BNMUClassDefinition classDefinition; \
    extern const BNM::Class &clazz;
#define BNMU_DefineField(type, fieldName) extern type fieldName; \
extern BNMUFieldDefinition _##fieldName;

#define BNMU_DefineMethod(type, methodName, ...) extern type methodName; \
extern BNMUMethodDefinition _##methodName;

#define BNMU_DefineMethodName(type, name, methodName, ...) extern type name; \
extern BNMUMethodDefinition _##name;

#define BNMU_DefineProperty(type, propertyName) extern type propertyName; \
extern BNMUPropertyDefinition _##propertyName;

#endif

#define BNMU_EndDefineClass() };

class BNMUFieldDefinition;

class BNMUMethodDefinition;

class BNMUPropertyDefinition;

class BNMUClassDefinition {
public:
    static std::vector<BNMUClassDefinition *> classDefinitions;
    std::vector<BNMUFieldDefinition> fieldDefinitions;
    std::vector<BNMUMethodDefinition *> methodDefinitions;
    std::vector<BNMUPropertyDefinition> propertyDefinitions;
    std::string_view namezpace;
    std::string_view name;
    BNM::Class clazz;

    BNMUClassDefinition(std::string_view namezpace, std::string_view name) : namezpace(namezpace),
                                                                             name(name) {
        classDefinitions.emplace_back(this);
    }
};

#ifndef BNMU_EXTERN
std::vector<BNMUClassDefinition *> BNMUClassDefinition::classDefinitions = std::vector<BNMUClassDefinition *>();
#endif

class BNMUFieldDefinition {
public:
    std::string_view fieldName;
    BNM::FieldBase &field;
    BNMUClassDefinition &classDefinition;

    BNMUFieldDefinition(BNMUClassDefinition &classDefinition, BNM::FieldBase &field,
                        std::string_view fieldName) : field(field), fieldName(fieldName),
                                                      classDefinition(classDefinition) {
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
    int parameters = -1;
    std::vector<std::string_view> parametersName;
    std::vector<BNM::CompileTimeClass> parametersType;

    BNMUMethodDefinition(BNMUClassDefinition &classDefinition, BNM::MethodBase &method,
                         std::string_view methodName, int parameters = -1) : classDefinition(
            classDefinition), method(method), methodName(methodName), parameters(parameters) {
        findType = FindType::ByParametersCount;
        classDefinition.methodDefinitions.emplace_back(this);
    }

    BNMUMethodDefinition(BNMUClassDefinition &classDefinition, BNM::MethodBase &method,
                         std::string_view methodName,
                         std::initializer_list<std::string_view> parametersName) : classDefinition(
            classDefinition), method(method), methodName(methodName), parametersName(parametersName) {
        findType = FindType::ByParametersName;
        classDefinition.methodDefinitions.emplace_back(this);
    }

    BNMUMethodDefinition(BNMUClassDefinition &classDefinition, BNM::MethodBase &method,
                         std::string_view methodName,
                         std::initializer_list<BNM::CompileTimeClass> parametersType)
            : classDefinition(classDefinition), method(method), methodName(methodName),
              parametersType(parametersType) {
        findType = FindType::ByParametersType;
        classDefinition.methodDefinitions.emplace_back(this);
    }
};

class BNMUPropertyDefinition {
public:
    std::string_view propertyName;
    BNM::PropertyBase &property;
    BNMUClassDefinition &classDefinition;

    BNMUPropertyDefinition(BNMUClassDefinition &classDefinition, BNM::PropertyBase &property,
                           std::string_view propertyName) : property(property),
                                                            propertyName(propertyName),
                                                            classDefinition(classDefinition) {
        classDefinition.propertyDefinitions.emplace_back(*this);
    }
};

#ifdef BNMU_EXTERN
extern void BNMU_OnLoaded();
#else
void BNMU_OnLoaded() {
    static bool hasInited = false;
    if (hasInited) return;
    hasInited = true;
    for (BNMUClassDefinition *classDefinition: BNMUClassDefinition::classDefinitions) {
        classDefinition->clazz = BNM::Class(classDefinition->namezpace, classDefinition->name);
        for (BNMUFieldDefinition &fieldDefinition: classDefinition->fieldDefinitions) {
            fieldDefinition.field = classDefinition->clazz.GetField(fieldDefinition.fieldName);
        }
        for (BNMUMethodDefinition *methodDefinition: classDefinition->methodDefinitions) {
            switch (methodDefinition->findType) {
                case BNMUMethodDefinition::FindType::ByParametersCount:
                    methodDefinition->method = classDefinition->clazz.GetMethod(
                            methodDefinition->methodName, methodDefinition->parameters);
                    break;
                case BNMUMethodDefinition::FindType::ByParametersName:
                    methodDefinition->method = classDefinition->clazz.GetMethod(
                            methodDefinition->methodName, methodDefinition->parametersName);
                    break;
                case BNMUMethodDefinition::FindType::ByParametersType:
                    methodDefinition->method = classDefinition->clazz.GetMethod(
                            methodDefinition->methodName, methodDefinition->parametersType);
                    break;
            }
        }
        for (BNMUPropertyDefinition &propertyDefinition: classDefinition->propertyDefinitions) {
            propertyDefinition.property = classDefinition->clazz.GetProperty(
                    propertyDefinition.propertyName);
        }
    }
}
#endif