#include "zip.h"

#include <stdio.h>
#include <string.h>

#include "./global_include.h"

#include "./minizip/unzip.h"

#define dir_delimter '/'
#define MAX_FILENAME 512
#define READ_SIZE 8192

static void mkdir_rec(const char *dir) {
        char tmp[256];
        char *p = nullptr;
        size_t len;

        snprintf(tmp, sizeof(tmp),"%s",dir);
        len = strlen(tmp);
        if(tmp[len - 1] == '/')
                tmp[len - 1] = 0;
        for(p = tmp + 1; *p; p++)
                if(*p == '/') {
                        *p = 0;
                        sceIoMkdir(tmp, 0777);
                        *p = '/';
                }
        sceIoMkdir(tmp, 0777);
}

std::string dirnameOf(const std::string& fname)
{
     size_t pos = fname.find_last_of("\\/");
     return (std::string::npos == pos)
         ? ""
         : fname.substr(0, pos);
}


Zipfile::Zipfile(const std::string zip_path)
{
    zipfile_ = unzOpen(zip_path.c_str());
    if (!zipfile_)
    {
        throw std::runtime_error("Cannot open zip");
    }

    if (unzGetGlobalInfo(zipfile_, &global_info_) != UNZ_OK)
    {
        throw std::runtime_error("Cannot read zip info");
    }

}

Zipfile::~Zipfile()
{
    if(zipfile_) unzClose(zipfile_);
}

int Zipfile::Unzip(const std::string outpath)
{
    if (uncompressed_size_ == 0) {
        UncompressedSize();
    }

    char read_buffer[READ_SIZE];

    uLong i;
    uint64_t s_progress = 0;
    if(unzGoToFirstFile(zipfile_) != UNZ_OK) {
        throw std::runtime_error("Error going to first file");
    }

    for (i = 0; i < global_info_.number_entry; ++i)
    {
        unz_file_info file_info;
        char filename[MAX_FILENAME];
        char fullfilepath[MAX_FILENAME];
        if (unzGetCurrentFileInfo(
            zipfile_,
            &file_info,
            filename,
            MAX_FILENAME,
            nullptr, 0, nullptr, 0) != UNZ_OK)
        {
            throw std::runtime_error("Error reading zip file info");
        }

        sprintf(fullfilepath, "%s%s", outpath.c_str(), filename);

        // Check if this entry is a directory or file.
        const size_t filename_length = strlen(fullfilepath);
        if (fullfilepath[filename_length-1] == dir_delimter)
        {
            //sceIoMkdir(fullfilepath, 0777);
            mkdir_rec(fullfilepath);
        }
        else
        {
            // Create the dir where the file will be placed
            std::string destdir = dirnameOf(std::string(fullfilepath));
            mkdir_rec(destdir.c_str());

            // Entry is a file, so extract it.
            if (unzOpenCurrentFile(zipfile_) != UNZ_OK)
            {
                throw std::runtime_error("Cannot open file from zip");
            }

            // Open a file to write out the data.
            FILE *out = fopen(fullfilepath, "wb");
            if (out == nullptr)
            {
                unzCloseCurrentFile(zipfile_);
                throw std::runtime_error("Cannot open destination file");
            }

            int error;
            do
            {
                error = unzReadCurrentFile(zipfile_, read_buffer, READ_SIZE);
                if (error < 0)
                {
                    unzCloseCurrentFile(zipfile_);
                    throw std::runtime_error("Cannot read current zip file");
                }

                if (error > 0)
                {
                    fwrite(read_buffer, error, 1, out); // TODO check fwrite return
                    s_progress += error;
                }
            } while (error > 0);

            fclose(out);
        }

        unzCloseCurrentFile(zipfile_);

        // Go the the next entry listed in the zip file.
        if ((i+1) < global_info_.number_entry)
        {
            if (unzGoToNextFile(zipfile_) != UNZ_OK)
            {
                throw std::runtime_error("Error getting next zip file");
            }
        }
    }

    return 0;
}

int Zipfile::UncompressedSize()
{
    // Progress is not done yet in this function
    uncompressed_size_ = 0;

    if(unzGoToFirstFile(zipfile_) != UNZ_OK) {
        throw std::runtime_error("Error going to first file");
    }

    uLong i;
    for (i = 0; i < global_info_.number_entry; ++i)
    {
        unz_file_info file_info;
        char filename[MAX_FILENAME];
        if (unzGetCurrentFileInfo(
            zipfile_,
            &file_info,
            filename,
            MAX_FILENAME,
            nullptr, 0, nullptr, 0) != UNZ_OK)
        {
            throw std::runtime_error("Error reading zip file info");
        }

        uncompressed_size_ += file_info.uncompressed_size;


        // Go the the next entry listed in the zip file.
        if ((i+1) < global_info_.number_entry)
        {
            if (unzGoToNextFile(zipfile_) != UNZ_OK)
            {
                throw std::runtime_error("Error calculating zip size");
            }
        }
    }
    return 0;
}