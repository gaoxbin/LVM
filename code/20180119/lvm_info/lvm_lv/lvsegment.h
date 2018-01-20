/*
 * lvsegment.h
 *
 *  Created on: 2018年1月17日
 *      Author: Administrator
 */

#ifndef LV_LVSEGMENT_H_
#define LV_LVSEGMENT_H_

#include "PhysicalVolume.h"

class lv_segment {
public:
    uint32_t start_extent;
    uint32_t extent_count;
    struct stripe *stripe;
    PhysicalVolume *pvolumes;

    lv_segment(uint32_t start, uint32_t count)
    {
        start_extent = start;
        extent_count = count;

        pvolumes = NULL;
        stripe = NULL;
    }
};

#endif /* LV_LVSEGMENT_H_ */
