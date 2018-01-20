
#include <string.h>

#include "lvm.h"
#include "logmsg.h"

/**
 * 测试程序
 * TODO 按接口要求，需要传入文件句柄，以及分区偏移，在分区的第二个扇区识别出LVM执行相关操作
 *
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char **argv)
{
	// FIXME:所有参数目前是固定的，以后需要调整为参数传入
	LVM myLVM(NULL, 0, NULL);
	int ret = 0;

	// 1.打开设备
	ret = myLVM.lvm_open_img(argv[1]);
	if(-1 == ret) {
		// 输出log
		WTX_TRACE_OUT_ERROR("Dont open img");

		return -1;
	}

	// 2.解析镜像
	// 扫描镜像，获得LVM分区信息， 返回分区是否存在LVM信息，在类中设置相关信息
	ret = myLVM.parse_mbr_partition();
	if(ret == -1) {
		// 输出log
		WTX_TRACE_OUT_ERROR("Dont parse img");

		return -1;
	}

	// 3.打印信息
	myLVM.print_header_info();

	return 0;
}

