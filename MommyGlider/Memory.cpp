/**
 * @file Memory.cpp
 * @brief Implementation of the Memory class for reading and managing game state through Shixel offsets.
 *
 * This file defines methods for accessing game-related data stored as RGB pixel values on the screen.
 * The offsets and their logic are defined using macros to simplify the code structure.
 *
 * @author [Your Name]
 */

#include <iostream>
#include <map>
#include <string>
#include <stdexcept>
#include <windows.h>
#include <cmath>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <chrono>
#include "Memory.h"
#include "Util.h"
#include "CalibrationData.h"
#include <fstream>


// Static member initialization
std::map<std::string, OffsetMetadata> Memory::offsetIndices;

// Constructor
Memory::Memory(const CalibrationData& calibrationData) : calibration(calibrationData), stopThread(false) {
    // Start the data capture thread
    captureThread = std::thread(&Memory::captureOffsets, this);
}

// Destructor
Memory::~Memory() {
    // Signal the thread to stop and join it
    {
        std::lock_guard<std::mutex> lock(threadMutex);
        stopThread = true;
    }
    if (captureThread.joinable()) {
        captureThread.join();
    }
}

/**
 * @brief Reads the pixel color from a specific screen coordinate.
 *
 * @param x The x-coordinate of the pixel.
 * @param y The y-coordinate of the pixel.
 * @return A Color struct containing the RGB values of the pixel.
 */
Color Memory::getPixelColor(int x, int y) const {
    HDC hdc = GetDC(NULL);
    COLORREF pixel = GetPixel(hdc, x, y);
    ReleaseDC(NULL, hdc);
    return { GetRValue(pixel), GetGValue(pixel), GetBValue(pixel) };
}

/**
 * @brief Calculates the pixel coordinates for a given offset key, assuming a single row.
 *
 * @param key The offset key as a string.
 * @return A pair containing the x and y coordinates in monitor coordinates.
 */
std::pair<int, int> Memory::calculatePixelCoordinates(const std::string& key) const {
    auto it = offsetIndices.find(key);
    if (it == offsetIndices.end()) {
        throw std::runtime_error("Offset key not found: " + key);
    }

    auto meta = it->second;
    int column = meta.index + 1;

    float uiX = calibration.refX + (column * calibration.spacing) - calibration.pixelSize;
    float uiY = calibration.refY - calibration.pixelSize;

    return translateToMonitorCoords(uiX, uiY, calibration);
}
/**
 * @brief Calculates the bounding box for the offsets and converts it to monitor coordinates.
 *
 * @return A RECT structure defining the bounding box in monitor coordinates.
 */
RECT Memory::calculateBoundingBox() const {
    // Initial bounding box based on calibration
    RECT boundingBox;
    boundingBox.left = static_cast<int>(calibration.refX);
    boundingBox.top = static_cast<int>(calibration.refY);
    boundingBox.right = boundingBox.left + static_cast<int>(offsetIndices.size() * calibration.spacing);
    boundingBox.bottom = boundingBox.top + calibration.pixelSize;

    std::cout << "Initial Bounding Box (UI Coordinates): "
        << "Left: " << boundingBox.left << ", Top: " << boundingBox.top
        << ", Right: " << boundingBox.right << ", Bottom: " << boundingBox.bottom << std::endl;

    // Convert to monitor coordinates
    auto [screenLeft, screenTop] = translateToMonitorCoords(boundingBox.left, boundingBox.top, calibration);
    auto [screenRight, screenBottom] = translateToMonitorCoords(boundingBox.right, boundingBox.bottom, calibration);

    boundingBox.left = screenLeft;
    boundingBox.top = screenTop;
    boundingBox.right = screenRight;
    boundingBox.bottom = screenBottom;

    std::cout << "Bounding Box After Conversion (Monitor Coordinates): "
        << "Left: " << boundingBox.left << ", Top: " << boundingBox.top
        << ", Right: " << boundingBox.right << ", Bottom: " << boundingBox.bottom << std::endl;

    return boundingBox;
}
/**
 * @brief Captures all offset values in a thread-safe manner and saves the screen region to a file.
 */
void Memory::captureOffsets() {
    HDC hdc = GetDC(NULL);
    RECT boundingBox = calculateBoundingBox();

    int width = boundingBox.right - boundingBox.left;
    int height = calibration.pixelSize;

    auto [spacing, _] = translateToMonitorCoords(calibration.spacing, 0, calibration);

    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, width, height);
    SelectObject(memDC, hBitmap);

    while (true) {
        {
            std::lock_guard<std::mutex> lock(threadMutex);
            if (stopThread) break;

            // Copy the screen region to memory
            BOOL result = BitBlt(memDC, 0, 0, width, height, hdc, boundingBox.left, boundingBox.top, SRCCOPY);
            if (!result) {
                std::cerr << "Error: BitBlt failed. Check coordinates and device contexts." << std::endl;
                continue;
            }

            // Validate calibration color
            COLORREF firstPixel = GetPixel(memDC, 0, 0);
            Color calibrationColor = { GetRValue(firstPixel), GetGValue(firstPixel), GetBValue(firstPixel) };

            if (calibrationColor.r != 255 || calibrationColor.g != 217 || calibrationColor.b != 4) {
                continue;
            }

            // Extract pixel data
            for (const auto& [key, metadata] : offsetIndices) {
                auto [x, _] = translateToMonitorCoords(((metadata.index + 1) * calibration.spacing) - calibration.pixelSize, 0, calibration);

                COLORREF pixel = GetPixel(memDC, x, 0);
                Color color = { GetRValue(pixel), GetGValue(pixel), GetBValue(pixel) };

                try {
                    capturedData[key] = decodeRGBToValue<int>(color, metadata.type);
                }
                catch (const std::exception& e) {
                    std::cerr << "Error capturing offset for key " << key << ": " << e.what() << std::endl;
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }

    DeleteObject(hBitmap);
    DeleteDC(memDC);
    ReleaseDC(NULL, hdc);
}


/**
 * @brief Saves a bitmap to a file.
 *
 * @param hBitmap The handle to the bitmap to save.
 * @param width The width of the bitmap.
 * @param height The height of the bitmap.
 * @param filename The name of the file to save the bitmap to.
 */
void Memory::saveBitmapToFile(HBITMAP hBitmap, int width, int height, const std::string& filename) {
    BITMAP bmp;
    GetObject(hBitmap, sizeof(BITMAP), &bmp);

    BITMAPFILEHEADER bmfHeader;
    BITMAPINFOHEADER bi;

    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = width;
    bi.biHeight = -height; // Negative height to ensure top-down bitmap
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    DWORD dwBmpSize = ((width * bi.biBitCount + 31) / 32) * 4 * height;
    HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize);
    char* lpbitmap = (char*)GlobalLock(hDIB);

    GetDIBits(GetDC(0), hBitmap, 0, (UINT)height, lpbitmap, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    // Create the .bmp file
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Unable to open file for writing: " << filename << std::endl;
        return;
    }

    // Write the BITMAPFILEHEADER
    bmfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bmfHeader.bfSize = dwBmpSize + bmfHeader.bfOffBits;
    bmfHeader.bfType = 0x4D42; // BM
    file.write(reinterpret_cast<const char*>(&bmfHeader), sizeof(BITMAPFILEHEADER));

    // Write the BITMAPINFOHEADER
    file.write(reinterpret_cast<const char*>(&bi), sizeof(BITMAPINFOHEADER));

    // Write the bitmap data
    file.write(lpbitmap, dwBmpSize);

    // Cleanup
    GlobalUnlock(hDIB);
    GlobalFree(hDIB);
    file.close();

    std::cout << "Bitmap saved to file: " << filename << std::endl;
}


/**
 * @brief Retrieves the captured value for a specific key.
 *
 * @param key The key for the offset value.
 * @return The captured value.
 */
int Memory::getCapturedValue(const std::string& key) const {
    std::lock_guard<std::mutex> lock(threadMutex);
    auto it = capturedData.find(key);
    if (it == capturedData.end()) {
        throw std::runtime_error("Captured value not found for key: " + key);
    }
    return it->second;
}

/**
 * @brief Decodes an RGB color to a value of the specified type.
 *
 * @tparam T The data type to decode.
 * @param color The color to decode.
 * @param type The type of the value (e.g., "bool", "int").
 * @param maxValue The maximum value for scaling (optional).
 * @return The decoded value.
 */
template <typename T>
T Memory::decodeRGBToValue(const Color& color, const std::string& type) const {
    if (type == "bool") {
        return static_cast<T>(color.g > 128);
    }
    else if (type == "int") {
        return static_cast<T>((color.r << 16) | (color.g << 8) | color.b);
    }
    else {
        std::cerr << "Unsupported data type for decodeRGBToValue: " << type << std::endl;
        return T{};
    }
}
