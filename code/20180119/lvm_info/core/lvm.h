/*
 * LVM.h
 *
 *  Created on: 2018年1月17日
 *      Author: Administrator
 */

#ifndef CORE_LVM_H_
#define CORE_LVM_H_

#include "lvm_dep.h"
#include "lvm_type.h"
#include "VolumeGroup.h"
#include "partition.h"

class LVM {

private:
    FileHandle fHandle;
    char uuid[UUID_LEN + 1];

//    QString pv_metadata;
    Ext2Read *ext2read;
    MBRpartition *p_mbr;
    int m_str_index;
    int m_length;

public:
    LVM(FileHandle handle, lvm_uint64_t offset, Ext2Read *rd);
    ~LVM();
    /// 打开文件
    int lvm_open_img(const char * path);

    /// 解析MBR获得分区的首扇区
    int parse_mbr_partition();
    /// 解析获得lvm数据头
    int parse_lvm_head(int index);

    int parse_metadata();
    string lvm_parse_metadata_string(string str, int value_len);
    VolumeGroup *find_volgroup(string &uuid);
    VolumeGroup *add_volgroup(string &uuid, string &name, int seq, int size);

#ifndef NONE_TEST
    /// TODO 以下只是测试专用

	FileHandle getPvHandle() const {
		return fHandle;
	}

	void setPvHandle(FileHandle Handle) {
		fHandle = Handle;
	}

	lvm_uint64_t getPvOffset() const {
		return m_pv_offset;
	}

	void setPvOffset(lvm_uint64_t pvOffset) {
		m_pv_offset = pvOffset;
	}

	void print_header_info();

#endif

public:
	int m_ppMap[4];
    lvm_uint64_t m_pv_offset;
    string m_pv_metadata;
    VolumeGroup m_vg;
};

#endif /* CORE_LVM_H_ */
