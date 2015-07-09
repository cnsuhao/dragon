#ifndef ISKINDATASOURCE_H_1BE75159_FE4C_48cc_980A_BD613C7DA944
#define ISKINDATASOURCE_H_1BE75159_FE4C_48cc_980A_BD613C7DA944

namespace Gdiplus { class Bitmap; }
namespace UI
{

enum  SKIN_PACKET_TYPE
{
    SKIN_PACKET_TYPE_DIR,
    SKIN_PACKET_TYPE_ZIP,
};

// 跨模块加载gdiplus::bitmap时使用。确保Bitmap*都在同一个模块new/delete
class UISDKAPI GdiplusBitmapLoadWrap
{
public:
    GdiplusBitmapLoadWrap();
    ~GdiplusBitmapLoadWrap();
    Gdiplus::Bitmap*  GetBitmap();

    void  Destroy();
    bool  LoadFromFile(LPCTSTR szPath);
    bool  LoadFromByte(LPBYTE pByte, int nSize);

private:
    Gdiplus::Bitmap*  m_pBitmap;
};

interface ImageWrap;
interface IStreamBufferReader;
interface ISkinDataSource : public IRootInterface
{
    virtual void  Release() = 0;
    virtual void  SetDirection(LPCTSTR szDir) = 0;
    virtual void  SetName(LPCTSTR szName) = 0;

    virtual LPCTSTR  GetDirection() = 0;
    virtual LPCTSTR  GetName() = 0;
    virtual SKIN_PACKET_TYPE  GetType() = 0;

    virtual bool  Load_UIDocument(IUIDocument* pDocument, LPCTSTR szPath) = 0;
    virtual bool  Load_RenderBitmap(IRenderBitmap* pBitmap, LPCTSTR szPath, bool bCreateAlphaChannel) = 0;
    virtual bool  Load_Image(ImageWrap* pImage, LPCTSTR szPath) = 0;
    virtual bool  Load_StreamBuffer(LPCTSTR szPath, IStreamBufferReader** pp) = 0;
    virtual bool  Load_GdiplusImage(LPCTSTR szPath, GdiplusBitmapLoadWrap* pImage) = 0;
};


enum  STREAM_TYPE
{
    STREAM_TYPE_FILE,
    STREAM_TYPE_BYTE,
};

interface IStreamBufferReader
{
    virtual void  Release() = 0;
    virtual int   read(char* pread, int nread) = 0;
    virtual void  seek(int npos, ios_base::seek_dir dir) = 0;
    virtual int   tell() = 0;
    virtual bool  save(LPCTSTR szPath) = 0;
};

interface IFileBufferReader : public IStreamBufferReader
{
    virtual bool  load(LPCTSTR szPath) = 0;
};
interface IByteBufferReader : public IStreamBufferReader
{
    virtual bool  load(const byte* pdata, int nsize, bool bdeletedata) = 0;
};

}

#endif // ISKINDATASOURCE_H_1BE75159_FE4C_48cc_980A_BD613C7DA944