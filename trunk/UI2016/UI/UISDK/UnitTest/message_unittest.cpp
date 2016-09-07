#include "stdafx.h"
#ifdef UNITTEST
#include "Src/Base/Message/message.h"

void main()
{
	IObject* pObj = ObjectCreator<IObject>::CreateInstance(NULL);
	pObj->Release();
}
#endif