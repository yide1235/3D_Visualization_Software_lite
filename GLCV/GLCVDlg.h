
// GLCVDlg.h
//

#pragma once
#include <vector>
#include "CVtkViewer.h"
#include <pcl/common/common.h>
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include <pcl/kdtree/kdtree_flann.h>
#include <opencv2/core.hpp>
// CGLCVDlg 
class CGLCVDlg : public CDialogEx
{
//
public:
	CGLCVDlg(CWnd* pParent = nullptr);	


#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GLCV_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	


// 
protected:
	HICON m_hIcon;

	cv::Mat colorMap;
	cv::Mat depthMap;
	float _max_height;
	float _min_height;
	float _now_max;
	float _now_min;
	CString m_fileName;
	// 
	std::vector<Pointf> showBufferPt;
	std::vector<facev> showFace;
	std::vector<myPoint3f> rawPt;
	std::vector<myPoint2f> rawVt;
	std::vector<myPoint3i> rawFace;
	pcl::KdTreeFLANN< pcl::PointXYZ> kdtree;
	pcl::KdTreeFLANN< pcl::PointXYZ> kdface;
	pcl::PointCloud<pcl::PointXYZ> ptCloud;
	pcl::PointCloud<pcl::PointXYZ> ptFaceCenter;
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	bool ReadObj(const char* path);
	afx_msg void OnBnClickedOpenobj();
	CVtkViewer m_vtk;

	//afx_msg void OnBnClickedClear();


	afx_msg void OnBnClickedShowcolor();
	//afx_msg void OnEnChangeEdit1();
	CSliderCtrl m_MaxHeight;
	CSliderCtrl m_MinHeight;
	afx_msg void OnNMCustomdrawSlider2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMCustomdrawSlider3(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedLoadcsv();
	//afx_msg void OnBnClickedOpenimage();
	//afx_msg void OnEnChangeEdit2();
	afx_msg void OnStnClickedGlwindow();
};
