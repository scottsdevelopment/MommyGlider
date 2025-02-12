/**
 * @file LuaEngine.cpp
 * @brief Implementation of the LuaEngine class for integrating Lua scripts with game logic.
 *
 * This file provides bindings to Lua for accessing game memory and invoking actions.
 *
 * @license MIT
 * @author [Your Name]
 */

#include <iostream>
#include <string>
#include "Memory.h"
#include "Controller.h"
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

// LuaEngine class definition
class LuaEngine {
private:
    lua_State* luaState;         // Lua state
    Memory& memory;              // Reference to Memory instance
    Controller& controller;      // Reference to Controller instance

    static LuaEngine* instance;  // Static instance pointer for Lua callbacks

    // Lua bindings

    static int lua_UnitHealth(lua_State* L);
    static int lua_UnitHealthMax(lua_State* L);
    static int lua_UnitPower(lua_State* L);
    static int lua_UnitPowerMax(lua_State* L);
    static int lua_IsPlayerMoving(lua_State* L);
    static int lua_UnitAffectingCombat(lua_State* L);
    static int lua_GetMoney(lua_State* L);
    static int lua_UnitBuff(lua_State* L);
    static int lua_UnitDebuff(lua_State* L);
    static int lua_CastSpell(lua_State* L);
    static int lua_TargetNearestEnemy(lua_State* L);
    static int lua_IsControlKeyDown(lua_State* L);
    static int lua_Calibration(lua_State* L);
    static int lua_UnitPosition(lua_State* L);
    static int lua_UnitCastingInfo(lua_State* L);
    static int lua_TargetUnit(lua_State* L);
    static int lua_GetSpellCooldown(lua_State* L);
    static int lua_GetNumLootItems(lua_State* L);
    static int lua_HasWandEquipped(lua_State* L);
    static int lua_UnitExists(lua_State* L);
    static int lua_UnitThreatSituation(lua_State* L);
    static int lua_UnitIsPlayer(lua_State* L);
    static int lua_SpellStopCasting(lua_State* L);
    static int lua_IsSpellKnown(lua_State* L);
    static int lua_IsSpellInRange(lua_State* L);
    static int lua_JumpOrAscendStart(lua_State* L);
    static int lua_MoveForwardStart(lua_State* L);
public:
    LuaEngine(Memory& mem, Controller& ctrl);
    ~LuaEngine();

    void executeScript(const std::string& filePath);
    void callFunction(const std::string& functionName);
};

// Static member initialization
LuaEngine* LuaEngine::instance = nullptr;

LuaEngine::LuaEngine(Memory& mem, Controller& ctrl) : memory(mem), controller(ctrl) {
    luaState = luaL_newstate();
    luaL_openlibs(luaState);
    instance = this;

    lua_register(luaState, "UnitHealth", lua_UnitHealth);
    lua_register(luaState, "UnitHealthMax", lua_UnitHealthMax);
    lua_register(luaState, "UnitPower", lua_UnitPower);
    lua_register(luaState, "UnitPowerMax", lua_UnitPowerMax);
    lua_register(luaState, "IsPlayerMoving", lua_IsPlayerMoving);
    lua_register(luaState, "GetMoney", lua_GetMoney);
    lua_register(luaState, "UnitBuff", lua_UnitBuff);
    lua_register(luaState, "UnitDebuff", lua_UnitDebuff);
    lua_register(luaState, "CastSpell", lua_CastSpell);
    lua_register(luaState, "TargetNearestEnemy", lua_TargetNearestEnemy);
    lua_register(luaState, "IsControlKeyDown", lua_IsControlKeyDown);
    lua_register(luaState, "Calibration", lua_Calibration);
    lua_register(luaState, "UnitPosition", lua_UnitPosition);
    lua_register(luaState, "UnitCastingInfo", lua_UnitCastingInfo);
    lua_register(luaState, "TargetUnit", lua_TargetUnit);
    lua_register(luaState, "GetSpellCooldown", lua_GetSpellCooldown);
    lua_register(luaState, "GetNumLootItems", lua_GetNumLootItems);
    lua_register(luaState, "HasWandEquipped", lua_HasWandEquipped);
    lua_register(luaState, "UnitExists", lua_UnitExists);
    lua_register(luaState, "UnitAffectingCombat", lua_UnitAffectingCombat);
    lua_register(luaState, "UnitThreatSituation", lua_UnitThreatSituation);
    lua_register(luaState, "UnitIsPlayer", lua_UnitIsPlayer);
    lua_register(luaState, "SpellStopCasting", lua_SpellStopCasting);
    lua_register(luaState, "IsSpellKnown", lua_IsSpellKnown); // Breaks signature
    lua_register(luaState, "IsSpellInRange", lua_IsSpellInRange);
    lua_register(luaState, "JumpOrAscendStart", lua_JumpOrAscendStart);
    lua_register(luaState, "MoveForwardStart", lua_MoveForwardStart);
    std::cout << "Lua engine initialized.\n";
}

// LuaEngine destructor
LuaEngine::~LuaEngine() {
    lua_close(luaState);
}

int LuaEngine::lua_JumpOrAscendStart(lua_State* L) {
    instance->controller.pressKey(VK_SPACE);
    return 1;
}

int LuaEngine::lua_MoveForwardStart(lua_State* L) {
    instance->controller.pressKey(VK_NUMLOCK);
    return 1;
}

int LuaEngine::lua_GetMoney(lua_State* L) {
    lua_pushinteger(L, instance->memory.GetMoney());
    return 1;
}

int LuaEngine::lua_TargetUnit(lua_State* L) {
    const char* unit = luaL_checkstring(L, 1);
    std::cout << "Lua requests targeting unit: " << unit << "\n";

    if (std::string(unit) == "party1") {
        instance->controller.pressKey(VK_F2);
    }
    else if (std::string(unit) == "party2") {
        instance->controller.pressKey(VK_F3);
    }
    else if (std::string(unit) == "party3") {
        instance->controller.pressKey(VK_F4);
    }
    else if (std::string(unit) == "party4") {
        instance->controller.pressKey(VK_F5);
    }
    else if (std::string(unit) == "player") {
        instance->controller.pressKey(VK_F1);
    }
    else {
        std::cerr << "Unknown unit: " << unit << "\n";
    }
    return 0; // No return value
}

int LuaEngine::lua_IsSpellInRange(lua_State* L) {
    const char* aura = luaL_checkstring(L, 1);

    // Remove non-alphanumeric characters from aura
    std::string sanitizedAura;
    for (const char& c : std::string(aura)) {
        if (isalnum(c)) {
            sanitizedAura += c;
        }
    }

    // Construct the dynamic function name
    std::string callKey = "IsSpellInRange__" + sanitizedAura;

    // Check if the memory offset exists
    auto it = instance->memory.offsetIndices.find(callKey);
    if (it == instance->memory.offsetIndices.end()) {
        std::cerr << "Error: Function not found for " << callKey << "\n";
        lua_pushboolean(L, false);
        return 1;
    }

    // Call the function dynamically
    try {
        bool hasBuff = instance->memory.getCapturedValue(callKey) > 0;
        lua_pushboolean(L, hasBuff);
    }
    catch (const std::exception& e) {
        std::cerr << "Error accessing value for " << callKey << ": " << e.what() << "\n";
        lua_pushboolean(L, false);
    }

    return 1;
}


int LuaEngine::lua_UnitExists(lua_State* L) {
    const char* unit = luaL_checkstring(L, 1);
    bool exists = instance->memory.getCapturedValue("UnitExists__" + std::string(unit)) == 1;
    lua_pushboolean(L, exists);
    return 1;
}

int LuaEngine::lua_UnitAffectingCombat(lua_State* L) {
    const char* unit = luaL_checkstring(L, 1);
    bool inCombat = instance->memory.getCapturedValue("UnitAffectingCombat__" + std::string(unit)) == 1;
    lua_pushboolean(L, inCombat);
    return 1;
}

int LuaEngine::lua_UnitThreatSituation(lua_State* L) {
    const char* unit = luaL_checkstring(L, 1);
    const char* otherUnit = luaL_checkstring(L, 2);

    std::string offsetKey = "UnitThreatSituation__" + std::string(unit) + "_" + std::string(otherUnit);
    int threatLevel = instance->memory.getCapturedValue(offsetKey);

    lua_pushinteger(L, threatLevel);
    return 1;
}

int LuaEngine::lua_UnitCastingInfo(lua_State* L) {
    const char* unit = luaL_checkstring(L, 1);
    bool isCasting = false;

    if (std::string(unit) == "player") {
        isCasting = instance->memory.UnitCastingInfo__player();
    }
    else if (std::string(unit) == "target") {
        isCasting = instance->memory.UnitCastingInfo__target();
    }
    else if (std::string(unit) == "focus") {
        isCasting = instance->memory.UnitCastingInfo__focus();
    }
    else {
        lua_pushnil(L); // Invalid unit
        return 1;
    }

    lua_pushboolean(L, isCasting);
    return 1;
}


int LuaEngine::lua_SpellStopCasting(lua_State* L) {
    instance->controller.pressKey(0x30);
    return 0;
}

int LuaEngine::lua_HasWandEquipped(lua_State* L) {
    bool hasWand = instance->memory.HasWandEquipped();
    lua_pushboolean(L, hasWand);
    return 1;
}

/*
-- List of unit types and auras to check
local units = { "player", "target", "party1", "party2", "party3", "party4" }
local buffs = { "Power Word: Shield", "Power Word: Fortitude", "Renew" }
local debuffs = { "Shadow Word: Pain", "Weakened Soul" }
local spells = {
    "Mind Blast",
    "Shadowfiend",
    "Inner Fire",
    "Vampiric Embrace",
    "Power Word: Solace",
    "Penance",
    "Power Word: Radiance",
    "Psychic Scream",
    "Silence"
}
*/
int LuaEngine::lua_UnitBuff(lua_State* L) {
    const char* aura = luaL_checkstring(L, 2);
    const char* unit = luaL_checkstring(L, 1);

    // Remove non-alphanumeric characters from aura
    std::string sanitizedAura;
    for (const char& c : std::string(aura)) {
        if (isalnum(c)) {
            sanitizedAura += c;
        }
    }

    // Construct the dynamic function name
    std::string buffKey = "UnitBuff__" + std::string(unit) + "_"  + sanitizedAura;

    // Check if the memory offset exists
    auto it = instance->memory.offsetIndices.find(buffKey);
    if (it == instance->memory.offsetIndices.end()) {
        std::cerr << "Error: Buff function not found for " << buffKey << "\n";
        lua_pushboolean(L, false);
        return 1;
    }

    // Call the function dynamically
    try {
        bool hasBuff = instance->memory.getCapturedValue(buffKey) > 0;
        lua_pushboolean(L, hasBuff);
    }
    catch (const std::exception& e) {
        std::cerr << "Error accessing buff value for " << buffKey << ": " << e.what() << "\n";
        lua_pushboolean(L, false);
    }

    return 1;
}


int LuaEngine::lua_UnitDebuff(lua_State* L) {
    const char* debuff = luaL_checkstring(L, 2);
    const char* unit = luaL_checkstring(L, 1);

    // Remove non-alphanumeric characters from debuff
    std::string sanitizedDebuff;
    for (const char& c : std::string(debuff)) {
        if (isalnum(c)) {
            sanitizedDebuff += c;
        }
    }

    // Construct the dynamic function name
    std::string debuffKey = "UnitDebuff__" + std::string(unit) + "_" + sanitizedDebuff;

    // Check if the memory offset exists
    auto it = instance->memory.offsetIndices.find(debuffKey);
    if (it == instance->memory.offsetIndices.end()) {
        std::cerr << "Error: Debuff function not found for " << debuffKey << "\n";
        lua_pushboolean(L, false);
        return 1;
    }

    // Call the function dynamically
    try {
        bool hasDebuff = instance->memory.getCapturedValue(debuffKey) > 0;
        lua_pushboolean(L, hasDebuff);
    }
    catch (const std::exception& e) {
        std::cerr << "Error accessing debuff value for " << debuffKey << ": " << e.what() << "\n";
        lua_pushboolean(L, false);
    }

    return 1;
}



int LuaEngine::lua_GetSpellCooldown(lua_State* L) {
    const char* spell = luaL_checkstring(L, 1);

    // Remove non-alphanumeric characters from spell
    std::string sanitizedSpell;
    for (const char& c : std::string(spell)) {
        if (isalnum(c)) {
            sanitizedSpell += c;
        }
    }

    // Construct the dynamic function name
    std::string cooldownKey = "GetSpellCooldown__" + sanitizedSpell;

    // Check if the memory offset exists
    auto it = instance->memory.offsetIndices.find(cooldownKey);
    if (it == instance->memory.offsetIndices.end()) {
        std::cerr << "Error: Cooldown function not found for " << cooldownKey << "\n";
        lua_pushinteger(L, 0);
        return 1;
    }

    // Call the function dynamically
    try {
        int cooldown = instance->memory.getCapturedValue(cooldownKey);
        lua_pushinteger(L, cooldown);
    }
    catch (const std::exception& e) {
        std::cerr << "Error accessing cooldown value for " << cooldownKey << ": " << e.what() << "\n";
        lua_pushinteger(L, 0);
    }

    return 1;
}

int LuaEngine::lua_IsSpellKnown(lua_State* L) {
    const char* spell = luaL_checkstring(L, 1);

    // Remove non-alphanumeric characters from spell
    std::string sanitizedSpell;
    for (const char& c : std::string(spell)) {
        if (isalnum(c)) {
            sanitizedSpell += c;
        }
    }

    // Construct the dynamic function name
    std::string cooldownKey = "IsSpellKnown__" + sanitizedSpell;

    // Check if the memory offset exists
    auto it = instance->memory.offsetIndices.find(cooldownKey);
    if (it == instance->memory.offsetIndices.end()) {
        std::cerr << "Error: Cooldown function not found for " << cooldownKey << "\n";
        lua_pushinteger(L, 0);
        return 1;
    }

    // Call the function dynamically
    try {
        int cooldown = instance->memory.getCapturedValue(cooldownKey);
        lua_pushinteger(L, cooldown);
    }
    catch (const std::exception& e) {
        std::cerr << "Error accessing cooldown value for " << cooldownKey << ": " << e.what() << "\n";
        lua_pushinteger(L, 0);
    }

    return 1;
}


int LuaEngine::lua_GetNumLootItems(lua_State* L) {
    lua_pushinteger(L, instance->memory.GetNumLootItems());
    return 1;
}

int LuaEngine::lua_IsPlayerMoving(lua_State* L) {
    lua_pushboolean(L, instance->memory.IsPlayerMoving());
    return 1;
}

int LuaEngine::lua_UnitHealth(lua_State* L) {
    const char* unit = luaL_checkstring(L, 1);
    int health = 0;

    if (std::string(unit) == "party1") {
        health = instance->memory.UnitHealth__party1();
    }
    else if (std::string(unit) == "party2") {
        health = instance->memory.UnitHealth__party2();
    }
    else if (std::string(unit) == "party3") {
        health = instance->memory.UnitHealth__party3();
    }
    else if (std::string(unit) == "party4") {
        health = instance->memory.UnitHealth__party4();
    }
    else if (std::string(unit) == "target") {
        health = instance->memory.UnitHealth__target();
    }
    else if (std::string(unit) == "player") {
        health = instance->memory.UnitHealth__player();
    }
    else {
        lua_pushnil(L);
        return 1;
    }

    lua_pushinteger(L, health);
    return 1;
}

int LuaEngine::lua_UnitIsPlayer(lua_State* L) {
    const char* unit = luaL_checkstring(L, 1);

    // Determine if the unit is a player
    bool isPlayer = instance->memory.UnitIsPlayer__target();
    lua_pushboolean(L, isPlayer);
    return 1;
}

int LuaEngine::lua_UnitHealthMax(lua_State* L) {
    const char* unit = luaL_checkstring(L, 1);
    int maxHealth = 0;

    if (std::string(unit) == "party1") {
        maxHealth = instance->memory.UnitHealthMax__party1();
    }
    else if (std::string(unit) == "party2") {
        maxHealth = instance->memory.UnitHealthMax__party2();
    }
    else if (std::string(unit) == "party3") {
        maxHealth = instance->memory.UnitHealthMax__party3();
    }
    else if (std::string(unit) == "party4") {
        maxHealth = instance->memory.UnitHealthMax__party4();
    }
    else if (std::string(unit) == "target") {
        maxHealth = instance->memory.UnitHealthMax__target();
    }
    else if (std::string(unit) == "player") {
        maxHealth = instance->memory.UnitHealthMax__player();
    }
    else {
        lua_pushnil(L);
        return 1;
    }

    lua_pushinteger(L, maxHealth);
    return 1;
}

int LuaEngine::lua_UnitPower(lua_State* L) {
    const char* unit = luaL_checkstring(L, 1);
    int power = (std::string(unit) == "player") ? instance->memory.UnitPower__player() : 0;
    lua_pushinteger(L, power);
    return 1;
}

int LuaEngine::lua_UnitPowerMax(lua_State* L) {
    const char* unit = luaL_checkstring(L, 1);
    int powerMax = (std::string(unit) == "player") ? instance->memory.UnitPowerMax__player() : 0;
    lua_pushinteger(L, powerMax);
    return 1;
}

// Lua bindings to check if Control key is pressed
int LuaEngine::lua_IsControlKeyDown(lua_State* L) {
    // Use GetAsyncKeyState to detect Control key press
    bool isPressed = (GetAsyncKeyState(VK_CAPITAL) & 0x8000) != 0;

    // Push result to Lua
    lua_pushboolean(L, isPressed);
    return 1; // One return value
}

int LuaEngine::lua_Calibration(lua_State* L) {
    lua_pushinteger(L, instance->memory.Calibration());
    return 1;
}

int LuaEngine::lua_UnitPosition(lua_State* L) {
    const char* unit = luaL_checkstring(L, 1);

    if (std::string(unit) == "player") {
        float x = instance->memory.UnitPosition__player_1();
        float y = instance->memory.UnitPosition__player_2();
        float z = instance->memory.UnitPosition__player_3();
        lua_pushnumber(L, x);
        lua_pushnumber(L, y);
        lua_pushnumber(L, z);
    }
    else {
        lua_pushnil(L); // Push nil if the unit is not supported
        lua_pushnil(L);
        lua_pushnil(L);
    }

    return 3; // Return three values (x, y, z)
}

int LuaEngine::lua_TargetNearestEnemy(lua_State* L) {
    std::cout << "Lua requests targeting nearest enemy\n";
    instance->controller.pressKey(VK_TAB);
    return 0;
}

int LuaEngine::lua_CastSpell(lua_State* L) {
    const char* spell = luaL_checkstring(L, 1);
    std::cout << "Lua requests casting spell: " << spell << "\n";

    if (std::string(spell) == "Smite") {
        instance->controller.pressKey(0x31); // VK code for '1'
    }
    else if (std::string(spell) == "Shadow Word: Pain") {
        instance->controller.pressKey(0x32); // VK code for '2'
    }
    else if (std::string(spell) == "Power Word: Shield") {
        instance->controller.pressKey(0x33); // VK code for '3'
    }
    else if (std::string(spell) == "Shoot") {
        instance->controller.pressKey(0x34); // VK code for '4'
    }
    else if (std::string(spell) == "Heal" || std::string(spell) == "Lesser Heal") {
        instance->controller.pressKey(0x48); // VK code for '1' (Heal and Smite share the same key)
    }
    else if (std::string(spell) == "Power Word: Fortitude") {
        instance->controller.pressKey(0x58); // VK code for 'X'
    }
    else if (std::string(spell) == "Drink") {
        instance->controller.pressKey(0x37); // VK code for '7'
    }
    else if (std::string(spell) == "Eat") {
        instance->controller.pressKey(0x36); // VK code for '6'
    }
    else if (std::string(spell) == "Mind Blast") {
        instance->controller.pressKey(0x47); // VK code for 'G'
    }
    else if (std::string(spell) == "Renew") {
        instance->controller.pressKey(0x52); // VK code for '2'
    }
    else {
        std::cerr << "Unknown spell: " << spell << "\n";
    }
    return 0;
}


void LuaEngine::executeScript(const std::string& filePath) {
    if (luaL_dofile(luaState, filePath.c_str())) {
        std::cerr << "Lua error: " << lua_tostring(luaState, -1) << "\n";
    }
}

void LuaEngine::callFunction(const std::string& functionName) {
    lua_getglobal(luaState, functionName.c_str());
    if (lua_pcall(luaState, 0, 0, 0) != LUA_OK) {
        std::cerr << "Error calling Lua function '" << functionName << "': " << lua_tostring(luaState, -1) << "\n";
    }
}
