#ifndef LEAKDETECT_H_DB346733_FCF0_4f4d_A7DE_DB09E9527BC4
#define LEAKDETECT_H_DB346733_FCF0_4f4d_A7DE_DB09E9527BC4

// TODO: #include <crtdbg.h>  看看这个头文件能否用得上，直接替代
// 
//////////////////////////////////////////////////////////////////////////
//
//   内存泄露检查改进版本（还需要在每个模块的入口处添加：
//    _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
//
//	 注：如果发现有new定义冲突，则需要在该cpp中添加 #undef new取消该功能
//
//////////////////////////////////////////////////////////////////////////

#if defined(_DEBUG) && !defined(_IOSTREAM_)
#include <iostream>  // 不加这个include，会导致下面的内存泄露检测编译报错，好像是因为和stl冲突了?
#endif

#ifdef _DEBUG
#	define DEBUG_CLIENTBLOCK new(_CLIENT_BLOCK, __FILE__, __LINE__)
#else
#	define DEBUG_CLIENTBLOCK
#endif

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#ifdef _DEBUG
#	define new DEBUG_CLIENTBLOCK
#endif


#endif // LEAKDETECT_H_DB346733_FCF0_4f4d_A7DE_DB09E9527BC4