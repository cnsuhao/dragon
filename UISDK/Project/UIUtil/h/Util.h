#pragma once 

// 如果要从标准 C++ 库中的类型派生，并且要编译调试版本 (/MTd)，而且编译器错误信息引用 _Container_base，则在 Microsoft Visual C++ 2005 中可以忽略 C4251。---vector<Object*>会报错
#pragma warning(disable:4251)
// This function or variable may be unsafe. Consider using freopen_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
#pragma warning(disable:4996)


#include <windows.h>
#include <vector>
#include <list>
#include <map>
#include <string>
#include <fstream>
#include <utility>
#include <iostream>
#include <tchar.h>
#include <assert.h>
#include <OleAuto.h>  // BSTR
using namespace std;

#ifdef UIUTIL_EXPORTS
#define UIUTILAPI __declspec(dllexport)
#else
#define UIUTILAPI __declspec(dllimport)
#endif


#ifdef _UNICODE
typedef wstring String;
#else
typedef string  String;
#endif

typedef bool (CALLBACK* EnumFileInDirProc)(LPCTSTR, LPCTSTR, WPARAM);

//
//	定义要导出的函数接口集，如果要禁用某一util函数类型，
//  则不要#define即可
//
#define UTIL_STRING
#define UTIL_APP
#define UTIL_FILE
#define UTIL_GDI
#define UTIL_DEBUG

namespace UI { namespace Util {

#ifdef UTIL_CRYPT
	//===============================================================
	// Crypt
	//===============================================================
	BOOL UIUTILAPI MD5_Digest( BYTE* bOrign, int nOriLen, 
			CBuffer *pDigestBuf );
	BOOL UIUTILAPI MD5_String_Digest( TCHAR* str, 
			CBuffer* pDigestBuf );
	BOOL UIUTILAPI MD5_File_Digest( TCHAR* szPath, 
			CBuffer* pDigestBuf );

	bool UIUTILAPI ImportPfxCert( TCHAR* szCertPath, 
			TCHAR* szPassword, WCHAR* wcsImportToWhere, 
			TCHAR* szErrInfo = NULL );
	bool UIUTILAPI RemoveCert( TCHAR* szSubject, TCHAR* szIssuer,
			WCHAR* wcsRemoveFromWhere, TCHAR* szErrInfo = NULL );
	bool UIUTILAPI ImportDerCert( TCHAR* szCertPath, 
			WCHAR* wcsImportToWhere, TCHAR* szErrInfo = NULL );

	void UIUTILAPI Encrypt( BYTE* bOrign, int nOriLen, 
			TCHAR* szPassword, CBuffer* pEncryptBuf );
#endif 

#ifdef UTIL_STRING
	//===============================================================
	// String
	//===============================================================
	int  UIUTILAPI U2A(const wchar_t*  wcsUnicode, 
			char* pAsciiBuf, int nBufferSize);
    int  UIUTILAPI U2UTF8(const wchar_t* unicode, 
			char* pAsciiBuf, int nBufferSize);
    int  UIUTILAPI UTF82U(const char* utf8, 
			wchar_t* pUnicodeBuf, int nBufferSize);
#if 0
	void  UIUTILAPI A2U( const char*  strAscii, 
			CBufferT<wchar_t>& unicodeBuf );
	void  UIUTILAPI B2HS( const BYTE* bBin, int nBinLen, 
			CBuffer& hexStrBuf );
	void  UIUTILAPI UTF82A( const char* utf8, 
			CBufferT<char>& asciiBuf );
	void  UIUTILAPI A2UTF8( const char*  ascii, 
			CBufferT<char>& asciiBuf );
#endif
	int  UIUTILAPI ISO8859_1ToUnicode(
			const BYTE* pSrc, 
			int nSrcLen, 
			wchar_t* pBuffer, 
			int nBufLen
	);
    // szEncode的长度为 = new TCHAR[4*wcslen(szText) + 1]
    void  UIUTILAPI UnicodeEncode(
			const WCHAR*  szText, 
			TCHAR* szEncode
	);  
    void  UIUTILAPI UnicodeDecode(
			const WCHAR*  szText,
			TCHAR* szDecode
	);
	int  UIUTILAPI CompareNoCase(
			LPCTSTR str1, 
			LPCTSTR str2
	);

	void UIUTILAPI FormatV( 
			TCHAR* pszFormat, 
			va_list argList, 
			BSTR& bstr 
	);

	bool UIUTILAPI TranslateRECT(
			LPCTSTR strRect,
			RECT* pRect, 
			TCHAR szSep = _T(',') 
	);

	bool UIUTILAPI TranslateSIZE(
			LPCTSTR strRect,
			SIZE* pSize, 
			TCHAR szSep = _T(',') 
	);

	COLORREF UIUTILAPI TranslateRGB(
			LPCTSTR strCol, 
			TCHAR szSep = _T(',') 
	);

    COLORREF UIUTILAPI TranslateHexColor(LPCTSTR szColor);
    COLORREF UIUTILAPI TranslateColor(LPCTSTR szColor);
	bool UIUTILAPI TranslateImage9Region(
			LPCTSTR str, 
			void* p9Region, 
			TCHAR szSep = _T(',') 
	);

    interface ISplitStringEnum
    {
        virtual void  Release() = 0;
        virtual LPCTSTR GetText(UINT nIndex) = 0;
        virtual int   GetCount() = 0;
    };
	UINT UIUTILAPI SplitString(
			LPCTSTR szText, 
			TCHAR szSep,
			ISplitStringEnum** pEnum
	);
#endif

#ifdef UTIL_FILE
	//=================================================================
	// File
	//=================================================================
#if 0
	BOOL UIUTILAPI GetFileBuffer( TCHAR* szPath, 
        CBufferT<char>& fileBuf );
#endif
	BOOL UIUTILAPI OpenFolderAndSelectFile( TCHAR* szFullPath );

	void UIUTILAPI ShowFileProp( TCHAR* szFile );
	void UIUTILAPI ExecFile( TCHAR* szFile, BOOL bWait = FALSE );
	
	BOOL UIUTILAPI GetPathDir( 
			LPCTSTR szPath, 
			TCHAR* szOutDir 
	);
	BOOL UIUTILAPI GetPathFileName(
			LPCTSTR szPath, 
			TCHAR* szOutFileName
	);
    BOOL UIUTILAPI GetPathFileExt(
			LPCTSTR szPath, 
			TCHAR* szOutExt
	);
	BOOL UIUTILAPI IsFullPath( LPCTSTR szPath );
	BOOL UIUTILAPI CalcRelativePathToDir(
			LPCTSTR szDir, 
			LPCTSTR szPath, 
			TCHAR* szOutRelativePath
	);
	BOOL UIUTILAPI CalcRelativePathToFile(
			LPCTSTR szFile, 
			LPCTSTR szPath, 
			TCHAR* szOutRelativePath 
	);
	BOOL UIUTILAPI CalcFullPathByRelative(
			LPCTSTR szDir,
			LPCTSTR szRelative, 
			TCHAR* szOut
	); 
	void UIUTILAPI _WtlAbbreviateName(
			_Inout_z_ LPTSTR lpszCanon, 
			int cchMax,
			BOOL bAtLeastNam
	);
    void UIUTILAPI PathReplace2Backslashes(TCHAR* szPath);

	//bool EnumFileInDirProc(TCHAR* szFileName);
	BOOL UIUTILAPI EnumFileInDirectory(
			LPCTSTR szDir, 
			EnumFileInDirProc proc, 
			WPARAM wParam
	);

	bool UIUTILAPI CreateEmptyXmlFileA(
			const char* szPath, 
			const char* szRoot, 
			const char* szNode = NULL
	);
	bool UIUTILAPI CreateEmptyXmlFileW(
			const wchar_t* szPath, 
			const wchar_t* szRoot, 
			const wchar_t* szNode = NULL
	);

#ifdef _UNICODE
#	define CreateEmptyXmlFile  CreateEmptyXmlFileW
#else
#	define CreateEmptyXmlFile  CreateEmptyXmlFileA
#endif
#endif

#ifdef UTIL_APP
	//===============================================================
	// Application
	//===============================================================
	void UIUTILAPI GetAppPath(HINSTANCE hInst, TCHAR* strPath);
	void UIUTILAPI GetAppPath_(HINSTANCE hInst, TCHAR* strPath);
	void UIUTILAPI GetAppPath(TCHAR* strPath);
	void UIUTILAPI GetAppPath_(TCHAR* strPath);
	bool UIUTILAPI IsKeyDown(UINT vk);
	void UIUTILAPI DeflatRect(RECT* pfc, LPCRECT pDeflatRc);
	bool UIUTILAPI RectInRect(RECT* prcBig, RECT* prcSmall);
	
	bool UIUTILAPI InstallAutoRun(BOOL bInstall, TCHAR* szName, 
        TCHAR* szPath);
	bool UIUTILAPI QueryAutoRun(TCHAR* szName, TCHAR* szPath);
#endif

#ifdef UTIL_DEBUG

	void UIUTILAPI CreateDumpFileWhenCrash(
			LPCTSTR szFilePath,
			bool bAddTime
	);
    void UIUTILAPI ReplaceIATEntryInOneModule(
			PCSTR pszCalleeModuleName, 
			PROC pfnCurrent, 
			PROC pfnNew, 
			HMODULE hModCaller
	);
    void UIUTILAPI ReplaceIATEntryInAllModules(
			PCSTR pszCalleeModName, 
			PROC pfnCurrent, 
			PROC pfnNew,
			BOOL fExcludeAPIHookMod
	);

    class UIUTILAPI CApiHook
    {
    public:
		CApiHook();
        CApiHook(
            PSTR pszCalleeModName, 
            PROC pfnCurrent, 
            PROC pfnNew,
            BOOL fExcludeAPIHookMod);
        ~CApiHook();

		void  Hook(
			PSTR pszCalleeModName, 
			PROC pfnCurrent, 
			PROC pfnNew,
			BOOL fExcludeAPIHookMod);
		void  UnHook();
    public:
        PCSTR  m_pszCalleeModName;
        PROC   m_pfnOrig; 
        PROC   m_pfnHook;
        BOOL   m_fExcludeAPIHookMod;
    };

#endif

#ifdef UTIL_OTHER
	//  atlmfc 提供了Base64Decode/Base64Encode API，直接调用即可
#endif

#ifdef UTIL_GDI
	int UIUTILAPI FontSize2Height(int nSize);
	int UIUTILAPI FontHeight2Size(int nHeight);

	BOOL UIUTILAPI IsFontExist(LPCTSTR pszFaceName);
	BOOL UIUTILAPI GradientFillH( HDC hDC, const RECT* prc,
        COLORREF colFrom, COLORREF colTo );
	BOOL UIUTILAPI GradientFillV( HDC hDC, const RECT* prc, 
        COLORREF colFrom, COLORREF colTo );

	LONG UIUTILAPI HimetricXtoDX(LONG xHimetric, LONG xPerInch=-1);
	LONG UIUTILAPI HimetricYtoDY(LONG yHimetric, LONG yPerInch=-1);
	LONG UIUTILAPI DXtoHimetricX(LONG dx, LONG xPerInch=-1);
	LONG UIUTILAPI DYtoHimetricY(LONG dy, LONG yPerInch=-1);
	LONG UIUTILAPI TwipstoDX(long xTwips);
	LONG UIUTILAPI TwipstoDY(long dx);
	LONG UIUTILAPI DXtoTwips(long yTwips);
	LONG UIUTILAPI DYtoTwips(long dy);

	BOOL UIUTILAPI IsLayeredWindow(HWND hWnd);

	enum FixAlphaMode
	{
        // 直接将alpha设置为255
		SET_ALPHA_255,                 

        // 如果当前alpha==0，将alpha设置为255
		SET_ALPHA_255_IF_ALPHA_IS_0,   

        // 如果当前像素不是0，则将alpha设置为255
        SET_ALPHA_255_IF_RGBA_ISNOT_0, 

        // alpha 反转 0->255, 255->0
        SET_ALPHA_INVERSE_0_255,    

		// 将alpha设置为指定值wParam
		SET_ALPHA_VALUE,

		// ...
        // 如果当前alpha==wParam，将alpha设置为lParam
	//	SET_ALPHA_IF_ALPHA_EQUAL,      

        // 如果当前alpha<wParam，将alpha设置为lParam
	//	SET_ALPHA_IF_ALPHA_LESS,       
	};
	struct FixAlphaData
	{
        // 当仅指定了hDC，没有指定hBitmap时，从hDC中获取CurrentBitap
		HDC      hDC;      
		HBITMAP  hBitmap;
		BOOL     bTopDownDib;
		RECT*    lprc;
		FixAlphaMode  eMode;
		WPARAM   wParam;
		LPARAM   lParam;
	};
	BOOL UIUTILAPI FixBitmapAlpha(FixAlphaData* pData);

    BOOL UIUTILAPI PreMultiAlpha(HBITMAP hBitmap, RECT* lprc, BOOL bTopDownDib);
#endif

}}