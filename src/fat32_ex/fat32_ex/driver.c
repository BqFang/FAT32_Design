#include <stdio.h>
#include <assert.h>
#include "xdisk.h"
#include "xfat.h"

static xfat_err_t xdisk_hw_open(struct _xdisk_t* disk, void* init_data)
{
	assert(disk);
	assert(init_data);

	const char* path = (const char*)init_data;
	FILE* file = fopen(path, "rb+");

	if (NULL == file)
	{
		printf("open disk failed: %s\n", path);
		return FS_ERR_IO;
	}

	disk->data = file;
	disk->sector_size = 512;
	
	fseek(file, 0, SEEK_END);
	disk->total_sector = ftell(file) / disk->sector_size;

	return FS_ERR_OK;
}

static xfat_err_t xdisk_hw_close(struct _xdisk_t* disk)
{
	assert(disk);
	FILE* file = (FILE*)disk->data;
	fclose(file);
	return FS_ERR_OK;
}
static xfat_err_t xdisk_hw_read_sector(struct _xdisk_t* disk, u8_t* buffer, u32_t start_sector, u32_t count)
{
	assert(disk);
	assert(buffer);

	u32_t offset = start_sector * disk->sector_size;
	FILE* file = (FILE*)disk->data;

	int err = fseek(file, offset, SEEK_SET);
	if (err == -1)
	{
		printf("seek disk failed: 0x%x\n", offset);
		return FS_ERR_IO;
	}

	err = fread(buffer, disk->sector_size, count, file);
	if (err == -1)
	{
		printf("read disk failed: sector : %d, count:%d\n", start_sector, count);
		return FS_ERR_IO;
	}

	return FS_ERR_OK;
}
static xfat_err_t xdisk_hw_write_sector(struct _xdisk_t* disk, u8_t* buffer, u32_t start_sector, u32_t count)
{
	assert(disk);
	assert(buffer);

	u32_t offset = start_sector * disk->sector_size;
	FILE* file = (FILE*)disk->data;

	int err = fseek(file, offset, SEEK_SET);
	if (err == -1)
	{
		printf("seek disk failed: 0x%x\n", offset);
		return FS_ERR_IO;
	}

	err = fwrite(buffer, disk->sector_size, count, file);
	if (err == -1)
	{
		printf("write disk failed: sector : %d, count:%d\n", start_sector, count);
		return FS_ERR_IO;
	}

	fflush(file);
	return FS_ERR_OK;
}


xdisk_driver_t vdisk_driver = {
	.open = xdisk_hw_open,
	.close = xdisk_hw_close,
	.read_sector = xdisk_hw_read_sector,
	.write_sector = xdisk_hw_write_sector
};