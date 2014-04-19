#include "stdafx.h"
#include "goom.h"
#include "goom\goom_core.h"

Goom::Goom():m_buffer(NULL)
{

}

Goom::~Goom()
{
    Release();
}

void  Goom::Init(unsigned int width, unsigned int height)
{
    goom_init(width, height, 0);
}

void  Goom::Release()
{
    goom_close();
}

void  Goom::Render(float* fData, int nDataSize, HBITMAP hBitmap)
{
    unsigned int* last = m_buffer;

    int numSamps = 512;
    if (nDataSize < 512)
        numSamps = nDataSize;

    signed short int data[2][512];
    int i= 0;
    for (; i < numSamps; i++)
    {
        data[0][i] = (int)fData[i];
        data[1][i] = data[0][i];
    }

    for (; i < 512; i++)
    {
        data[0][i] = 0;
        data[1][i] = 0;
    }

    m_buffer = goom_update(data, 0);

    if (m_buffer != last)
    {
        BITMAP bm = {0};
        ::GetObject(hBitmap, sizeof(bm), &bm);
        int*  pnBits = (int*)bm.bmBits;

        memcpy(pnBits, m_buffer, bm.bmWidth*bm.bmHeight*4);
    }
}

void  Goom::Resize(unsigned int width, unsigned int height)
{
    goom_set_resolution(width, height, 0);
}