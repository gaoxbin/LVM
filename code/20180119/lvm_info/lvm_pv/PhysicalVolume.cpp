/*
 * PhysicalVolume.cpp
 *
 *  Created on: 2018年1月17日
 *      Author: Administrator
 */

#include "PhysicalVolume.h"

PhysicalVolume::PhysicalVolume(string &id, lvm_uint64_t devsize, uint32_t start, uint32_t count, FileHandle file, lvm_uint64_t dsk_offset)
{
    uuid = id;
    dev_size = devsize;
    pe_start = start;
    pe_count = count;
    handle = file;
    offset = dsk_offset;
}


PhysicalVolume::~PhysicalVolume() {
	// TODO Auto-generated destructor stub
}


