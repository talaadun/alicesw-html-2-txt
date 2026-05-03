
// AliceHtmlMergeDlg.h: 头文件
//

#pragma once

#include <fstream>
#include <vector>


typedef struct _Book
{
	bool		bSelected;
	CString		cstrBookName;
	bool		bTxtReady;
	CString		cstrStatus;
}Book;

// CAliceHtmlMergeDlg 对话框
class CAliceHtmlMergeDlg : public CDialogEx
{
// 构造
public:
	CAliceHtmlMergeDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ALICEHTMLMERGE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnItemClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnBookUpdateMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonSelectAllBooks();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();

private:
	void AddChapterTitle(std::ofstream& file, int nChapterIndex, std::string& line, Book& book);
	void AddChapterText(std::ofstream& file, int nChapterIndex, std::string& line);

	void GetBooks();
	void UpdateBookList();

	void MergeImpl();
	void MergeBook(Book& book);

public:
	CListCtrl m_listBooks;
	afx_msg void OnBnClickedButtonRefreshBookList();

private:
	bool				m_bInsertListDone;
	CString				m_cstrDir;
	std::vector<Book>	m_vtrBooks;
};
