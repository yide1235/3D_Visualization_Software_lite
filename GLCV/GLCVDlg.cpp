
// GLCVDlg.cpp
//

#include "pch.h"
#include "framework.h"
#include "GLCV.h"
#include "GLCVDlg.h"
#include "afxdialogex.h"
#include <vtkWin32OpenGLRenderWindow.h>  
#include <vtkWin32RenderWindowInteractor.h> 
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include <string>
#include <pcl/common/common.h>
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include <pcl/kdtree/kdtree_flann.h>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>


#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Point_set_3.h>
#include <CGAL/Point_set_3/IO.h>
#include <CGAL/remove_outliers.h>
#include <CGAL/grid_simplify_point_set.h>
#include <CGAL/jet_smooth_point_set.h>
#include <CGAL/jet_estimate_normals.h>
#include <CGAL/mst_orient_normals.h>
#include <CGAL/poisson_surface_reconstruction.h>
#include <CGAL/Advancing_front_surface_reconstruction.h>
#include <CGAL/Scale_space_surface_reconstruction_3.h>
#include <CGAL/Scale_space_reconstruction_3/Jet_smoother.h>
#include <CGAL/Scale_space_reconstruction_3/Advancing_front_mesher.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Polygon_mesh_processing/polygon_soup_to_polygon_mesh.h>
#include <opencv2/calib3d.hpp>

typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef Kernel::FT FT;
typedef Kernel::Point_3 Point_3;
typedef Kernel::Vector_3 Vector_3;
typedef Kernel::Sphere_3 Sphere_3;
//typedef CGAL::Point_set_3<Point_3, Vector_3> Point_set;
typedef CGAL::Point_set_3<Point_3> Point_set;
std::string UnicodeToANSI(const std::wstring& str)

{
	char* pElementText;
	int    iTextLen;

	//wide char to multi char
	iTextLen = WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, NULL, 0, NULL, NULL);

	pElementText = new char[iTextLen + 1];

	memset((void*)pElementText, 0, sizeof(char) * (iTextLen + 1));

	::WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, pElementText, iTextLen, NULL, NULL);

	std::string strText;

	strText = pElementText;

	delete[] pElementText;

	return strText;

}




class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();


#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    


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






CGLCVDlg::CGLCVDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_GLCV_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGLCVDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GLWINDOW, m_vtk);

}

BEGIN_MESSAGE_MAP(CGLCVDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_OPENOBJ, &CGLCVDlg::OnBnClickedOpenobj)




	ON_BN_CLICKED(ID_SHOWColor, &CGLCVDlg::OnBnClickedShowcolor)
	//ON_EN_CHANGE(IDC_EDIT1, &CGLCVDlg::OnEnChangeEdit1)

	ON_BN_CLICKED(ID_LOADCSV, &CGLCVDlg::OnBnClickedLoadcsv)
	//ON_BN_CLICKED(ID_OpenImage, &CGLCVDlg::OnBnClickedOpenimage)
	//ON_EN_CHANGE(IDC_EDIT2, &CGLCVDlg::OnEnChangeEdit2)
	ON_STN_CLICKED(IDC_GLWINDOW, &CGLCVDlg::OnStnClickedGlwindow)
END_MESSAGE_MAP()


// CGLCVDlg 

BOOL CGLCVDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 

	
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

	// 

	SetIcon(m_hIcon, TRUE);			
	SetIcon(m_hIcon, FALSE);		

	
	_max_height = 0;
	_min_height = 0;
	return TRUE;  
}

void CGLCVDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// using mfc



void CGLCVDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// make it centered
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}



}

//minimize mouse

HCURSOR CGLCVDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

bool CGLCVDlg::ReadObj(const char* path)
{
	rawPt.clear();
	rawVt.clear();
	rawFace.clear();
	FILE* fp = fopen(path, "rt");
	if (path == NULL || fp == NULL)
	{
		return false;
	}

	char dst[128];
	float x, y, z;
	int vstart = 0;
	int vend = 0;
	int vtstart = 0;
	int vtend = 0;
	int fstart = 0;
	int fend = 0;
	int fcount = 0;
	//find v
	while (fgets(dst, 128, fp) != NULL)
	{

		if (dst[0] == 'v' && dst[1] != 't' && dst[1] != 'n')
		{
			break;
		}
		//std::cout << dst[0] << '\t' << dst[1] << '\n';
		vstart++;
	}
	vend = vstart;
	while (fgets(dst, 128, fp) != NULL)
	{
		if (vend==vstart)
		{
			

		}
		if (dst[0] == 'v' && dst[1] == 't')
		{
			break;
		}
		if (dst[0] == 'v' && dst[1] == 'n')
		{
			break;
		}
		if (dst[0] != 'v')
		{
			break;
		}
		vend++;
	}

	
	fseek(fp, 0, 0);
	//find vt
	while (fgets(dst, 128, fp) != NULL)
	{

		if (dst[0] == 'v' && dst[1] == 't')
		{
			break;
		}
		vtstart++;
	}

	vtend = vtstart;
	while (fgets(dst, 128, fp) != NULL)
	{
		if (dst[0] == 'v' && dst[1] != 't')
		{
			break;
		}
		if (dst[0] != 'v')
		{
			break;
		}
		vtend++;
	}
	fseek(fp, 0, 0);
	//find f

	while (fgets(dst, 128, fp) != NULL)
	{

		if (dst[0] == 'f' && dst[1] == ' ')
		{
			break;
		}
		fstart++;
	}

	fend = fstart;
	while (fgets(dst, 128, fp) != NULL)
	{
		if (fend == fstart)
		{
			for (size_t i = 0; i < strlen(dst); i++)
			{
				if (dst[i] == '/')
				{
					fcount++;
				}

			}
		}
		if (dst[0] == 'f' && dst[1] != ' ')
		{
			
			break;
		}
		if (dst[0] != 'f')
		{
			break;
		}
		fend++;
	}
	fseek(fp, 0, 0);

	if (vstart<vend)
	{
		for (size_t i = 0; i < vstart; i++)
		{
			fgets(dst, 128, fp) != NULL;
		}
		for (size_t i = vstart; i <= vend; i++)
		{
			fscanf(fp, "%s %f %f %f\n", dst, &x, &y, &z);
			rawPt.push_back(myPoint3f(x, y, z));
		}
		fseek(fp, 0, 0);
	}
	
	if (vtstart<vtend)
	{
		for (size_t i = 0; i < vtstart; i++)
		{
			fgets(dst, 128, fp) != NULL;
		}
		for (size_t i = vtstart; i <= vtend; i++)
		{
			fscanf(fp, "%s %f %f\n", dst, &x, &y);
			rawVt.push_back(myPoint2f(x, y));

		}
		fseek(fp, 0, 0);
	}
	if (fstart<fend)
	{
		int a = 0, b = 0, c = 0;

		for (size_t i = 0; i < fstart; i++)
		{
			fgets(dst, 128, fp) != NULL;
		}

		if (fcount == 0)
		{
			for (size_t i = fstart; i <= fend; i++)
			{
				fscanf(fp, "%s %d %d %d\n", dst, &a, &b, &c);
				rawFace.push_back(myPoint3i(a, b, c));
			}
		}
		if (fcount == 3)
		{
			int aa = 0, bb = 0, cc = 0;
			for (size_t i = fstart; i <= fend; i++)
			{
				fscanf(fp, "%s %d/%d %d/%d %d/%d\n", dst, &a, &aa, &b, &bb, &c, &cc);
				rawFace.push_back(myPoint3i(a, b, c));
			}
		}
		if (fcount == 6)
		{
			int aa = 0, bb = 0, cc = 0;
			int aaa = 0, bbb = 0, ccc = 0;
			
			for (size_t i = fstart; i <= fend; i++)
			{
				fscanf(fp, "%s %d/%d/%d %d/%d/%d %d/%d/%d\n", dst, &a, &aa, &aaa, &b, &bb, &bbb, &c, &cc, &ccc);
				rawFace.push_back(myPoint3i(a, b, c));
			}
			
		}
	}
	

	fclose(fp);

	return true;
}



void CGLCVDlg::OnBnClickedOpenobj()
{
	static TCHAR szFilter[] = _T("Chart Files (*.obj)|*.OBJ|");
	CFileDialog fileDlg(TRUE, NULL, NULL, AFX_IDS_OPENFILE|OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, szFilter);
	fileDlg.m_ofn.lpstrTitle = _T("open the obj model");  
	//fileDlg.m_ofn.lpstrFilter = _T("Text Files(*.txt)\0*.txt\0All Files(*.*)\0*.*\0\0");    //filtering out different file type
	fileDlg.m_ofn.lpstrDefExt = _T("obj");    
	

	if (IDOK == fileDlg.DoModal()) {
		
		m_fileName = fileDlg.GetFileName();
		//CFile file(fileDlg.GetPathName(), CFile::modeCreate | CFile::modeWrite);
		//char pBufWrite[] = "test:abcdefghijklmny";
		//file.Write(pBufWrite, strlen(pBufWrite));
		//file.Close();
		char path[256];
		WideCharToMultiByte(CP_ACP, 0, m_fileName.GetBuffer(), -1,path, 256, NULL, NULL);
		ReadObj(path);
	}
	//show vtk
	/*std::ifstream ifs("rvecs.txt");
	std::vector<cv::Mat> testMat(98);
	for (size_t i = 0; i < testMat.size(); i++)
	{
		testMat[i].create(3, 1, CV_64F);
		ifs>>testMat[i].at<double>(0, 0)
			>> testMat[i].at<double>(1, 0)
			>> testMat[i].at<double>(2, 0);
		cv::Rodrigues(testMat[i], testMat[i]);
		
		
	}
	ifs.close();*/
	try {
		// The operation that might throw an exception
		showBufferPt.resize(rawPt.size());
	}
	catch (const std::exception& e) {
		// Handle exception
		std::cerr << "Exception caught: " << e.what() << std::endl;
	}

	showBufferPt.resize(rawPt.size());
	showFace.resize(rawFace.size());
	for (size_t i = 0; i < rawPt.size(); i++)
	{
		showBufferPt[i].x = rawPt[i].x;
		showBufferPt[i].y = rawPt[i].y;
		showBufferPt[i].z = rawPt[i].z;
		showBufferPt[i].r = 1;
		showBufferPt[i].g = 0;
		showBufferPt[i].b = 0;
	}
	for (size_t i = 0; i < rawFace.size(); i++)
	{
		showFace[i].a = rawFace[i].x;
		showFace[i].b = rawFace[i].y;
		showFace[i].c = rawFace[i].z;
	}
	
	CRect pect;
	ptCloud.resize(rawPt.size());
	for (size_t i = 0; i < ptCloud.size(); i++)
	{
		ptCloud[i].x = rawPt[i].x;
		ptCloud[i].y = rawPt[i].y;
		ptCloud[i].z = rawPt[i].z;

	}
	ptFaceCenter.resize(showFace.size());
	for (size_t i = 0; i < ptFaceCenter.size(); i++)
	{
		if (showFace[i].a <=ptCloud.size()
			&& showFace[i].b <= ptCloud.size()
			&& showFace[i].c <= ptCloud.size())
		{
			ptFaceCenter[i].x = (ptCloud[showFace[i].a - 1].x
				+ ptCloud[showFace[i].b - 1].x
				+ ptCloud[showFace[i].c - 1].x) / 3;
			ptFaceCenter[i].y = (ptCloud[showFace[i].a - 1].y
				+ ptCloud[showFace[i].b - 1].y
				+ ptCloud[showFace[i].c - 1].y) / 3;
			ptFaceCenter[i].z = (ptCloud[showFace[i].a - 1].z
				+ ptCloud[showFace[i].b - 1].z
				+ ptCloud[showFace[i].c - 1].z) / 3;
		}
	}
	kdtree.setInputCloud(ptCloud.makeShared());
	kdface.setInputCloud(ptFaceCenter.makeShared());
	CWnd* pWnd = GetDlgItem(IDC_GLWINDOW);

	pWnd->GetClientRect(&pect);

	m_vtk.ReadPointAndFace(showBufferPt, showFace);
	m_vtk.MoveWindow(pect);
	m_vtk.ShowWindow(SW_SHOW);


	
}





void HSVtoRGB(unsigned char* r, unsigned char* g, unsigned char* b, int h, int s, int v)
{
	// convert from HSV/HSB to RGB color
	// R,G,B from 0-255, H from 0-260, S,V from 0-100
	// ref http://colorizer.org/

	// The hue (H) of a color refers to which pure color it resembles
	// The saturation (S) of a color describes how white the color is
	// The value (V) of a color, also called its lightness, describes how dark the color is
	float RGB_min, RGB_max;
	RGB_max = v * 2.55f;
	RGB_min = RGB_max * (100 - s) / 100.0f;

	int i = h / 60;
	int difs = h % 60; // factorial part of h

	// RGB adjustment amount by hue 
	float RGB_Adj = (RGB_max - RGB_min) * difs / 60.0f;

	switch (i) {
	case 0:
		*r = RGB_max;
		*g = RGB_min + RGB_Adj;
		*b = RGB_min;
		break;
	case 1:
		*r = RGB_max - RGB_Adj;
		*g = RGB_max;
		*b = RGB_min;
		break;
	case 2:
		*r = RGB_min;
		*g = RGB_max;
		*b = RGB_min + RGB_Adj;
		break;
	case 3:
		*r = RGB_min;
		*g = RGB_max - RGB_Adj;
		*b = RGB_max;
		break;
	case 4:
		*r = RGB_min + RGB_Adj;
		*g = RGB_min;
		*b = RGB_max;
		break;
	default:		// case 5:
		*r = RGB_max;
		*g = RGB_min;
		*b = RGB_max - RGB_Adj;
		break;
	}
}




void CGLCVDlg::OnBnClickedShowcolor()
{
	std::vector<float> tempz(showBufferPt.size());
	CRect pect;
	CWnd* pWnd = GetDlgItem(IDC_GLWINDOW);
	if (showBufferPt.size()>0)
	{


		if (_max_height==0&&_min_height==0)
		{
#pragma omp parallel for
			for (int64 i = 0; i < tempz.size(); i++)
			{
				tempz[i] = showBufferPt[i].z;
			}
			float tempMax, tempMin;
			tempMax = *std::max_element(tempz.begin(), tempz.end());
			tempMin = *std::min_element(tempz.begin(), tempz.end());
			std::vector<Pointf> showPt(showBufferPt.size());
#pragma omp parallel for
			for (int64 i = 0; i < tempz.size(); i++)
			{
				int h = 260 * (showBufferPt[i].z - tempMin) / (tempMax - tempMin);
				int s = 100;
				int v = 100;
				uchar r, g, b;
				HSVtoRGB(&r, &g, &b, h, s, v);
				showPt[i].x = showBufferPt[i].x;
				showPt[i].y = showBufferPt[i].y;
				showPt[i].z = showBufferPt[i].z;
				showPt[i].r = r / 255.0;
				showPt[i].g = g / 255.0;
				showPt[i].b = b / 255.0;
			}

			m_vtk.IsShowLut = true;
			m_vtk.SetLutHeight(tempMax, tempMin);
			pWnd->GetClientRect(&pect);
			m_vtk.ReadPointAndFace(showPt, showFace);
			m_vtk.MoveWindow(pect);
			m_vtk.ShowWindow(SW_SHOW);
			_max_height = tempMax;
			_min_height = tempMin;
			_now_max = tempMax;
			_now_min = tempMin;
			m_MaxHeight.SetRange(0, 100);
			m_MaxHeight.SetPos(0);
			m_MinHeight.SetRange(0, 100);
			m_MinHeight.SetPos(100);
			std::wstring ws;
			ws = std::to_wstring(tempMax);
			SetDlgItemText(IDC_EDIT1, ws.c_str());
			ws = std::to_wstring(tempMin);
			SetDlgItemText(IDC_EDIT2, ws.c_str());
		}
		


	}

	
}




void CGLCVDlg::OnBnClickedLoadcsv()
{

	static TCHAR szFilter[] = _T("image Files (*.csv)|*.csv|(*.txt)|*.txt||");
	CFileDialog fileDlg(TRUE, NULL, NULL, AFX_IDS_OPENFILE | OFN_HIDEREADONLY, szFilter);
	fileDlg.m_ofn.lpstrTitle = _T("open the .obj");    //
	//fileDlg.m_ofn.lpstrFilter = _T("Text Files(*.txt)\0*.txt\0All Files(*.*)\0*.*\0\0");    //
	fileDlg.m_ofn.lpstrDefExt = _T("csv");    //

	if (IDOK == fileDlg.DoModal()) {

		m_fileName = fileDlg.GetFolderPath() + CString(_T("\\")) + fileDlg.GetFileName();

		CRect pect;
		char path[256];
		WideCharToMultiByte(CP_ACP, 0, m_fileName.GetBuffer(), -1, path, 256, NULL, NULL);
		//char buffer[65536];
		std::ifstream ifs(path);
		std::vector<std::vector<char>> temp;
		while (!ifs.eof())
		{
			std::vector<char> length(65536);
			ifs.getline(length.data(), 65536);
			temp.push_back(length);
		}
		//count ,
		char buff[64];
		std::vector<std::vector<float>> map;
		for (size_t i = 0; i < temp.size(); i++)
		{
			int buffcount = 0;
			int datalen = strlen(temp[i].data());
			std::vector<float> line;
			for (size_t j = 0; j < datalen; j++)
			{
				if (temp[i][j] == ',')
				{
					line.push_back(std::atof(buff));
					buffcount = 0;
					memset(buff, 0, 64);
					continue;
				}
				buff[buffcount] = temp[i][j];
				buffcount++;
				if (j == datalen - 1)
				{
					line.push_back(std::atof(buff));
				}
			}
			map.push_back(line);
		}
		int maxline = map[0].size();
		for (size_t i = 0; i < map.size(); i++)
		{
			maxline = MAX(maxline, map[i].size());
		}


		cv::Mat mytiff(map.size(), maxline, CV_32F);
		memset(mytiff.data, 0, mytiff.size().height * mytiff.size().width * 4);
		for (size_t i = 0; i < map.size(); i++)
		{
			for (size_t j = 0; j < map[i].size(); j++)
			{
				mytiff.at<float>(i, j) = map[i][j];
			}
		}


		showBufferPt.reserve(mytiff.size().width * mytiff.size().height);
		int ptcount = 0;
		CWnd* pWnd = GetDlgItem(IDC_GLWINDOW);//
		for (size_t i = 0; i < mytiff.size().height; i=i+1)
		{
			for (size_t j = 0; j < mytiff.size().width; j=j+1)
			{
				Pointf tempPt;
				tempPt.x = j * 2;
				tempPt.y = i * 2;
				tempPt.z = mytiff.at<float>(i,j)+1;
				tempPt.r = 0.75;
				tempPt.g = 0.75;
				tempPt.b = 0.75;
				showBufferPt.push_back(tempPt);
				//ptcount++;
			}
		}

		//cv::imwrite("debug.tiff",mytiff);


		Point_set points;
		for (size_t i = 0; i < showBufferPt.size(); i++)
		{
			points.insert(Point_3(showBufferPt[i].x, showBufferPt[i].y, showBufferPt[i].z));
		}
		typedef std::array<std::size_t, 3> Facet;
		std::vector<Facet> facets;
		// The function is called using directly the points raw iterators
		CGAL::advancing_front_surface_reconstruction(points.points().begin(),
			points.points().end(),
			std::back_inserter(facets), 20.0, 0.95);
		showFace.reserve(facets.size());
		for (size_t i = 0; i < facets.size(); i++)
		{
			if (facets[i][0] >= showBufferPt.size()
				&& facets[i][1] >= showBufferPt.size()
				&& facets[i][2] >= showBufferPt.size())
			{
				continue;
			}
			facev face;
			face.a = facets[i][0] + 1;
			face.b = facets[i][1] + 1;
			face.c = facets[i][2] + 1;
			showFace.push_back(face);

		}

		pWnd->GetClientRect(&pect);
		m_vtk.ReadPointAndFace(showBufferPt,showFace);
		m_vtk.MoveWindow(pect);
		m_vtk.ShowWindow(SW_SHOW);
	

	}
	
}





void CGLCVDlg::OnStnClickedGlwindow()
{
	// TODO: Add your control notification handler code here
}
