#include "MDBDefines.h"
#include "MDBImplGeneral.hpp"
#include "ReadWriteExoII.hpp"
#include "MDB_MeshSet.hpp"

#include <iostream>

std::ostream& operator<<(std::ostream& out, const std::vector<int> & v)
{
  std::copy(v.begin(), v.end(), 
            std::ostream_iterator<int>(std::cout," "));
  return out;
}

int main(int argc, char *argv[]) 
{
    // check for file name on command line
  if (2 != argc) {
    std::cout << "Usage: " << argv[0] << " <exoII_file_name>" << std::endl;
    return 1;
  }

  MDBImplGeneral mdb;
  ReadWriteExoII reader(&mdb);
  MDBErrorCode result = reader.load_file(argv[1], NULL);

  std::cout << "Result of reading file = " 
            << (MDB_FAILURE == result ? "FAILURE." : "SUCCESS.")
            << std::endl;

    // print some data about the mesh
  int num_nodes = mdb.total_num_nodes();
  int num_elements = mdb.total_num_elements();
  std::cout << "Total number of nodes, elements read = " << num_nodes 
            << ", " << num_elements << std::endl;
  
    // get the nodeset meshsets and blocks
  std::vector<MDB_MeshSet*> blocks, nodesets, sidesets;
  std::vector<int> block_ids, nodeset_ids, sideset_ids;
  const std::set<MDB_MeshSet*> &gms = MDB_MeshSet::GlobalMeshSets();
  
  std::set<MDB_MeshSet*>::const_iterator
    this_it = gms.begin(),
    end_it = MDB_MeshSet::GlobalMeshSets().end();
  MDB_MeshSet *this_meshset;
  int bc_tag;
  for (; this_it != end_it; this_it++) {
      // get the next set
    this_meshset = *this_it;
    
    bc_tag = reader.get_block_id(this_meshset);
    if (-1 != bc_tag) {
      blocks.push_back(this_meshset);
      block_ids.push_back(bc_tag);
    }
    
      // same for nodeset tag
    bc_tag = reader.get_nodeset_id(this_meshset);
    if (-1 != bc_tag) {
      nodesets.push_back(this_meshset);
      nodeset_ids.push_back(bc_tag);
    }

      // same for sideset tag
    bc_tag = reader.get_sideset_id(this_meshset);
    if (-1 != bc_tag) {
      sidesets.push_back(this_meshset);
      sideset_ids.push_back(bc_tag);
    }
  }
  
  std::vector<int>::iterator set_it;
  std::cout << "Block numbers read: " << std::endl;
  if (!blocks.empty()) std::cout << block_ids << std::endl;
  else std::cout << "(no blocks)" << std::endl;

  std::cout << "Nodeset numbers read: " << std::endl;
  if (!nodesets.empty()) std::cout << nodeset_ids << std::endl;
  else std::cout << "(no nodesets)" << std::endl;

  std::cout << "Sideset numbers read: " << std::endl;
  if (!sidesets.empty()) std::cout << sideset_ids << std::endl;
  else std::cout << "(no sidesets)" << std::endl;

}
