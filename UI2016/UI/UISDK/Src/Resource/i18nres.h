#pragma once
#include "Inc\Interface\iuires.h"


class I18nRes : public II18nCallback
{
public:
	I18nRes(SkinRes*);
public:
	virtual LPCTSTR I18nMap(LPCTSTR key) override;
	void  Add(LPCTSTR szId, LPCTSTR szValue);

private:
	SkinRes*  m_pSkinRes;
	map<String, String>  m_dict;
};