#pragma once
#pragma once
#include "afxwin.h"

#include <vtkResliceCursor.h>  
#include <vtkResliceCursorWidget.h>  

#include <vtkPlane.h>  
#include <vtkPlaneSource.h>  
#include <vtkPlaneWidget.h>  

#include <vtkImagePlaneWidget.h>  
#include <vtkResliceCursorThickLineRepresentation.h>  
#include <vtkResliceCursor.h>  

#include <vtkCommand.h>  
#include <vtkViewport.h>  
#include <vtkViewDependentErrorMetric.h>  

#include <vtkSmartPointer.h>  

#include <vtkRenderer.h>  
#include <vtkRendererSource.h>  
#include <vtkRenderingOpenGL2Module.h>  
#include <vtkRenderWindow.h>  
#include <vtkWin32OpenGLRenderWindow.h>  
#include <vtkWin32RenderWindowInteractor.h> 

#include <vtkPolyVertex.h>
#include <vtkUnstructuredGrid.h>
#include <vtkDataSetMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkCamera.h>
#include <vtkLookupTable.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>

#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkInteractionStyle)
VTK_MODULE_INIT(vtkRenderingFreeType)


struct myPoint3i
{
	int x;
	int y;
	int z;
	myPoint3i(int a, int b, int c) { x = a; y = b; z = c; }
	myPoint3i() { x = 0, y = 0, z = 0; }
};

struct myPoint3f
{
	float x;
	float y;
	float z;
	myPoint3f(float a, float b, float c) { x = a; y = b; z = c; }
	myPoint3f(myPoint3f& src) { x = src.x;y = src.y;z = src.z; }
	myPoint3f() { x = 0, y = 0, z = 0; }
};
struct myPoint2f
{
	float x;
	float y;

	myPoint2f(float a, float b) { x = a; y = b; }
	myPoint2f() { x = 0, y = 0; }
};
using Pointf = struct _Pointf {
	float x;
	float y;
	float z;
	float r;
	float g;
	float b;
};
using facev = struct _facev {
	int a;
	int b;
	int c;
};
class HighlightInteractorStyle;
class MouseInteractorStyle;
class CVtkViewer : public CStatic
{
	DECLARE_DYNAMIC(CVtkViewer)
	
public:
	bool IsShowLut;
	
	CVtkViewer();
	virtual ~CVtkViewer();
	void ReadPointAndFace(std::vector<Pointf>& pointcloud, std::vector<facev>& myface,  int count = 0);
	vtkSmartPointer<vtkActor> actor;
	void ReadPointCloud(std::vector<Pointf>&);
	vtkSmartPointer< HighlightInteractorStyle> m_InterAct;
	vtkSmartPointer<MouseInteractorStyle> m_Mouse;
public:

	void PreSubclassWindow();
	void SetImageData(vtkSmartPointer<vtkImageData> ImageData);
	void SetupReslice();
	void MoveWindow(CRect);
	bool GetCapPoint(std::vector<myPoint3f>&dst);
	bool Clear();
	bool SetLutHeight(float _max, float _min) { _lut_max = _max, _lut_min = _min; return true; }
private:
	vtkSmartPointer< vtkImagePlaneWidget >   m_ImagePlaneWidget;
	vtkSmartPointer< vtkResliceCursorWidget> m_ResliceCursorWidget;
	vtkSmartPointer< vtkResliceCursor >      m_ResliceCursor;
	vtkSmartPointer< vtkResliceCursorThickLineRepresentation > m_ResliceCursorRep;
	vtkSmartPointer<vtkPolyData>    m_grid;
	vtkSmartPointer<vtkRenderer>         m_Renderer;
	vtkSmartPointer<vtkRenderWindow> m_RenderWindow;
	vtkSmartPointer<vtkImageData>       m_ImageData;
	


	int m_Direction;

protected:
	float _lut_max, _lut_min;
	DECLARE_MESSAGE_MAP()
};