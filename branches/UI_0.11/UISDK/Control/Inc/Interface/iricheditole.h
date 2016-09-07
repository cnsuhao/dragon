#ifndef UI_IRICHEDITOLE_H_201411071253
#define UI_IRICHEDITOLE_H_201411071253

namespace UI
{
class REOleBase;
class PictureOle;

enum REOLETYPE
{
    REOLE_INVALID,
    REOLE_COM,
    REOLE_PICTURE,
    REOLE_GIF,
};

interface  IREOleExternalData
{
    virtual void  Release() PURE;
};
interface UICTRLAPI IREOleBase
{
public:
    IREOleBase(REOleBase* p);

    long  GetOleType();
    void  SetExternalData(IREOleExternalData*);
    IREOleExternalData*  GetExternalData();

private:
    REOleBase*  m_pImplBase;
};


enum REOleLoadStatus
{
    REOleLoadStatus_NOT_INITIALIZE =-1,  // δ��ʼ�������ó��ֵ�״̬
    REOleLoadStatus_LOADING = 0,
    REOleLoadStatus_SUCCESS,
    REOleLoadStatus_OVERDUE,
    REOleLoadStatus_FAILURE,
};
struct PictureREOleInfo
{
    long lReverse;

    LPCTSTR  szId;
    LPCTSTR  szName;
    LPCTSTR  szPath;
    LPCTSTR  szDesc;
    LPCTSTR  szType;
    REOleLoadStatus  eStatus;
};

// �����ͼƬ�ϵķŴ󾵰�ť
// wParam: IPictureREOle
#define UI_REOLE_PICTURE_MAGNIFIER_BTN_CLICK  141421315

// ˫����ͼƬ
// wParam: IPictureREOle
#define UI_REOLE_PICTURE_DBCLICK  141421316

interface UICTRLAPI IPictureREOle : public IREOleBase
{
public:
    IPictureREOle(PictureOle*);
    LPCTSTR  GetPath();
    void  UpdateStatus(REOleLoadStatus e, LPCTSTR szPath=NULL);

private:
    PictureOle*  m_pImpl;
};


}


#endif // UI_IRICHEDITOLE_H_201411071253