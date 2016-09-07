#pragma once

namespace UI
{
// ����RichEdit�ı����ֲ�Ole������
class RETextEnum
{
public:
    enum ENUMRESULT
    {
        FINISH = 0,
        TEXT = 1,
        OLE = 2
    };

    RETextEnum(WindowlessRichEdit* pRE, LPCTSTR szText, int ncpOffset)
    {
        m_pRE = pRE;
        m_szBuffer = szText;
        m_ncpOffset = ncpOffset;
        m_nBufferSize = _tcslen(szText);
        Reset();
    }

    void  Reset()
    {
        m_iPos = 0;
        m_nTextStartPos = 0;
        m_pCurOleObj = NULL;
        m_curText.clear();
    }

    ENUMRESULT Next()
    {
        if (-1 == m_iPos)
            return FINISH;

        for (; m_iPos < m_nBufferSize; )
        {
            if ((WCHAR)m_szBuffer[m_iPos] == WCH_EMBEDDING)
            {
                int nTextStartPosSave = m_nTextStartPos;
                m_nTextStartPos = m_iPos + 1;

                m_pCurOleObj = NULL;
                m_curText.clear();

                int nTextLength = m_iPos - nTextStartPosSave;
                if (nTextLength > 0)
                {
                    m_curText.append(m_szBuffer + nTextStartPosSave, nTextLength);
                    return TEXT;
                }

                m_pCurOleObj = m_pRE->GetOleByCharPos(m_iPos + m_ncpOffset);
                m_iPos ++;
                return OLE;
            }

            m_iPos ++;
        }

        m_pCurOleObj = NULL;
        m_curText.clear();

        // ���һ���ı�
        int nTextLength = m_iPos - m_nTextStartPos;
        if (nTextLength > 0)
        {
            m_curText.append(m_szBuffer + m_nTextStartPos, nTextLength);

            m_iPos = -1;  // ��һ��ֱ�ӽ���
            return TEXT;
        }

        return FINISH;
    }

    REOleBase*  GetOle()
    {
        return m_pCurOleObj;
    }
    LPCTSTR  GetText()
    {
        return m_curText.c_str();
    }
    int  GetTextLength()
    {
        return m_curText.length();
    }


private:
    LPCTSTR  m_szBuffer;
    WindowlessRichEdit*  m_pRE;
    int  m_ncpOffset;       // ��ǰ�������ı�����RE�����е�ƫ�ƣ�����ȡole

    int  m_nBufferSize;
    int  m_iPos;            // ��ǰ����λ��
    int  m_nTextStartPos; // ��һ���ı���ʼ��λ��

    // Get
    REOleBase*  m_pCurOleObj;
    String  m_curText;
};

}