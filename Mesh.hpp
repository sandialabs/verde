//-------------------------------------------------------------------------
// Filename      : Mesh.hpp 
//
// Purpose       : This class contains the global mesh information and
//                 controls the MeshBlocks for the mesh.
//
// Special Notes : 
//
// Creator       : Ray Meyers
//
// Date          : 10/14/98
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------

#ifndef MESH_HPP
#define MESH_HPP

#include <deque>

#include "VerdeDefines.hpp"

class ElementBlock;
class MeshContainer;
class VerdeVector;
class ElemRef;
class NodeBC;
class ElementBC;

//! mesh object
class Mesh
{

public:
   
   //static Mesh* instance();
     //- singleton access to the object

   //! Constructor
   Mesh();

   //! Destructor
   ~Mesh ();

   //! opens exodus file and reads ini variables and coordinates
   //! also reads the mesh into memory
   VerdeStatus open_exodus_file(const char *file_name,
       std::deque<int> *active_block_id_list);

   //! read exodus header information
   VerdeStatus read_exodus_header(const char *file_name,
       std::deque<int> *active_block_id_list);

   //! read active blocks
   void load_active_element_blocks();

   //! read block by id
   VerdeStatus load_element_block(int block_id);

   //! read block by pointer
   VerdeStatus load_element_block(ElementBlock *block);

   //! reads a single block from the exodus file
   VerdeStatus read_exodus_block(int block_index);
   
   //! creates and processes each mesh block in turn.
   VerdeStatus verify_mesh(VerdeBoolean do_metrics,
                           VerdeBoolean do_topology,
                           VerdeBoolean do_interface,
                           VerdeBoolean do_verbose,
                           VerdeBoolean do_individual,
                           VerdeBoolean &metric_success,
                           VerdeBoolean &topology_success,
                           VerdeBoolean &interface_success);

   //! access to the exodus file id
   int exodus_file();

   //! access to numberDimensions
   int number_dimensions();

   //! access to numberNodes
   int number_nodes();

   //! access to numberPreviousNodes
   int number_previous_nodes();

   //! number of elements in mesh
   int number_elements();
   //! number of hexes in mesh
   int number_hexes();
   //! number of tets in mesh
   int number_tets();
   //! number of pyramids in mesh
   int number_pyramids();
   //! number of wedges in mesh
   int number_wedges();
   //! number of knives in mesh
   int number_knives();
   
   //! access to numberElementBlocks
   int number_element_blocks();

   //! gets a pointer to a block of the mesh by its id
   ElementBlock *get_element_block(int id);

   //! returns total volume of the mesh
   double volume();

   //! returns total area of quad/tri blocks of the mesh
   double area();

  //! returns a copy of the element block list
  void element_blocks(std::deque<ElementBlock*> &element_blocks);

  //! returns a copy of the node boundary condition list
  void get_node_bc_list(std::deque<NodeBC*> &node_bc_list);

  //! returns a copy of the element boundary condition list
  void get_element_bc_list(std::deque<ElementBC*> &element_bc_list);

  //int node_id_of_element(int element_index, int node_index);
  // returns the id of a node in an element

  //! returns the original id number of an element
  int original_id(int index);

//  int block_id(ElemRef* element);
  // returns which block an element is in

  //! returns the mesh container that contains the element
  MeshContainer* mesh_container(ElemRef* element);

  //! returns a pointer to the node container
  MeshContainer *node_container();

  //! returns the coordinates of a node
  VerdeVector coordinates(int node_index);

  //! finds the current exterior boundary of the mesh based on the active blocks
  VerdeStatus find_current_boundary();

  //! given the id, returns a pointer to the NodeBC, or NULL if not found
  NodeBC *get_node_bc(int id);

  //! given the id, returns a pointer to the ElementBC, or NULL if not found
  ElementBC *get_element_bc(int id);

  //! finds the element block associated with the given element id
  ElementBlock *find_block_for_element(int element_id);

  //! returns list of NodeBCs
  std::deque<NodeBC*> get_node_bc_list(); 

  //! returns list of ElementBCs
  std::deque<ElementBC*> get_element_bc_list(); 

  //! returns block id offset 
  int blockIdOffset();

  //! returns list of qa records
  std::deque<char*> *get_qa_record_list();

  //! returns number of qa strings defined
  int number_qa_strings();
  
private:

   //static Mesh *instance_;
   
   int exodusFile;        // integer id of the exodus input file
   int numberDimensions;         // dimension of this exodus file (2 or 3)
   int numberNodes;              // number of nodes in this dbase
   int numberPreviousNodes;      // number of nodes in previous model 
   int numberElements;           // number of elements in this dbase
   int numberHexes;
   int numberTets;
   int numberPyramids;
   int numberWedges;
   int numberKnives;
   int numberQuads;
   int numberTris;
   int numberEdges;
   int numberElementBlocks;      // number of element blocks in this dbase
   int block_id_offset; 
   int nodeset_id_offset; 
   int sideset_id_offset; 


   MeshContainer *nodeContainer;
   //! This container contains all nodes of the input model
   
   std::deque<ElementBlock*> elementBlocks;
   //! contains the element blocks associated with this mesh

   //! List of NodeBCs defined for this Mesh
   std::deque<NodeBC*> nodeBCs;

   //! List of ElementBCs defined for this Mesh
   std::deque<ElementBC*> elementBCs;

   //! List of qa_record inforamtion for this mesh
   std::deque<char*> qaRecords;

   //Mesh();
   //! Constructor

   double meshVolume;
   //! total volume calculated for the mesh

   double meshArea;
   //! total area calculated for the mesh

   //! gets offset for blocks, sidesets and nodesets
   int get_offset( int flag );

   //! Resets the Mesh, deleting old objects and memory
   void reset();

   //! reads a single qa string from an exodus format file
   VerdeStatus read_qa_string(int genesis_file_id,
                              char *string,
                              int record_number,
                              int record_position);

   //! read qa records from a genesis file into a list of strings
   VerdeStatus read_qa_information(int genesis_file_id,
                                   std::deque<char*> &qa_record_list);
};

// inline functions

inline MeshContainer* Mesh::node_container()
{
	return nodeContainer;
}

inline int Mesh::exodus_file() 
{
   return exodusFile;
}

inline int Mesh::number_dimensions() 
{
   return numberDimensions;
}

inline int Mesh::number_nodes()
{
   return numberNodes;
}

inline int Mesh::number_previous_nodes()
{
   return numberPreviousNodes;
}

inline int Mesh::number_elements()
{
   return numberElements;
}

inline int Mesh::number_hexes()
{
   return numberHexes;
}

inline int Mesh::number_tets()
{
   return numberTets;
}

inline int Mesh::number_pyramids()
{
   return numberPyramids;
}

inline int Mesh::number_wedges()
{
   return numberWedges;
}

inline int Mesh::number_knives()
{
   return numberKnives;
}

inline int Mesh::number_element_blocks()
{
   return elementBlocks.size();
}
  
inline double Mesh::volume()
{
   return meshVolume;
}  

inline double Mesh::area()
{
   return meshArea;
}

inline int Mesh::blockIdOffset()
{
   return block_id_offset;
}

inline int Mesh::number_qa_strings()
{
  return qaRecords.size();
}

inline std::deque<char*> *Mesh::get_qa_record_list()
{
  return &qaRecords;
}

#endif

