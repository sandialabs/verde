//----------------------------------------------------------------------
// Filename         : OpenMeshFileDialog.cpp
//
// Purpose          : A file dialog box for opening mesh files
//
// Creator          : Clinton Stimpson
//
// Date             : 25 Oct 2001
//
// Owner            : Clinton Stimpson
//---------------------------------------------------------------------- 


#include "OpenMeshFileDialog.hpp"
#include "../VerdeApp.hpp"


OpenMeshFileDialog::OpenMeshFileDialog(QString working_directory)
    : QFileDialog(working_directory, QString::null, NULL),
    select_block_checkbox(QString("Select Blocks"), this)
{
    addFilter(QString("Mesh Files (*.g; *.gen; *.exoII; *.exo)"));
    setCaption("Open Mesh File");
    addWidgets(NULL, &select_block_checkbox, NULL);

    add_mesh_checkbox = NULL;
   
    // add mesh checkbox only appears if a mesh is present
    if(verde_app->mesh())
    {
      add_mesh_checkbox = new QCheckBox(QString("Add Mesh" ), this);
      addWidgets(NULL, add_mesh_checkbox, NULL);
    }

}

OpenMeshFileDialog::~OpenMeshFileDialog(){}


QString OpenMeshFileDialog::getOpenFileName()
{
    if (exec() == QDialog::Accepted)
    {
       return selectedFile();
    }
    else
	return QString::null;
}

bool OpenMeshFileDialog::SpecifyBlocks()
{
    return select_block_checkbox.isChecked(); 
}

bool OpenMeshFileDialog::AddMesh()
{
  if(add_mesh_checkbox)
    return add_mesh_checkbox->isChecked(); 
  else
    return false;
}
