#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <string>

#include "filesystem.hpp"

std::string Filesystem::readFile(std::string file) {
  int fd = sceIoOpen(file.c_str(), SCE_O_RDONLY, 0777);

  int fileSize = sceIoLseek ( fd, 0, SCE_SEEK_END );
  sceIoLseek (fd, 0, SCE_SEEK_SET ); // reset 'cursor' in file

  if (fd < 0)
    return "";

  std::string readString(fileSize, '\0');
  sceIoRead(fd, &readString[0], fileSize);

  sceIoClose(fd);

  return readString;
}

int Filesystem::writeFile(std::string file, std::string buf) {
  int fd = sceIoOpen(file.c_str(), SCE_O_WRONLY | SCE_O_CREAT, 0777);

  int fileSize = sceIoLseek ( fd, 0, SCE_SEEK_END );
  sceIoLseek (fd, 0, SCE_SEEK_SET ); // reset 'cursor' in file

  if (fd < 0)
    return fd;

  int written = sceIoWrite(fd, buf.c_str(), buf.length());

  sceIoClose(fd);
  return written;
}

int Filesystem::copyFile(std::string src, std::string dst) {

  // The destination is a subfolder of the source folder
  SceUID fdsrc = sceIoOpen(src.c_str(), SCE_O_RDONLY, 0);
  if (fdsrc < 0)
    return fdsrc;

  SceUID fddst = sceIoOpen(dst.c_str(), SCE_O_WRONLY | SCE_O_CREAT | SCE_O_TRUNC, 0777);
  if (fddst < 0) {
    sceIoClose(fdsrc);
    return fddst;
  }

  void *buf = memalign(4096, TRANSFER_SIZE);

  while (1) {
    int read = sceIoRead(fdsrc, buf, TRANSFER_SIZE);

    if (read < 0) {
      free(buf);

      sceIoClose(fddst);
      sceIoClose(fdsrc);

      sceIoRemove(dst.c_str());

      return read;
    }

    if (read == 0)
      break;

    int written = sceIoWrite(fddst, buf, read);

    if (written < 0) {
      free(buf);

      sceIoClose(fddst);
      sceIoClose(fdsrc);

      sceIoRemove(dst.c_str());

      return written;
    }
  }

  free(buf);

  // Inherit file stat
  SceIoStat stat;
  memset(&stat, 0, sizeof(SceIoStat));
  sceIoGetstatByFd(fdsrc, &stat);
  sceIoChstatByFd(fddst, &stat, 0x3B);

  sceIoClose(fddst);
  sceIoClose(fdsrc);

  return 1;
}

int Filesystem::copyPath(std::string src, std::string dst) {

  SceUID dfd = sceIoDopen(src.c_str());
  if (dfd >= 0) {
    SceIoStat stat;
    memset(&stat, 0, sizeof(SceIoStat));
    sceIoGetstatByFd(dfd, &stat);

    stat.st_mode |= SCE_S_IWUSR;

    int ret = sceIoMkdir(dst.c_str(), stat.st_mode & 0xFFF);
    if (ret < 0 && ret != SCE_ERROR_ERRNO_EEXIST) {
      sceIoDclose(dfd);
      return ret;
    }

    if (ret == SCE_ERROR_ERRNO_EEXIST) {
      sceIoChstat(dst.c_str(), &stat, 0x3B);
    }

    int res = 0;

    do {
      SceIoDirent dir;
      memset(&dir, 0, sizeof(SceIoDirent));

      res = sceIoDread(dfd, &dir);
      if (res > 0) {
        char *new_src_path = (char *)malloc(src.length() + strlen(dir.d_name) + 2);
        snprintf(new_src_path, MAX_PATH_LENGTH, "%s%s%s", src.c_str(), hasEndSlash(src) ? "" : "/", dir.d_name);

        char *new_dst_path = (char *)malloc(dst.length() + strlen(dir.d_name) + 2);
        snprintf(new_dst_path, MAX_PATH_LENGTH, "%s%s%s", dst.c_str(), hasEndSlash(dst) ? "" : "/", dir.d_name);

        int ret = 0;

        if (SCE_S_ISDIR(dir.d_stat.st_mode)) {
          ret = copyPath(new_src_path, new_dst_path);
        } else {
          ret = copyFile(new_src_path, new_dst_path);
        }

        free(new_dst_path);
        free(new_src_path);

        if (ret <= 0) {
          sceIoDclose(dfd);
          return ret;
        }
      }
    } while (res > 0);

    sceIoDclose(dfd);
  } else {
    return copyFile(src.c_str(), dst.c_str());
  }

  return 1;
}

int Filesystem::removePath(std::string path) {
  SceUID dfd = sceIoDopen(path.c_str());
  if (dfd >= 0) {
    int res = 0;

    do {
      SceIoDirent dir;
      memset(&dir, 0, sizeof(SceIoDirent));

      res = sceIoDread(dfd, &dir);
      if (res > 0) {
        char *new_path = (char *)malloc(path.length() + strlen(dir.d_name) + 2);
        snprintf(new_path, MAX_PATH_LENGTH, "%s%s%s", path, hasEndSlash(path) ? "" : "/", dir.d_name);

        if (SCE_S_ISDIR(dir.d_stat.st_mode)) {
          int ret = removePath(new_path);
          if (ret <= 0) {
            free(new_path);
            sceIoDclose(dfd);
            return ret;
          }
        } else {
          int ret = sceIoRemove(new_path);
          if (ret < 0) {
            free(new_path);
            sceIoDclose(dfd);
            return ret;
          }
        }

        free(new_path);
      }
    } while (res > 0);

    sceIoDclose(dfd);

    int ret = sceIoRmdir(path.c_str());
    if (ret < 0)
      return ret;
  } else {
    int ret = sceIoRemove(path.c_str());
    if (ret < 0)
      return ret;
  }

  return 1;
}

bool Filesystem::hasEndSlash(std::string str) {
    std::string slash = "/";

    if (slash.size() > slash.size()) return false;
    return std::equal(slash.rbegin(), slash.rend(), str.rbegin());
}