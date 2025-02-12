    #ifndef MEMORY_H
    #define MEMORY_H

    #include <map>
    #include <string>
    #include <unordered_map>
    #include <mutex>
    #include <thread>
    #include <windows.h>
    #include "CalibrationData.h"

    // Color structure for RGB values
    struct Color {
        int r, g, b;
    };

    // Struct to store offset metadata
    struct OffsetMetadata {
        int index;          // Offset index
        std::string type;   // Offset type ("bool", "int", etc.)
    };

    class Memory {
    public:
        Memory(const CalibrationData& calibrationData);
        ~Memory();

        void saveBitmapToFile(HBITMAP hBitmap, int width, int height, const std::string& filename);

        // Public methods
        int getCapturedValue(const std::string& key) const;

 

        // Static member initialization
        static std::map<std::string, OffsetMetadata> offsetIndices;
        /**
         * @brief Macro to define memory offsets and their getter functions using thread-safe captured values.
         *
         * This macro generates a getter function that retrieves the offset value from the
         * thread-safe `capturedData` structure maintained by the `Memory` class.
         */
#define DEFINE_MEMORY_OFFSET(name, type, index) \
    type name() const { \
        try { \
            return static_cast<type>(getCapturedValue(#name)); \
        } catch (const std::exception& e) { \
            std::cerr << "Error retrieving value for " << #name << ": " << e.what() << std::endl; \
            return {}; /* Return default value for the type */ \
        } \
    } \
    struct __##name##_Registrar { \
        __##name##_Registrar() { \
            Memory::offsetIndices[#name] = {index, #type}; \
        } \
    } __##name##_registrar_instance;
#include "Offsets.h" // Include offset definitions
#undef DEFINE_MEMORY_OFFSET
    private:
        // Private methods
        Color getPixelColor(int x, int y) const;
        std::pair<int, int> calculatePixelCoordinates(const std::string& key) const;
        RECT calculateBoundingBox() const; // Ensure this matches implementation
        void captureOffsets();

        // Helper template for decoding RGB to value
        template <typename T>
        T decodeRGBToValue(const Color& color, const std::string& type) const;

        // Member variables
        CalibrationData calibration;
        mutable std::mutex threadMutex;
        std::unordered_map<std::string, int> capturedData;
        bool stopThread;
        std::thread captureThread;
    };

    #endif // MEMORY_H
