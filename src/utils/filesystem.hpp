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

std::string getSfoString(const char *path, const char *name);

class Filesystem {
    public:
        std::string readFile(std::string file);
        static int log(std::string log);
        int writeFile(std::string file, std::string buf);
        int copyFile(std::string src, std::string dst);
        int copyPath(std::string src, std::string dst);
        int removePath(std::string path);
        bool fileExists(std::string path);
    private:
        bool hasEndSlash(std::string str);
};

int copyFile(const char *src_path, const char *dst_path);