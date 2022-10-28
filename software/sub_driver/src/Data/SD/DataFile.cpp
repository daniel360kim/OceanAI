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


#include "SD.h"
#include "../../core/config.h"
#include "../../core/debug.h"

#include <Arduino.h>

bool DataFile::file_initialized = false;


DataFile::DataFile(const char* file_name, ENDING ending)
{
	m_filename_length = strlen(file_name);
	switch(ending)
	{
		case CSV:
			m_filename = appendChars(file_name, "00.csv");
			break;

		case TXT:
			m_filename = appendChars(file_name, "00.txt");
			break;
		
		case DAT:
			m_filename = appendChars(file_name, "00.data");
			break;
		
		case JPG:
			m_filename = appendChars(file_name, "00.jpg");
			break;
		
		case PNG:
			m_filename = appendChars(file_name, "00.png");
			break;
		
		case MSGPACK:
			m_filename = appendChars(file_name, "00.msgpack");
			break;
		
		default:
			m_filename = appendChars(file_name, "00.txt"); //default to .txt
	
	}	
}

/**
 * @brief Creates a folder in the SD card
 * 
 * @param folder_name name of the folder that will be created
 * @return true folder creation successful
 * @return false folder creation unsuccessful
 */
bool DataFile::createFolder(const char* folder_name)
{
	if(!initializeSD()) //initialize SD
	{
		return false;
	}

	if(sd.exists(folder_name)) //return if the folder already exists
	{
		return true;
	}
	else
	{
		if(!sd.mkdir(folder_name))
		{
			ERROR_LOG(Debug::Fatal, "Failed to create folder");
			return false;
		}
		else
		{
			SUCCESS_LOG("Folder created");
			return true;
		}
	}
}

/**
 * @brief Creates a file name, automatically numbering if the file name already exists
 * 
 * @return true file creation successful
 * @return false file creation unsuccessful
 */
bool DataFile::createFile()
{
	if(!initializeSD())
	{
		return false;
	}
	
	//Iterate through all files, adding a number to the file name if it already exists
	//Range of possible files is from 00-99
	while(sd.exists(m_filename))
	{
		if(m_filename[m_filename_length + 1] != '9')
		{
			m_filename[m_filename_length + 1]++;
		}
		else if(m_filename[m_filename_length] != '9')
		{
			m_filename[m_filename_length + 1] = '0';
			m_filename[m_filename_length]++;
		}
		else
		{
			ERROR_LOG(Debug::Critical_Error, "Could not number files");
			return false;
		}
	}

	return true;
}

bool DataFile::initializeSD()
{
	if(!DataFile::file_initialized) //only initialize SD once
	{
		if(!sd.begin(SdioConfig(FIFO_SDIO))) //Initialize SD for FIFO with TEENSY builtin
		{
			ERROR_LOG(Debug::Critical_Error, "SD initialization failed");
			return false;
		}
		else
		{
			SUCCESS_LOG("SD initialization successful");
		}
		DataFile::file_initialized = true; //after an initialization, we set this to true so we don't initialize again
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
		delete [] *buff;                     // ..delete the memory we allocated before
		*buff = NULL;                                    // Set the pointer to NULL.
	}
	if (num_bytes > 0) 
	{                                // If we got a positive non zero value..
		*buff = new uint8_t[num_bytes];                    // Allocate the memory.
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