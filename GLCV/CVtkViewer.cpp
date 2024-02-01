#include "pch.h"  
#include "CVtkViewer.h"  
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>

#include <vtkActor.h>
#include <vtkAreaPicker.h>
#include <vtkDataSetMapper.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkExtractPolyDataGeometry.h>
#include <vtkIdFilter.h>
#include <vtkIdTypeArray.h>
#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPlanes.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkUnstructuredGrid.h>
#include <vtkVersion.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkCellPicker.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkSelection.h>
#include <vtkExtractSelection.h>
#include <vtkSelectionNode.h>
#include <vtkVectorText.h>
#include <vtkTextActor.h>
#include <vtkTransformFilter.h>
#include <vtkAppendPolyData.h>
#include <vtkTextProperty.h>
#include <vtkActor2DCollection.h>
#include <vtkScalarBarActor.h>
#include <vtkWindowToImageFilter.h>
#include <vtkBMPWriter.h>
#define VTKISRBP_ORIENT 0
#define VTKISRBP_SELECT 1






class HighlightInteractorStyle : public vtkInteractorStyleRubberBandPick
{
public:
	static HighlightInteractorStyle* New();
	vtkTypeMacro(HighlightInteractorStyle, vtkInteractorStyleRubberBandPick);
	std::vector<myPoint3f> GetRawPoint;

	HighlightInteractorStyle() : vtkInteractorStyleRubberBandPick()
	{
		this->SelectedMapper = vtkSmartPointer<vtkDataSetMapper>::New();
		this->SelectedActor = vtkSmartPointer<vtkActor>::New();
		this->SelectedActor->SetMapper(SelectedMapper);
	}



	virtual void OnMiddleButtonDown()
	{
			//vtkInteractorStyleRubberBandPick::OnMiddleButtonDown();
			// Get the location of the click (in window coordinates)
		//std::ofstream ofs("p.txt");
		int* pos = this->GetInteractor()->GetEventPosition();

		vtkSmartPointer<vtkCellPicker> picker =
			vtkSmartPointer<vtkCellPicker>::New();
		picker->SetTolerance(/*0.0005*/ 0.001);

		// Pick from this location.
		picker->Pick(pos[0], pos[1], 0, this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
		cellId = picker->GetCellId();
		if (cellId==-1)
		{
			vtkInteractorStyleTrackballCamera::OnMiddleButtonDown();
			return;
		}
		double* worldPosition = picker->GetPickPosition();
		vtkCell* cell = this->PolyData->GetCell(cellId);
		int pt3[3];
		pt3[0] = cell->GetPointId(0);
		pt3[1] = cell->GetPointId(1);
		pt3[2] = cell->GetPointId(2);
		vtkPoints* pt = PolyData->GetPoints();



		if (this->CurrentMode == VTKISRBP_ORIENT)
		{
			

			vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
			lut->SetNumberOfTableValues(pointNum);
			lut->Build();
			

			for (int i = 0; i < pointNum; ++i) {
			
				if (i== cell->GetPointId(0))
				{
					lut->SetTableValue(i, 0, 1, 0);
					continue;
				}
				if (i == cell->GetPointId(1))
				{
					lut->SetTableValue(i, 0, 1, 0);
					continue;
				}
				if (i == cell->GetPointId(2))
				{
					lut->SetTableValue(i, 0, 1, 0);
					continue;
				}
				lut->SetTableValue(i, 1, 0, 0);
			}

			std::vector<int> tempIds;
			for (size_t i = 0; i < PolyData->GetNumberOfCells(); i++)
			{
				if (cellId == i)
				{
					continue;
				}
				cell = this->PolyData->GetCell(i);
				if (cell->GetPointId(0) == pt3[0] ||
					cell->GetPointId(0) == pt3[1] ||
					cell->GetPointId(0) == pt3[2] ||
					cell->GetPointId(1) == pt3[0] ||
					cell->GetPointId(1) == pt3[1] ||
					cell->GetPointId(1) == pt3[2] ||
					cell->GetPointId(2) == pt3[0] ||
					cell->GetPointId(2) == pt3[1] ||
					cell->GetPointId(2) == pt3[2])
				{
					lut->SetTableValue(cell->GetPointId(0), 0, 0, 1);
					lut->SetTableValue(cell->GetPointId(1), 0, 0, 1);
					lut->SetTableValue(cell->GetPointId(2), 0, 0, 1);
					tempIds.push_back(cell->GetPointId(0));
					tempIds.push_back(cell->GetPointId(1));
					tempIds.push_back(cell->GetPointId(2));
				}
				
			}


			for (size_t i = 0; i < PolyData->GetNumberOfCells(); i++)
			{
				
				
				cell = this->PolyData->GetCell(i);
				for (size_t j = 0; j < tempIds.size(); j++)
				{
					if (cell->GetPointId(2)==tempIds[j])
					{
						lut->SetTableValue(cell->GetPointId(0), 1, 1, 1);
						lut->SetTableValue(cell->GetPointId(1), 1, 1, 1);
						lut->SetTableValue(cell->GetPointId(2), 1, 1, 1);
						break;
					}
					if (cell->GetPointId(1) == tempIds[j])
					{
						lut->SetTableValue(cell->GetPointId(0), 1, 1, 1);
						lut->SetTableValue(cell->GetPointId(1), 1, 1, 1);
						lut->SetTableValue(cell->GetPointId(2), 1, 1, 1);
						break;
					}
					if (cell->GetPointId(0) == tempIds[j])
					{
						lut->SetTableValue(cell->GetPointId(0), 1, 1, 1);
						lut->SetTableValue(cell->GetPointId(1), 1, 1, 1);
						lut->SetTableValue(cell->GetPointId(2), 1, 1, 1);
						break;
					}
					
				}
				

			}

			for (size_t i = 0; i < PolyData->GetNumberOfCells(); i++)
			{
				if (cellId == i)
				{
					continue;
				}
				cell = this->PolyData->GetCell(i);
				if (cell->GetPointId(0) == pt3[0] ||
					cell->GetPointId(0) == pt3[1] ||
					cell->GetPointId(0) == pt3[2] ||
					cell->GetPointId(1) == pt3[0] ||
					cell->GetPointId(1) == pt3[1] ||
					cell->GetPointId(1) == pt3[2] ||
					cell->GetPointId(2) == pt3[0] ||
					cell->GetPointId(2) == pt3[1] ||
					cell->GetPointId(2) == pt3[2])
				{
					lut->SetTableValue(cell->GetPointId(0), 0, 0, 1);
					lut->SetTableValue(cell->GetPointId(1), 0, 0, 1);
					lut->SetTableValue(cell->GetPointId(2), 0, 0, 1);
				}

			}

			cell = this->PolyData->GetCell(cellId);
			lut->SetTableValue(cell->GetPointId(0), 0, 1, 0);
			lut->SetTableValue(cell->GetPointId(1), 0, 1, 0);
			lut->SetTableValue(cell->GetPointId(2), 0, 1, 0);
			/*ofs << PolyData->GetNumberOfCells() << '\n';
			ofs.close();*/
			
			DefaultMapper->ClearColorArrays();
			DefaultMapper->SetLookupTable(lut);
			/*DefaultMapper->SetSeamlessU(false);
			DefaultMapper->SetSeamlessV(false);*/
			DefaultRender->RemoveActor(SelectedActor);
			SelectedActor->SetMapper(DefaultMapper);


			std::string tempString;
			tempString = tempString + "p1:";
			tempString = tempString + " x= "+ std::to_string(cell->GetPoints()->GetPoint(0)[0]);
			tempString = tempString + " y= " + std::to_string(cell->GetPoints()->GetPoint(0)[1]);
			tempString = tempString + " z= " + std::to_string(cell->GetPoints()->GetPoint(0)[2]);
			tempString = tempString + "\np2:";
			tempString = tempString + " x= " + std::to_string(cell->GetPoints()->GetPoint(0)[0]);
			tempString = tempString + " y= " + std::to_string(cell->GetPoints()->GetPoint(1)[1]);
			tempString = tempString + " z= " + std::to_string(cell->GetPoints()->GetPoint(2)[2]);
			tempString = tempString + "\np3:";
			tempString = tempString + " x= " + std::to_string(cell->GetPoints()->GetPoint(0)[0]);
			tempString = tempString + " y= " + std::to_string(cell->GetPoints()->GetPoint(1)[1]);
			tempString = tempString + " z= " + std::to_string(cell->GetPoints()->GetPoint(2)[2]);
			vtkSmartPointer<vtkTextActor> textActor = vtkSmartPointer<vtkTextActor>::New();
			textActor->SetTextScaleModeToProp();
			textActor->SetDisplayPosition(0, 0);
			textActor->SetInput(tempString.c_str());

			
			textActor->GetPosition2Coordinate()->SetCoordinateSystemToNormalizedViewport();
			textActor->GetPosition2Coordinate()->SetValue(0.6, 0.1);

			vtkTextProperty* tprop = textActor->GetTextProperty();
			tprop->SetFontSize(18);
			tprop->SetFontFamilyToArial();
			tprop->SetJustificationToCentered();
			tprop->BoldOn();
			tprop->ItalicOn();
			tprop->ShadowOn();
			tprop->SetColor(0, 0, 1);

		
			
			






			DefaultRender->Clear();
			DefaultRender->SetBackground(0, 0, 0);
			vtkSmartPointer<vtkActor2D> render2 = DefaultRender->GetActors2D()->GetLastActor2D();
			if (render2!=nullptr)
			{
				DefaultRender->RemoveActor2D(render2);
			}
			
			DefaultRender->AddActor2D(textActor);
			DefaultRender->AddActor(SelectedActor);





		}

		if (this->CurrentMode == VTKISRBP_SELECT)
		{
			float len = (cell->GetPoints()->GetPoint(0)[0] - worldPosition[0]) * (cell->GetPoints()->GetPoint(0)[0] - worldPosition[0])
				+ (cell->GetPoints()->GetPoint(0)[1] - worldPosition[0]) * (cell->GetPoints()->GetPoint(0)[1] - worldPosition[1])
				+ (cell->GetPoints()->GetPoint(0)[2] - worldPosition[2]) * (cell->GetPoints()->GetPoint(0)[2] - worldPosition[2]);
			float showPt[3];
			int cellId = 0;
			for (size_t i = 0; i < 3; i++)
			{
				float tempLen = (cell->GetPoints()->GetPoint(i)[0] - worldPosition[0]) * (cell->GetPoints()->GetPoint(i)[0] - worldPosition[0])
					+ (cell->GetPoints()->GetPoint(i)[1] - worldPosition[0]) * (cell->GetPoints()->GetPoint(i)[1] - worldPosition[1])
					+ (cell->GetPoints()->GetPoint(i)[2] - worldPosition[2]) * (cell->GetPoints()->GetPoint(i)[2] - worldPosition[2]);
				if (tempLen<len)
				{
					len = tempLen;
					showPt[0] = worldPosition[0];
					showPt[1] = worldPosition[1];
					showPt[2] = worldPosition[2];
					cellId = cell->GetPointId(i);

				}
				

			}
			std::vector<int> tempIds;
			for (size_t i = 0; i < PolyData->GetNumberOfCells(); i++)
			{
				cell = this->PolyData->GetCell(i);
				if (cell->GetPointId(0)==cellId
					|| cell->GetPointId(1) == cellId
					|| cell->GetPointId(2) == cellId)
				{
					tempIds.push_back(cell->GetPointId(0));
					tempIds.push_back(cell->GetPointId(1));
					tempIds.push_back(cell->GetPointId(2));
				}

			}
			vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
			lut->SetNumberOfTableValues(pointNum);
			lut->Build();


			for (int i = 0; i < pointNum; ++i) {
				lut->SetTableValue(i, 1, 0, 0);
			}

			//获取附近点
			for (size_t i = 0; i < PolyData->GetNumberOfCells(); i++)
			{


				cell = this->PolyData->GetCell(i);





				for (size_t j = 0; j < tempIds.size(); j++)
				{
					if (cell->GetPointId(2) == tempIds[j])
					{
						lut->SetTableValue(cell->GetPointId(0), 1, 1, 1);
						lut->SetTableValue(cell->GetPointId(1), 1, 1, 1);
						lut->SetTableValue(cell->GetPointId(2), 1, 1, 1);
						break;
					}
					if (cell->GetPointId(1) == tempIds[j])
					{
						lut->SetTableValue(cell->GetPointId(0), 1, 1, 1);
						lut->SetTableValue(cell->GetPointId(1), 1, 1, 1);
						lut->SetTableValue(cell->GetPointId(2), 1, 1, 1);
						break;
					}
					if (cell->GetPointId(0) == tempIds[j])
					{
						lut->SetTableValue(cell->GetPointId(0), 1, 1, 1);
						lut->SetTableValue(cell->GetPointId(1), 1, 1, 1);
						lut->SetTableValue(cell->GetPointId(2), 1, 1, 1);
						break;
					}

				}
				

			}
			for (size_t i = 0; i < PolyData->GetNumberOfCells(); i++)
			{
				cell = this->PolyData->GetCell(i);
				if (cell->GetPointId(0) == cellId ||
					cell->GetPointId(0) == cellId ||
					cell->GetPointId(0) == cellId)

				{
					lut->SetTableValue(cell->GetPointId(0), 0, 0, 1);
					lut->SetTableValue(cell->GetPointId(1), 0, 0, 1);
					lut->SetTableValue(cell->GetPointId(2), 0, 0, 1);
				}

			}
			DefaultMapper->ClearColorArrays();
			DefaultMapper->SetLookupTable(lut);
			/*DefaultMapper->SetSeamlessU(false);
			DefaultMapper->SetSeamlessV(false);*/
			DefaultRender->RemoveActor(SelectedActor);
			SelectedActor->SetMapper(DefaultMapper);
			//显示文字
			std::string tempString;
			tempString = tempString + "p1:";
			tempString = tempString + " x= " + std::to_string(worldPosition[0]);
			tempString = tempString + " y= " + std::to_string(worldPosition[1]);
			tempString = tempString + " z= " + std::to_string(worldPosition[2]);
			vtkSmartPointer<vtkTextActor> textActor = vtkSmartPointer<vtkTextActor>::New();
			textActor->SetTextScaleModeToProp();
			textActor->SetDisplayPosition(0, 0);
			textActor->SetInput(tempString.c_str());


			textActor->GetPosition2Coordinate()->SetCoordinateSystemToNormalizedViewport();
			textActor->GetPosition2Coordinate()->SetValue(0.6, 0.1);

			vtkTextProperty* tprop = textActor->GetTextProperty();
			tprop->SetFontSize(18);
			tprop->SetFontFamilyToArial();
			tprop->SetJustificationToCentered();
			tprop->BoldOn();
			tprop->ItalicOn();
			tprop->ShadowOn();
			tprop->SetColor(0, 0, 1);
			DefaultRender->SetBackground(0, 0, 0);
			DefaultRender->Clear();
			vtkSmartPointer<vtkActor2D> render2 = DefaultRender->GetActors2D()->GetLastActor2D();
			if (render2 != nullptr)
			{
				DefaultRender->RemoveActor2D(render2);
			}
			DefaultRender->AddActor2D(textActor);
			DefaultRender->AddActor(SelectedActor);


		}
		vtkInteractorStyleTrackballCamera::OnMiddleButtonDown();
	}




	virtual void OnLeftButtonUp() override
	{
		// Forward events
		vtkInteractorStyleRubberBandPick::OnLeftButtonUp();
		GetRawPoint.clear();

		if (this->CurrentMode == VTKISRBP_SELECT)
		{
			vtkNew<vtkNamedColors> colors;

			vtkPlanes* frustum =
				static_cast<vtkAreaPicker*>(this->GetInteractor()->GetPicker())
				->GetFrustum();

			vtkNew<vtkExtractPolyDataGeometry> extractPolyDataGeometry;
			extractPolyDataGeometry->SetInputData(this->PolyData);
			extractPolyDataGeometry->SetImplicitFunction(frustum);
			extractPolyDataGeometry->Update();

			std::cout << "Extracted "
				<< extractPolyDataGeometry->GetOutput()->GetNumberOfCells()
				<< " cells." << std::endl;
			size_t PointId;
			double m[4];

			for (size_t i = 0; i < extractPolyDataGeometry->GetOutput()->GetNumberOfCells(); i++)
			{
				vtkCell* pCell = extractPolyDataGeometry->GetOutput()->GetCell(i);
				vtkPoints* mypoints = pCell->GetPoints();
				for (size_t j = 0; j < 3; j++)
				{
					
					m[3] = 1.0;
					mypoints->GetPoint(j, m);

					GetRawPoint.push_back(myPoint3f(m[0], m[1], m[2]));
				}

			}
			
			this->SelectedMapper->SetInputData(extractPolyDataGeometry->GetOutput());
			this->SelectedMapper->ScalarVisibilityOff();

			//        vtkIdTypeArray* ids =
			//        dynamic_cast<vtkIdTypeArray*>(selected->GetPointData()->GetArray("OriginalIds"));

			this->SelectedActor->GetProperty()->SetColor(
				colors->GetColor3d("Tomato").GetData());
			this->SelectedActor->GetProperty()->SetPointSize(5);
			this->SelectedActor->GetProperty()->SetRepresentationToWireframe();

			this->GetInteractor()
				->GetRenderWindow()
				->GetRenderers()
				->GetFirstRenderer()
				->AddActor(SelectedActor);
			this->GetInteractor()->GetRenderWindow()->Render();
			this->HighlightProp(NULL);
		}
	}

	void SetPolyData(vtkSmartPointer<vtkPolyData>& polyData)
	{
		this->PolyData = polyData;
	}
	void SetDefaultMapper(vtkSmartPointer<vtkPolyDataMapper>& mapper)
	{
		this->DefaultMapper = mapper;
	}
	void SetPointNum(int tpointNum)
	{
		this->pointNum = tpointNum;
	}
	void SetRenderer(vtkSmartPointer<vtkRenderer>& render)
	{
		this->DefaultRender = render;
	}
	vtkSmartPointer<vtkPolyDataMapper> GetFaultMapper() { return DefaultMapper; }
	int GetPointNum() { return pointNum; }
private:
	int cellId;
	int pointNum;

	vtkSmartPointer<vtkPoints> PointData;
	vtkSmartPointer<vtkPolyData> PolyData;
	vtkSmartPointer<vtkActor> SelectedActor;
	vtkSmartPointer<vtkDataSetMapper> SelectedMapper;
	vtkSmartPointer<vtkPolyDataMapper> DefaultMapper;
	vtkSmartPointer<vtkRenderer> DefaultRender;
};
vtkStandardNewMacro(HighlightInteractorStyle);




IMPLEMENT_DYNAMIC(CVtkViewer, CStatic)

CVtkViewer::CVtkViewer()
{

	IsShowLut = false;
}

CVtkViewer::~CVtkViewer()
{
}


bool CVtkViewer::Clear()
{

		vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();

		lut->SetNumberOfTableValues(m_InterAct->GetPointNum());
		lut->Build();

		//m_RenderWindow->RemoveRenderer(m_Renderer);
		for (int i = 0; i < m_InterAct->GetPointNum(); ++i) {


			lut->SetTableValue(i, 1, 0, 0);
		}
		if (m_InterAct->GetPointNum()>0)
		{
			m_InterAct->GetFaultMapper()->SetLookupTable(lut);
			m_Renderer->Clear();
			
			actor->SetMapper(m_InterAct->GetFaultMapper());

			
			vtkSmartPointer<vtkActor2D> render2 = m_Renderer->GetActors2D()->GetLastActor2D();
			m_Renderer->RemoveActor2D(render2);
			m_Renderer->AddActor(actor);
						m_Renderer->SetBackground(0, 0, 0);
			m_Renderer->ResetCamera();
			m_RenderWindow->AddRenderer(m_Renderer);
			m_RenderWindow->Render();
			return true;
		}
		

	return false;
}

bool CVtkViewer::GetCapPoint(std::vector<myPoint3f>& dst)
{
	dst.clear();
	dst.resize(m_InterAct->GetRawPoint.size());
	for (size_t i = 0; i < m_InterAct->GetRawPoint.size(); i++)
	{
		dst[i] = m_InterAct->GetRawPoint[i];
	}
	return 0;
}

void CVtkViewer::ReadPointCloud(std::vector<Pointf>& pointcloud) {
	int n = pointcloud.size();
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();

	points->SetNumberOfPoints(n);
	vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
	lut->SetNumberOfTableValues(n);
	lut->Build();
	for (int i = 0; i < n; ++i) {
		points->InsertPoint(i, pointcloud[i].x, pointcloud[i].y, pointcloud[i].z);
		lut->SetTableValue(i, pointcloud[i].r, pointcloud[i].g, pointcloud[i].b);
	}

	vtkSmartPointer<vtkPolyVertex> polyVertex = vtkSmartPointer<vtkPolyVertex>::New();
	polyVertex->GetPointIds()->SetNumberOfIds(n);
	vtkSmartPointer<vtkFloatArray> pointsScalars = vtkSmartPointer<vtkFloatArray>::New();
	pointsScalars->SetNumberOfTuples(n);
	for (int i = 0; i < n; ++i) {
		polyVertex->GetPointIds()->SetId(i, i);
		pointsScalars->InsertValue(i, i);
	}

	vtkSmartPointer<vtkUnstructuredGrid> grid = vtkSmartPointer<vtkUnstructuredGrid>::New();
	grid->Allocate(1, 1);
	grid->SetPoints(points);

	grid->GetPointData()->SetScalars(pointsScalars);
	grid->InsertNextCell(polyVertex->GetCellType(), polyVertex->GetPointIds());

	vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->SetInputData(grid);
	mapper->ScalarVisibilityOn();
	mapper->SetScalarRange(0, n - 1);
	mapper->SetLookupTable(lut);

	m_Renderer->RemoveActor(actor);
	actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	actor->GetProperty()->SetRepresentationToPoints();
	actor->GetProperty()->SetPointSize(2);

	m_Renderer->AddActor(actor);
	m_Renderer->SetBackground(0, 0, 0);
	m_Renderer->ResetCamera();
	m_RenderWindow->Render();


}




void CVtkViewer::ReadPointAndFace(std::vector<Pointf>& pointcloud,std::vector<facev>& myface,int count) {
	int n = pointcloud.size();
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();

	points->SetNumberOfPoints(n);
	vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
	lut->SetNumberOfTableValues(n);
	
	vtkSmartPointer<vtkCellArray> strips = vtkSmartPointer<vtkCellArray>::New();
	strips->SetNumberOfCells(myface.size());

//	if (IsShowLut==false)
//	{
		for (int i = 0; i < n; ++i) 
		{
			points->InsertPoint(i, pointcloud[i].x, pointcloud[i].y, pointcloud[i].z);
			lut->SetTableValue(i, pointcloud[i].r, pointcloud[i].g, pointcloud[i].b);
		}
		lut->Build();
//	}
//	else
//	{
//		vtkLookupTable* lut = vtkLookupTable::New();
//		std::vector<float> tempz(n);
//#pragma omp parallel for
//		for (__int64 i = 0; i < n; i++)
//		{
//			tempz[i] = pointcloud[i].z;
//		}

//		lut->SetTableRange(*std::min_element(tempz.begin(),tempz.end())
//			, *std::max_element(tempz.begin(), tempz.end()));
//		lut->Build();
//	}
	

	for (size_t i = 0; i < myface.size(); i++)
	{
		
		if (myface[i].a <= pointcloud.size()&& myface[i].b <= pointcloud.size()&& myface[i].c <= pointcloud.size())
		{
			strips->InsertNextCell(3);
			strips->InsertCellPoint(myface[i].a - 1);
			strips->InsertCellPoint(myface[i].b - 1);
			strips->InsertCellPoint(myface[i].c - 1);
		}
		
	}
	int c = strips->GetSize();
	//设置面片顶点
	vtkSmartPointer<vtkPolyVertex> polyVertex = vtkSmartPointer<vtkPolyVertex>::New();
	polyVertex->GetPointIds()->SetNumberOfIds(n);
	vtkSmartPointer<vtkFloatArray> pointsScalars = vtkSmartPointer<vtkFloatArray>::New();
	pointsScalars->SetNumberOfTuples(n);
	for (int i = 0; i < n; ++i) {
		polyVertex->GetPointIds()->SetId(i, i);
		pointsScalars->InsertValue(i, i);
	}

	m_grid = vtkSmartPointer<vtkPolyData>::New();
	//grid->Allocate(1, 1);
	m_grid->SetPoints(points);
	m_grid->SetStrips(strips);
	m_grid->GetPointData()->SetScalars(pointsScalars);

	//grid->InsertNextCell(polyVertex->GetCellType(), polyVertex->GetPointIds());

	vtkSmartPointer<vtkPolyDataNormals> normFilter = vtkSmartPointer<vtkPolyDataNormals>::New();
	normFilter->SetInputData(m_grid);

	normFilter->SetComputePointNormals(0);
	
	normFilter->SetComputeCellNormals(1);
	normFilter->SetAutoOrientNormals(1);
	normFilter->SetSplitting(0);
	normFilter->Update();
	

	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(normFilter->GetOutput());

	mapper->ScalarVisibilityOn();
	mapper->SetScalarRange(0, n - 1);
	mapper->SetLookupTable(lut);
	
	vtkSmartPointer<vtkCamera> pcamera;
	
	m_Renderer->RemoveActor(actor);
	actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	





	//actor->GetProperty()->SetRepresentationToPoints();
	//actor->GetProperty()->SetPointSize(2);

	if (IsShowLut)
	{
		
		vtkLookupTable* lut2 = vtkLookupTable::New();
//		std::vector<float> tempz(n);
//#pragma omp parallel for
//		for (__int64 i = 0; i < n; i++)
//		{
//			tempz[i] = pointcloud[i].z;
//		}

//		float a = *std::min_element(tempz.begin(), tempz.end());
//		float b = *std::max_element(tempz.begin(), tempz.end());
		lut2->SetTableRange(_lut_min,_lut_max);
		
		lut2->Build();
		vtkSmartPointer<vtkScalarBarActor> scalarBar = vtkScalarBarActor::New();
		//scalarBar->SetLookupTable(boneMapper->GetLookupTable());
		scalarBar->SetLookupTable(lut2);
		//设置标题
		scalarBar->SetTitle("Distance(mm)");

		scalarBar->SetNumberOfLabels(6);
		for (size_t i = 0; i < 100; i++)
		{
			vtkSmartPointer<vtkActor2D> render2 = m_Renderer->GetActors2D()->GetLastActor2D();
			if (render2 == nullptr)
			{
				break;
			}
			m_Renderer->RemoveActor2D(render2);
		}
		
		m_Renderer->AddActor2D(scalarBar);
		//actor->GetProperty()->EdgeVisibilityOn();
	}
	IsShowLut = false;
	
	m_Renderer->AddActor(actor);
	
	m_Renderer->SetBackground(0, 0, 0);
	m_Renderer->ResetCamera();
	m_RenderWindow->Render();
	

	/*vtkSmartPointer<vtkWindowToImageFilter> windowto_image_filter = vtkSmartPointer<vtkWindowToImageFilter>::New();
	windowto_image_filter->SetInput(m_RenderWindow);
	windowto_image_filter->SetScale(1);
	windowto_image_filter->SetInputBufferTypeToRGB();
	windowto_image_filter->ReadFrontBufferOff();
	windowto_image_filter->Update();
	vtkSmartPointer<vtkBMPWriter> writer = vtkSmartPointer<vtkBMPWriter>::New();
	writer->SetFileName((std::to_string(count)+std::string(".bmp")).c_str());
	writer->SetInputConnection(windowto_image_filter->GetOutputPort());
	writer->Write();*/
	


	//重载互动API
	if (m_RenderWindow->GetInteractor() == NULL)
	{
		vtkSmartPointer<vtkRenderWindowInteractor> RenderWindowInteractor =
			vtkSmartPointer<vtkRenderWindowInteractor>::New();
		RenderWindowInteractor->SetRenderWindow(m_RenderWindow);
		//RenderWindowInteractor->Initialize();
		
		vtkSmartPointer<vtkAreaPicker> areaPicker =
			vtkSmartPointer<vtkAreaPicker>::New();

		RenderWindowInteractor->SetPicker(areaPicker);
		RenderWindowInteractor->SetRenderWindow(m_RenderWindow);
		m_InterAct
			= vtkSmartPointer<HighlightInteractorStyle>::New();
		m_InterAct->SetPolyData(m_grid);
		m_InterAct->SetDefaultMapper(mapper);
		m_InterAct->SetPointNum(pointcloud.size());
		m_InterAct->SetRenderer(m_Renderer);
		RenderWindowInteractor->SetInteractorStyle(m_InterAct);

		/*m_Mouse =
			vtkSmartPointer<MouseInteractorStyle>::New();

		
		m_Mouse->SetDefaultRenderer(m_Renderer);
		m_Mouse->Data = m_grid;

		RenderWindowInteractor->SetInteractorStyle(m_Mouse);*/

		m_Renderer->AddActor(actor);
		



		RenderWindowInteractor->Start();


		
		//RenderWindowInteractor->Start();
	}




/*	vtkPoints * points = vtkPoints::New();
	int n = pointcloud.size(); int idx = 0;
	for (int i = 0; i < n; ++i){
		points->InsertPoint(i, pointcloud[i].x, pointcloud[i].y, pointcloud[i].z);
	}

	vtkPolyVertex * polyvertex = vtkPolyVertex::New();
	polyvertex->GetPointIds()->SetNumberOfIds(n);
	for (int i = 0; i < n; ++i){
		polyvertex->GetPointIds()->SetId(i, i);
	}

	vtkUnstructuredGrid * grid = vtkUnstructuredGrid::New();
	grid->SetPoints(points);
	grid->InsertNextCell(polyvertex->GetCellType(), polyvertex->GetPointIds());

	vtkDataSetMapper * map = vtkDataSetMapper::New();
	map->SetInputData(grid);

	vtkActor * actor = vtkActor::New();
	actor->SetMapper(map);
	actor->GetProperty()->SetColor(0.194, 0.562, 0.75);

	m_Renderer->AddActor(actor);
	m_Renderer->SetBackground(0, 0, 0);
	m_Renderer->ResetCamera();
	m_RenderWindow->Render();

	map->Delete();
	grid->Delete();
	actor->Delete();
	points->Delete();
	polyvertex->Delete();*/


	
}

void CVtkViewer::PreSubclassWindow()
{
	// TODO: Add your specialized code here and/or call the base class    
	CRect rect;
	GetClientRect(rect);

	m_Renderer = vtkSmartPointer<vtkRenderer>::New();
	m_RenderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	m_RenderWindow->SetParentId(this->m_hWnd);
	m_RenderWindow->SetSize(rect.Width(), rect.Height());
	m_RenderWindow->AddRenderer(m_Renderer);
	

	//test
	

	//if (m_RenderWindow->GetInteractor() == NULL)
	//{
	//	vtkSmartPointer<vtkRenderWindowInteractor> RenderWindowInteractor =
	//		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	//	RenderWindowInteractor->SetRenderWindow(m_RenderWindow);
	//	RenderWindowInteractor->Initialize();
	//	/*vtkSmartPointer<vtkAreaPicker> areaPicker =
	//		vtkSmartPointer<vtkAreaPicker>::New();*/

	//	//RenderWindowInteractor->SetPicker(areaPicker);
	//	//RenderWindowInteractor->SetRenderWindow(m_RenderWindow);
	//	//vtkSmartPointer<HighlightInteractorStyle> style
	//	//	= vtkSmartPointer<HighlightInteractorStyle>::New();
	//	////style->SetPolyData(m_grid);
	//	//RenderWindowInteractor->SetInteractorStyle(style);



	//	
	//	//RenderWindowInteractor->Start();
	//}
	
	m_RenderWindow->Start();
	
	CStatic::PreSubclassWindow();
}

void CVtkViewer::SetImageData(vtkSmartPointer<vtkImageData> ImageData)
{
	if (ImageData == NULL) return;

	m_ImageData = ImageData;
	SetupReslice();
}

void CVtkViewer::SetupReslice()
{
	if (m_ImageData == NULL) return;
	int dims[3];
	m_ImageData->GetDimensions(dims);

	//////////////////////////////////////////////////////////////////////////    
	m_ImagePlaneWidget->SetInputData(m_ImageData);
	m_ImagePlaneWidget->SetPlaneOrientation(m_Direction);
	m_ImagePlaneWidget->SetSliceIndex(dims[m_Direction] / 2);
	m_ImagePlaneWidget->On();
	m_ImagePlaneWidget->InteractionOn();

	//////////////////////////////////////////////////////////////////////////    
	m_ResliceCursor->SetCenter(m_ImageData->GetCenter());
	m_ResliceCursor->SetImage(m_ImageData);
	m_ResliceCursor->SetThickMode(0);

	//m_ResliceCursorRep->GetResliceCursorActor()->GetCursorAlgorithm()->SetResliceCursor(m_ResliceCursor);  
	//m_ResliceCursorRep->GetResliceCursorActor()->GetCursorAlgorithm()->SetReslicePlaneNormal(m_Direction);  

	m_ResliceCursorWidget->SetEnabled(1);
	m_Renderer->ResetCamera();
   
	double range[2];
	m_ImageData->GetScalarRange(range);
	m_ResliceCursorWidget->GetResliceCursorRepresentation()->
		SetWindowLevel(range[1] - range[0], (range[0] + range[1]) / 2.0);
	m_ImagePlaneWidget->SetWindowLevel(range[1] - range[0], (range[0] + range[1]) / 2.0);
}

void CVtkViewer::MoveWindow(CRect rect) {
	m_RenderWindow->SetSize(rect.Width(), rect.Height());
	CStatic::MoveWindow(rect);
}

BEGIN_MESSAGE_MAP(CVtkViewer, CStatic)
END_MESSAGE_MAP()