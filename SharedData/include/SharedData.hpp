#pragma once

#include <map>
#include <string>
#include <vector>

namespace SharedData {
    extern std::map<std::string, void *> sharedDataMap;
    inline __attribute__((always_inline)) bool HasData(const std::string &name) {
        return sharedDataMap.contains(name);
    }
    template<typename T>
    inline __attribute__((always_inline)) T GetData(const std::string &name) {
        if(!HasData(name)) return {};
        return *((T *) sharedDataMap[name]);
    }
    template<typename T>
    inline __attribute__((always_inline)) T &GetDataRef(const std::string &name) {
        return *((T *) sharedDataMap[name]);
    }
    template<typename T = void>
    inline __attribute__((always_inline)) T *GetDataPtr(const std::string &name) {
        if(!HasData(name)) return nullptr;
        return (T *) sharedDataMap[name];
    }
    template<typename T>
    inline __attribute__((always_inline)) void RemoveData(const std::string &name) {
        if(!HasData(name)) return;
        delete (T *) sharedDataMap[name];
        sharedDataMap.erase(name);
    }
    template<typename T>
    inline __attribute__((always_inline)) void SetData(const std::string &name, T value) {
        if(!HasData(name)) sharedDataMap[name] = (void *) new T(value);
        else *((T *)sharedDataMap[name]) = value;
    }
    template<typename T = void>
    inline __attribute__((always_inline)) void SetDataPtr(const std::string &name, T *value) {
        sharedDataMap[name] = (void *) value;
    }
    template<typename T>
    void AddCallback(const std::string &name, const std::function<T> &callback) {
        if(!HasData(name)) sharedDataMap[name] = (void *) new std::vector<std::function<T>>;
        GetDataRef<std::vector<std::function<T>>>(name).push_back(callback);
    }
    template<typename T, typename... Args>
    void InvokeCallback(const std::string &name, Args... args) {
        if(!HasData(name)) return;
        for(const auto &func : GetDataRef<std::vector<std::function<T>>>(name)) {
            func(args...);
        }
    }
}
