#pragma once
class CPropertyDialog;

// ���Դ��ڿ���������ʾ�������ԡ�ÿ��������Ҫע���Լ���handler

enum PROPERTY_COMMAND_TYPE
{
    PROPERTY_COMMAND_UNKNOWN,
    PROPERTY_COMMAND_INSERTAFTER,
    PROPERTY_COMMAND_INSERTBEFORE,
    PROPERTY_COMMAND_ADD,
    PROPERTY_COMMAND_DELETE,
    PROPERTY_COMMAND_CLEAR,
    PROPERTY_COMMAND_MODIFY
};

interface  IPropertyHandler : public IMessage
{
	virtual long  GetHandlerType() PURE;
    virtual void  OnLoad(CPropertyDialog* pWnd) PURE;
    virtual void  OnUnload() PURE;

};