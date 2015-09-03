#pragma  once


// Ϊ��ʵ�ֲ�����IRenderResource�ֻ࣬��¶���ӿڣ���IRenderResource��һЩʵ�ֲ���ģ������ʵ��
namespace UI
{
interface IRenderResource;

template<class T>
class IRenderResourceImpl : public T
{
protected:
    IRenderResourceImpl()
    {
        m_dwRef = 0;
        m_ppOutRef = NULL;
    }

public:
    virtual ~IRenderResourceImpl()
    {
        if (m_ppOutRef!=NULL)
        {
            *m_ppOutRef = NULL;
            m_ppOutRef = NULL;
        }
    }
    //virtual GRAPHICS_RENDER_LIBRARY_TYPE GetGraphicsRenderLibraryType() = 0;

    void   SetOutRef(IRenderResource** ppOutRef) { m_ppOutRef = ppOutRef; }

    long   AddRef()
    {
        m_dwRef ++;
        return m_dwRef;
    }
    long   Release()
    {
        m_dwRef--;
        if (0 >= m_dwRef)
        {
            assert(m_dwRef==0);  // �п��ܵ���������addref��ֱ��release��

            delete this;
            return 0;
        }

        return m_dwRef;
    }

protected:
    long   m_dwRef;
    IRenderResource**   m_ppOutRef;  // �ⲿ���ã����ڴ���������release�Զ��ͷ�
    // Ϊ�˽����Ҫ��CImageItem�б���һ��Image*ָ�룬Ҫ�ⲿGetʱ��ֵ������������UIImage*�ֲ��ܸ����������á�
    // �������ⲿ��Release������CImageItem::Image*ȴ�޷��ÿգ��������������һ��m_pOutRef��ָ�����
    // Image*�������������н����ÿ�
};
}