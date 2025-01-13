#ifndef MEMORY_MACROS_H
#define MEMORY_MACROS_H

#include "LuaEngine.h"

// Macro to dynamically obtain the LuaEngine memory instance
#define GET_MEMORY_INSTANCE() \
    LuaEngine* engine = LuaEngine::getInstance(); \
    if (!engine) { \
        lua_pushnil(L); \
        return 1; \
    } \
    Memory& memory = engine->getMemory();

#define PUSH_ARGUMENT_OFFSET(name, type, index, maxValue)                    \
    do {                                                                    \
        LuaEngine* engine = LuaEngine::getInstance();                       \
        if (!engine) {                                                      \
            lua_pushnil(L);                                                 \
            return 1;                                                       \
        }                                                                   \
        Memory& memory = engine->getMemory();                               \
        struct __##name##_Registrar {                                       \
            __##name##_Registrar() {                                        \
                Memory::offsetIndices[#name] = index;                       \
            }                                                               \
        } __##name##_registrar_instance;                                    \
                                                             \
            auto [x, y] = memory.calculatePixelCoordinates(#name);          \
            Color color = memory.getPixelColor(x, y);                       \
            auto value = memory.decodeRGBToValue<type>(color, #type, maxValue);   \
        lua_pushnumber(L, static_cast<lua_Number>(value));                  \
        return 1;                                                           \
    } while (0)

// Macro to define a Lua function and expand argument branches
#define DEFINE_LUA_FUNCTION(name, ...) \
    static int lua_##name(lua_State* L) { \
        GET_MEMORY_INSTANCE(); \
        __VA_ARGS__; \
        lua_pushnil(L); \
        return 1; \
    } \
    struct __##name##_Registrar { \
        __##name##_Registrar() { \
            lua_pushcclosure(LuaEngine::getLuaState(), lua_##name, 0); \
            lua_setglobal(LuaEngine::getLuaState(), #name); \
        } \
    } __##name##_registrar_instance;

// Macro to handle argument branching
#define ARGUMENT_BRANCH(arg, code) \
    if (std::string(luaL_checkstring(L, 1)) == arg) { \
        code \
    }

#endif // MEMORY_MACROS_H
