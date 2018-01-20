/*
 * LogicalVolume.h
 *
 *  Created on: 2018年1月17日
 *      Author: Administrator
 */

#ifndef LV_LOGICALVOLUME_H_
#define LV_LOGICALVOLUME_H_

#include "lvm_dep.h"
#include "lvm_type.h"

#include "lvsegment.h"

class LogicalVolume {
    int segment_count;

public:
    void *this_group;
    string uuid;
    string volname;
    std::list <lv_segment *> segments;
    //std::list <PhysicalVolume *> pvolumes;

    LogicalVolume(string &id, int nsegs, string &vname, void *);
    ~LogicalVolume();
    lvm_uint64_t lvm_mapper(lvm_uint64_t block);
};
#endif /* LV_LOGICALVOLUME_H_ */
