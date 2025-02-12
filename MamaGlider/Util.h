/**
 * @file Util.h
 * @brief Utility functions for coordinate translation and other shared logic.
 *
 * This header provides reusable utility functions for coordinate translations
 * between UI and monitor systems.
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

#ifndef UTIL_H
#define UTIL_H

#include "CalibrationData.h"
#include <iostream>
#include <utility>

 // Constants for scaling
constexpr float UI_SCALE_FACTOR = 0.53333333; // Magic constant for UI scaling

/**
 * @brief Translates UI coordinates to monitor coordinates.
 *
 * @param uiX The X coordinate in the UI space.
 * @param uiY The Y coordinate in the UI space.
 * @param calibration Calibration data with screen and UI scaling information.
 * @return A pair of integers representing the monitor (screen) coordinates.
 */
inline std::pair<int, int> translateToMonitorCoords(float uiX, float uiY, const CalibrationData& calibration) {
    // Calculate the maximum UI layout dimensions
    float UILayOutMax_X = (calibration.screenWidth * calibration.uiScale) * UI_SCALE_FACTOR;
    float UILayOutMax_Y = (calibration.screenHeight * calibration.uiScale) * UI_SCALE_FACTOR;

    // Log the layout dimensions
    //std::cout << "UI Layout Max Dimensions:\n"
    //    << "  UILayOutMax_X: " << UILayOutMax_X << "\n"
    //    << "  UILayOutMax_Y: " << UILayOutMax_Y << "\n";

    // Calculate the fractional position within the UI layout
    float fractionX = uiX / UILayOutMax_X;
    float fractionY = uiY / UILayOutMax_Y;

    // Log the fractional coordinates
    //std::cout << "Fractional Coordinates:\n"
    //    << "  FractionX: " << fractionX << "\n"
    //    << "  FractionY: " << fractionY << "\n";

    // Translate to monitor coordinates by applying the fraction to screen dimensions
    float screenX = static_cast<int>(fractionX * calibration.screenWidth);
    int screenY = static_cast<int>(calibration.screenHeight - (fractionY * calibration.screenHeight));

    // Log the final monitor coordinates
 /*   std::cout << "Monitor Coordinates:\n"
        << "  ScreenX: " << screenX << "\n"
        << "  ScreenY: " << screenY << "\n";*/

    return { screenX, screenY };
}



/**
 * @brief Translates monitor coordinates to UI coordinates.
 *
 * @param screenX The X coordinate in the monitor space.
 * @param screenY The Y coordinate in the monitor space.
 * @param calibration Calibration data with screen and UI scaling information.
 * @return A pair of floats representing the UI coordinates.
 */
inline std::pair<float, float> translateToUICoords(int screenX, int screenY, const CalibrationData& calibration) {
    float uiX = (screenX * UI_SCALE_FACTOR) / (calibration.screenWidth / calibration.uiScale);
    float uiY = (screenY * UI_SCALE_FACTOR) / (calibration.screenHeight / calibration.uiScale);

    // Log translation details
    // std::cout << "Monitor to UI Translation:\n"
    //    << "Monitor Coords: (" << screenX << ", " << screenY << ") -> UI Coords: ("
    //    << uiX << ", " << uiY << ")\n";

    return { uiX, uiY };
}

#endif // UTIL_H
