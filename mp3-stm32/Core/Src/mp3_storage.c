#include "mp3_storage.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

MP3FileInfo* mp3Infos[MAX_MP3_FILES];
uint32_t mp3FileCount = 0;

static FATFS mp3_fs;
static DIR mp3_dir;
static FILINFO mp3_fno;

static uint32_t ReadSynchsafeInteger(uint8_t* bytes)
{
    return (bytes[0] << 21) | (bytes[1] << 14) | (bytes[2] << 7) | bytes[3];
}

uint32_t MP3_Storage_Init(void)
{
    return f_mount(&mp3_fs, "", 0);
}

uint32_t MP3_Storage_ListFiles(const char* dirPath, char* fileTable[])
{
    FRESULT res;
    uint32_t index = 0;
    res = f_findfirst(&mp3_dir, &mp3_fno, dirPath, "*.mp3");
    while (mp3_fno.fname[0] && res == FR_OK && index < MAX_MP3_FILES)
    {
        snprintf(fileTable[index++], MAX_MP3_FILE_NAME, "%s", mp3_fno.fname);
        res = f_findnext(&mp3_dir, &mp3_fno);
    }
    f_closedir(&mp3_dir);
    return index;
}

uint8_t MP3_Storage_ReadID3v1(const char* filePath, ID3v1Tag* tag)
{
    FIL file;
    UINT bytesRead;
    if (f_open(&file, filePath, FA_READ) != FR_OK) return 1;
    if (f_lseek(&file, f_size(&file) - 128) != FR_OK) { f_close(&file); return 2; }
    if (f_read(&file, tag, sizeof(ID3v1Tag), &bytesRead) != FR_OK || bytesRead != 128) { f_close(&file); return 3; }
    f_close(&file);
    return (strncmp(tag->tag, "TAG", 3) == 0) ? 0 : 4;
}

uint8_t MP3_Storage_ReadID3v2Tag(const char* filePath, ID3v2Tag* tag)
{
    FIL file;
    UINT bytesRead;
    uint8_t header[10], frameHeader[10];
    uint32_t tagSize, position = 10;
    memset(tag, 0, sizeof(ID3v2Tag));

    if (f_open(&file, filePath, FA_READ) != FR_OK) return 0;
    if (f_read(&file, header, 10, &bytesRead) != FR_OK || bytesRead != 10) { f_close(&file); return 0; }
    if (strncmp((char*)header, "ID3", 3) != 0) { f_close(&file); return 0; }

    tagSize = ReadSynchsafeInteger(&header[6]);

    while (position < tagSize)
    {
        if (f_lseek(&file, position) != FR_OK) break;
        if (f_read(&file, frameHeader, 10, &bytesRead) != FR_OK || bytesRead != 10) break;
        if (frameHeader[0] == 0) break;

        char frameID[5] = {0};
        memcpy(frameID, frameHeader, 4);
        uint32_t frameSize = (frameHeader[4] << 24) | (frameHeader[5] << 16) |
                             (frameHeader[6] << 8) | frameHeader[7];
        if (frameSize < 1 || frameSize > 512) break;

        char content[128] = {0};
        if (f_read(&file, content, frameSize, &bytesRead) != FR_OK || bytesRead != frameSize) break;
        char* text = &content[1];

        if (strcmp(frameID, "TIT2") == 0) strncpy(tag->title, text, MAX_ID3V2_TEXT - 1);
        else if (strcmp(frameID, "TPE1") == 0) strncpy(tag->artist, text, MAX_ID3V2_TEXT - 1);
        else if (strcmp(frameID, "TALB") == 0) strncpy(tag->album, text, MAX_ID3V2_TEXT - 1);
        else if (strcmp(frameID, "TYER") == 0 || strcmp(frameID, "TDRC") == 0) strncpy(tag->year, text, 4);

        position += 10 + frameSize;
    }

    f_close(&file);
    return 1;
}

MP3_TagType MP3_Storage_DetectTag(const char* filePath)
{
    FIL file;
    char buf[10];
    UINT bytesRead;
    uint8_t hasV1 = 0, hasV2 = 0;

    if (f_open(&file, filePath, FA_READ) == FR_OK) {
        if (f_read(&file, buf, 3, &bytesRead) == FR_OK && bytesRead == 3 && strncmp(buf, "ID3", 3) == 0)
            hasV2 = 1;

        if (f_lseek(&file, f_size(&file) - 128) == FR_OK &&
            f_read(&file, buf, 3, &bytesRead) == FR_OK && bytesRead == 3 &&
            strncmp(buf, "TAG", 3) == 0)
            hasV1 = 1;

        f_close(&file);
    }

    if (hasV1 && hasV2) return MP3_TAG_BOTH;
    if (hasV2) return MP3_TAG_ID3V2;
    if (hasV1) return MP3_TAG_ID3V1;
    return MP3_TAG_NONE;
}

uint32_t MP3_Storage_LoadAllMetadata(const char* dirPath)
{
    char* tempNames[MAX_MP3_FILES];
    char debug[64];

    for (int i = 0; i < MAX_MP3_FILES; i++) {
        tempNames[i] = malloc(MAX_MP3_FILE_NAME);
        if (!tempNames[i]) {
            BSP_LCD_DisplayStringAtLine(0, (uint8_t*)"Erreur malloc nom");
            return 0;
        }
    }

    mp3FileCount = MP3_Storage_ListFiles(dirPath, tempNames);

    // Afficher le nombre de fichiers trouvés
    snprintf(debug, sizeof(debug), "MP3 trouves : %lu", mp3FileCount);
    BSP_LCD_DisplayStringAtLine(1, (uint8_t*)debug);

    if (mp3FileCount == 0) {
        BSP_LCD_DisplayStringAtLine(2, (uint8_t*)"Aucun fichier .mp3");
    }

    for (uint32_t i = 0; i < mp3FileCount; i++) {
        // Afficher le nom du fichier trouvé
        snprintf(debug, sizeof(debug), "%lu: %s", i, tempNames[i]);
        BSP_LCD_DisplayStringAtLine(3 + i, (uint8_t*)debug);

        mp3Infos[i] = malloc(sizeof(MP3FileInfo));
        if (!mp3Infos[i]) continue;

        mp3Infos[i]->filename = malloc(strlen(tempNames[i]) + 1);
        strcpy(mp3Infos[i]->filename, tempNames[i]);

        char path[64];
        snprintf(path, sizeof(path), "%s%s", dirPath, tempNames[i]);

        mp3Infos[i]->tagType = MP3_Storage_DetectTag(path);

        if (mp3Infos[i]->tagType == MP3_TAG_ID3V1 || mp3Infos[i]->tagType == MP3_TAG_BOTH) {
            mp3Infos[i]->tagV1 = malloc(sizeof(ID3v1Tag));
            mp3Infos[i]->tagV2 = NULL;
            MP3_Storage_ReadID3v1(path, mp3Infos[i]->tagV1);
        } else if (mp3Infos[i]->tagType == MP3_TAG_ID3V2) {
            mp3Infos[i]->tagV2 = malloc(sizeof(ID3v2Tag));
            mp3Infos[i]->tagV1 = NULL;
            MP3_Storage_ReadID3v2Tag(path, mp3Infos[i]->tagV2);
        } else {
            mp3Infos[i]->tagV1 = NULL;
            mp3Infos[i]->tagV2 = NULL;
        }
    }

    for (int i = 0; i < MAX_MP3_FILES; i++) {
        free(tempNames[i]);
    }

    return mp3FileCount;
}

void MP3_Storage_FreeMetadata(void)
{
    for (uint32_t i = 0; i < mp3FileCount; i++) {
        if (mp3Infos[i]) {
            if (mp3Infos[i]->filename) free(mp3Infos[i]->filename);
            if (mp3Infos[i]->tagV1) free(mp3Infos[i]->tagV1);
            if (mp3Infos[i]->tagV2) free(mp3Infos[i]->tagV2);
            free(mp3Infos[i]);
            mp3Infos[i] = NULL;
        }
    }
    mp3FileCount = 0;
}
