/*
	VitaShell
	Copyright (C) 2015-2016, TheFloW

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "main.h"
#include "archive.h"
#include "file.h"
#include "utils.h"
#include "sha1.h"

int allocateReadFile(char *file, void **buffer) {
	SceUID fd = sceIoOpen(file, SCE_O_RDONLY, 0);
	if (fd < 0)
		return fd;

	int size = sceIoLseek32(fd, 0, SCE_SEEK_END);
	sceIoLseek32(fd, 0, SCE_SEEK_SET);

	*buffer = malloc(size);
	if (!*buffer) {
		sceIoClose(fd);
		return -1;
	}

	int read = sceIoRead(fd, *buffer, size);
	sceIoClose(fd);

	return read;
}

int ReadFile(char *file, void *buf, int size) {
	SceUID fd = sceIoOpen(file, SCE_O_RDONLY, 0);
	if (fd < 0)
		return fd;

	int read = sceIoRead(fd, buf, size);

	sceIoClose(fd);
	return read;
}

int WriteFile(char *file, void *buf, int size) {
	SceUID fd = sceIoOpen(file, SCE_O_WRONLY | SCE_O_CREAT | SCE_O_TRUNC, 0777);
	if (fd < 0)
		return fd;

	int written = sceIoWrite(fd, buf, size);

	sceIoClose(fd);
	return written;
}

int getFileSize(char *pInputFileName)
{
	SceUID fd = sceIoOpen(pInputFileName, SCE_O_RDONLY, 0);
	if (fd < 0)
		return fd;

	int fileSize = sceIoLseek(fd, 0, SCE_SEEK_END);
	
	sceIoClose(fd);
	return fileSize;
}

int removePath(char *path, FileProcessParam *param) {
	SceUID dfd = sceIoDopen(path);
	if (dfd >= 0) {
		int res = 0;

		do {
			SceIoDirent dir;
			memset(&dir, 0, sizeof(SceIoDirent));

			res = sceIoDread(dfd, &dir);
			if (res > 0) {
				if (strcmp(dir.d_name, ".") == 0 || strcmp(dir.d_name, "..") == 0)
					continue;

				char *new_path = malloc(strlen(path) + strlen(dir.d_name) + 2);
				snprintf(new_path, MAX_PATH_LENGTH, "%s%s%s", path, hasEndSlash(path) ? "" : "/", dir.d_name);

				if (SCE_S_ISDIR(dir.d_stat.st_mode)) {
					int ret = removePath(new_path, param);
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

					if (param) {
						if (param->value)
							(*param->value)++;

						if (param->SetProgress)
							param->SetProgress(param->value ? *param->value : 0, param->max);

						if (param->cancelHandler && param->cancelHandler()) {
							free(new_path);
							sceIoDclose(dfd);
							return 0;
						}
					}
				}

				free(new_path);
			}
		} while (res > 0);

		sceIoDclose(dfd);

		int ret = sceIoRmdir(path);
		if (ret < 0)
			return ret;

		if (param) {
			if (param->value)
				(*param->value)++;

			if (param->SetProgress)
				param->SetProgress(param->value ? *param->value : 0, param->max);

			if (param->cancelHandler && param->cancelHandler()) {
				return 0;
			}
		}
	} else {
		int ret = sceIoRemove(path);
		if (ret < 0)
			return ret;

		if (param) {
			if (param->value)
				(*param->value)++;

			if (param->SetProgress)
				param->SetProgress(param->value ? *param->value : 0, param->max);

			if (param->cancelHandler && param->cancelHandler()) {
				return 0;
			}
		}
	}

	return 1;
}

int copyFile(char *src_path, char *dst_path, FileProcessParam *param) {
	// The source and destination paths are identical
	if (strcasecmp(src_path, dst_path) == 0) {
		return -1;
	}

	// The destination is a subfolder of the source folder
	int len = strlen(src_path);
	if (strncasecmp(src_path, dst_path, len) == 0 && (dst_path[len] == '/' || dst_path[len - 1] == '/')) {
		return -2;
	}

	SceUID fdsrc = sceIoOpen(src_path, SCE_O_RDONLY, 0);
	if (fdsrc < 0)
		return fdsrc;

	SceUID fddst = sceIoOpen(dst_path, SCE_O_WRONLY | SCE_O_CREAT | SCE_O_TRUNC, 0777);
	if (fddst < 0) {
		sceIoClose(fdsrc);
		return fddst;
	}

	void *buf = malloc(TRANSFER_SIZE);

	uint64_t seek = 0;

	while (1) {
		int read = sceIoRead(fdsrc, buf, TRANSFER_SIZE);
		if (read == SCE_ERROR_ERRNO_ENODEV) {
			fdsrc = sceIoOpen(src_path, SCE_O_RDONLY, 0);
			if (fdsrc >= 0) {
				sceIoLseek(fdsrc, seek, SCE_SEEK_SET);
				read = sceIoRead(fdsrc, buf, TRANSFER_SIZE);
			}
		}

		if (read < 0) {
			free(buf);

			sceIoClose(fddst);
			sceIoClose(fdsrc);

			return read;
		}

		if (read == 0)
			break;

		int written = sceIoWrite(fddst, buf, read);
		if (written == SCE_ERROR_ERRNO_ENODEV) {
			fddst = sceIoOpen(dst_path, SCE_O_WRONLY | SCE_O_CREAT, 0777);
			if (fddst >= 0) {
				sceIoLseek(fddst, seek, SCE_SEEK_SET);
				written = sceIoWrite(fddst, buf, read);
			}
		}

		if (written != read) {
			free(buf);

			sceIoClose(fddst);
			sceIoClose(fdsrc);

			return (written < 0) ? written : -1;
		}

		seek += written;

		if (param) {
			if (param->value)
				(*param->value) += read;

			if (param->SetProgress)
				param->SetProgress(param->value ? *param->value : 0, param->max);

			if (param->cancelHandler && param->cancelHandler()) {
				free(buf);

				sceIoClose(fddst);
				sceIoClose(fdsrc);

				return 0;
			}
		}
	}

	free(buf);

	sceIoClose(fddst);
	sceIoClose(fdsrc);

	return 1;
}

int copyPath(char *src_path, char *dst_path, FileProcessParam *param) {
	// The source and destination paths are identical
	if (strcasecmp(src_path, dst_path) == 0) {
		return -1;
	}

	// The destination is a subfolder of the source folder
	int len = strlen(src_path);
	if (strncasecmp(src_path, dst_path, len) == 0 && (dst_path[len] == '/' || dst_path[len - 1] == '/')) {
		return -2;
	}

	SceUID dfd = sceIoDopen(src_path);
	if (dfd >= 0) {
		int ret = sceIoMkdir(dst_path, 0777);
		if (ret < 0 && ret != SCE_ERROR_ERRNO_EEXIST) {
			sceIoDclose(dfd);
			return ret;
		}

		if (param) {
			if (param->value)
				(*param->value)++;

			if (param->SetProgress)
				param->SetProgress(param->value ? *param->value : 0, param->max);

			if (param->cancelHandler && param->cancelHandler()) {
				sceIoDclose(dfd);
				return 0;
			}
		}

		int res = 0;

		do {
			SceIoDirent dir;
			memset(&dir, 0, sizeof(SceIoDirent));

			res = sceIoDread(dfd, &dir);
			if (res > 0) {
				if (strcmp(dir.d_name, ".") == 0 || strcmp(dir.d_name, "..") == 0)
					continue;

				char *new_src_path = malloc(strlen(src_path) + strlen(dir.d_name) + 2);
				snprintf(new_src_path, MAX_PATH_LENGTH, "%s%s%s", src_path, hasEndSlash(src_path) ? "" : "/", dir.d_name);

				char *new_dst_path = malloc(strlen(dst_path) + strlen(dir.d_name) + 2);
				snprintf(new_dst_path, MAX_PATH_LENGTH, "%s%s%s", dst_path, hasEndSlash(dst_path) ? "" : "/", dir.d_name);

				int ret = 0;

				if (SCE_S_ISDIR(dir.d_stat.st_mode)) {
					ret = copyPath(new_src_path, new_dst_path, param);
				} else {
					ret = copyFile(new_src_path, new_dst_path, param);
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
		return copyFile(src_path, dst_path, param);
	}

	return 1;
}

int movePath(char *src_path, char *dst_path, int flags, FileProcessParam *param) {
	// The source and destination paths are identical
	if (strcasecmp(src_path, dst_path) == 0) {
		return -1;
	}

	// The destination is a subfolder of the source folder
	int len = strlen(src_path);
	if (strncasecmp(src_path, dst_path, len) == 0 && (dst_path[len] == '/' || dst_path[len - 1] == '/')) {
		return -2;
	}

	int res = sceIoRename(src_path, dst_path);
	if (res == SCE_ERROR_ERRNO_EEXIST && flags & (MOVE_INTEGRATE | MOVE_REPLACE)) {
		// Src stat
		SceIoStat src_stat;
		memset(&src_stat, 0, sizeof(SceIoStat));
		res = sceIoGetstat(src_path, &src_stat);
		if (res < 0)
			return res;

		// Dst stat
		SceIoStat dst_stat;
		memset(&dst_stat, 0, sizeof(SceIoStat));
		res = sceIoGetstat(dst_path, &dst_stat);
		if (res < 0)
			return res;

		// Is dir
		int src_is_dir = SCE_S_ISDIR(src_stat.st_mode);
		int dst_is_dir = SCE_S_ISDIR(dst_stat.st_mode);

		// One of them is a file and the other a directory, no replacement or integration possible
		if (src_is_dir != dst_is_dir)
			return -3;

		// Replace file
		if (!src_is_dir && !dst_is_dir && flags & MOVE_REPLACE) {
			sceIoRemove(dst_path);

			res = sceIoRename(src_path, dst_path);
			if (res < 0)
				return res;

			return 1;
		}

		// Integrate directory
		if (src_is_dir && dst_is_dir && flags & MOVE_INTEGRATE) {
			SceUID dfd = sceIoDopen(src_path);
			if (dfd < 0)
				return dfd;

			int res = 0;

			do {
				SceIoDirent dir;
				memset(&dir, 0, sizeof(SceIoDirent));

				res = sceIoDread(dfd, &dir);
				if (res > 0) {
					if (strcmp(dir.d_name, ".") == 0 || strcmp(dir.d_name, "..") == 0)
						continue;

					char *new_src_path = malloc(strlen(src_path) + strlen(dir.d_name) + 2);
					snprintf(new_src_path, MAX_PATH_LENGTH, "%s%s%s", src_path, hasEndSlash(src_path) ? "" : "/", dir.d_name);

					char *new_dst_path = malloc(strlen(dst_path) + strlen(dir.d_name) + 2);
					snprintf(new_dst_path, MAX_PATH_LENGTH, "%s%s%s", dst_path, hasEndSlash(dst_path) ? "" : "/", dir.d_name);

					// Recursive move
					int ret = movePath(new_src_path, new_dst_path, flags, param);

					free(new_dst_path);
					free(new_src_path);

					if (ret <= 0) {
						sceIoDclose(dfd);
						return ret;
					}
				}
			} while (res > 0);

			sceIoDclose(dfd);

			// Integrated, now remove this directory
			sceIoRmdir(src_path);
		}
	}

	return 1;
}

FileListEntry *fileListFindEntry(FileList *list, char *name) {
	FileListEntry *entry = list->head;

	int name_length = strlen(name);

	while (entry) {
		if (entry->name_length == name_length && strcasecmp(entry->name, name) == 0)
			return entry;

		entry = entry->next;
	}

	return NULL;
}

FileListEntry *fileListGetNthEntry(FileList *list, int n) {
	FileListEntry *entry = list->head;

	while (n > 0 && entry) {
		n--;
		entry = entry->next;
	}

	if (n != 0)
		return NULL;

	return entry;
}

int fileListGetNumberByName(FileList *list, char *name) {
	FileListEntry *entry = list->head;

	int name_length = strlen(name);

	int n = 0;

	while (entry) {
		if (entry->name_length == name_length && strcasecmp(entry->name, name) == 0)
			break;

		n++;
		entry = entry->next;
	}

	return n;
}

void fileListAddEntry(FileList *list, FileListEntry *entry, int sort) {
	entry->next = NULL;
	entry->previous = NULL;

	if (list->head == NULL) {
		list->head = entry;
		list->tail = entry;
	} else {
		if (sort != SORT_NONE) {
			FileListEntry *p = list->head;
			FileListEntry *previous = NULL;

			while (p) {
				// Sort by type
				if (entry->is_folder > p->is_folder)
					break;

				// '..' is always at first
				if (strcmp(entry->name, "..") == 0)
					break;

				if (strcmp(p->name, "..") != 0) {
					// Get the minimum length without /
					int len = MIN(entry->name_length, p->name_length);
					if (entry->name[len - 1] == '/' || p->name[len - 1] == '/')
						len--;

					// Sort by name
					if (entry->is_folder == p->is_folder) {
						int diff = strncasecmp(entry->name, p->name, len);
						if (diff < 0 || (diff == 0 && entry->name_length < p->name_length)) {
							break;
						}
					}
				}

				previous = p;
				p = p->next;
			}

			if (previous == NULL) { // Order: entry (new head) -> p (old head)
				entry->next = p;
				p->previous = entry;
				list->head = entry;
			} else if (previous->next == NULL) { // Order: p (old tail) -> entry (new tail)
				FileListEntry *tail = list->tail;
				tail->next = entry;
				entry->previous = tail;
				list->tail = entry;
			} else { // Order: previous -> entry -> p
				previous->next = entry;
				entry->previous = previous; 
				entry->next = p;
				p->previous = entry;
			}
		} else {
			FileListEntry *tail = list->tail;
			tail->next = entry;
			entry->previous = tail;
			list->tail = entry;
		}
	}

	list->length++;
}

int fileListRemoveEntry(FileList *list, FileListEntry *entry) {
	if (entry) {
		if (entry->previous) {
			entry->previous->next = entry->next;
		} else {
			list->head = entry->next;
		}

		if (entry->next) {
			entry->next->previous = entry->previous;
		} else {
			list->tail = entry->previous;
		}

		list->length--;
		free(entry);

		if (list->length == 0) {
			list->head = NULL;
			list->tail = NULL;
		}

		return 1;
	}

	return 0;
}

int fileListRemoveEntryByName(FileList *list, char *name) {
	FileListEntry *entry = list->head;
	FileListEntry *previous = NULL;

	int name_length = strlen(name);

	while (entry) {
		if (entry->name_length == name_length && strcasecmp(entry->name, name) == 0) {
			if (previous) {
				previous->next = entry->next;
			} else {
				list->head = entry->next;
			}

			if (list->tail == entry) {
				list->tail = previous;
			}

			list->length--;
			free(entry);

			if (list->length == 0) {
				list->head = NULL;
				list->tail = NULL;
			}

			return 1;
		}

		previous = entry;
		entry = entry->next;
	}

	return 0;
}

void fileListEmpty(FileList *list) {
	FileListEntry *entry = list->head;

	while (entry) {
		FileListEntry *next = entry->next;
		free(entry);
		entry = next;
	}

	list->head = NULL;
	list->tail = NULL;
	list->length = 0;
	list->files = 0;
	list->folders = 0;
}
