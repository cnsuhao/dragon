

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Sun Jul 05 20:03:35 2015
 */
/* Compiler settings for .\UILog.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __UILog_i_h__
#define __UILog_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ILogItem_FWD_DEFINED__
#define __ILogItem_FWD_DEFINED__
typedef interface ILogItem ILogItem;
#endif 	/* __ILogItem_FWD_DEFINED__ */


#ifndef __ILog_FWD_DEFINED__
#define __ILog_FWD_DEFINED__
typedef interface ILog ILog;
#endif 	/* __ILog_FWD_DEFINED__ */


#ifndef __Log_FWD_DEFINED__
#define __Log_FWD_DEFINED__

#ifdef __cplusplus
typedef class Log Log;
#else
typedef struct Log Log;
#endif /* __cplusplus */

#endif 	/* __Log_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_UILog_0000_0000 */
/* [local] */ 

typedef 
enum eLogLevel
    {	LOG_LEVEL_DEBUG	= 0,
	LOG_LEVEL_INFO	= 1,
	LOG_LEVEL_WARN	= 2,
	LOG_LEVEL_ERROR	= 3,
	LOG_LEVEL_FATAL	= 4
    } 	LOG_LEVEL;

typedef 
enum eLogFlag
    {	LOG_FLAG_THREADID	= 0x1,
	LOG_FLAG_CONSOLE	= 0x2,
	LOG_FLAG_DBGVIEW	= 0x4,
	LOG_FLAG_FILE	= 0x8,
	LOG_FLAG_FUNCTION	= 0x10,
	LOG_FLAG_LINE	= 0x20
    } 	LOG_FLAG;

typedef 
enum eLogFileFlag
    {	LOG_FLAG_FILE_DISABLE	= 0x1,
	LOG_FLAG_FILE_SIMPLE	= 0x2,
	LOG_FLAG_FILE_TIME	= 0x4,
	LOG_FLAG_FILE_LOOP	= 0x8
    } 	LOG_FILE_FLAG;

typedef struct tagLOGITEM
    {
    VARIANT_BOOL bEnable;
    BSTR logName;
    BSTR logFile;
    LOG_LEVEL logLevel;
    long uLogFlag;
    long uLogFileFlag;
    long uLogFileLoopCount;
    long uCookie;
    } 	LOGITEM;

typedef struct tagLogData
    {
    long lCookie;
    LOG_LEVEL level;
    OLECHAR *szContent;
    OLECHAR *szFile;
    OLECHAR *szFunction;
    long lLine;
    } 	LOGDATA;



extern RPC_IF_HANDLE __MIDL_itf_UILog_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_UILog_0000_0000_v0_0_s_ifspec;

#ifndef __ILogItem_INTERFACE_DEFINED__
#define __ILogItem_INTERFACE_DEFINED__

/* interface ILogItem */
/* [unique][helpstring][nonextensible][uuid][object] */ 


EXTERN_C const IID IID_ILogItem;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C63D124C-7D48-4fc8-9C52-6BF9752C1765")
    ILogItem : public IUnknown
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IsEnable( 
            /* [retval][out] */ VARIANT_BOOL *pbEnable) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetName( 
            /* [retval][out] */ BSTR *pRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPath( 
            /* [retval][out] */ BSTR *pRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetLevel( 
            /* [retval][out] */ LONG *pRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetLogFlag( 
            /* [retval][out] */ LONG *pRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetFileFlag( 
            /* [retval][out] */ LONG *pRetVal) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetCookie( 
            /* [retval][out] */ LONG *pRetVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ILogItemVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ILogItem * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ILogItem * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ILogItem * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *IsEnable )( 
            ILogItem * This,
            /* [retval][out] */ VARIANT_BOOL *pbEnable);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetName )( 
            ILogItem * This,
            /* [retval][out] */ BSTR *pRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPath )( 
            ILogItem * This,
            /* [retval][out] */ BSTR *pRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetLevel )( 
            ILogItem * This,
            /* [retval][out] */ LONG *pRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetLogFlag )( 
            ILogItem * This,
            /* [retval][out] */ LONG *pRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetFileFlag )( 
            ILogItem * This,
            /* [retval][out] */ LONG *pRetVal);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetCookie )( 
            ILogItem * This,
            /* [retval][out] */ LONG *pRetVal);
        
        END_INTERFACE
    } ILogItemVtbl;

    interface ILogItem
    {
        CONST_VTBL struct ILogItemVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILogItem_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ILogItem_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ILogItem_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ILogItem_IsEnable(This,pbEnable)	\
    ( (This)->lpVtbl -> IsEnable(This,pbEnable) ) 

#define ILogItem_GetName(This,pRetVal)	\
    ( (This)->lpVtbl -> GetName(This,pRetVal) ) 

#define ILogItem_GetPath(This,pRetVal)	\
    ( (This)->lpVtbl -> GetPath(This,pRetVal) ) 

#define ILogItem_GetLevel(This,pRetVal)	\
    ( (This)->lpVtbl -> GetLevel(This,pRetVal) ) 

#define ILogItem_GetLogFlag(This,pRetVal)	\
    ( (This)->lpVtbl -> GetLogFlag(This,pRetVal) ) 

#define ILogItem_GetFileFlag(This,pRetVal)	\
    ( (This)->lpVtbl -> GetFileFlag(This,pRetVal) ) 

#define ILogItem_GetCookie(This,pRetVal)	\
    ( (This)->lpVtbl -> GetCookie(This,pRetVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ILogItem_INTERFACE_DEFINED__ */


#ifndef __ILog_INTERFACE_DEFINED__
#define __ILog_INTERFACE_DEFINED__

/* interface ILog */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ILog;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("47F812B2-1295-4115-92B9-283A31DF971A")
    ILog : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Create( 
            /* [in] */ BSTR strXmlPath) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Load( 
            /* [in] */ BSTR strXmlPath) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Log( 
            /* [in] */ LOGDATA *__MIDL__ILog0000) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE EnableLog( 
            /* [in] */ VARIANT_BOOL bEnable) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddLogItem( 
            /* [in] */ LOGITEM *pLogItem,
            /* [out] */ long *pOutCookie) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ModifyLogItem( 
            /* [in] */ long lCookie,
            /* [in] */ LOGITEM *pLogItem) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DeleteLogItem( 
            /* [in] */ long lCookie) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Save( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IsDirty( 
            /* [retval][out] */ VARIANT_BOOL *pbDirty) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IsEnable( 
            /* [retval][out] */ VARIANT_BOOL *pbEnable) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetLogItemCount( 
            /* [retval][out] */ LONG *pRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetLogItemByIndex( 
            /* [in] */ LONG lIndex,
            /* [retval][out] */ ILogItem **ppRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetLogItemByName( 
            /* [in] */ BSTR bstrLogItemName,
            /* [retval][out] */ ILogItem **ppRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetLogItemByCookie( 
            /* [in] */ long lCookie,
            /* [retval][out] */ ILogItem **ppRetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE TestCanLog( 
            /* [in] */ long lCookie,
            /* [in] */ LOG_LEVEL level,
            /* [retval][out] */ VARIANT_BOOL *pbRet) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ILogVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ILog * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ILog * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ILog * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ILog * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ILog * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ILog * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ILog * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Create )( 
            ILog * This,
            /* [in] */ BSTR strXmlPath);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Load )( 
            ILog * This,
            /* [in] */ BSTR strXmlPath);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Log )( 
            ILog * This,
            /* [in] */ LOGDATA *__MIDL__ILog0000);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *EnableLog )( 
            ILog * This,
            /* [in] */ VARIANT_BOOL bEnable);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddLogItem )( 
            ILog * This,
            /* [in] */ LOGITEM *pLogItem,
            /* [out] */ long *pOutCookie);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ModifyLogItem )( 
            ILog * This,
            /* [in] */ long lCookie,
            /* [in] */ LOGITEM *pLogItem);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DeleteLogItem )( 
            ILog * This,
            /* [in] */ long lCookie);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Save )( 
            ILog * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *IsDirty )( 
            ILog * This,
            /* [retval][out] */ VARIANT_BOOL *pbDirty);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *IsEnable )( 
            ILog * This,
            /* [retval][out] */ VARIANT_BOOL *pbEnable);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetLogItemCount )( 
            ILog * This,
            /* [retval][out] */ LONG *pRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetLogItemByIndex )( 
            ILog * This,
            /* [in] */ LONG lIndex,
            /* [retval][out] */ ILogItem **ppRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetLogItemByName )( 
            ILog * This,
            /* [in] */ BSTR bstrLogItemName,
            /* [retval][out] */ ILogItem **ppRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetLogItemByCookie )( 
            ILog * This,
            /* [in] */ long lCookie,
            /* [retval][out] */ ILogItem **ppRetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *TestCanLog )( 
            ILog * This,
            /* [in] */ long lCookie,
            /* [in] */ LOG_LEVEL level,
            /* [retval][out] */ VARIANT_BOOL *pbRet);
        
        END_INTERFACE
    } ILogVtbl;

    interface ILog
    {
        CONST_VTBL struct ILogVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILog_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ILog_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ILog_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ILog_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ILog_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ILog_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ILog_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ILog_Create(This,strXmlPath)	\
    ( (This)->lpVtbl -> Create(This,strXmlPath) ) 

#define ILog_Load(This,strXmlPath)	\
    ( (This)->lpVtbl -> Load(This,strXmlPath) ) 

#define ILog_Log(This,__MIDL__ILog0000)	\
    ( (This)->lpVtbl -> Log(This,__MIDL__ILog0000) ) 

#define ILog_EnableLog(This,bEnable)	\
    ( (This)->lpVtbl -> EnableLog(This,bEnable) ) 

#define ILog_AddLogItem(This,pLogItem,pOutCookie)	\
    ( (This)->lpVtbl -> AddLogItem(This,pLogItem,pOutCookie) ) 

#define ILog_ModifyLogItem(This,lCookie,pLogItem)	\
    ( (This)->lpVtbl -> ModifyLogItem(This,lCookie,pLogItem) ) 

#define ILog_DeleteLogItem(This,lCookie)	\
    ( (This)->lpVtbl -> DeleteLogItem(This,lCookie) ) 

#define ILog_Save(This)	\
    ( (This)->lpVtbl -> Save(This) ) 

#define ILog_IsDirty(This,pbDirty)	\
    ( (This)->lpVtbl -> IsDirty(This,pbDirty) ) 

#define ILog_IsEnable(This,pbEnable)	\
    ( (This)->lpVtbl -> IsEnable(This,pbEnable) ) 

#define ILog_GetLogItemCount(This,pRetVal)	\
    ( (This)->lpVtbl -> GetLogItemCount(This,pRetVal) ) 

#define ILog_GetLogItemByIndex(This,lIndex,ppRetVal)	\
    ( (This)->lpVtbl -> GetLogItemByIndex(This,lIndex,ppRetVal) ) 

#define ILog_GetLogItemByName(This,bstrLogItemName,ppRetVal)	\
    ( (This)->lpVtbl -> GetLogItemByName(This,bstrLogItemName,ppRetVal) ) 

#define ILog_GetLogItemByCookie(This,lCookie,ppRetVal)	\
    ( (This)->lpVtbl -> GetLogItemByCookie(This,lCookie,ppRetVal) ) 

#define ILog_TestCanLog(This,lCookie,level,pbRet)	\
    ( (This)->lpVtbl -> TestCanLog(This,lCookie,level,pbRet) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ILog_INTERFACE_DEFINED__ */



#ifndef __UILogLib_LIBRARY_DEFINED__
#define __UILogLib_LIBRARY_DEFINED__

/* library UILogLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_UILogLib;

EXTERN_C const CLSID CLSID_Log;

#ifdef __cplusplus

class DECLSPEC_UUID("3A24D68D-EBB5-4836-B45A-3A954BFD07A1")
Log;
#endif
#endif /* __UILogLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


