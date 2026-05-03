
// AliceHtmlMergeDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "AliceHtmlMerge.h"
#include "AliceHtmlMergeDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include <string>
#include <string_view>
#include <thread>


#define WM_BOOK_UPDATE_MESSAGE				(WM_USER + 100)

const std::string g_str2ChineseBlankSpaces = u8"　　";


// UTF-8 string 转 wstring
std::wstring utf8_to_wstring(const std::string& utf8)
{
	if (utf8.empty())
		return {};

	int len = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);

	std::wstring wstr(len, 0);

	MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &wstr[0], len);
	if (!wstr.empty() && wstr.back() == 0)
		wstr.pop_back();

	return wstr;
}

std::string wstring_to_utf8(const std::wstring& wstr) {
	if (wstr.empty()) return "";

	int utf8_length = WideCharToMultiByte(
		CP_UTF8,
		0,
		wstr.c_str(),
		-1,
		nullptr,
		0,
		nullptr,
		nullptr
	);

	if (utf8_length == 0) {
		return "";
	}

	std::string utf8_str(utf8_length, 0);
	WideCharToMultiByte(
		CP_UTF8,
		0,
		wstr.c_str(),
		-1,
		&utf8_str[0],
		utf8_length,
		nullptr,
		nullptr
	);

	// 移除末尾的null字符
	if (!utf8_str.empty() && utf8_str.back() == '\0') {
		utf8_str.pop_back();
	}

	return utf8_str;
}


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CAliceHtmlMergeDlg 对话框



CAliceHtmlMergeDlg::CAliceHtmlMergeDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ALICEHTMLMERGE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAliceHtmlMergeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_BOOKS, m_listBooks);
}

BEGIN_MESSAGE_MAP(CAliceHtmlMergeDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CAliceHtmlMergeDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH_BOOK_LIST, &CAliceHtmlMergeDlg::OnBnClickedButtonRefreshBookList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_BOOKS, &CAliceHtmlMergeDlg::OnItemClick)
	ON_MESSAGE(WM_BOOK_UPDATE_MESSAGE, OnBookUpdateMessage)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_ALL_BOOKS, &CAliceHtmlMergeDlg::OnBnClickedButtonSelectAllBooks)
END_MESSAGE_MAP()


// CAliceHtmlMergeDlg 消息处理程序

BOOL CAliceHtmlMergeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	GetDlgItem(IDC_EDIT_DIR)->SetWindowText(_T("D:\\alicesw_books"));

	// 设置列表控件扩展样式
	DWORD dwExStyle = LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES;
	m_listBooks.SetExtendedStyle(dwExStyle);

	// 创建3个列
	m_listBooks.InsertColumn(0, _T("书籍文件夹"), LVCFMT_LEFT, 300);
	m_listBooks.InsertColumn(1, _T("已生成txt文件"), LVCFMT_LEFT, 100);
	m_listBooks.InsertColumn(2, _T("状态"), LVCFMT_LEFT, 500);


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CAliceHtmlMergeDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CAliceHtmlMergeDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CAliceHtmlMergeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CAliceHtmlMergeDlg::OnBnClickedOk()
{
	std::thread t(&CAliceHtmlMergeDlg::MergeImpl, this);
	t.detach();
}

void CAliceHtmlMergeDlg::AddChapterTitle(std::ofstream& file, int nChapterIndex, std::string& line, Book& book)
{
	auto start = line.find_first_of(">");
	auto end = line.find_last_of("<");
	auto title = line.substr(start + 1, end - start - 1);

	file << title << std::endl << std::endl;

	book.cstrStatus.Format(_T("提取章节: %s"), utf8_to_wstring(title).c_str());
	PostMessage(WM_BOOK_UPDATE_MESSAGE, 0, (LPARAM)(&book));
}

void CAliceHtmlMergeDlg::AddChapterText(std::ofstream& file, int nChapterIndex, std::string& line)
{
	// NOTE: line format is like:
	// ...<p>Paragragh1<p>Paragragh1<p>...<p>ParagraghN</p>...
	// Extract all the paragraghs


	std::string_view svHtmlChapterText = line;

	while (true)
	{
		bool bLastText = false;

		// find paragragh start <p>
		auto posParagraghStartWithP = svHtmlChapterText.find("<p>");

		auto svParagraph = svHtmlChapterText.substr(posParagraghStartWithP + 3);

		// find paragragh end <p> or </p>
		auto posParagraghEnd = svParagraph.find("<p>");
		if (posParagraghEnd != std::string::npos)
		{
			// make view to next text start <p>
			svHtmlChapterText = svParagraph.substr(posParagraghEnd);

			svParagraph = svParagraph.substr(0, posParagraghEnd);
		}
		else
		{
			// this means get to end, need to find </p>
			posParagraghEnd = svParagraph.find("</p>");
			if (posParagraghEnd != std::string::npos)
			{
				svParagraph = svParagraph.substr(0, posParagraghEnd);
			}
			else
			{
				// In case there is no end </p>
				ASSERT(false);
			}

			bLastText = true;
		}

		file << svParagraph << std::endl;

		if (bLastText)
		{
			break;
		}
	}

	file << std::endl << std::endl;
}

void CAliceHtmlMergeDlg::OnBnClickedButtonRefreshBookList()
{
	m_bInsertListDone = false;
	GetBooks();
	UpdateBookList();
	m_bInsertListDone = true;
}

void CAliceHtmlMergeDlg::GetBooks()
{
	m_vtrBooks.clear();

	GetDlgItem(IDC_EDIT_DIR)->GetWindowText(m_cstrDir);
	
	if (m_cstrDir.GetLength() == 0)
	{
		AfxMessageBox(_T("请输入书籍根目录！"));
		return;
	}

	// 查找所有书籍目录

	WIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFile((LPCTSTR)(m_cstrDir + _T("\\*.html")), &findFileData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		DWORD err = GetLastError();
		if (err == ERROR_NO_MORE_FILES)
		{
			AfxMessageBox(_T("该目录下没有找到.html 文件\n"));
		}
		else
		{
			AfxMessageBox(_T("无效目录！"));
		}
		return;
	}

	do {
		if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			CString cstrHtmlFileName = findFileData.cFileName;
			// Get file name without .html
			CString cstrFileName = cstrHtmlFileName.Left(cstrHtmlFileName.GetLength() - 5);

			// 添加HTML
			m_vtrBooks.push_back({ false, cstrFileName, false, _T("") });
		}

	} while (FindNextFile(hFind, &findFileData) != 0);

	// 确认所有书籍目录是否已有对应txt文件
	for (auto it = m_vtrBooks.begin(); it != m_vtrBooks.end(); ++it)
	{
		WIN32_FIND_DATA findFileData;
		HANDLE hFind = FindFirstFile((LPCTSTR)(m_cstrDir + _T("\\") + it->cstrBookName + _T(".txt")), &findFileData);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			it->bTxtReady = true;
		}
		else
		{
			it->bSelected = true;
		}
	}

}

void CAliceHtmlMergeDlg::UpdateBookList()
{
	m_listBooks.DeleteAllItems();

	for (auto it = m_vtrBooks.begin(); it != m_vtrBooks.end(); ++it)
	{
		int nIndex = m_listBooks.InsertItem(0, it->cstrBookName);
		if (it->bTxtReady)
		{
			m_listBooks.SetItemText(nIndex, 1, _T("√"));
		}
		else
		{
			m_listBooks.SetCheck(nIndex, TRUE);
		}

		m_listBooks.SetItemData(nIndex, (DWORD_PTR)(&(*it)));
	}
}
void CAliceHtmlMergeDlg::OnItemClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	// 检查是否是状态改变事件
	if ((pNMLV->uChanged & LVIF_STATE) && (pNMLV->uNewState & LVIS_STATEIMAGEMASK))
	{
		if (m_bInsertListDone)
		{
			int nIndex = pNMLV->iItem;
			Book* pBook = (Book*)m_listBooks.GetItemData(nIndex);
			pBook->bSelected = m_listBooks.GetCheck(nIndex);
		}
	}

	*pResult = 0;
}

void CAliceHtmlMergeDlg::MergeImpl()
{
	for (auto it = m_vtrBooks.begin(); it != m_vtrBooks.end(); ++it)
	{
		if (it->bSelected)
		{
			 MergeBook(*it);
		}
	}
}

void CAliceHtmlMergeDlg::MergeBook(Book& book)
{
	CString cstrOutputFile = m_cstrDir + _T("\\") + book.cstrBookName + _T(".txt");
	std::ofstream ofOutputFile((LPCTSTR)cstrOutputFile, std::ios::binary);
	if (!ofOutputFile.is_open()) {
		book.cstrStatus = _T("创建txt文件失败");
		PostMessage(WM_BOOK_UPDATE_MESSAGE, 0, (LPARAM)(&book));
		return;
	}

	book.bTxtReady = true;
	book.cstrStatus = _T("txt文件已创建");
	PostMessage(WM_BOOK_UPDATE_MESSAGE, 0, (LPARAM)(&book));

	// Write BOM
	std::vector<unsigned char> bom = { 0xEF, 0xBB, 0xBF };
	ofOutputFile.write(reinterpret_cast<const char*>(bom.data()), bom.size());

	// Write book name
	ofOutputFile << wstring_to_utf8((LPCTSTR)book.cstrBookName) << std::endl << std::endl << std::endl;


	CString cstrFullHtmlName = m_cstrDir + _T("\\") + book.cstrBookName + _T(".html");

	book.cstrStatus.Format(_T("开始处理文件: %s"), (LPCTSTR)cstrFullHtmlName);
	PostMessage(WM_BOOK_UPDATE_MESSAGE, 0, (LPARAM)(&book));


	WIN32_FIND_DATA nextInfo;
	HANDLE handle = FindFirstFile((LPCTSTR)cstrFullHtmlName, &nextInfo);
	if (handle == INVALID_HANDLE_VALUE)
	{
		book.cstrStatus.Format(_T("未找到文件: %s，提取结束"), (LPCTSTR)cstrFullHtmlName);
		PostMessage(WM_BOOK_UPDATE_MESSAGE, 0, (LPARAM)(&book));
		return;
	}

	// read file
	std::ifstream inFile((LPCTSTR)cstrFullHtmlName, std::ios::in | std::ios::binary);

	if (!inFile.is_open())
	{
		book.cstrStatus.Format(_T("无法打开文件: %s"), (LPCTSTR)cstrFullHtmlName);
		PostMessage(WM_BOOK_UPDATE_MESSAGE, 0, (LPARAM)(&book));
		return;
	}

	int nChapterCount = 0;

	std::string line;
	while (std::getline(inFile, line))
	{
		if (line.find("j_chapterName") != std::string::npos)
		{
			nChapterCount++;
			AddChapterTitle(ofOutputFile, 0, line, book);
			continue;
		}

		if (line.find("read-content j_readContent user_ad_content") != std::string::npos)
		{
			AddChapterText(ofOutputFile, 0, line);
			continue;
		}

	}

	CString cstrChapterCountInfo;
	cstrChapterCountInfo.Format(_T("。提取全部完成，共提取【 %d 】章"), nChapterCount);
	book.cstrStatus += cstrChapterCountInfo;
	PostMessage(WM_BOOK_UPDATE_MESSAGE, 0, (LPARAM)(&book));

	inFile.close();
	ofOutputFile.close();
}

LRESULT CAliceHtmlMergeDlg::OnBookUpdateMessage(WPARAM wParam, LPARAM lParam)
{
	// 在这里处理线程发送的消息
	Book* pBook = reinterpret_cast<Book*>(lParam);
	if (pBook)
	{
		for (int nIndex = 0; nIndex < m_listBooks.GetItemCount(); nIndex++)
		{
			if (lParam == m_listBooks.GetItemData(nIndex))
			{
				m_listBooks.SetItemText(nIndex, 1, pBook->bTxtReady ? _T("√") : _T(""));
				m_listBooks.SetItemText(nIndex, 2, pBook->cstrStatus);
			}
		}
	}

	return 0;
}

void CAliceHtmlMergeDlg::OnBnClickedButtonSelectAllBooks()
{
	// Note: no need to set book.bSelected in m_vtrBooks, it will be updated in OnItemClick() when
	// m_listBooks.SetCheck(nIndex, true) is called
	for (int nIndex = 0; nIndex < m_listBooks.GetItemCount(); nIndex++)
	{
		m_listBooks.SetCheck(nIndex, true);
	}
}
