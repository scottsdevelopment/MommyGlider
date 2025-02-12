/**
 * @file Controller.cpp
 * @brief Implementation of the Controller class for simulating keyboard and mouse interactions with the game.
 *
 * This file defines the Controller class, which facilitates hardware-level interactions
 * such as simulating key presses and mouse clicks, and ensures the World of Warcraft (WoW) game window is in focus.
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

#include "Controller.h"
#include <iostream>
#include <windows.h>
#include "Util.h"
#include "CalibrationData.h"

 // Helper function to find the WoW game window
HWND Controller::findGameWindow() {
    HWND hwnd = FindWindowA(NULL, "World of Warcraft");
    if (!hwnd) {
        std::cerr << "Game window not found. Ensure WoW is running.\n";
    }
    return hwnd;
}

// Constructor
Controller::Controller() : gameWindow(findGameWindow()) {}

#include <random>
#include <thread>

void Controller::simulateKeyPress(WORD key) const {
    if (!gameWindow) {
        std::cerr << "Error: gameWindow is null. Cannot simulate key press.\n";
        return;
    }

    // Bring the game window to the foreground
    //if (!SetForegroundWindow(gameWindow)) {
    //    std::cerr << "Failed to set the game window to the foreground.\n";
    //    return;
    //}

    int minDelayMs = 50;
    int maxDelayMs = 150;

    std::cout << "Game window successfully set to the foreground.\n";

    // Random delay generator
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> dist(minDelayMs, maxDelayMs);
    int delayMs = dist(rng);

    // Simulate key press using PostMessage
    LPARAM lParam = (MapVirtualKey(key, MAPVK_VK_TO_VSC) << 16) | 1; // Build LPARAM with scan code and repeat count

    // Send WM_KEYDOWN message
    if (!PostMessage(gameWindow, WM_KEYDOWN, key, lParam)) {
        std::cerr << "Failed to send WM_KEYDOWN message for key: " << key << "\n";
    }
    else {
        std::cout << "WM_KEYDOWN message sent successfully for key: " << key << "\n";
    }

    // Random delay to simulate key hold duration
    std::cout << "Holding key for " << delayMs << " milliseconds.\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));

    // Send WM_KEYUP message
    lParam |= 1 << 31; // Set the transition state to key-up
    if (!PostMessage(gameWindow, WM_KEYUP, key, lParam)) {
        std::cerr << "Failed to send WM_KEYUP message for key: " << key << "\n";
    }
    else {
        std::cout << "WM_KEYUP message sent successfully for key: " << key << "\n";
    }
}



// Simulate a mouse click
void Controller::simulateMouseClick(DWORD button) const {
    INPUT inputs[2] = {}; // Array for press and release events

    // Press the button
    inputs[0].type = INPUT_MOUSE;
    inputs[0].mi.dwFlags = button;

    // Release the button
    inputs[1].type = INPUT_MOUSE;
    inputs[1].mi.dwFlags = button;

    // Send both events
    SendInput(2, inputs, sizeof(INPUT));
} 


// Simulate mouse movement to a specific position
void Controller::simulateMouseMovement(int x, int y) const {
    INPUT input = {};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
    input.mi.dx = static_cast<LONG>((x * 65535) / GetSystemMetrics(SM_CXSCREEN));
    input.mi.dy = static_cast<LONG>((y * 65535) / GetSystemMetrics(SM_CYSCREEN));
    SendInput(1, &input, sizeof(INPUT));
}

// Check if WoW is in the foreground
bool Controller::isWoWInForeground() const {
    HWND hwnd = GetForegroundWindow();
    return hwnd == gameWindow;
}

// Simulate a click at monitor coordinates
void Controller::clickAtMonitorCoords(int screenX, int screenY) const {
    if (!gameWindow) {
        std::cerr << "Game window not found. Skipping mouse click.\n";
        return;
    }

    // Simulate mouse movement and click
    simulateMouseMovement(screenX, screenY);
    simulateMouseClick(MOUSEEVENTF_LEFTDOWN);
    simulateMouseClick(MOUSEEVENTF_LEFTUP);

    std::cout << "Clicked at Monitor Coords: (" << screenX << ", " << screenY << ")\n";
}

// Simulate a click at UI coordinates
void Controller::clickAtUICoords(float uiX, float uiY, const CalibrationData& calibration) const {
    auto [screenX, screenY] = translateToMonitorCoords(uiX, uiY, calibration);
    clickAtMonitorCoords(screenX, screenY);
}

// Press a keyboard key
void Controller::pressKey(WORD key) const {
    if (!gameWindow) {
        std::cerr << "Game window not found. Skipping key press.\n";
        return;
    }
    simulateKeyPress(key);
}

// Ensure WoW window is active
void Controller::focusGameWindow() {
    if (!gameWindow) {
        gameWindow = findGameWindow();
    }
    if (gameWindow && !isWoWInForeground()) {
        SetForegroundWindow(gameWindow);
    }
}

// Additional functions like leftClick(), rightClick(), mouse4Click(), etc., can remain the same.
