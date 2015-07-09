#pragma once

struct RecentProjItem
{
	TCHAR   szFilePath[MAX_PATH];
	TCHAR   szFileName[MAX_PATH];
	long    nLastVisitTime;

	friend bool operator <(const RecentProjItem& o1, const RecentProjItem& o2);
	friend bool operator >(const RecentProjItem& o1, const RecentProjItem& o2);
	
};


class CRecentProjList
{
public:
	CRecentProjList(void);
	~CRecentProjList(void);

	bool   Add(const TCHAR* szProjPath);
	bool   Remove(const TCHAR* szProjPath);
	bool   Load();
	int    GetItemCount() { return (int)m_arrRecentProj.size(); }
	RecentProjItem* GetItemByIndex(int n);

protected:
	void   Clear();
	RecentProjItem*   GetItem(const TCHAR* szProjPath);
	bool   Save();

public:
	vector<RecentProjItem*>  m_arrRecentProj;
};
