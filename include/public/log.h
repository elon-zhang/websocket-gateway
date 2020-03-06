#pragma once

#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>

#include "config.h"
#include "typedef.h"

#define NONE         "\033[m"
#define RED          "\033[0;32;31m"
#define LIGHT_RED    "\033[1;31m"
#define GREEN        "\033[0;32;32m"
#define LIGHT_GREEN  "\033[1;32m"
#define BLUE         "\033[0;32;34m"
#define LIGHT_BLUE   "\033[1;34m"
#define DARY_GRAY    "\033[1;30m"
#define CYAN         "\033[0;36m"
#define LIGHT_CYAN   "\033[1;36m"
#define PURPLE       "\033[0;35m"
#define LIGHT_PURPLE "\033[1;35m"
#define BROWN        "\033[0;33m"
#define YELLOW       "\033[1;33m"
#define LIGHT_GRAY   "\033[0;37m"
#define WHITE        "\033[1;37m"

// 初始化日志
bool init_log(const char* config_file);
// 写日志
void log(int lev, const char* color, const char* file, int line, const char* fmt, ...);

// 打印内存信息以16进制方式输出
void loghex(void* p, int len);

// 打印普通日志信息
#define INFO(fmt, args...)\
	log(3, "", myfilename(__FILE__), __LINE__, fmt, ##args);

// 打印告警日志信息
#define WARNING(fmt, args...)\
	log(4, LIGHT_BLUE, myfilename(__FILE__), __LINE__, fmt, ##args);

// 打印错误日志信息
#define ERROR(fmt, args...)\
	log(5, RED, myfilename(__FILE__), __LINE__, fmt, ##args);


// 检查条件若不成功则返回错误代码并打印日志
#define LogCheckCond(con, ret, fmt, args...)\
	if(!(con))\
	{\
	log(5, RED, myfilename(__FILE__), __LINE__, fmt, ##args); \
		return ret;\
	}

// 检查条件若不成功则返回错误代码并打印日志
#define LogCheckCondVoid(con, fmt, args...)\
	if(!(con))\
	{\
	log(5, RED, myfilename(__FILE__), __LINE__, fmt, ##args); \
		return;\
	}

// 检查是否满足条件，不满足条件就break并打印错误信息 [2015-5-6 15:37:49 Jacky]
#define LogCheckBreak(con, fmt, args...)\
	if ( !(con) )\
	{\
	log(5, RED, myfilename(__FILE__), __LINE__, fmt, ##args); \
		break;\
	}


// 检查是否满足条件，不满足条件就continue并打印错误信息 [2015-5-6 15:38:15 Jacky]
#define LogCheckContinue(con, fmt, args...)\
	if ( !(con) )\
	{\
	log(5, RED, myfilename(__FILE__), __LINE__, fmt, ##args); \
		continue;\
	}
