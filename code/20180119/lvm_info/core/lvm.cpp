/*
 * LVM.cpp
 *
 *  Created on: 2018年1月17日
 *      Author: Administrator
 */

#include "lvm_dep.h"
#include "lvm.h"
#include "VolumeGroup.h"
#include "platform.h"
#include "parttypes.h"
#include "logmsg.h"

LVM::LVM(FileHandle handle, lvm_uint64_t offset, Ext2Read *rd)
{
    fHandle = handle;
    m_pv_offset = offset;
    ext2read = rd;
    p_mbr = NULL;
    m_str_index = 0;
    m_length = 0;

    memset(((char *)m_ppMap), 0, sizeof(m_ppMap));
}

LVM::~LVM()
{
	/// 清除资源
	fclose(fHandle);
}

/**
 * 打开镜像文件
 * @param path：文件名
 * @return
 */
int LVM::lvm_open_img(const char * path)
{
	/// XXX 作为测试需要不需要，判断文件是否存在
	fHandle = fopen(path, "rb");
	if(NULL == fHandle) {
		// 报错
		WTX_TRACE_OUT_ERROR("IMG File error");

		return -1;
	}

	return 0;
}

int LVM::parse_mbr_partition()
{
    unsigned char sector[SECTOR_SIZE];
    struct MBRpartition *part;
//    Ext2Partition *partition;
    int sector_size = SECTOR_SIZE;
    int ret, i;

    /// 获取第0个扇区到缓存，即MBR扇区
    ret = read_disk(fHandle, sector, 0, 1, sector_size);
    if(ret < sector_size) {

        WTX_TRACE_OUT_ERROR("Error Reading the MBR on image\n");
        return -1;
    }

    /// 获取MBR分区表信息（4组信息）
    /// 解析LVM
    if(!valid_part_table_flag(sector)) {
    	/// 默认没有进行分区
    	WTX_TRACE_OUT_ERROR("Partition Table default  on image, 00 End of sector marker\n");
    	/// 用户没有分区直接创建文件系统，直接读第1个扇区，判断“LABELONE”
    	m_pv_offset = 1;
    	/// 只有一个分区
    	parse_lvm_head(0);
    } else {
        /** First Scan primary Partitions 4个主分区进行扫描 */
        for(i = 0; i < 4; i++) {

            part = pt_offset(sector, i);
            if((part->sys_ind != 0x00) || (get_nr_sects(part) != 0x00)) {

                WTX_TRACE_OUT_INFO("index %u ID %X size %d \n", i, part->sys_ind, get_nr_sects(part));
                WTX_TRACE_OUT_INFO("LVM Physical Volume found disk %d partition %d\n", 0, i);

                /// 1.常规模式用户会设置分区类型
                /// 2.非常规下用户不会设置分区类型，需要读分区头（对于LVM），否则直接判断文件系统幻数
                if(part->sys_ind == LINUX_LVM || part->sys_ind == LINUX_EX) {
                	/// 相对2个扇区的的“LABELONE”
                	/// 根据分区表获得分区的位置
                	m_pv_offset = get_start_sect(part) + SECTOR_SIZE;
                	/// 查找第i个分区
                	parse_lvm_head(i);
                }
            }
        }
    }

    return 0;
}

VolumeGroup *LVM::find_volgroup(string &uuid)
{
//    VolumeGroup *grp;
    list<VolumeGroup *>::iterator i;
//    list <VolumeGroup *> grouplist = ext2read->get_volgroups();


//    for(i = grouplist.begin(); i != grouplist.end(); i++)
//    {
//        grp = (*i);
//        if(grp->uuid.compare(uuid) == 0)
//        {
//            return grp;
//        }
//    }

    return NULL;
}

VolumeGroup *LVM::add_volgroup(string &uuid, string &name, int seq, int size)
{
    VolumeGroup *vol;
    //list <VolumeGroup *> grouplist = ext2read->get_volgroups();

    vol = new VolumeGroup(uuid, name, seq, size);
    if(!vol)
        return NULL;

//    ext2read->groups.push_back(vol);
//    LOG("Volgroup added \n");

    return vol;
}

/**
 * 扫描磁盘镜像，判断磁盘分区是否是LVM镜像管理方式
 *
 * @return
 */
int LVM::parse_lvm_head(int index)
{
    int ret;
    PV_LABEL_HEADER *header;
    PV_LABEL *label;
    uint64_t sector;
    int length;
    char buffer[SECTOR_SIZE];

    /// 2.Note,RedHat比较特殊，注意事项如下
    /// According to [REDHAT] the physical volume label can be stored in any of the first four sectors.
    for(int i = 0; i < 4; i ++) {
        sector = m_pv_offset + i;

        ret = read_disk(fHandle, buffer, sector, 1, SECTOR_SIZE);
        if(-1 == ret) {
        	WTX_TRACE_OUT_ERROR("Cant read disk");
        	return -1;
        }

        header = (PV_LABEL_HEADER *) &buffer[0];
        if(strncmp(header->pv_name, "LABELONE", LVM_SIGLEN) != 0) {
            continue;
            //LOG("Invalid label. The partition is not LVM2 volume\n");
            //return -1;
        }

    	/// 该主分区是否为LVM格式
    	m_ppMap[index] = 1;

        WTX_TRACE_OUT_INFO("PV Metadata: %s %UUID=%s offset %I64u \n", header->pv_name, header->pv_uuid, header->pv_labeloffset);

        /// 1.UUID
        strncpy(uuid, header->pv_uuid, UUID_LEN);
        uuid[UUID_LEN] = '\0';

        /// 2.计算label扇区
//        sector = (header->pv_labeloffset / SECTOR_SIZE) + m_pv_offset;
        sector = (header->pv_labeloffset / SECTOR_SIZE);
        read_disk(fHandle, buffer, sector, 1, SECTOR_SIZE);
        label = (PV_LABEL *) &buffer[0];

        /// 3.计算metadata
//        sector = pv_offset + ((label->pv_offset_low + label->pv_offset_high)/SECTOR_SIZE);
        sector = ((label->pv_offset_low + label->pv_offset_high) / SECTOR_SIZE);
        length = (label->pv_length + SECTOR_SIZE - 1) / SECTOR_SIZE;

        char * metadata = NULL;
        metadata = new char[length * SECTOR_SIZE];
        if(NULL == metadata) {
        	WTX_TRACE_OUT_ERROR("Cannt new metadata");

        	return -1;
        }

        read_disk(fHandle, metadata, sector, length, SECTOR_SIZE);
        /// 字符串最后一位添加结尾符
        metadata[label->pv_length] = 0;

        string str_metadata(metadata);
        m_pv_metadata = str_metadata;

        WTX_TRACE_OUT_INFO("\n%s", metadata);
        printf("%s\n", metadata); fflush(stdout);

//        pv_metadata = string::fromUtf8(metadata, label->pv_length);

        /// 4.解析metadata
        parse_metadata();
        delete [] metadata;

        break;
    }

    return 0;
}

string LVM::lvm_parse_metadata_string(string str, int value_len)
{
	m_str_index = m_pv_metadata.find(str, m_str_index);
	if(m_str_index > m_length) {
		WTX_TRACE_OUT_INFO("Cannt found VG UUID info\n");
	}

	return (m_pv_metadata.substr(m_str_index + LVM_FIX_SPACE_FORMAT + str.length() - 1, value_len));
}

/// TODO 因为是按照字符串进行存储，不知道存储数据是否有先后之分，这里处理字符串都按固定模式处理
/// 因此会出现问题,以及赋值过程中，等号和等号左右的空格符，都要进行考虑

// NOTE: Do error checking
int LVM::parse_metadata()
{
	int index = 0;
	int length = 0;

	m_length = m_pv_metadata.length();
	length = m_length;

	/// 1.查找第一个左大括号的索引减1，找到卷组
	index = m_pv_metadata.find("{", index);
	if(index > length) {
		WTX_TRACE_OUT_INFO("Cannt found VG info\n");
	}

	/// 减去一个空格，得到卷组名
	m_vg.volname = m_pv_metadata.substr(0, index - 1);

	/// 1.1 UUID
	m_vg.uuid = lvm_parse_metadata_string("id", VG_UUID_LEN);

	/// 1.2 seqno
	string str = lvm_parse_metadata_string("seqno", VG_SEQNO_LEN);
	m_vg.seqno = atoi(str.data());

	/// 1.3 format
	m_vg.format = lvm_parse_metadata_string("format", VG_FORMAT_LEN);

	/// 1.4 status

	/// 1.5 flags

	/// 1.6 extent_size,比较重要,单元extent的大小。

	int i_start , j_num;

	i_start = m_pv_metadata.find("extent_size", 0);
	j_num = m_pv_metadata.find("\n", i_start);

	str = m_pv_metadata.substr(i_start + LVM_FIX_SPACE_FORMAT + strlen("extent_size") - 1, j_num - (i_start + LVM_FIX_SPACE_FORMAT + strlen("extent_size") - 1));
	m_vg.extent_size = atoi(str.data());
	cout << m_vg.extent_size << endl;

	/// 1.7 max_lv

	/// 1.8 max_pv

	/// 1.9 metadata_copies

	/// 2.物理卷解析
	/// 不重要的区域没有进行解析
	/// 2.x pestart
	i_start = m_pv_metadata.find("pe_start", 0);
	j_num = m_pv_metadata.find("\n", i_start);
	
	str = m_pv_metadata.substr(i_start + LVM_FIX_SPACE_FORMAT + strlen("pe_start") - 1, j_num - (i_start + LVM_FIX_SPACE_FORMAT + strlen("pe_start") - 1));
	m_vg.pe_start = atoi(str.data());
	cout << m_vg.extent_size << endl;


	/// 3.逻辑卷解析，找到逻辑卷的地址
	/// 2.x 只关心start_extent
	i_start = m_pv_metadata.find("start_extent", 0);
	j_num = m_pv_metadata.find("\n", i_start);
	
	str = m_pv_metadata.substr(i_start + LVM_FIX_SPACE_FORMAT + strlen("start_extent") - 1, j_num - (i_start + LVM_FIX_SPACE_FORMAT + strlen("start_extent") - 1));
	m_vg.pe_start = atoi(str.data());
	cout << m_vg.extent_size << endl;
	
	cout << "Logic Volume Start(size):" << m_vg.extent_size * 512 + m_vg.pe_start * 512 << endl;

//    int num, num2, numbase;
//    string volname, suuid;
//    int seq = 0, size = 0;
//    bool ok;
//    VolumeGroup *grp;
//    QByteArray ba;
//
//    num = pv_metadata.indexOf("{");
//    volname = pv_metadata.left(num - 1);
//    num = pv_metadata.indexOf(QRegExp("[a-zA-Z0-9]*-{1,}[a-zA-Z0-9]*"), 0);
//    if(num > 0)
//    {
//        suuid = pv_metadata.mid(num, 38);
//        suuid.replace("-", "");
//    }
//    num = pv_metadata.indexOf(QRegExp("[0-9]"), num + 38);
//    if(num > 0)
//    {
//        seq = pv_metadata.mid(num, 1).toInt(&ok);
//        if(!ok)
//        {
//            LOG("Cannot Parse LVM Metadata :-( \n");
//            return -1;
//        }
//    }
//    num = pv_metadata.indexOf(QRegExp("[0-9]+"), num + 1);
//    if(num > 0)
//    {
//        size = pv_metadata.mid(num, 5).toInt(&ok);
//        if(!ok)
//        {
//            LOG("Cannot Parse LVM Metadata :-( \n");
//            return -1;
//        }
//    }
//
//    LOG("Volgroup found seq %d, extent_size %d\n",seq, size);
//    grp = find_volgroup(suuid);
//    if(!grp)
//    {
//        grp = add_volgroup(suuid, volname, seq, size);
//        grp->set_ext2read(ext2read);
//    }
//
//    // Parse Physical Volume
//    lvm_uint64_t dev_size;
//    uint32_t pe_start, pe_count;
//    num = pv_metadata.indexOf("physical_volumes", 0);
//    if(num < 0)
//        return -1;
//
//    while((num = pv_metadata.indexOf(QRegExp("pv[0-9\\s\\t]+\\{"), num)) > 0)
//    {
//        num = pv_metadata.indexOf(QRegExp("[a-zA-Z0-9]*-{1,}[a-zA-Z0-9]*"), num);
//        if(num < 0)
//            break;
//
//        suuid = pv_metadata.mid(num, 38);
//        suuid.replace("-", "");
//        num += 38;
//        numbase = num;
//        num = pv_metadata.indexOf(QRegExp("dev_size"), num);
//        num = pv_metadata.indexOf(QRegExp("[0-9]+"), num);
//        num2 = pv_metadata.indexOf(QRegExp("\\n"), num);
//        dev_size = pv_metadata.mid(num, num2-num).toULongLong(&ok);
//        if(!ok)
//        {
//            LOG("Cannot Parse LVM Metadata (Physical Volume) :-( \n");
//            return -1;
//        }
//
//        num = pv_metadata.indexOf(QRegExp("pe_start"), numbase);
//        num = pv_metadata.indexOf(QRegExp("[0-9]+"), num);
//        num2 = pv_metadata.indexOf(QRegExp("\\n"), num);
//        pe_start = pv_metadata.mid(num, num2-num).toUInt(&ok);
//        if(!ok)
//        {
//            LOG("Cannot Parse LVM Metadata :-( \n");
//            return -1;
//        }
//
//        num = pv_metadata.indexOf(QRegExp("pe_count"), numbase);
//        num = pv_metadata.indexOf(QRegExp("[0-9]+"), num);
//        num2 = pv_metadata.indexOf(QRegExp("\\n"), num);
//        pe_count = pv_metadata.mid(num, num2-num).toUInt(&ok);
//        if(!ok)
//        {
//            LOG("Cannot Parse LVM Metadata (Physical Volume) :-( \n");
//            return -1;
//        }
//
//        LOG("Physical Volume found. start %d, count %d, size %Ld\n", pe_start, pe_count, dev_size);
//        PhysicalVolume *pvol;
//        pvol = grp->find_physical_volume(suuid);
//        num2 = suuid.compare(uuid);
//        if(!pvol && (num2 == 0))
//        {
//            pvol = grp->add_physical_volume(suuid, dev_size, pe_start, pe_count, pv_handle, pv_offset);
//        }
//    }
//
//    // Parse Logical Volume
//    int nsegs;
//    num = pv_metadata.indexOf(QRegExp("logical_volumes"), 0);
//    if(num < 0)
//        return -1;
//    num = pv_metadata.indexOf(QRegExp("\\n"), num);
//    num += 2;
//
//    while((num = pv_metadata.indexOf(QRegExp("[a-zA-Z_0-9\\s\\t]+\\{"), num)) > 0)
//    {
//        string lvolname = volname;
//        num2 = pv_metadata.indexOf(QRegExp("[\\s\\t]+\\{"), num);
//        lvolname.append("_");
//        lvolname.append(pv_metadata.mid(num+1, num2-num));
//        num = pv_metadata.indexOf(QRegExp("[a-zA-Z0-9]*-{1,}[a-zA-Z0-9]*"), num);
//        if(num < 0)
//            break;
//
//        suuid = pv_metadata.mid(num, 38);
//        suuid.replace("-", "");
//        num += 38;
//        num = pv_metadata.indexOf("flags", num);
//        num = pv_metadata.indexOf(QRegExp("[0-9]+"), num);
//        num2 = pv_metadata.indexOf(QRegExp("\\n"), num);
//        nsegs = pv_metadata.mid(num, num2-num).toInt(&ok);
//        if(!ok)
//        {
//            LOG("Cannot Parse LVM Metadata (Logical Volume) :-( \n");
//            return -1;
//        }
//
//        LogicalVolume *lvol;
//        uint32_t start_extent, extent_count;
//        lvol = grp->find_logical_volume(suuid);
//        if(!lvol)
//            lvol = grp->add_logical_volume(suuid, nsegs, lvolname);
//        LOG("Logical Volume found. Name %s, segments %d\n", lvol->volname.toUtf8().data(), nsegs);
//        for(int i = 0; i < nsegs; i++)
//        {
//            num = pv_metadata.indexOf(QRegExp("segment[0-9]+"), num);
//            num += 8;
//            numbase = num;
//            num = pv_metadata.indexOf("start_extent", num);
//            num = pv_metadata.indexOf(QRegExp("[0-9]+"), num);
//            num2 = pv_metadata.indexOf(QRegExp("\\n"), num);
//            start_extent = pv_metadata.mid(num, num2-num).toInt(&ok);
//
//            num = pv_metadata.indexOf("extent_count", numbase);
//            num = pv_metadata.indexOf(QRegExp("[0-9]+"), num);
//            num2 = pv_metadata.indexOf(QRegExp("\\n"), num);
//            extent_count = pv_metadata.mid(num, num2-num).toInt(&ok);
//
//            // Multiple stripes NOT Implemented: we only support linear for now.
//            lv_segment *seg = new lv_segment(start_extent, extent_count);
//            seg->stripe = new struct stripe;
//            seg->stripe->stripe_pv = 0;
//            num = pv_metadata.indexOf(QRegExp("pv[0-9]+"), num);
//            num += 4;
//
//            num = pv_metadata.indexOf(QRegExp("[0-9]+"), num);
//            num2 = pv_metadata.indexOf(QRegExp("\\n"), num);
//            seg->stripe->stripe_start_extent = pv_metadata.mid(num, num2-num).toInt(&ok);
//            num = num2;
//            seg->pvolumes = NULL;   // we do the segment -> pv mapping later because this pv might not be found yet
//            lvol->segments.push_back(seg);
//            LOG("Segment found. start %d, count %d\n", start_extent + seg->stripe->stripe_start_extent, extent_count);
//        }
//    }

    return 0;
}

void LVM::print_header_info()
{
	int i;

	cout << "LVM format:";

	for(i = 0; i < 4; i ++ ) {
		  cout << "    " <<  "分区" << i;
	}

	cout << endl;

	i = strlen("LVM format:");
	while((i --) > 0) {
		cout << " ";
	}

	// 4.打印数据,需要打印分区，以及文件系统信息
	for(i = 0; i < 4; i ++ ) {
		if(1 == m_ppMap[i]) {
			// LVM
			cout << "    " <<  "yes" << i;
		} else {
			// 非LVM
			cout << "    " <<  "no " << i;
		}
	}
}

