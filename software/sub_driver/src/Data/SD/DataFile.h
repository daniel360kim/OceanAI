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
    ~DataFile() {}

    bool createFile();

    char* file_name;

private:
    char* appendChars(const char* hostname, const char* def_host);
    bool resizeBuff(int num_bytes, uint8_t** buff);
    bool resizeBuff(int num_bytes, char** buff) { return resizeBuff(num_bytes,(uint8_t**)buff); } //overloaded function to support appendChars()

    uint8_t num_bytes;
};

#endif