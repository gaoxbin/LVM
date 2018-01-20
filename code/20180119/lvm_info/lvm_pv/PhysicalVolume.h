/*
 * PhysicalVolume.h
 *
 *  Created on: 2018年1月17日
 *      Author: Administrator
 */

#ifndef PV_PHYSICALVOLUME_H_
#define PV_PHYSICALVOLUME_H_

#include "lvm_type.h"
#include "lvm_dep.h"

class PhysicalVolume {
public:
    lvm_uint64_t dev_size;
    FileHandle handle;
    uint32_t pe_start, pe_count;
    lvm_uint64_t offset;     // offset from the start of disk to lvm volume
    string uuid;
    PhysicalVolume(string &id, lvm_uint64_t devsize, uint32_t start, uint32_t count, FileHandle file, lvm_uint64_t dsk_offset);
    ~PhysicalVolume();
};

#endif /* PV_PHYSICALVOLUME_H_ */
