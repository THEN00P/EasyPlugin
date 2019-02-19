#pragma once

#include <psp2/io/stat.h>
#include <psp2/io/dirent.h>
#include <psp2/io/fcntl.h>

#define SCE_ERROR_ERRNO_EEXIST 0x80010011
#define MAX_PATH_LENGTH 1024

#define TRANSFER_SIZE (128 * 1024)

typedef struct SfoHeader {
  uint32_t magic;
  uint32_t version;
  uint32_t keyofs;
  uint32_t valofs;
  uint32_t count;
} __attribute__((packed)) SfoHeader;

typedef struct SfoEntry {
  uint16_t nameofs;
  uint8_t  alignment;
  uint8_t  type;
  uint32_t valsize;
  uint32_t totalsize;
  uint32_t dataofs;
} __attribute__((packed)) SfoEntry;

#define SFO_MAGIC 0x46535000

int getSfoString(void *buffer, char *name, char *stringS, int length);

int allocateReadFile(const char *file, void **buffer);

class Filesystem {
    public:
        static std::string readFile(std::string file);
        static int writeFile(std::string file, std::string buf);
        static int copyFile(std::string src, std::string dst);
        static int copyPath(std::string src, std::string dst);
        static int removePath(std::string path);
        static bool fileExists(std::string path);
};

int copyFile(const char *src_path, const char *dst_path);