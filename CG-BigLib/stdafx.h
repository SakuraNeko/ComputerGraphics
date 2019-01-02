// stdafx.h: 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 项目特定的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
// Windows 头文件
#include <windows.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>



// 在此处引用程序需要的其他标头
#include <math.h>
#include <vector>
#include <mmsystem.h> //导入声音头文件库
#include "GLTools.h"
#include "3ds.h"
#include "Texture.h"
#pragma comment(lib,"winmm.lib")//导入声音的链接库
