/*
 * VolumeGroup.cpp
 *
 *  Created on: 2018年1月17日
 *      Author: Administrator
 */

#include "VolumeGroup.h"
#include "LogicalVolume.h"
#include "PhysicalVolume.h"

VolumeGroup::VolumeGroup(string &id, string &name, int seq, int size)
{
    uuid = id;
    volname = name;
    seqno = seq;
    extent_size = size;

    ext2read = NULL;
    max_lv = 0;
    max_pv = 0;
}

VolumeGroup::VolumeGroup()
{
    seqno = 0;
    extent_size = 0;

    ext2read = NULL;
    max_lv = 0;
    max_pv = 0;
}

VolumeGroup::~VolumeGroup()
{
    list<PhysicalVolume *>::iterator i;
    list<LogicalVolume *>::iterator j;
    for(i = pvolumes.begin(); i != pvolumes.end(); i++)
    {
        delete (*i);
    }

    for(j = lvolumes.begin(); j != lvolumes.end(); j++)
    {
        delete (*j);
    }
}

PhysicalVolume *VolumeGroup::find_physical_volume(string &id)
{
    PhysicalVolume *pvol;
    list<PhysicalVolume *>::iterator i;

    for(i = pvolumes.begin(); i != pvolumes.end(); i++)
    {
        pvol = (*i);
        if(pvol->uuid.compare(id) == 0)
        {
            return pvol;
        }
    }

    return NULL;
}

PhysicalVolume *VolumeGroup::add_physical_volume(string &id, lvm_uint64_t devsize, uint32_t start, uint32_t count, FileHandle file, lvm_uint64_t dsk_offset)
{
    PhysicalVolume *pvol;

    pvol = new PhysicalVolume(id, devsize, start, count, file, dsk_offset);
    if(!pvol)
        return NULL;

    pvolumes.push_back(pvol);
    return pvol;
}

LogicalVolume *VolumeGroup::find_logical_volume(string &id)
{
    LogicalVolume *lvol;
    list<LogicalVolume *>::iterator i;

    for(i = lvolumes.begin(); i != lvolumes.end(); i++)
    {
        lvol = (*i);
        if(lvol->uuid.compare(id) == 0)
        {
            return lvol;
        }
    }

    return NULL;
}

LogicalVolume *VolumeGroup::add_logical_volume(string &id, int count, string &vname)
{
    LogicalVolume *lvol;

    lvol = new LogicalVolume(id, count, vname, this);
    if(!lvol)
        return NULL;

    lvolumes.push_back(lvol);
    return lvol;
}

void VolumeGroup::logical_mount()
{
    //EXT2_SUPER_BLOCK sblock;
    LogicalVolume *lvol;

//    //PhysicalVolume *pvol;
//    Ext2Partition *partition;

    lv_segment *seg;
    lv_segment *root = NULL;
    list<LogicalVolume *>::iterator i;
    list<lv_segment *>::iterator j;
    list<PhysicalVolume *>::iterator k;
    lvm_uint64_t start;
    int index = 0;

//    LOG("Mouning Logical VOLUMES\n");

    for(i = lvolumes.begin(); i != lvolumes.end(); i++)
    {
        lvol = (*i);
        for(j = lvol->segments.begin(); j != lvol->segments.end(); j++)
        {
            seg = (*j);
            if(seg->start_extent == 0)
                root = seg;

            index = 0;
            for(k = pvolumes.begin(); k != pvolumes.end(); k++)
            {
                if(seg->stripe->stripe_pv == index)
                {
                    seg->pvolumes = (*k);
                    break;
                }
                index++;
            }
        }

        if(!root)
            continue;

        int off = ((root->start_extent + root->stripe->stripe_start_extent) * ((VolumeGroup *)(lvol->this_group))->extent_size);
#if 0
        LOG("PE start %d offset %d extoff %d B=%d A=%d %s\n", root->pvolumes->pe_start,
            root->pvolumes->offset, lvol->this_group->extent_size, root->start_extent, root->stripe->stripe_start_extent, lvol->volname.toUtf8().data());
        start = root->pvolumes->pe_start + root->pvolumes->offset + off;
        partition = new Ext2Partition(root->pvolumes->dev_size, start, SECTOR_SIZE, root->pvolumes->handle, lvol);
        if(partition->is_valid)
        {
            QByteArray ba;
            ba = lvol->volname.toUtf8();
            partition->set_image_name(ba.data());
            LOG("adding %s\n", partition->get_linux_name().c_str());
            ext2read->add_partition(partition);
        }
        else
        {
            LOG("Invalid Partition %s\n", partition->get_linux_name().c_str());
            delete partition;
        }
#endif
    }

}
