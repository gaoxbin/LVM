/*
 * VolumeGroup.h
 *
 *  Created on: 2018年1月17日
 *      Author: Administrator
 */

#ifndef VG_VOLUMEGROUP_H_
#define VG_VOLUMEGROUP_H_

#include "lvm_dep.h"
#include "lvm_type.h"
#include "PhysicalVolume.h"
#include "LogicalVolume.h"

class VolumeGroup {
public:
    string volname;
    string uuid;
    string format;
    int extent_size;
    int seqno;
    int max_lv, max_pv;
    std::list <PhysicalVolume *> pvolumes;
    std::list <LogicalVolume *> lvolumes;
    Ext2Read *ext2read;

public:
    VolumeGroup(string &id, string &name, int seq, int size);
    VolumeGroup();
    ~VolumeGroup();
    PhysicalVolume *find_physical_volume(string &id);
    PhysicalVolume *add_physical_volume(string &id, lvm_uint64_t devsize, uint32_t start, uint32_t count, FileHandle file, lvm_uint64_t dsk_offset);
    LogicalVolume *find_logical_volume(string &id);
    LogicalVolume *add_logical_volume(string &id, int count, string &vname);
    void logical_mount();
    void set_ext2read(Ext2Read *ext2) { ext2read = ext2; }
};

#endif /* VG_VOLUMEGROUP_H_ */
