/*  File       :     MeshOutput.hpp
 *
 *  Purpose    :     Base class for any class that writes a mesh to disk.
 *
 *  Creator    :     Clinton Stimpson
 *
 *  Date       :     15 Jan 2001
 *
 *  Owner      :     Clinton Stimpson
 */


#ifndef MESHOUTPUT_HPP
#define MESHOUTPUT_HPP

#include "VerdeDefines.hpp"
#include "Mesh.hpp"

class QString;

//! MeshOutput class for writing meshes to files
class MeshOutput
{
public:
   MeshOutput(){};
   virtual ~MeshOutput(){};
   //! write an active mesh to a file
   virtual VerdeStatus output_mesh_file( Mesh* mesh, QString filename ) = 0;
   // futhur abstraction would mean replacing the Mesh* with the abstract meshset
   
};

#endif // MESHOUTPUT_HPP

