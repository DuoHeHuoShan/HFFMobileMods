#include <vector>
#include "LevelNumbers.hpp"
#include "SubsplitsManager.hpp"

std::map<uint64_t, std::vector<Subsplit *>> subsplitConfig {
    {
        static_cast<uint64_t>(LevelNumbers::Default),
        {
            new GetUpSubsplit("起身"),
            new FallSubsplit("刷新")
        }
    },
    {
        static_cast<uint64_t>(LevelNumbers::Mansion),
        {
            new GetUpSubsplit("起身"),
            new LandAfterTriggerSubsplit("跳岛", {4, 5.5f, 1.2f}, {30, 5, 1}),
            new LogicSubsplit("按按钮", []() {
                return GrabTriggerSubsplit::Process({1.5f, 4, 31.4f}, {0.8f, 0.8f, 0.6f})
                    && GrabTriggerSubsplit::Process({2.5f, 4, 31.4f}, {0.8f, 0.8f, 0.6f});
            }),
            new EnterTriggerSubsplit("起飞", {-4.1f, 6.1f, -22}, {8, 0.4f, 10}),
            new FallSubsplit("刷新")
        }
    },
    {
        static_cast<uint64_t>(LevelNumbers::Train),
        {
            new GetUpSubsplit("起身"),
            new JumpAfterTriggerSubsplit("出青苔", {-8, 4, 5.5f}, {4, 1, 1}),
            new FallSubsplit("刷新")
        }
    },
    {
        static_cast<uint64_t>(LevelNumbers::Carry),
        {
            new GetUpSubsplit("起身"),
            new JumpAfterTriggerSubsplit("出火车", {4, -1, 17}, {5, 2, 1}),
            new FallSubsplit("刷新")
        }
    },
    {
        static_cast<uint64_t>(LevelNumbers::Mountain),
        {
            new GetUpSubsplit("起身"),
            new GrabTriggerSubsplit("抓墙", {-4, 3.25f, -10}, {10, 1, 5}),
            new GrabSubsplit("抓火车", "Level/Plateau/Platform8x2x3/Cargo8x2x2"),
            new JumpAfterTriggerSubsplit("跳岛", {6.5f, 20.5f, 13.3f}, {14, 3, 1}),
            new JumpAfterTriggerSubsplit("碰楼梯", {36, 23, 57}, {3, 2, 1}),
            new JumpAfterTriggerSubsplit("进门", {18, 31, 64}, {1, 2, 2}),
            new GrabSubsplit("抓遥控器", "Level/Tutorial/TutorialRemote"),
            new EnterTriggerSubsplit("起飞", {7.6f, 2.4f, -40.8f}, {1.4f, 0.4f, 1.6f}),
            new FallSubsplit("刷新")
        }
    },
    {
        static_cast<uint64_t>(LevelNumbers::Demolition),
        {
            new GetUpSubsplit("起身"),
            new JumpAfterTriggerSubsplit("上起重机", {10.5f, 4.7f, 33}, {3, 1, 6}),
            new GrabTriggerSubsplit("抓水管", {-0.7f, 9.5f, 37.1f}, {2, 2, 4.5f}),
            new FallSubsplit("刷新")
        }
    },
    {
        static_cast<uint64_t>(LevelNumbers::Castle),
        {
            new GetUpSubsplit("起身"),
            new JumpBeforeTriggerSubsplit("出门", {55, -9, 1.4f}, {1, 2, 4}),
            new JumpBeforeTriggerSubsplit("出岛", {26, -10, 18}, {5, 5, 1})
        }
    },
    {
        static_cast<uint64_t>(LevelNumbers::Water),
        {
            new GetUpSubsplit("起身"),
            new JumpAfterTriggerSubsplit("上山", {-11, 6.5f, 30}, {3, 1, 1}),
            new GrabTriggerSubsplit("上水车", {-7.9f, 19.5f, 37.2f}, {1, 1, 5}),
            new FallSubsplit("刷新")
        }
    },
    {
        static_cast<uint64_t>(LevelNumbers::PowerPlant),
        {
            new GetUpSubsplit("起身"),
            new GrabTriggerSubsplit("低飞", {21.4f, 23, 58.7f}, {13, 8, 13}),
        }
    },
    {
        static_cast<uint64_t>(LevelNumbers::Aztec),
        {
            new GetUpSubsplit("起身"),
            new GrabTriggerSubsplit("抓墙", {7.5f, 0.6f, -16}, {7.1f, 2, 0.3f}),
            new GrabSubsplit("抬门", "Level/Lane4/02LiftDoorPuzzle/D_GapDoor")
        }
    },
    {
        static_cast<uint64_t>(LevelNumbers::Dark),
        {
            new GetUpSubsplit("起身"),
            new GrabSubsplit("抓斧", "AxeHandle"),
            new JumpAfterTriggerSubsplit("进洞", {-19.5f, 3.2f, 145}, {4, 1, 3}),
            new FallSubsplit("刷新")
        }
    },
    {
        static_cast<uint64_t>(LevelNumbers::Steam),
        {
            new GetUpSubsplit("起身"),
            new EnterTriggerSubsplit("跳出", {-18.2f, 1, -15.2f}, {0.3f, 2, 2}),
            new FallSubsplit("刷新")
        }
    },
    {
        static_cast<uint64_t>(LevelNumbers::Ice),
        {
            new GetUpSubsplit("起身"),
            new EnterTriggerSubsplit("上cp1", {57, 28, -99}, {1, 1, 11}),
            new JumpAfterTriggerSubsplit("上墙", {49.6f, 29.3f, -63}, {4, 0.4f, 5}),
            new GrabSubsplit("按引爆器", "finish_detonator_lever")
        }
    },
};