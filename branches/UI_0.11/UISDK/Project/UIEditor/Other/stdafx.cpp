// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// UIBuilder.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"

// TODO: �� STDAFX.H ��
// �����κ�����ĸ���ͷ�ļ����������ڴ��ļ�������


long g_lUiBuilderLogCookie = 0;  //_T("ui_builder")

void UIEDITOR_LOG_DEBUG( TCHAR* szContent, ... )
{
	//UIBUILDER_LOG_LEVEL(LOG_LEVEL_DEBUG, szContent );
}
void UIEDITOR_LOG_INFO( TCHAR* szContent, ... )
{
	//UIBUILDER_LOG_LEVEL(LOG_LEVEL_INFO, szContent );
}
void UIEDITOR_LOG_WARN( TCHAR* szContent, ... )
{
	//UIBUILDER_LOG_LEVEL(LOG_LEVEL_WARN, szContent );
}
void UIEDITOR_LOG_ERROR( TCHAR* szContent, ... )
{
	//UIBUILDER_LOG_LEVEL(LOG_LEVEL_ERROR, szContent );
}
void UIEDITOR_LOG_FATAL( TCHAR* szContent, ...)
{
	//UIBUILDER_LOG_LEVEL(LOG_LEVEL_FATAL, szContent );
}

