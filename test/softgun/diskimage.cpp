/*
 **************************************************************************************************
 * diskimage.c
 *	Library for diskimages for use in nonvolatile memory device
 *	emulators like amdflash or serial eeproms and MMC/SD-Cards
 *
 * (C) 2005 Jochen Karrer
 *    Author: Jochen Karrer
 *
 * Status: Working 
 *
 * Copyright 2010 Jochen Karrer. All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 * 
 *   1. Redistributions of source code must retain the above copyright notice, this list of
 *       conditions and the following disclaimer.
 * 
 *   2. Redistributions in binary form must reproduce the above copyright notice, this list
 *       of conditions and the following disclaimer in the documentation and/or other materials
 *       provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY Jochen Karrer ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * The views and conclusions contained in the software and documentation are those of the
 * authors and should not be interpreted as representing official policies, either expressed
 * or implied, of Jochen Karrer.
 *
 *************************************************************************************************
 */

#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
//#include <sys/file.h>
//#include <sys/mman.h>
#include <sys/types.h>
//#include <sys/ioctl.h>
#include <io.h>
#include "diskimage.h"
#include "sgstring.h"
#define S_IFBLK (~S_IFMT)
/*
 * ------------------------------------------------------------------------
 * Fill a diskimage with emptyval. This is intended for 
 * new non-sparse diskimages
 * ------------------------------------------------------------------------
 */
static int
diskimage_fill(DiskImage * di, uint8_t emptyval)
{
	int i;
	uint8_t buf[1024];
	int64_t count;		/* must be signed because file might be oversized */
	off_t pos;
	if ((pos = _lseek(di->fd, 0, SEEK_END)) == (off_t) - 1) {
		perror("lseek64 on flashfile failed");
		return -1;
	}
	count = di->size - pos;
	for (i = 0; i < sizeof(buf); i++) {
		buf[i] = emptyval;
	}
	while (count >= (int64_t) sizeof(buf)) {
		int result;
		result = _write(di->fd, buf, sizeof(buf));
		if (result <= 0) {
			perror("write failed");
			break;
		} else {
			count -= result;
		}
	}
	while (count > 0) {
		if (_write(di->fd, buf, 1) <= 0) {
			perror("write failed");
			break;
		}
		count--;
	}
	return 0;
}

static int
diskimage_sparse_fill(DiskImage * di, uint8_t emptyval)
{
	off_t pos;
	if (di->size < 1) {
		fprintf(stderr, "Sparse diskimage is < 1 Byte\n");
		return -1;
	}
	if ((pos = _lseek(di->fd, 0, SEEK_END)) == -1) {
		perror("lseek64 on flashfile failed");
		return -1;
	}
	if (pos >= di->size) {
		return 0;
	}
	if ((pos = _lseek(di->fd, di->size - 1, SEEK_SET)) != (di->size - 1)) {
		perror("lseek64 on sparse file failed");
		return -1;
	}
	if (_write(di->fd, &emptyval, 1) != 1) {
		perror("writing last byte of sparse diskimage failed");
		return -1;
	}
	return 0;
}

static int
check_fill(DiskImage * di, int flags)
{
	uint8_t emptyval;
	if (flags & DI_CREAT_FF) {
		emptyval = 0xff;
	} else {
		emptyval = 0x00;
	}
	if (flags & DI_SPARSE) {
		if (diskimage_sparse_fill(di, emptyval) < 0) {
			fprintf(stderr, "Filling the sparse diskimage failed\n");
			return -1;
		}
	} else {
		if (diskimage_fill(di, emptyval) < 0) {
			fprintf(stderr, "Filling the diskimage failed\n");
			return -1;
		}
	}
	return 0;
}

/**
 ************************************************************************
 * Return the size of a block device in bytes
 ************************************************************************
 */
static uint64_t
blockdev_get_size(DiskImage * di)
{
#if 0
	uint64_t size;
	ioctl(di->fd, BLKGETSIZE64, &size);
	return size;
#endif
	return 0;
}

DiskImage *
DiskImage_Open(const char *name, uint64_t size, int flags)
{
	DiskImage *di;
	struct stat stat;
	di = sg_new(DiskImage);
	di->size = size;
	di->flags = flags;
	if (flags & DI_RDWR) {
		if (flags & (DI_CREAT_FF | DI_CREAT_00)) {
			di->fd = _open(name, O_RDWR | O_CREAT | O_LARGEFILE, 0644);
		} else {
			di->fd = _open(name, O_RDWR | O_LARGEFILE, 0644);
		}
	} else {
		di->fd = _open(name, O_RDONLY);
	}
	if (di->fd < 0) {
		fprintf(stderr, "Can't open image \"%s\" ", name);
		perror("");
		free(di);
		return NULL;
	}
	/*if (flock(di->fd, LOCK_EX | LOCK_NB) < 0) {
		fprintf(stderr, "Can't get lock for diskimage \"%s\"\n", name);
		_close(di->fd);
		free(di);
		return NULL;
	}*/
	if (fstat(di->fd, &stat) < 0) {
		fprintf(stderr, "Stat on diskimage failed\n");
		exit(1);
	}
	if ((stat.st_mode & S_IFMT) == S_IFBLK) {
		fprintf(stderr, "Diskimage \"%s\" is a block device\n", name);
		if (size) {
			fprintf(stderr, "Can not set the size of a block device\n");
			exit(1);
		}
		di->size = blockdev_get_size(di);
		fprintf(stderr, "Block device access not implemented\n");
		exit(1);
	} else if ((stat.st_mode & S_IFMT) == S_IFREG) {
		fprintf(stderr, "Diskimage \"%s\" is a regular file\n", name);
		if (check_fill(di, flags) < 0) {
			_close(di->fd);
			free(di);
			return NULL;
		}
	} else {
		fprintf(stderr, "Diskimage \"%s\" is of unknown type\n", name);
	}
	return di;
}

int
DiskImage_Read(DiskImage * di, off_t ofs, uint8_t * buf, int count)
{
	int result;
	int cnt;
	if (_lseek(di->fd, ofs, SEEK_SET) != ofs) {
		return -EINVAL;
	}
	for (cnt = 0; cnt < count;) {
		result = _read(di->fd, buf + cnt, count);
		if (result <= 0) {
			if (cnt) {
				return cnt;
			} else {
				return result;
			}
		}
		cnt += result;
	}
	return cnt;

}

int
DiskImage_Write(DiskImage * di, off_t ofs, const uint8_t * buf, int count)
{
	int result;
	int cnt;
	if (_lseek(di->fd, ofs, SEEK_SET) != ofs) {
		return -EINVAL;
	}
	for (cnt = 0; cnt < count;) {
		result = _write(di->fd, buf + cnt, count);
		if (result <= 0) {
			if (cnt) {
				return cnt;
			} else {
				return result;
			}
		}
		cnt += result;
	}
	return cnt;
}

void *
DiskImage_Mmap(DiskImage * di)
{
	/*if (di->flags & DI_RDWR) {
		di->map = mmap(0, di->size, PROT_READ | PROT_WRITE, MAP_SHARED, di->fd, 0);
	} else {
		di->map = mmap(0, di->size, PROT_READ, MAP_SHARED, di->fd, 0);
	}*/
	di->map = (void *)-1;
	if (di->map == (void *)-1) {
		perror("mmap of diskimage failed");
		_close(di->fd);
		free(di);
		return NULL;
	}
	return di->map;
}

void
DiskImage_Close(DiskImage * di)
{
	/*if (di->map) {
		munmap(di->map, di->size);
		di->map = NULL;
	}*/
	//flock(di->fd, LOCK_UN);
	_close(di->fd);
	free(di);
}
