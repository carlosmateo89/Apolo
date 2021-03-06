#include "faceWindow.h"

BEGIN_EVENT_TABLE(FaceWindow, wxPanel)

EVT_BUTTON(ID_OTHERFACE, FaceWindow::FaceButton)
EVT_BUTTON(ID_CHANGEVIEW, FaceWindow::FaceButton)
EVT_BUTTON(ID_COLOR, FaceWindow::ColorChanged)	
EVT_COMMAND(wxID_ANY, wxEVT_GENERIC_SLIDER_CHANGE, FaceWindow::FaceOrientation)
EVT_RADIOBOX(wxID_ANY, FaceWindow::FaceAlign)


END_EVENT_TABLE()



FaceWindow::FaceWindow(wxWindow *parent,NodeTree *obj,const wxString& title, const wxPoint& pos,const wxSize& size)
: wxPanel(parent, wxID_ANY, pos, size)
{
	mainWin=(MainWindow*)parent->GetParent();;
	node=obj;
	worldView=false;
	red=green=blue=1.0f;
	CreatePanel();
	align->SetSelection(1); //Set align OFF to start
}

void FaceWindow::CreatePanel()
{
		wxBoxSizer *fbox=new wxBoxSizer(wxHORIZONTAL);//general box
		wxBoxSizer *rbox=new wxBoxSizer(wxVERTICAL);
		wxBoxSizer *sbox=new wxBoxSizer(wxVERTICAL);
		
		
		PointsList *points=new PointsList(this,wxT("Face Coordenates"));
		
		wxButton *af = new wxButton(this,ID_OTHERFACE,wxT("Add another face"),wxDefaultPosition,wxDefaultSize);
		cView = new wxButton(this,ID_CHANGEVIEW,wxT("World View3D"),wxDefaultPosition,wxDefaultSize);

		canvas=new FaceWidget(this,node->getSimu(),wxDefaultPosition,wxDefaultSize);
		canvas->AssociatePointTable(points);
		
		PositionableWidget *pw=new PositionableWidget(this,node,wxT("Face Set Orientation"),wxDefaultPosition,wxDefaultSize,mainWin->getSliderValue(),false);

		FaceControlButtons* controlButtons=new FaceControlButtons(this,ID_ADDFACESET,wxDefaultPosition,wxDefaultSize,canvas);
		
		wxBoxSizer *pbox=new wxBoxSizer(wxHORIZONTAL);
		wxStaticBoxSizer *buttons=new wxStaticBoxSizer(wxVERTICAL,this,wxT("Canvas3D"));

		wxStaticBoxSizer *obox=new wxStaticBoxSizer(wxVERTICAL,this,wxT("Face Properties"));
		roll = new GenericSlider(this,wxT("Face Roll"),wxDefaultPosition,wxDefaultSize,false);//true = vertical
		pitch = new GenericSlider(this,wxT("Face Pitch"),wxDefaultPosition,wxDefaultSize,false);
		x_pos = new GenericSlider(this,wxT("X position"),wxDefaultPosition,wxDefaultSize,false);
		y_pos = new GenericSlider(this,wxT("Y position)"),wxDefaultPosition,wxDefaultSize,false);
		plane_dis = new GenericSlider(this,wxT("Normal Distance (Z)"),wxDefaultPosition,wxDefaultSize,false);
		
		wxString string[2]={wxT("1"), wxT("0")};
		wxString string2[2]={wxT("On"), wxT("Off")};
		
		transparency = new GenericSlider(this,wxT("Face Transparency"),wxDefaultPosition,wxDefaultSize,false);
		
		align = new wxRadioBox(this,wxID_ANY,wxT("Align Face Drawing "),wxDefaultPosition,wxDefaultSize,2,string2);
		
		wxBitmapButton *color_box = new wxBitmapButton(this,ID_COLOR,wxIcon(colour_xpm),wxDefaultPosition,wxSize(100,50),4,wxDefaultValidator,wxT("Change face color"));
		
		//sliders
		roll->setProperties(-180,180);
		roll->setValue(0);
		pitch->setProperties(-180,180);
		pitch->setValue(0);
		x_pos->setProperties(-10,10,false);
		x_pos->setValue(0);
		y_pos->setProperties(-10,10,false);
		y_pos->setValue(0);
		plane_dis->setProperties(-10,10,false);
		plane_dis->setValue(0);
		transparency->setProperties(0,1);
		transparency->setValue(1);

		//sliders in box
		obox->Add(roll,0,wxALL|wxEXPAND,5);
		obox->Add(pitch,0,wxALL|wxEXPAND,5);
		obox->Add(x_pos,0,wxALL|wxEXPAND,5);
		obox->Add(y_pos,0,wxALL|wxEXPAND,5);
		obox->Add(plane_dis,0,wxALL|wxEXPAND,5);
		obox->Add(transparency,0,wxEXPAND|wxALL,5);
		obox->Add(align,0,wxEXPAND|wxALL,5);	
		obox->Add(color_box,0,wxEXPAND|wxALL,5);
		
		buttons->Add(cView,0,wxEXPAND);
		pbox->Add(controlButtons,1,wxEXPAND |wxALL);//horizontal
		pbox->Add(buttons,1,wxEXPAND |wxALL);

			
	
		sbox->Add(canvas,0,wxEXPAND );//vertical		
		sbox->Add(pbox,0,wxEXPAND);
		sbox->Add(pw,0,wxEXPAND);



		rbox->Add(points,1,wxEXPAND|wxALL,5);//vertical
		rbox->Add(af,0,wxEXPAND|wxALL,5);
		
		fbox->Add(obox,0,wxEXPAND|wxALL,5);	//horizontal
		fbox->Add(sbox,0,wxEXPAND|wxALL,5);
		fbox->Add(rbox,0,wxEXPAND);
		
	
		SetSizer(fbox);
	
}



void FaceWindow::FaceAlign(wxCommandEvent& event)
{
	if(align->GetSelection()==0)
		canvas->design1->SetAlign(true);
	else
		canvas->design1->SetAlign(false);
}



void FaceWindow::FaceOrientation(wxCommandEvent& WXUNUSED(event))
{	
	Transformation3D trans;
	trans.position.x=x_pos->getValue();
	trans.position.y=y_pos->getValue();
	trans.position.z=plane_dis->getValue();
	trans.orientation.setRPY(deg2rad(roll->getValue()),deg2rad(pitch->getValue()),0);
	canvas->GetFace()->setBase(trans);
	canvas->GetFace()->setColor(red,green,blue,transparency->getValue());
	canvas->RefreshCanvas();

}

void FaceWindow::FaceButton(wxCommandEvent& event)
{
	int id=event.GetId();
	if(id==ID_CHANGEVIEW)
	{
		worldView=!worldView;
		canvas->ChangeView(worldView);
		if(worldView) cView->SetLabel(wxT("Face View3D"));
		else	cView->SetLabel(wxT("World View3D"));
			
	}

	else
	{
		node->pointer.facesetpart->addFace((*canvas->GetFace()));
		canvas->GetCanvas3D()->ClearObjects();
		canvas->GetCanvas3D()->UpdateWorld(node->getSimu()->getWorld());
		roll->setValue(0);
		pitch->setValue(0);
		plane_dis->setValue(0);
		canvas->CreateFace();
	}

	
}

void FaceWindow::AddFace()
{

	node->pointer.facesetpart->addFace((*canvas->GetFace()));
	canvas->GetCanvas3D()->ClearObjects();
	canvas->GetCanvas3D()->UpdateWorld(node->getSimu()->getWorld());
	roll->setValue(0);
	pitch->setValue(0);
	plane_dis->setValue(0);
	canvas->CreateFace();

}


void  FaceWindow::ColorChanged(wxCommandEvent& event)
{
	
		wxColor color=wxGetColourFromUser(this);
		if(color.IsOk())
		{
			red = color.Red();
			green = color.Green();
			blue = color.Blue();
			canvas->GetFace()->setColor(red/255,green/255,blue/255,transparency->getValue());
			canvas->RefreshCanvas();
		
		}		
}

