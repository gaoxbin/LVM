/*
 * lvm_type.h
 *
 *  Created on: 2018年1月17日
 *      Author: Administrator
 */

#ifndef INCLUDE_LVM_TYPE_H_
#define INCLUDE_LVM_TYPE_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifndef MY_TYPE

#define SECTOR_SIZE 512
typedef uint64_t lvm_uint64_t;
//typedef HANDLE FileHandle;
typedef FILE* FileHandle;
typedef void (*Ext2Read)();

#endif

//#include "ext2read.h"

#define LVM_SIGLEN	8
#define LVM_MAGIC_LEN	8
#define UUID_LEN	32
/// " = \n"共4个字符
#define LVM_FIX_SPACE_FORMAT 4
/// 包含两个双引号字符 38 + 2
#define VG_UUID_LEN (38 + 2)
#define VG_SEQNO_LEN 1
#define VG_FORMAT_LEN 6

#endif /* INCLUDE_LVM_TYPE_H_ */
