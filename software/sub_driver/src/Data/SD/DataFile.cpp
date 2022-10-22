/**
 * @file DataFile.cpp
 * @author Daniel Kim
 * @brief Creates file names
 * @version 0.1
 * @date 2022-07-01
 * 
 * @copyright Copyright (c) 2022 OceanAI (https://github.com/daniel360kim/OceanAI)
 * 
 */
#include "DataFile.h"
#include "../../core/config.h"
#include "../../core/debug.h"
#include "../../core/Timer.h"
#include "SD.h"


#include <Arduino.h>

bool DataFile::file_initialized = false;

DataFile::DataFile(const char* file_name, ENDING ending)
{
	num_bytes = strlen(file_name);
	switch(ending)
	{
		case CSV:
			this->file_name = appendChars(file_name, "00.csv");
			break;

		case TXT:
			this->file_name = appendChars(file_name, "00.txt");
			break;
		
		case DAT:
			this->file_name = appendChars(file_name, "00.data");
			break;
		
		case JPG:
			this->file_name = appendChars(file_name, "00.jpg");
			break;
		
		case PNG:
			this->file_name = appendChars(file_name, "00.png");
			break;
		
		default:
			this->file_name = appendChars(file_name, "00.txt"); //default to .txt
	
	}	
}

/**
 * Creates variable file name after initializing SD card
 *
 * @param sd_cs Chip select pin of the SD.
 * @return True: file creation successful/False: file creation unsuccessful.
 */

bool DataFile::createFile()
{
	if(!DataFile::file_initialized) //only initialize SD once
	{
		if(!sd.begin(SdioConfig(FIFO_SDIO)))
		{
			#if DEBUG_ON
				char* msg = (char*)"SD card initialization failed";
				ERROR_LOG(Debug::Critical_Error, msg);
				#if LIVE_DEBUG
					Serial.println(msg);
				#endif
			#endif

			return false;
		}
		else
		{
			#if DEBUG_ON
				char* msg = (char*)"SD card initialization successful";
				SUCCESS_LOG(msg);
				#if LIVE_DEBUG
					Serial.println(msg);
				#endif
			#endif
		}
		DataFile::file_initialized = true;
	}
	
	while(sd.exists(file_name))
	{
		if(file_name[num_bytes + 1] != '9')
		{
			file_name[num_bytes + 1]++;
		}
		else if(file_name[num_bytes] != '9')
		{
			file_name[num_bytes + 1] = '0';
			file_name[num_bytes]++;
		}
		else
		{
			#if DEBUG_ON
				char* msg = (char*)"Too many files; Could not make successive file name";
				ERROR_LOG(Debug::Critical_Error, msg);
				#if LIVE_DEBUG
					Serial.println(msg);
				#endif
			#endif
			return false;
		}

	}

	return true;
}

/**
 * Used to append two chars. 
 *
 * @param num_bytes length of both chars + 1
 * @param buff char that the resized product is returned to
 * @return True: allocation successful/False: allocation failed
 */
bool DataFile::resizeBuff(int num_bytes, uint8_t** buff) {

	if(*buff) 
	{                                        // If we did NOT get passed in a NULL..
		free(*buff);                                    // We free the memory.
		*buff = NULL;                                    // Set the pointer to NULL.
	}
	if (num_bytes > 0) 
	{                                // If we got a positive non zero value..
		*buff = (uint8_t*)malloc(num_bytes);    // We attempt allocate that number of bytes.
		return *buff != NULL;                        // And we return true for non NULL result (non-NULL = Success)
	}
	return true;                                        // In this case we were not asked to allocate anything so it was a success.
}

/**
 * Appends or concatenate two chars
 *
 * @param hostname first char for combining
 * @param def_host second char that gets appended to hostname
 * @return the concatenated char
 */
char* DataFile::appendChars(const char *hostname, const char *def_host)
{
  	// create hostname
	int num_bytes = strlen(hostname) + strlen(def_host) + 1; // +1 for the null terminator | allocate a buffer of the required size
	char *hostname_str = NULL;
	resizeBuff(num_bytes, &hostname_str);
	strcpy(hostname_str, hostname);
	strcat(hostname_str, def_host); // append default hostname to hostname
	return hostname_str;
}

