#pragma once
#include <map>

enum class Achievement
{
    ACH_LVL_INTRO,
    ACH_LVL_PUSH,
    ACH_LVL_CARRY,
    ACH_LVL_CLIMB,
    ACH_LVL_BREAK,
    ACH_LVL_SIEGE,
    ACH_LVL_RIVER_FEET,
    ACH_LVL_RIVER_HEAD,
    ACH_LVL_POWER,
    ACH_LVL_AZTEC,
    ACH_SINGLE_RUN,
    ACH_TRAVEL_1KM,
    ACH_TRAVEL_10KM,
    ACH_TRAVEL_100KM,
    ACH_FALL_1,
    ACH_FALL_1000,
    ACH_JUMP_1000,
    ACH_CLIMB_100M,
    ACH_CARRY_1000M,
    ACH_DROWN_10,
    ACH_SHIP_1000M,
    ACH_DRIVE_1000M,
    ACH_DUMPSTER_50M,
    ACH_INTRO_STATUE_HEAD,
    ACH_INTRO_JUMP_GAP,
    ACH_PUSH_CLEAN_DEBRIS,
    ACH_PUSH_BENCH_ALIGN,
    ACH_CARRY_JAM_DOOR,
    ACH_CARRY_BIG_STACK,
    ACH_CLIMB_ROPE,
    ACH_CLIMB_SPEAKERS,
    ACH_CLIMB_GEMS,
    ACH_BREAK_WINDOW_SHORTCUT,
    ACH_BREAK_SURPRISE,
    ACH_BREAK_BARE_HANDS,
    ACH_SIEGE_HUMAN_CANNON,
    ACH_SIEGE_BELL,
    ACH_SIEGE_ZIPLINE,
    ACH_SIEGE_ASSASIN,
    ACH_WATER_ROW_BOAT,
    ACH_WATER_REVERSE_GEAR,
    ACH_WATER_LIGHTHOUSE,
    ACH_WATER_ALMOST_DROWN,
    ACH_WATER_SURF,
    ACH_POWER_SHORT_CIRCUIT,
    ACH_POWER_3VOLTS,
    ACH_POWER_COAL_DELIVER,
    ACH_POWER_STATUE_BATTERY,
    ACH_AZTEC_OVERLOOK,
    ACH_AZTEC_INDIANA,
    ACH_AZTEC_CLOCWORK,
    ACH_LVL_HALLOWEEN,
    ACH_HALLOWEEN_LIKE_CLOCKWORK,
    ACH_HALLOWEEN_FRY_ME_TO_THE_MOON,
    ACH_HALLOWEEN_PLANKS_NO_THANKS,
    ACH_LVL_STEAM,
    ACH_STEAM_WALK_THE_PLANK,
    ACH_STEAM_GET_DIZZY,
    ACH_STEAM_WHOOPS,
    ACH_LVL_ICE,
    ACH_ICE_TRICKY,
    ACH_ICE_NO_ICE_BABY,
    ACH_ICE_TAKING_THE_PISTE,
    ACH_XMAS_SLIDE,
    ACH_XMAS_SKI_LAND,
    ACH_XMAS_ANGEL_FALL,
    ACH_XMAS_SNOWMAN_HAT,
    ACH_THERMAL_COMPLETE,
    ACH_THERMAL_PAYDAY,
    ACH_THERMAL_SHORTCUT,
    ACH_THERMAL_WIRES_TIME_TRIAL,
    ACH_FACTORY_COMPLETE,
    ACH_FACTORY_RADIOS,
    ACH_FACTORY_DARK,
    ACH_FACTORY_FIRED,
    ACH_GOLF_COMPLETE,
    ACH_GOLF_PAR,
    ACH_GOLF_BOGEY,
    ACH_GOLF_MULLIGAN,
    ACH_CITY_COMPLETE,
    ACH_CITY_TENPIN,
    ACH_CITY_HITCHHIKER,
    ACH_CITY_TANTRUM,
    ACH_LUNAR_LUCKY_CARP,
    ACH_LUNAR_ZODIAC,
    ACH_LUNAR_LOAJAN_FURNACE,
    ACH_LUNAR_FORTUNE_TREE,
    ACH_FOREST_WRECKING_CREW,
    ACH_FOREST_OVERCOOKED,
    ACH_FOREST_ON_THIN_ICE,
    ACH_FOREST_WRONG_TURN,
    ACH_LAB_KABOOM,
    ACH_LAB_BULLSEYE,
    ACH_LAB_CAST_IT_INTO_THE_FIRE,
    ACH_LAB_OVERACHIEVER,
    ACH_LUMBER_THE_GREAT_OUTDOORS,
    ACH_LUMBER_SAFETY_FIRST,
    ACH_LUMBER_BEDWETTER,
    ACH_LUMBER_LIGHTS_OUT,
    ACH_REDROCK_SUNSHINE,
    ACH_REDROCK_PIPEDREAM,
    ACH_REDROCK_PRETTY_FLY_CACTI,
    ACH_REDROCK_CURRENTLY_ATTRACTIVE,
    ACH_TOWER_FISHING_ROD,
    ACH_TOWER_CROSS_LAVA_PIPE,
    ACH_TOWER_BLOCK_DOOR,
    ACH_TOWER_FINISH_LEVEL,
    ACH_MINIATURE_FINISH_LEVEL,
    ACH_MINIATURE_REACHED_TOP_OF_THE_SHELF,
    ACH_MINIATURE_ASH,
    ACH_MINIATURE_TOPOTHECRATE,
    ACH_COPPERWORLD_SQUAREBATTERY,
    ACH_COPPERWORLD_LASERLOOP,
    ACH_COPPERWORLD_COINS,
    ACH_COPPERWORLD_FINISHLEVEL,
    ACH_PORT_FINISHLEVEL,
    ACH_PORT_BEACONS,
    ACH_PORT_CLEAROUT_ROOM,
    ACH_PORT_REACH_GOLF_ISLAND,
    ACH_UNDERWATER_FINISHLEVEL,
    ACH_UNDERWATER_COVER_UP,
    ACH_UNDERWATER_MINES,
    ACH_UNDERWATER_REACH_AREA,
    ACH_DOCKYARD_FINISHLEVEL,
    ACH_DOCKYARD_SWING_OUTSIDE,
    ACH_DOCKYARD_DESTROY_GLASS,
    ACH_DOCKYARD_SPEEDBOAT,
    ACH_MUSEUM_FINISHLEVEL,
    ACH_MUSEUM_BUCKSHOT,
    ACH_MUSEUM_LASERS,
    ACH_MUSEUM_EYES,
    ACH_HIKE_FINISHLEVEL,
    ACH_HIKE_HUMAN_THWAED_FLAT,
    ACH_HIKE_THREE_LAPS,
    ACH_HIKE_STORAGE,
    ACH_CANDYLAND_FINISHLEVEL,
    ACH_CANDYLAND_MARHSMALLOW,
    ACH_CANDYLAND_DONUT,
    ACH_CANDYLAND_BALLS
};

std::map<Achievement, std::string> Achievement2Str
{
        { Achievement::ACH_LVL_INTRO, "ACH_LVL_INTRO" },
        { Achievement::ACH_LVL_PUSH, "ACH_LVL_PUSH" },
        { Achievement::ACH_LVL_CARRY, "ACH_LVL_CARRY" },
        { Achievement::ACH_LVL_CLIMB, "ACH_LVL_CLIMB" },
        { Achievement::ACH_LVL_BREAK, "ACH_LVL_BREAK" },
        { Achievement::ACH_LVL_SIEGE, "ACH_LVL_SIEGE" },
        { Achievement::ACH_LVL_RIVER_FEET, "ACH_LVL_RIVER_FEET" },
        { Achievement::ACH_LVL_RIVER_HEAD, "ACH_LVL_RIVER_HEAD" },
        { Achievement::ACH_LVL_POWER, "ACH_LVL_POWER" },
        { Achievement::ACH_LVL_AZTEC, "ACH_LVL_AZTEC" },
        { Achievement::ACH_SINGLE_RUN, "ACH_SINGLE_RUN" },
        { Achievement::ACH_TRAVEL_1KM, "ACH_TRAVEL_1KM" },
        { Achievement::ACH_TRAVEL_10KM, "ACH_TRAVEL_10KM" },
        { Achievement::ACH_TRAVEL_100KM, "ACH_TRAVEL_100KM" },
        { Achievement::ACH_FALL_1, "ACH_FALL_1" },
        { Achievement::ACH_FALL_1000, "ACH_FALL_1000" },
        { Achievement::ACH_JUMP_1000, "ACH_JUMP_1000" },
        { Achievement::ACH_CLIMB_100M, "ACH_CLIMB_100M" },
        { Achievement::ACH_CARRY_1000M, "ACH_CARRY_1000M" },
        { Achievement::ACH_DROWN_10, "ACH_DROWN_10" },
        { Achievement::ACH_SHIP_1000M, "ACH_SHIP_1000M" },
        { Achievement::ACH_DRIVE_1000M, "ACH_DRIVE_1000M" },
        { Achievement::ACH_DUMPSTER_50M, "ACH_DUMPSTER_50M" },
        { Achievement::ACH_INTRO_STATUE_HEAD, "ACH_INTRO_STATUE_HEAD" },
        { Achievement::ACH_INTRO_JUMP_GAP, "ACH_INTRO_JUMP_GAP" },
        { Achievement::ACH_PUSH_CLEAN_DEBRIS, "ACH_PUSH_CLEAN_DEBRIS" },
        { Achievement::ACH_PUSH_BENCH_ALIGN, "ACH_PUSH_BENCH_ALIGN" },
        { Achievement::ACH_CARRY_JAM_DOOR, "ACH_CARRY_JAM_DOOR" },
        { Achievement::ACH_CARRY_BIG_STACK, "ACH_CARRY_BIG_STACK" },
        { Achievement::ACH_CLIMB_ROPE, "ACH_CLIMB_ROPE" },
        { Achievement::ACH_CLIMB_SPEAKERS, "ACH_CLIMB_SPEAKERS" },
        { Achievement::ACH_CLIMB_GEMS, "ACH_CLIMB_GEMS" },
        { Achievement::ACH_BREAK_WINDOW_SHORTCUT, "ACH_BREAK_WINDOW_SHORTCUT" },
        { Achievement::ACH_BREAK_SURPRISE, "ACH_BREAK_SURPRISE" },
        { Achievement::ACH_BREAK_BARE_HANDS, "ACH_BREAK_BARE_HANDS" },
        { Achievement::ACH_SIEGE_HUMAN_CANNON, "ACH_SIEGE_HUMAN_CANNON" },
        { Achievement::ACH_SIEGE_BELL, "ACH_SIEGE_BELL" },
        { Achievement::ACH_SIEGE_ZIPLINE, "ACH_SIEGE_ZIPLINE" },
        { Achievement::ACH_SIEGE_ASSASIN, "ACH_SIEGE_ASSASIN" },
        { Achievement::ACH_WATER_ROW_BOAT, "ACH_WATER_ROW_BOAT" },
        { Achievement::ACH_WATER_REVERSE_GEAR, "ACH_WATER_REVERSE_GEAR" },
        { Achievement::ACH_WATER_LIGHTHOUSE, "ACH_WATER_LIGHTHOUSE" },
        { Achievement::ACH_WATER_ALMOST_DROWN, "ACH_WATER_ALMOST_DROWN" },
        { Achievement::ACH_WATER_SURF, "ACH_WATER_SURF" },
        { Achievement::ACH_POWER_SHORT_CIRCUIT, "ACH_POWER_SHORT_CIRCUIT" },
        { Achievement::ACH_POWER_3VOLTS, "ACH_POWER_3VOLTS" },
        { Achievement::ACH_POWER_COAL_DELIVER, "ACH_POWER_COAL_DELIVER" },
        { Achievement::ACH_POWER_STATUE_BATTERY, "ACH_POWER_STATUE_BATTERY" },
        { Achievement::ACH_AZTEC_OVERLOOK, "ACH_AZTEC_OVERLOOK" },
        { Achievement::ACH_AZTEC_INDIANA, "ACH_AZTEC_INDIANA" },
        { Achievement::ACH_AZTEC_CLOCWORK, "ACH_AZTEC_CLOCWORK" },
        { Achievement::ACH_LVL_HALLOWEEN, "ACH_LVL_HALLOWEEN" },
        { Achievement::ACH_HALLOWEEN_LIKE_CLOCKWORK, "ACH_HALLOWEEN_LIKE_CLOCKWORK" },
        { Achievement::ACH_HALLOWEEN_FRY_ME_TO_THE_MOON, "ACH_HALLOWEEN_FRY_ME_TO_THE_MOON" },
        { Achievement::ACH_HALLOWEEN_PLANKS_NO_THANKS, "ACH_HALLOWEEN_PLANKS_NO_THANKS" },
        { Achievement::ACH_LVL_STEAM, "ACH_LVL_STEAM" },
        { Achievement::ACH_STEAM_WALK_THE_PLANK, "ACH_STEAM_WALK_THE_PLANK" },
        { Achievement::ACH_STEAM_GET_DIZZY, "ACH_STEAM_GET_DIZZY" },
        { Achievement::ACH_STEAM_WHOOPS, "ACH_STEAM_WHOOPS" },
        { Achievement::ACH_LVL_ICE, "ACH_LVL_ICE" },
        { Achievement::ACH_ICE_TRICKY, "ACH_ICE_TRICKY" },
        { Achievement::ACH_ICE_NO_ICE_BABY, "ACH_ICE_NO_ICE_BABY" },
        { Achievement::ACH_ICE_TAKING_THE_PISTE, "ACH_ICE_TAKING_THE_PISTE" },
        { Achievement::ACH_XMAS_SLIDE, "ACH_XMAS_SLIDE" },
        { Achievement::ACH_XMAS_SKI_LAND, "ACH_XMAS_SKI_LAND" },
        { Achievement::ACH_XMAS_ANGEL_FALL, "ACH_XMAS_ANGEL_FALL" },
        { Achievement::ACH_XMAS_SNOWMAN_HAT, "ACH_XMAS_SNOWMAN_HAT" },
        { Achievement::ACH_THERMAL_COMPLETE, "ACH_THERMAL_COMPLETE" },
        { Achievement::ACH_THERMAL_PAYDAY, "ACH_THERMAL_PAYDAY" },
        { Achievement::ACH_THERMAL_SHORTCUT, "ACH_THERMAL_SHORTCUT" },
        { Achievement::ACH_THERMAL_WIRES_TIME_TRIAL, "ACH_THERMAL_WIRES_TIME_TRIAL" },
        { Achievement::ACH_FACTORY_COMPLETE, "ACH_FACTORY_COMPLETE" },
        { Achievement::ACH_FACTORY_RADIOS, "ACH_FACTORY_RADIOS" },
        { Achievement::ACH_FACTORY_DARK, "ACH_FACTORY_DARK" },
        { Achievement::ACH_FACTORY_FIRED, "ACH_FACTORY_FIRED" },
        { Achievement::ACH_GOLF_COMPLETE, "ACH_GOLF_COMPLETE" },
        { Achievement::ACH_GOLF_PAR, "ACH_GOLF_PAR" },
        { Achievement::ACH_GOLF_BOGEY, "ACH_GOLF_BOGEY" },
        { Achievement::ACH_GOLF_MULLIGAN, "ACH_GOLF_MULLIGAN" },
        { Achievement::ACH_CITY_COMPLETE, "ACH_CITY_COMPLETE" },
        { Achievement::ACH_CITY_TENPIN, "ACH_CITY_TENPIN" },
        { Achievement::ACH_CITY_HITCHHIKER, "ACH_CITY_HITCHHIKER" },
        { Achievement::ACH_CITY_TANTRUM, "ACH_CITY_TANTRUM" },
        { Achievement::ACH_LUNAR_LUCKY_CARP, "ACH_LUNAR_LUCKY_CARP" },
        { Achievement::ACH_LUNAR_ZODIAC, "ACH_LUNAR_ZODIAC" },
        { Achievement::ACH_LUNAR_LOAJAN_FURNACE, "ACH_LUNAR_LOAJAN_FURNACE" },
        { Achievement::ACH_LUNAR_FORTUNE_TREE, "ACH_LUNAR_FORTUNE_TREE" },
        { Achievement::ACH_FOREST_WRECKING_CREW, "ACH_FOREST_WRECKING_CREW" },
        { Achievement::ACH_FOREST_OVERCOOKED, "ACH_FOREST_OVERCOOKED" },
        { Achievement::ACH_FOREST_ON_THIN_ICE, "ACH_FOREST_ON_THIN_ICE" },
        { Achievement::ACH_FOREST_WRONG_TURN, "ACH_FOREST_WRONG_TURN" },
        { Achievement::ACH_LAB_KABOOM, "ACH_LAB_KABOOM" },
        { Achievement::ACH_LAB_BULLSEYE, "ACH_LAB_BULLSEYE" },
        { Achievement::ACH_LAB_CAST_IT_INTO_THE_FIRE, "ACH_LAB_CAST_IT_INTO_THE_FIRE" },
        { Achievement::ACH_LAB_OVERACHIEVER, "ACH_LAB_OVERACHIEVER" },
        { Achievement::ACH_LUMBER_THE_GREAT_OUTDOORS, "ACH_LUMBER_THE_GREAT_OUTDOORS" },
        { Achievement::ACH_LUMBER_SAFETY_FIRST, "ACH_LUMBER_SAFETY_FIRST" },
        { Achievement::ACH_LUMBER_BEDWETTER, "ACH_LUMBER_BEDWETTER" },
        { Achievement::ACH_LUMBER_LIGHTS_OUT, "ACH_LUMBER_LIGHTS_OUT" },
        { Achievement::ACH_REDROCK_SUNSHINE, "ACH_REDROCK_SUNSHINE" },
        { Achievement::ACH_REDROCK_PIPEDREAM, "ACH_REDROCK_PIPEDREAM" },
        { Achievement::ACH_REDROCK_PRETTY_FLY_CACTI, "ACH_REDROCK_PRETTY_FLY_CACTI" },
        { Achievement::ACH_REDROCK_CURRENTLY_ATTRACTIVE, "ACH_REDROCK_CURRENTLY_ATTRACTIVE" },
        { Achievement::ACH_TOWER_FISHING_ROD, "ACH_TOWER_FISHING_ROD" },
        { Achievement::ACH_TOWER_CROSS_LAVA_PIPE, "ACH_TOWER_CROSS_LAVA_PIPE" },
        { Achievement::ACH_TOWER_BLOCK_DOOR, "ACH_TOWER_BLOCK_DOOR" },
        { Achievement::ACH_TOWER_FINISH_LEVEL, "ACH_TOWER_FINISH_LEVEL" },
        { Achievement::ACH_MINIATURE_FINISH_LEVEL, "ACH_MINIATURE_FINISH_LEVEL" },
        { Achievement::ACH_MINIATURE_REACHED_TOP_OF_THE_SHELF, "ACH_MINIATURE_REACHED_TOP_OF_THE_SHE" },
        { Achievement::ACH_MINIATURE_ASH, "ACH_MINIATURE_ASH" },
        { Achievement::ACH_MINIATURE_TOPOTHECRATE, "ACH_MINIATURE_TOPOTHECRATE" },
        { Achievement::ACH_COPPERWORLD_SQUAREBATTERY, "ACH_COPPERWORLD_SQUAREBATTERY" },
        { Achievement::ACH_COPPERWORLD_LASERLOOP, "ACH_COPPERWORLD_LASERLOOP" },
        { Achievement::ACH_COPPERWORLD_COINS, "ACH_COPPERWORLD_COINS" },
        { Achievement::ACH_COPPERWORLD_FINISHLEVEL, "ACH_COPPERWORLD_FINISHLEVEL" },
        { Achievement::ACH_PORT_FINISHLEVEL, "ACH_PORT_FINISHLEVEL" },
        { Achievement::ACH_PORT_BEACONS, "ACH_PORT_BEACONS" },
        { Achievement::ACH_PORT_CLEAROUT_ROOM, "ACH_PORT_CLEAROUT_ROOM" },
        { Achievement::ACH_PORT_REACH_GOLF_ISLAND, "ACH_PORT_REACH_GOLF_ISLAND" },
        { Achievement::ACH_UNDERWATER_FINISHLEVEL, "ACH_UNDERWATER_FINISHLEVEL" },
        { Achievement::ACH_UNDERWATER_COVER_UP, "ACH_UNDERWATER_COVER_UP" },
        { Achievement::ACH_UNDERWATER_MINES, "ACH_UNDERWATER_MINES" },
        { Achievement::ACH_UNDERWATER_REACH_AREA, "ACH_UNDERWATER_REACH_AREA" },
        { Achievement::ACH_DOCKYARD_FINISHLEVEL, "ACH_DOCKYARD_FINISHLEVEL" },
        { Achievement::ACH_DOCKYARD_SWING_OUTSIDE, "ACH_DOCKYARD_SWING_OUTSIDE" },
        { Achievement::ACH_DOCKYARD_DESTROY_GLASS, "ACH_DOCKYARD_DESTROY_GLASS" },
        { Achievement::ACH_DOCKYARD_SPEEDBOAT, "ACH_DOCKYARD_SPEEDBOAT" },
        { Achievement::ACH_MUSEUM_FINISHLEVEL, "ACH_MUSEUM_FINISHLEVEL" },
        { Achievement::ACH_MUSEUM_BUCKSHOT, "ACH_MUSEUM_BUCKSHOT" },
        { Achievement::ACH_MUSEUM_LASERS, "ACH_MUSEUM_LASERS" },
        { Achievement::ACH_MUSEUM_EYES, "ACH_MUSEUM_EYES" },
        { Achievement::ACH_HIKE_FINISHLEVEL, "ACH_HIKE_FINISHLEVEL" },
        { Achievement::ACH_HIKE_HUMAN_THWAED_FLAT, "ACH_HIKE_HUMAN_THWAED_FLAT" },
        { Achievement::ACH_HIKE_THREE_LAPS, "ACH_HIKE_THREE_LAPS" },
        { Achievement::ACH_HIKE_STORAGE, "ACH_HIKE_STORAGE" },
        { Achievement::ACH_CANDYLAND_FINISHLEVEL, "ACH_CANDYLAND_FINISHLEVEL" },
        { Achievement::ACH_CANDYLAND_MARHSMALLOW, "ACH_CANDYLAND_MARHSMALLOW" },
        { Achievement::ACH_CANDYLAND_DONUT, "ACH_CANDYLAND_DONUT" },
        { Achievement::ACH_CANDYLAND_BALLS, "ACH_CANDYLAND_BALLS" }
};