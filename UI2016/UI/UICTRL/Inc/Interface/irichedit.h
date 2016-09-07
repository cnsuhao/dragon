#ifndef _UI_IRICHEDIT_H_
#define _UI_IRICHEDIT_H_

namespace UI
{
    
enum TextFilterResult
{
    TFR_NOT_HANDLED,  // 未处理，使用默认RE处理
    TFR_HANDLED,      // 已处理，不需要RE处理
    TFR_REPLACE_TEXT  // 未处理，但将文本进行替换后再处理
};
enum TextFilterType
{
    TFT_NONE,
    TFT_DIGIT,
    TFT_UINT,
    TFT_PHONENUMBER,
    TFT_ACCOUNT,
	TFT_PASSWORD,
};
interface ITextFilter
{   
    // 在RE析构时，不需要再还原状态了，bRestoreState设置为false
    // 在替换textfilter时，则需要将bRestoreState设置为true
    virtual void  Release(bool bRestoreState) = 0;
    virtual TextFilterType  GetType() = 0;

    virtual void   KeyFilter(UINT nKey, TextFilterResult& bHandled) = 0;
    virtual TCHAR   CharFilter(const TCHAR szSrc, TextFilterResult& bHandled) = 0;
    virtual LPCTSTR   StringFilter(LPCTSTR szSrc, TextFilterResult& bHandled) = 0;
};


}

#endif // _UI_IRICHEDIT_H_