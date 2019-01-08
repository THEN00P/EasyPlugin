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
#include "file.h"
#include "utils.h"

static int lock_power = 0;

int power_tick_thread(SceSize args, void *argp) {
	while (1) {
		if (lock_power > 0) {
			sceKernelPowerTick(SCE_KERNEL_POWER_TICK_DISABLE_AUTO_SUSPEND);
		}

		sceKernelDelayThread(10 * 1000 * 1000);
	}

	return 0;
}

void initPowerTickThread() {
	SceUID thid = sceKernelCreateThread("power_tick_thread", power_tick_thread, 0x10000100, 0x40000, 0, 0, NULL);
	if (thid >= 0)
		sceKernelStartThread(thid, 0, NULL);
}

void powerLock() {
	lock_power++;
}

void powerUnlock() {
	lock_power--;
	if (lock_power < 0) lock_power = 0;
}

int hasEndSlash(char *path) {
	return path[strlen(path) - 1] == '/';
}

int removeEndSlash(char *path) {
	int len = strlen(path);

	if (path[len - 1] == '/') {
		path[len - 1] = '\0';
		return 1;
	}

	return 0;
}

int addEndSlash(char *path) {
	int len = strlen(path);
	if (len < MAX_PATH_LENGTH - 2) {
		if (path[len - 1] != '/') {
			strcat(path, "/");
			return 1;
		}
	}

	return 0;
}
