#pragma once
#include "UISDK\Kernel\Src\Animate\3dwrap\3dlib\3dlib.h"

// 3d��ˮ�ߴ�����
namespace UI
{

class _3dPipeline
{
public:
    _3dPipeline();

    void  Rotate(int x, int y, int z);
    void  Move(int x, int y, int z);

    void  update_locale_pos(RECT* prc);
    void  Do();

public:
    int   m_nRotateX;
    int   m_nRotateY;
    int   m_nRotateZ;
    int   m_nOffsetX;
    int   m_nOffsetY;
    int   m_nOffsetZ;
    POINT    m_ptOrigin;  // ����ԭ�㣬�����λͼ�����꣨���Ͻ�0��0��

    POINT3D  m_ptLocale[4];    // �ֲ�����
    Quad     m_quad;           // 3D�仯֮�������
};

}