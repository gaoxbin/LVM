/*
 * LogicalVolume.cpp
 *
 *  Created on: 2018年1月17日
 *      Author: Administrator
 */

#include "LogicalVolume.h"
#include "VolumeGroup.h"

LogicalVolume::LogicalVolume(string &id, int nsegs, string &vname, void *vol)
{
    uuid = id;
    segment_count = nsegs;
    this_group = vol;
    volname = vname;
}

LogicalVolume::~LogicalVolume()
{

}

lvm_uint64_t LogicalVolume::lvm_mapper(lvm_uint64_t sectno)
{
    lv_segment *seg;
    lvm_uint64_t sect_mapped = 0;
    uint32_t extent_no, extent_offset;
    list<lv_segment *>::iterator iterate;
    VolumeGroup * pVG = (VolumeGroup *)this_group;

    extent_no = sectno / pVG->extent_size;
    extent_offset = sectno % pVG->extent_size;

    for(iterate = segments.begin(); iterate != segments.end(); iterate++)
    {
        seg = (*iterate);
        if((extent_no >= seg->start_extent) && (extent_no < (seg->start_extent + seg->extent_count)))
        {
            sect_mapped = (lvm_uint64_t)(extent_no *  pVG->extent_size) + extent_offset;
            sect_mapped += seg->pvolumes->pe_start + seg->pvolumes->offset + seg->stripe->stripe_start_extent;
            break;
        }
    }

    if(sect_mapped == 0)
    {
//        LOG("Error in LVM Mapping \n");
    }
    return sect_mapped;
}

