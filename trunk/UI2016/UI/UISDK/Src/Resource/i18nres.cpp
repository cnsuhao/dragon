#include "stdafx.h"
#include "i18nres.h"

I18nRes::I18nRes(SkinRes* p)
{
	m_pSkinRes = p;
}

LPCTSTR I18nRes::I18nMap(LPCTSTR key)
{
	if (!key)
		return NULL;

	map<String, String>::iterator iter = m_dict.find(String(key));
	if (iter == m_dict.end())
		return NULL;

	return iter->second.c_str();
}

void  I18nRes::Add(LPCTSTR szId, LPCTSTR szValue)
{
	if (!szId || !szValue)
		return;

	m_dict[String(szId)] = String(szValue);
}