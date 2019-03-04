#pragma once

#include <psp2/io/stat.h>
#include <psp2/io/dirent.h>
#include <psp2/io/fcntl.h>

#define SCE_ERROR_ERRNO_EEXIST 0x80010011
#define MAX_PATH_LENGTH 1024

#define TRANSFER_SIZE (128 * 1024)

class Filesystem {
    public:
        static std::string readFile(std::string file);
        static int writeFile(std::string file, std::string buf);
        static int copyFile(std::string src, std::string dst);
        static int copyPath(std::string src, std::string dst);
        static int mkDir(std::string path);
        static int removePath(std::string path);
        static bool fileExists(std::string path);
};

int doesDirExist(const char* path);

int copyFile(const char *src_path, const char *dst_path);