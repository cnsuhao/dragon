#include "stdafx.h"
#include "window3danimate.h"
#include "UISDK\Kernel\Src\Animate\3dwrap\3dlib\3dlib.h"
#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"
#include "UISDK\Kernel\Src\Animate\animatemgr.h"
#include "UISDK\Kernel\Src\Animate\windowanimate\layeredanimatewindow.h"
#include "UISDK\Kernel\Src\Animate\3dwrap\3dlib\3dpipeline.h"
#include "UISDK\Kernel\Src\RenderLayer2\soft3dtransform\soft3dtransform.h"
#include "UISDK\Kernel\Inc\Util\math\matrix.h"


Window3DAnimate::Window3DAnimate()
{
	m_nRotateX = 0;
	m_nRotateY = 0;
	m_nRotateZ = 0;
	m_nOffsetX = 0;
	m_nOffsetY = 0;
	m_nOffsetZ = 0;

    m_pIWindow3DAnimate = NULL;
}
Window3DAnimate::~Window3DAnimate()
{
}


void  Window3DAnimate::Initialize()
{
    __super::Initialize();

    Init_Sin_Cos_Tables();

	CRect  rc(0, 0, m_nSrcWndWidth, m_nSrcWndHeight);
	m_textmapping.SetTextureRect(&rc);

	// 设置纹理(src)和目标缓存(dest)
	ImageData  srcBuffer;
	srcBuffer.m_nbpp = 32;
	srcBuffer.m_ptr = m_pSrcBits;
	srcBuffer.m_pScan0 = m_pSrcBits;
	srcBuffer.m_nStride = m_nSrcPitch;
	srcBuffer.m_nWidth = m_nSrcWndWidth;
	srcBuffer.m_nHeight = m_nSrcWndHeight;
	m_textmapping.SetSrcBuffer(&srcBuffer);

	ImageData  dstBuffer;
	dstBuffer.m_nbpp = 32;
	dstBuffer.m_ptr = m_pLayeredWindow->m_pBits;
	dstBuffer.m_pScan0 = m_pLayeredWindow->m_pBits;
	dstBuffer.m_nStride = m_pLayeredWindow->m_nPitch;
	dstBuffer.m_nWidth = m_pLayeredWindow->m_nWidth;
	dstBuffer.m_nHeight = m_pLayeredWindow->m_nHeight;
	m_textmapping.SetDstBuffer(&dstBuffer);
}

void Window3DAnimate::OnTick(int nCount, IStoryboard** ppTimerArray)
{
	Transform3D  transform;
	transform.set_size(m_nSrcWndWidth, m_nSrcWndHeight);
	transform.rotate3d((float)m_nRotateX, (float)m_nRotateY, (float)m_nRotateZ);
	transform.translate3d((float)m_nOffsetX, (float)m_nOffsetY, (float)m_nOffsetZ);
	transform.perspective(2000);

	Matrix44  matrix;
	transform.get_matrix(&matrix);

	CRect  rcTexture(&m_rcWindowInBuffer);
	RECT  rPadding;
	m_textmapping.GetTexturePadding(&rPadding);
	rcTexture.OffsetRect(rPadding.left, rPadding.top);

	// 设置纹理映射坐标
	QUAD quad;
	matrix.MapRect2Quad(&rcTexture, &quad);  // 变换
	m_textmapping.SetQuad(&quad, &rcTexture);

 	m_pLayeredWindow->Clear();
 	POINT ptOffset = {0, 0};

	RECT rcClip = {0, 0, m_nSrcWndWidth, m_nSrcWndHeight};
 	m_textmapping.Do(&ptOffset, &rcClip);
}
