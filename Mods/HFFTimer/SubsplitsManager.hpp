#pragma once

#include <map>
#include <string>
#include "LevelNumbers.hpp"
#include "HFFTimer.hpp"
#include <BNM/UnityStructures.hpp>

using namespace BNM::Structures::Unity;

class Subsplit {
protected:
    bool triggered = false;
    float triggeredTime;
    std::string_view label;
    std::vector<SpeedrunMode> supportModes;
public:
    Subsplit(std::string_view label, std::initializer_list<SpeedrunMode> supportModes) : label(label), supportModes(supportModes) {}
    bool IsSupported() {
        if(supportModes.empty() || std::find(supportModes.begin(), supportModes.end(), HFFTimer::instance->mode) != supportModes.end()) {
            return true;
        }
        return false;
    }
    void Trigger() {
        if(triggered) return;
        triggered = true;
        triggeredTime = HFFTimer::instance->ssTime;
    }
    bool GetTriggered() {
        if(!IsSupported()) return false;
        return triggered;
    }
    std::string_view GetLabel() {
        return label;
    }
    float GetTriggeredTime() {
        return triggeredTime;
    }
    void Start() {
        triggered = false;
        triggeredTime = 0;
        OnStart();
    }
    void Reset() {
        triggered = false;
        triggeredTime = 0;
        OnReset();
    }
    virtual void OnStart() {}
    virtual void OnReset() {}
    virtual void Update() {}
};

extern std::map<uint64_t, std::vector<Subsplit *>> subsplitConfig;

class SubsplitsManager {
public:
    static uint64_t currentLevel;
    static void Init();
    static void Reset();
    static void Update();
    static std::string GetSubsplitsText();
};

static bool BoxContains(Vector3 point, Vector3 pos, Vector3 size) {
    Vector3 from = pos - size / 2.0f;
    Vector3 to = pos + size / 2.0f;
    if(point.x >= from.x && point.y >= from.y && point.z >= from.z && point.x <= to.x && point.y <= to.y && point.z <= to.z) return true;
    return false;
}

class LogicSubsplit : public Subsplit {
private:
    std::function<bool()> logic;
public:
    LogicSubsplit(std::string_view label, std::function<bool()> logic) : Subsplit(label, {}), logic(logic) {}
    LogicSubsplit(std::string_view label, std::initializer_list<SpeedrunMode> supportModes, std::function<bool()> logic) : Subsplit(label, supportModes), logic(logic) {}
    void Update() {
        if(logic()) Trigger();
    }
};

class GetUpSubsplit : public Subsplit {
public:
    GetUpSubsplit(std::string_view label) : Subsplit(label, {}) {}
    GetUpSubsplit(std::string_view label, std::initializer_list<SpeedrunMode> supportModes) : Subsplit(label, supportModes) {}
    void Update() {
        if(Human::state[Human::Localplayer] != HumanState::Unconscious && Human::state[Human::Localplayer] != HumanState::Spawning) Trigger();
    }
};

class FallSubsplit : public Subsplit {
public:
    FallSubsplit(std::string_view label) : Subsplit(label, {}) {}
    FallSubsplit(std::string_view label, std::initializer_list<SpeedrunMode> supportModes) : Subsplit(label, supportModes) {}
    static FallSubsplit *current;
    void OnStart();
};

class GrabSubsplit : public Subsplit {
private:
    BNM::UnityEngine::Object *objectPtr;
    std::string_view objectPath;
public:
    GrabSubsplit(std::string_view label, std::string_view objectPath) : Subsplit(label, {}), objectPath(objectPath) {}
    GrabSubsplit(std::string_view label, std::string_view objectPath, std::initializer_list<SpeedrunMode> supportModes) : Subsplit(label, supportModes), objectPath(objectPath) {}
    void OnStart() {
        using namespace UnityEngine;
        objectPtr = GameObject::Find(BNM::CreateMonoString(objectPath));
    }
    void Update() {
        if(Process(objectPtr)) Trigger();
    }
    void OnReset() {
        objectPtr = nullptr;
    }
    static bool Process(BNM::UnityEngine::Object *objectPtr) {
        if(!objectPtr->Alive()) return false;
        std::vector<void *> grabbedObjects = GrabManager::grabbedObjects[Human::grabManager[Human::Localplayer]].Get()->ToVector();
        for(auto object : grabbedObjects) {
            if(object == objectPtr) {
                return true;
            }
        }
        return false;
    }
};

class GrabTriggerSubsplit : public Subsplit {
private:
    Vector3 pos;
    Vector3 size;
public:
    GrabTriggerSubsplit(std::string_view label, Vector3 pos, Vector3 size) : Subsplit(label, {}), pos(pos), size(size) {}
    GrabTriggerSubsplit(std::string_view label, Vector3 pos, Vector3 size, std::initializer_list<SpeedrunMode> supportModes) : Subsplit(label, supportModes), pos(pos), size(size) {}
    void Update() {
        using namespace UnityEngine;
        if(Process(pos, size)) Trigger();
    }
    static bool Process(Vector3 pos, Vector3 size) {
        using namespace UnityEngine;
        void *leftHand = Ragdoll::partLeftHand[Human::ragdoll[Human::Localplayer]];
        void *rightHand = Ragdoll::partRightHand[Human::ragdoll[Human::Localplayer]];
        return (BoxContains(Transform::position[HumanSegment::transform[leftHand]], pos, size) && CollisionSensor::grabJoint[HumanSegment::sensor[leftHand]].Get()->Alive())
            || (BoxContains(Transform::position[HumanSegment::transform[rightHand]], pos, size) && CollisionSensor::grabJoint[HumanSegment::sensor[rightHand]].Get()->Alive());
    }
};

class EnterTriggerSubsplit : public Subsplit {
private:
    Vector3 pos;
    Vector3 size;
public:
    EnterTriggerSubsplit(std::string_view label, Vector3 pos, Vector3 size) : Subsplit(label, {}), pos(pos), size(size) {}
    EnterTriggerSubsplit(std::string_view label, Vector3 pos, Vector3 size, std::initializer_list<SpeedrunMode> supportModes) : Subsplit(label, supportModes), pos(pos), size(size) {}
    void Update() {
        if(Process(pos, size)) Trigger();
    }
    static bool Process(Vector3 pos, Vector3 size) {
        using namespace UnityEngine;
        return BoxContains(Transform::position[Component::transform[Human::Localplayer]], pos, size);
    }
};

class JumpAfterTriggerSubsplit : public Subsplit {
private:
    Vector3 pos;
    Vector3 size;
    bool triggerEntered;
public:
    JumpAfterTriggerSubsplit(std::string_view label, Vector3 pos, Vector3 size) : Subsplit(label, {}), pos(pos), size(size) {}
    JumpAfterTriggerSubsplit(std::string_view label, Vector3 pos, Vector3 size, std::initializer_list<SpeedrunMode> supportModes) : Subsplit(label, supportModes), pos(pos), size(size) {}
    void Update() {
        using namespace UnityEngine;
        if(BoxContains(Transform::position[Component::transform[Human::Localplayer]], pos, size)) {
            triggerEntered = true;
        }
        if(triggerEntered && Human::jump[Human::Localplayer]) {
            Trigger();
        }
    }
    void OnStart() {
        triggerEntered = false;
    }
    void OnReset() {
        triggerEntered = false;
    }
};

class JumpBeforeTriggerSubsplit : public Subsplit {
private:
    Vector3 pos;
    Vector3 size;
public:
    JumpBeforeTriggerSubsplit(std::string_view label, Vector3 pos, Vector3 size) : Subsplit(label, {}), pos(pos), size(size) {}
    JumpBeforeTriggerSubsplit(std::string_view label, Vector3 pos, Vector3 size, std::initializer_list<SpeedrunMode> supportModes) : Subsplit(label, supportModes), pos(pos), size(size) {}
    void Update() {
        using namespace UnityEngine;
        if(Human::jump[Human::Localplayer]) {
            triggeredTime = HFFTimer::instance->ssTime;
        }
        if(BoxContains(Transform::position[Component::transform[Human::Localplayer]], pos, size)) {
            if(triggeredTime == 0) triggeredTime = HFFTimer::instance->ssTime;
            triggered = true;
        }
    }
};

class LandAfterTriggerSubsplit : public Subsplit {
private:
    Vector3 pos;
    Vector3 size;
    bool triggerEntered;
public:
    LandAfterTriggerSubsplit(std::string_view label, Vector3 pos, Vector3 size) : Subsplit(label, {}), pos(pos), size(size) {}
    LandAfterTriggerSubsplit(std::string_view label, Vector3 pos, Vector3 size, std::initializer_list<SpeedrunMode> supportModes) : Subsplit(label, supportModes), pos(pos), size(size) {}
    void Update() {
        using namespace UnityEngine;
        if(BoxContains(Transform::position[Component::transform[Human::Localplayer]], pos, size)) {
            triggerEntered = true;
        }
        if(triggerEntered && Human::onGround[Human::Localplayer]) {
            Trigger();
        }
    }
    void OnStart() {
        triggerEntered = false;
    }
    void OnReset() {
        triggerEntered = false;
    }
};