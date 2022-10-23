/**
 * @file DataFile.h
 * @author Daniel Kim
 * @brief Creates file names
 * @version 0.1
 * @date 2022-07-01
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */
#ifndef DataFile_h
#define DataFile_h

#include <Arduino.h>
#include <SDFat.h>
#include <stdint.h>

#include "SD.h"
#include "../../core/config.h"
#include "../../core/debug.h"

class DataFile 
{
public:
     enum ENDING
    {
        CSV,
        TXT,
        DAT,
        JPG,
        PNG,
    };

    DataFile(const char* file_name, ENDING ending);

    static bool createFolder(const char* folder_name);
    bool createFile();

    inline const char* getFileName() const { return m_filename; }

private:
    static bool initializeSD();
    char* appendChars(const char* hostname, const char* def_host);
    bool resizeBuff(int num_bytes, uint8_t** buff);
    bool resizeBuff(int num_bytes, char** buff) { return resizeBuff(num_bytes,(uint8_t**)buff); } //overloaded function to support appendChars()

    uint8_t m_filename_length;
    static bool file_initialized;
    char* m_filename;
};

#endif