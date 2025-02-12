/**
 * @file Main.cpp
 * @brief Entry point for the application. Initializes calibration, memory, controller, and Lua engine.
 */

#include <iostream>
#include "LuaEngine.h"

int main() {
    // Example calibration data
    CalibrationData calibration = {
        2560,   // screenWidth
        1440,   // screenHeight
        1,   // scale
        36,     // refX
        79.5,     // refY
        1,      // pixelSize
        1       // spacing
    };

    // Initialize Memory
    Memory memory(calibration);

    // Corrected instantiation of Controller
    Controller controller;

    // Initialize LuaEngine
    LuaEngine luaEngine(memory, controller);

    std::cout << "Starting Lua engine.\n";
    //luaEngine.loadAddons("Interface/Addons/");
    luaEngine.executeScript("Interface/main.lua");
    return 0;
}