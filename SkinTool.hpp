//-------------------------------------------------------------------------
// Filename      : SkinTool.hpp 
//
// Purpose       : For a given set of elements, finds the exterior surface(s)
//
// Special Notes : 
//
// Creator       : Ray Meyers
//
// Date          : 11/5/98
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------

#ifndef SKIN_TOOL_HPP
#define SKIN_TOOL_HPP

#ifdef WIN32
#ifdef _DEBUG
// Turn off warnings that say a debugging identifier has been truncated.
// This warning comes up when using some STL containers.
#pragma warning(4 : 4786)
#endif
#endif

#include <deque>
#include <map>
#include "VerdeDefines.hpp"


class ElemRef;
class QuadElem;
class TriElem;
class ElementBlock;
class NodeRef;
class EdgeElem;

//! for a given set of elements, finds the exterior surface(s)
class SkinTool
{
public:
   
   //! constructor
   SkinTool();

   //! destructor
   ~SkinTool();
   
   //! adds a block for skin to be processed from
   void add_block(ElementBlock* block);

   //! removes a block from the skintool 
   void remove_block(ElementBlock* block);
  
   //! returns all quads still on the boundary of block
   void local_exterior_quads (std::deque<QuadElem*> &exterior_list);
   
   //! returns all tris still on the boundary of block
   void local_exterior_tris (std::deque<TriElem*> &exterior_list);

   //! returns all edges still on the boundary of block
   void local_edges (std::deque<EdgeElem*> &exterior_list);

   //! Resets the skintool, deletes any surface elements left in lists   
   void reset();

   //! skinned flag
   VerdeBoolean is_skinned() { return skinned; }

   //! resets the skintool and finds boundary of currently active blocks
   //void find_active_boundary();

   //! initializes parameters of the SkinTool
   void initialize(int number_elements);

   //-------------------Static Class-Wide Functions---------------//

   //! returns all quads on the boundary of model
   static void exterior_quads (std::deque<QuadElem*> &exterior_list);
   
   //! returns all tris  on the boundary of model
   static void exterior_tris (std::deque<TriElem*> &exterior_list);

   //! returns all edges and beam elements of model
   static void exterior_edges (std::deque<EdgeElem*> &exterior_list);
     
   //! returns the number of quads on the boundary
   static int number_exterior_quads();

   //! returns the number of tris on the boundary
   static int number_exterior_tris();
   
   //! returns the number of edges and  beams on the boundary
   static int number_exterior_edges();
  
   //! prints summary information about the current boundary
   static void print_summary();

   //! returns number of interior tris found during skinning
   static int number_interior_tris();

   //! returns the number of interior quads found during skinning
   static int number_interior_quads();

   //! searches current boundary lists for coincident elements
   static void find_coincident_elements();

   //! returns the coincident quads found in the skin
   static void coincident_quads(std::deque<QuadElem*> &quad_list);

   //! returns the number of coincident quads found during skinning
   static int number_coincident_quads();

   //! returns the coincident tris found in the skin
   static void coincident_tris(std::deque<TriElem*> &tri_list);

   //! returns the number of coincident tris found during skinning
   static int number_coincident_tris();
   
   //! Resets the static lists in the skintool
   static void clear_skin();

   //! finds tris on the boundary which are non-conformal to boundary quads
   static void find_non_conformal_tris();

   //! number of non-conformal tris
   static int number_non_conformal_tris();
  
   //! returns list of non-conformal tris
   static void non_conformal_tri_list(std::deque<TriElem*> &tri_list);
  
   //! finds nodes on skin
   static void find_coincident_nodes();

   //! finds all nodes in entire model 
   static void find_all_coincident_nodes();

   //! number of coincident nodes
   static int number_coincident_nodes();
  
   //! returns list of coincident nodes
   static void coincident_node_id_list(std::deque<int> &node_id_list);

   //! set whether to search for quads that share three nodes
   //! on the exterior quads or on exterior/interior quads
   static void set_quads_sharing_three_nodes_completeness( int check_level );
   static int get_quads_sharing_three_nodes_completeness();
   
   //! finds the quads that share three nodes
   static void find_quads_share_three_nodes();
   
   //! returns number of quads which share three nodes
   static int number_quads_share_three_nodes();

   //! returns list of quads which share three nodes
   static void quads_share_three_nodes_list(std::deque<QuadElem*> &quad_list);

   //! resets any calculated information
   static void reset_calculated();

private:

   //! deletes localQuadList and localTriList memory
   void delete_list_memory();

   //! enum used to signifying whether to add to skin or remove
   enum actionType { ADD, REMOVE };
 
   //! flag indicating block has been skinned
   VerdeBoolean skinned; 

   //! array of Quad Lists, each list contains quads based upon the
   //! lowest node id in the quad (hash like).
   std::deque<QuadElem*> **localQuadList;
 
   //! array of Tri containers, each container contains tris based upon the
   //! lowest node id in the tri (hash like).  Each tri in each list
   //! is indexed by the sum of the node ids of the tri.
   std::multimap<int, TriElem* >** localTriList;

   //! localQuadShellList stores the quads from shell or quad blocks. These are not
   //! processed with the main skin.
   std::deque<QuadElem*> localQuadShellList;

   //! localTriShellList stores the tris from shell or tri blocks. These are not
   //! processed with the main skin.
   std::deque<TriElem*> localTriShellList;

   //! localBeamEdgeList stores the edges from beam blocks. These are not
   //! processed with the main skin.
   std::deque<EdgeElem*> localBeamEdgeList;

   static void get_opposite_quads( std::deque<QuadElem*> & get_opposites, std::deque<QuadElem*>& opposites );

   static void get_opposite_tris( std::deque<TriElem*> & get_opposites, std::deque<TriElem*>& opposites );
  
   //! number of local interior quads of the local skin
   int numberLocalInteriorQuads;

   //! number of local interior tris of the local skin
   int numberLocalInteriorTris;


   //----------------Static Class-Wide Data---------------------//

   //! the maximum number of nodes for which a given list should store elements
   static int QUAD_NODES_PER_LIST;

   //! the maximum number of nodes for which a given container should store elements
   static int TRI_NODES_PER_LIST;

   //! static array of quads of entire model skin
   static std::deque<std::deque<QuadElem*> > quadList; 

   //! static array of tris of entire model skin
   static std::deque<std::multimap<int, TriElem*> > triList; 

   //! the number of interior quads found during skinning
   static int numberInteriorQuads;

   //! the number of interior tris found during skinning
   static int numberInteriorTris;

   //! list of coincident quads found by find_coincident_quads
   static std::deque<QuadElem*> coincidentQuadList;

   //! list of coincident tris found by find_coincident_tris
   static std::deque<TriElem*> coincidentTriList;

   //! list of coincident node ids found by find_coincident_nodes
   static std::deque<int> coincidentNodeIdList;

   //! list of tris which are non-conformal with quads on the boundary
   static std::deque<TriElem*> nonConformalTriList;

   //! list of quads which share three nodes
   static std::deque<QuadElem*> shareThreeNodesQuadList;

   //! flag for exterior or exterior/interior quads sharing three nodes checking
   //! 0 = none, 1 = exterior, 2 = all
   static int quad_three_node_check_level;

   //! finds (and deletes) any quad in quadList which has 
   //! connectivity opposite to the given quad 
   static VerdeStatus find_match(QuadElem *quad);

   //! finds (and deletes) any tri in triList which has 
   //! connectivity opposite to the given tri
   static VerdeStatus find_match(TriElem *tri);

   //! adds quad to skin 
   static void add_quad(QuadElem *new_quad);

   //! adds tri to skin 
   static void add_tri(TriElem *new_tri);

   //! finds whether two quads are coincident
   static VerdeBoolean coincident_quad(QuadElem *quad0,
                                       QuadElem *quad1);

   //! finds whether two tris are coincident
   static VerdeBoolean coincident_tri(TriElem *tri0,
                                      TriElem *tri1);

   //! gets the 6 quads of every hexes in the model and fills the quad list
   static void get_model_hex_quads(std::deque<QuadElem*>& quad_list);
 
   //! the number of lists allocated in the quadList 
   static int quadNumberLists;

   //! the number of containers allocated in the triList 
   static int triNumberLists;

//-------------------------------------------------------------//

   //! number of list in quadList
   int numberLocalQuadLists;

   //! number of list in triList
   int numberLocalTriLists;

   //! Processes an entire block of elements, adding or removing from skin
   void process_block(ElementBlock *block);

   //! Processes an entire skin of a block, adding or removing it from skin
   void process_global_skin(ElementBlock *block, enum actionType action);

   //! creates quads and/or tris for the element and processes
   void process_ref(ElemRef &new_elem);

   //! finds (and deletes) any quad in the block which has 
   //! connectivity opposite to the given quad
   VerdeStatus find_local_match(QuadElem *quad);

   //! finds (and deletes) any tri in the block which has 
   //! connectivity opposite to the given tri
   VerdeStatus find_local_match(TriElem *tri);

   //! adds a new quad to the appropriate localQuadList
   void add_local_quad(QuadElem *new_quad);

   //! adds a new tri to the appropriate localtriList
   void add_local_tri(TriElem *new_tri);

   //! initializes localQuadList based on number of elements in block
   void initialize_local_quad_list();

   //! initializes localTriList based on number of element in block
   void initialize_local_tri_list();


   double nodeDistanceTolerance;

   //! flag indicating that skin is in global skin
   bool mInGlobalSkin;

  
};

inline void SkinTool::set_quads_sharing_three_nodes_completeness( int check_level )
{
  quad_three_node_check_level = check_level;
}  

inline int SkinTool::get_quads_sharing_three_nodes_completeness()
{
  return quad_three_node_check_level;
}

inline int SkinTool::number_interior_quads()
{
   return numberInteriorQuads;
}

inline int SkinTool::number_interior_tris()
{
   return numberInteriorTris;
}

inline int SkinTool::number_coincident_quads()
{
   return coincidentQuadList.size();
}

inline void SkinTool::coincident_quads(std::deque<QuadElem*> &quad_list)
{
   quad_list = coincidentQuadList;
}

inline int SkinTool::number_coincident_tris()
{
   return coincidentTriList.size();
}

inline void SkinTool::coincident_tris(std::deque<TriElem*>& tri_list)
{
  for(unsigned int i=0; i<coincidentTriList.size(); i++)
    tri_list.push_back(coincidentTriList[i]);
}

inline int SkinTool::number_quads_share_three_nodes()
{
   return shareThreeNodesQuadList.size();
}

inline void SkinTool::quads_share_three_nodes_list(std::deque<QuadElem*> &quad_list)
{
  for(unsigned int i=0; i<shareThreeNodesQuadList.size(); i++)
    quad_list.push_back(shareThreeNodesQuadList[i]);
}

inline void SkinTool::add_block(ElementBlock *block)
{
  if(!skinned)
    process_block(block);
  process_global_skin(block,ADD);
}

inline void SkinTool::remove_block(ElementBlock *block)
{
  process_global_skin(block,REMOVE);
}

#endif
