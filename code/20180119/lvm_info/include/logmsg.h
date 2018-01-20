/*
 * logmsg.h
 *
 *  Created on: 2012-10-10
 *      Author: zenglj
 */

#ifndef __LOGMSG_H__
#define __LOGMSG_H__

#include <stdio.h>

#ifdef LOGMSGDLL_EXPORTS
#define LOGMSGDLL_API __declspec(dllexport)
#else
#define LOGMSGDLL_API __declspec(dllimport)
#endif

#define TARGETSIM_TRACEMODE_DBG  0
#define TARGETSIM_TRACEMODE_ERR  1
#define TARGETSIM_TRACEMODE_INF  2

#define TARGETSIM_TRACE_TBL_MAX_NUM  4

/************************************************************************************/
struct targetsim_traceinfo {
    char * vec;
    int len;
};

#ifdef __cplusplus
extern "C" {
#endif

LOGMSGDLL_API void targetsim_trace_common_func(unsigned char trace_mode, const struct targetsim_traceinfo * extra_trace_info,
                            const char * file_name, int code_line, const char *format_str);

LOGMSGDLL_API void targetsim_set_tracefile_path(const char * dir, const char * filename);

LOGMSGDLL_API void targetsim_printf_hex(char * hexdata, int len);

LOGMSGDLL_API void targetsim_printf_str(char * strdata);

#ifdef DEBUG
#define TARGETSIM_TRACE_OUT_DBG(format, arg...) do { \
	char format_str[1024]; \
	snprintf(format_str, sizeof(format_str), format, ##arg); \
	targetsim_trace_common_func(TARGETSIM_TRACEMODE_DBG, NULL, __FILE__, __LINE__, format_str); \
} while(0)
#else
#define TARGETSIM_TRACE_OUT_DBG(format, arg...) do {;} while(0)
#endif

#define TARGETSIM_TRACE_OUT_INFO(format, arg...) do { \
	char format_str[1024]; \
	snprintf(format_str, sizeof(format_str), format, ##arg); \
	targetsim_trace_common_func(TARGETSIM_TRACEMODE_INF, NULL, __FILE__, __LINE__, format_str); \
} while(0)

#define TARGETSIM_TRACE_OUT_ERROR(format, arg...) do { \
	char format_str[1024]; \
	snprintf(format_str, sizeof(format_str), format, ##arg); \
	targetsim_trace_common_func(TARGETSIM_TRACEMODE_ERR, NULL, __FILE__, __LINE__, format_str); \
} while(0)

#define WTX_TRACE_OUT_DBG  TARGETSIM_TRACE_OUT_DBG
#define WTX_TRACE_OUT_ERROR  TARGETSIM_TRACE_OUT_ERROR
#define WTX_TRACE_OUT_INFO  TARGETSIM_TRACE_OUT_INFO

#ifdef __cplusplus
}
#endif
#endif /* __LOGMSG_H__ */
