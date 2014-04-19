#pragma once
// 2014.4.3 �λ��ǿ�Ч��ʵ���ࡣ������Դ����:
//
//  Goom
// http://www.ios-software.com/?page=projet&quoi=1
//
//  Goom �������
// https://code.mythtv.org/doxygen/videovisualgoom_8cpp_source.html
//
// �����ļ����ص�ַ��
// https://raw.githubusercontent.com/MythTV/mythtv/master/mythtv/libs/libmythtv/visualisations/goom/zoom_filter_xmmx.c
//
//
class Goom
{
public:
    Goom();
    ~Goom();

public:
    void  Init(unsigned int width, unsigned int height);
    void  Release();
    void  Resize(unsigned int width, unsigned int height);

    void  Render(float* fData, int nDataSize, HBITMAP hBitmap);
private:
    unsigned int*   m_buffer;
};