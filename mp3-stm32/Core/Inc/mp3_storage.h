#ifndef __MP3_STORAGE_H
#define __MP3_STORAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ff.h"
#include <stdint.h>

#define MAX_MP3_FILES      25
#define MAX_MP3_FILE_NAME  64
#define MAX_ID3V2_TEXT     64
#define MAX_WAV_TEXT 64

typedef struct {
    char tag[3];
    char title[30];
    char artist[30];
    char album[30];
    char year[4];
    char comment[30];
    uint8_t genre;
} ID3v1Tag;

typedef struct {
    char title[MAX_ID3V2_TEXT];
    char artist[MAX_ID3V2_TEXT];
    char album[MAX_ID3V2_TEXT];
    char year[5];
} ID3v2Tag;

typedef enum {
    MP3_TAG_NONE = 0,
    MP3_TAG_ID3V1 = 1,
    MP3_TAG_ID3V2 = 2,
    MP3_TAG_BOTH  = 3
} MP3_TagType;

typedef struct MP3FileInfo {
    char* filename;
    MP3_TagType tagType;
    ID3v1Tag* tagV1;
    ID3v2Tag* tagV2;
} MP3FileInfo;

typedef struct {
    char title[MAX_WAV_TEXT];
    char artist[MAX_WAV_TEXT];
    char album[MAX_WAV_TEXT];
    char year[5];
} WAVTag;

extern MP3FileInfo* mp3Infos[MAX_MP3_FILES];
extern uint32_t mp3FileCount;

uint32_t MP3_Storage_Init(void);
uint32_t MP3_Storage_ListFiles(const char* dirPath, char* fileTable[]);
uint8_t MP3_Storage_ReadID3v1(const char* filePath, ID3v1Tag* tag);
uint8_t MP3_Storage_ReadID3v2Tag(const char* filePath, ID3v2Tag* tag);
MP3_TagType MP3_Storage_DetectTag(const char* filePath);
uint32_t MP3_Storage_LoadAllMetadata(const char* dirPath);
void MP3_Storage_FreeMetadata(void);
uint8_t WAV_Storage_ReadMetadata(const char* filePath, WAVTag* tag);

#ifdef __cplusplus
}
#endif

#endif /* __MP3_STORAGE_H */
