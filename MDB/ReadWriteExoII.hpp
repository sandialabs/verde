//-------------------------------------------------------------------------
// Filename      : ReadWriteExoII.hpp
//
// Purpose       : ExodusII reader/writer
//
// Special Notes : Lots of code taken from verde implementation
//
// Creator       : Tim Tautges
//
// Date          : 3/02
//
// Owner         : Tim Tautges
//-------------------------------------------------------------------------

#ifndef READWRITEEXOII_HPP
#define READWRITEEXOII_HPP

#include <set>
#include <map>
#include <vector>
#include <deque>
#include <functional>
#include <string>

#include "MDBDefines.h"
#include "MDBImplGeneral.hpp"

class MDBEntitySequence;
class MDB_MeshSet;

//! Output Exodus File for VERDE
class ReadWriteExoII // : public ReadWriteTool
{
   
public:
   //! Constructor
   ReadWriteExoII(MDBImplGeneral *impl = NULL);

   //! Destructor
  virtual ~ReadWriteExoII();

    //! load an ExoII file
  MDBErrorCode load_file(const char *exodus_file_name,
                         std::deque<int> *active_block_id_list);
             
    //! read the header from the ExoII file
  MDBErrorCode read_exodus_header(const char *exodus_file_name,
                                  std::deque<int> *active_block_id_list);
  
    //! read the nodes
  MDBErrorCode read_nodes();
  
    //! read block headers, containing info about element type, number, etc.
  MDBErrorCode read_block_headers(std::map<int,MDBEntitySequence*> &ids_n_blocks,
                                  std::deque<int> *active_block_id_list);
  
    //! read the element blocks
  MDBErrorCode read_elements(std::map<int,MDBEntitySequence*> &ids_n_blocks);
  
    //! read in the global element ids
  MDBErrorCode read_global_ids();
  
    //! read the nodesets into meshsets
  MDBErrorCode read_nodesets();
  
    //! read the sidesets (does nothing for now)
  MDBErrorCode read_sidesets();

    //! exodus file bound to this object
  int exodus_file();

    //! number of dimensions in this exo file
  int number_dimensions();

    //! given a meshset, return the block id, or -1 if it isn't a block
  int get_block_id(const MDB_MeshSet *this_ms);
  
    //! given a meshset, return the nodeset id, or -1 if it isn't a nodeset
  int get_nodeset_id(const MDB_MeshSet *this_ms);
  
    //! given a meshset, return the sideset id, or -1 if it isn't a sideset
  int get_sideset_id(const MDB_MeshSet *this_ms);

protected:

  //! contains the general information about a mesh
  struct ExodusMeshInfo
  {
//    Mesh* mesh;
    unsigned int num_dim;
    unsigned int num_nodes;
    unsigned int num_elements;
    unsigned int num_elementblocks;
    unsigned int num_sidesets;
    unsigned int num_nodesets;
  };

   
private:

    //! interface instance
  MDBImplGeneral *mdbImpl;
  
  int exodusFile;        // integer id of the exodus input file
  int numberDimensions;         // dimension of this exodus file (2 or 3)

    //! number of nodes in the current exoII file
  int numberNodes_loading;

    //! number of elements in the current exoII file
  int numberElements_loading;

    //! number of blocks in the current exoII file
  int numberElementBlocks_loading; 

    //! number of nodesets in the current exoII file
  int numberNodeSets_loading; 

    //! number of sidesets in the current exoII file
  int numberSideSets_loading; 

  int numberDimensions_loading;

    //! keep static sets of block/nodeset/sideset ids so we don't get
    //! duplicates
  static std::set<int> blockIds;
  static std::set<int> nodeSetIds;
  static std::set<int> sideSetIds;

    //! current block id offset
  static int blockIdOffset;

    //! current nodeset id offset
  static int nodesetIdOffset;

    //! current sideset id offset
  static int sidesetIdOffset;

    //! function to return current block number offset
  int get_offset(const int offset_type);

    //! compare the blocks in the exo file with those in the active list, removing
    //! any inactive ones
  MDBErrorCode remove_inactive_blocks(std::vector<int> &blocks_reading,
                                      std::deque<int> &active_blocks);
  
    //! map a character exodusII element type to a TSTT type & topology
  MDBErrorCode get_type(char *exo_element_type,
                        MDBEntityType &elem_type);
  
  int get_int_tag(const MDB_MeshSet *this_ms,
                  const TagHandle tag_id);
};

// inline functions
inline int ReadWriteExoII::exodus_file() 
{
   return exodusFile;
}

inline int ReadWriteExoII::number_dimensions() 
{
   return numberDimensions;
}

inline int ReadWriteExoII::get_offset(const int offset_type) 
{
  int result = -1;
  if (offset_type == 1) result = blockIdOffset;
  else if (offset_type == 2) result = sidesetIdOffset;
  else if (offset_type == 3) result = nodesetIdOffset;

  return result;
}

inline int ReadWriteExoII::get_block_id(const MDB_MeshSet *this_ms) 
{
  return get_int_tag(this_ms, mdbImpl->materialBC_tag());
}

inline int ReadWriteExoII::get_nodeset_id(const MDB_MeshSet *this_ms)
{
  return get_int_tag(this_ms, mdbImpl->dirichletBC_tag());
}

inline int ReadWriteExoII::get_sideset_id(const MDB_MeshSet *this_ms)
{
  return get_int_tag(this_ms, mdbImpl->neumannBC_tag());
}

#endif

