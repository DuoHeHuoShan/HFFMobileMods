#include <sstream>
#include "SubsplitsManager.hpp"

void (*old_Fall)(void *, void *, bool, bool);
void new_Fall(void *instance, void *humanBase, bool drown, bool fallAchievement) {
    if(FallSubsplit::current) FallSubsplit::current->Trigger();
    old_Fall(instance, humanBase, drown, fallAchievement);
}

void SubsplitsManager::Init() {
    BNM::VirtualHook(Game::clazz, Game::Fall, new_Fall, old_Fall);
}

void SubsplitsManager::Update() {
    if(!Game::instance.Get()->Alive()) return;
    if(Game::currentLevelNumber[Game::instance].Get() != currentLevel) {
        for(auto subsplit : subsplitConfig.contains(currentLevel) ? subsplitConfig[currentLevel] : subsplitConfig[static_cast<uint64_t>(LevelNumbers::Default)]) {
            subsplit->Reset();
        }
        currentLevel = Game::currentLevelNumber[Game::instance];
        for(auto subsplit : subsplitConfig.contains(currentLevel) ? subsplitConfig[currentLevel] : subsplitConfig[static_cast<uint64_t>(LevelNumbers::Default)]) {
            subsplit->Start();
        }
    }
    if(NetGame::isClient) return;
    for(auto subsplit : subsplitConfig.contains(currentLevel) ? subsplitConfig[currentLevel] : subsplitConfig[static_cast<uint64_t>(LevelNumbers::Default)]) {
        if(subsplit->GetTriggered() || !subsplit->IsSupported()) continue;
        subsplit->Update();
    }
}

void FallSubsplit::OnStart() {
    FallSubsplit::current = this;
}

std::string SubsplitsManager::GetSubsplitsText() {
    if(!Game::instance.Get()->Alive() || (Game::state[Game::instance] != GameState::PlayingLevel && Game::state[Game::instance] != GameState::Paused)) return "";
    std::stringstream ss;
    for(auto subsplit : subsplitConfig.contains(currentLevel) ? subsplitConfig[currentLevel] : subsplitConfig[static_cast<uint64_t>(LevelNumbers::Default)]) {
        if(subsplit->GetTriggered() && !subsplit->GetLabel().empty()) {
            ss << subsplit->GetLabel() << ": " << HFFTimer::FormatTime(subsplit->GetTriggeredTime()) << std::endl;
        }
    }
    return ss.str();
}

uint64_t SubsplitsManager::currentLevel = -1;

FallSubsplit *FallSubsplit::current;