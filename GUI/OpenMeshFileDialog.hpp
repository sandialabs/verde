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


#include <qfiledialog.h>
#include <qcheckbox.h>
#include <qlabel.h>

//! Opens mesh files
class OpenMeshFileDialog : public QFileDialog
{
public:
    
    OpenMeshFileDialog(QString working_directory );
    ~OpenMeshFileDialog();
    QString getOpenFileName();
    bool SpecifyBlocks();
    bool AddMesh();

private:

   QCheckBox select_block_checkbox;
   QCheckBox* add_mesh_checkbox;
   
};

