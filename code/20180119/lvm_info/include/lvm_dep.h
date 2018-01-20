/*
 * osdep.h
 *
 *  Created on: 2018年1月17日
 *      Author: Administrator
 */

#ifndef INCLUDE_LVM_DEP_H_
#define INCLUDE_LVM_DEP_H_

#include "lvm_type.h"

#include <string>
#include <iostream>
#include <list>

using namespace std;

/* Structure to hold Physical Volumes (PV) label*/
typedef struct pv_label_header {
        char        pv_name[LVM_SIGLEN];   // Physical volume signature
        uint64_t    pv_sector_xl;          // sector number of this label
        uint32_t    pv_crc;                // CRC value
        uint32_t    pv_offset_xl;          // Offset from start of struct to contents
        char        pv_vermagic[LVM_MAGIC_LEN]; // Physical Volume version "LVM2 001"
        char        pv_uuid[UUID_LEN];
        uint64_t    pv_unknown1[5];             // documentation lacks for lvm
        uint64_t    pv_labeloffset;             // location of the label
} __attribute__ ((__packed__)) PV_LABEL_HEADER;

typedef struct pv_label {
    uint32_t        pv_magic;
    char            pv_sig[4];          // signature
    uint64_t        unknown1[2];
    uint64_t        pv_offset_low;
    uint64_t        unknown2;
    uint64_t        pv_offset_high;
    uint64_t        pv_length;
} __attribute__ ((__packed__)) PV_LABEL;

// Multiple stripes NOT Implemented: we only support linear for now.
struct stripe {
    int stripe_pv;
    uint32_t stripe_start_extent;
};


#endif /* INCLUDE_LVM_DEP_H_ */

