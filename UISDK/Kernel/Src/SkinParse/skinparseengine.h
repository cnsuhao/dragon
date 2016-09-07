#pragma once


namespace UI
{
interface IUIElement;
interface IUIDocument;
interface ISkinDataSource;
class SkinRes;

class SkinParseEngine
{
public:
    SkinParseEngine(SkinRes* pSkinRes);
    ~SkinParseEngine();

    bool  Parse(ISkinDataSource* pDataSource, LPCTSTR szXmlFile);
	bool  Parse(ISkinDataSource* pDataSource, LPCTSTR szXmlFile, __out IUIDocument** ppDoc);
    void  NewChild(IUIElement* pElement);

    static HRESULT  UIParseIncludeTagCallback(IUIElement*, ISkinRes* pSkinRes);

    IUIApplication*  m_pUIApplication;
    SkinRes*  m_pSkinRes;
};

}