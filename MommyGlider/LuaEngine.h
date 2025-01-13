/**
 * @file LuaEngine.h
 * @brief Header file for the LuaEngine class, which integrates Lua scripts with game memory and control.
 *
 * This file declares the LuaEngine class and its methods for registering Lua bindings,
 * executing Lua scripts, and interfacing with game memory and the controller.
 *
 * @license MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
 * LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * @author [Your Name]
 */

#ifndef LUAENGINE_H
#define LUAENGINE_H

#include <string>
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "Memory.h"
#include "Controller.h"

 /**
  * @class LuaEngine
  * @brief Integrates Lua scripting capabilities with game memory and control.
  */
class LuaEngine {
private:
    lua_State* luaState;         ///< Lua state
    Memory& memory;              ///< Reference to Memory instance
    Controller& controller;      ///< Reference to Controller instance

    static LuaEngine* instance;  ///< Static instance pointer for Lua callbacks

public:
    /**
     * @brief Constructor to initialize LuaEngine with references to Memory and Controller.
     * @param mem Reference to Memory instance.
     * @param ctrl Reference to Controller instance.
     */
    LuaEngine(Memory& mem, Controller& ctrl);

    /**
     * @brief Destructor to clean up Lua state.
     */
    ~LuaEngine();

    /**
     * @brief Executes a Lua script from the specified file.
     * @param filePath Path to the Lua script file.
     */
    void executeScript(const std::string& filePath);

    /**
     * @brief Calls a Lua function by its name.
     * @param functionName Name of the Lua function to call.
     */
    void callFunction(const std::string& functionName);
};

#endif // LUAENGINE_H
