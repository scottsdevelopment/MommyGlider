/**
 * @file Controller.h
 * @brief Declaration of the Controller class for managing hardware-level interactions.
 *
 * This file defines the Controller class, which provides methods to simulate key presses,
 * mouse clicks, and manage interactions with the World of Warcraft (WoW) game window.
 *
 * @license MIT
 * @author [Your Name]
 */

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <windows.h>
#include "CalibrationData.h"

class Controller {
private:
    HWND gameWindow; // Handle to the WoW game window

    HWND findGameWindow();                      // Helper to find the WoW game window
    void simulateKeyPress(WORD key) const;      // Simulate a keyboard key press
    void simulateMouseClick(DWORD button) const; // Simulate a mouse click
    void simulateMouseMovement(int x, int y) const; // Simulate mouse movement

public:
    Controller();                              // Constructor

    // Check if WoW is the foreground window
    bool isWoWInForeground() const;

    // Keyboard interaction
    void pressKey(WORD key) const;             // Press a keyboard key
    void holdControlKey() const;               // Hold the Control key

    // Mouse interaction
    void leftClick() const;                    // Simulate left mouse click
    void rightClick() const;                   // Simulate right mouse click
    void mouse4Click() const;                  // Simulate Mouse4 (XButton1) click
    void mouse5Click() const;                  // Simulate Mouse5 (XButton2) click
    void moveMouseTo(int x, int y) const;      // Move mouse to specific coordinates

    // Window interaction
    void focusGameWindow();                    // Bring WoW to the foreground

    // Mouse click with coordinate translation
    void clickAtMonitorCoords(int screenX, int screenY) const;       // Click at monitor coordinates
    void clickAtUICoords(float uiX, float uiY, const CalibrationData& calibration) const; // Click at UI coordinates
};


#endif // CONTROLLER_H
