//-------------------------------------------------------------------------
// Filename      : BoundingBox.hpp
//
// Purpose       : Determines if  x, y, or z coordinate are > xmax, ymax, zmax or < xmin, ymin, zmin 
//                 and if so, replaces that max or min value....
// Special Notes :
//
// Creator       : Corey Ernst
//
// Date          : 1/21/02 
//
// Owner         : Corey ERnst 
//-------------------------------------------------------------------------

#ifndef BOUNDING_BOX_HPP
#define BOUNDING_BOX_HPP

#include "VerdeDefines.hpp"
#include "VerdeVector.hpp"

class QuadElem;
class TriElem;
class EdgeElem;
class ElemRef;

class BoundingBox 
{
public:

   //! Destructor
   ~BoundingBox();
   
   //! Constructor
   BoundingBox();

   //! compares coords of element 
   void add_quad( QuadElem *elem );

   //! compares coords of element 
   void add_tri( TriElem *elem );

   //! compares coords of element 
   void add_edge( EdgeElem *elem );

   //! compares coords of element
   void add_element( ElemRef &element, int num_nodes );
  
   //! flag indicating box has been used
   VerdeBoolean is_used() { return used; }  
 
   //! resets the BoundingBox
   void reset();

   //! returns centroid of bounding box
   VerdeVector get_centroid();

   //! add vector 
   void add_vector( VerdeVector vec );

   //! get max vector
   VerdeVector get_max();

   //! get min vector
   VerdeVector get_min();

private:
   
   //! minimum x,y,z values
   double xmin;
   double ymin;
   double zmin;

   //! maximum x,y,z values
   double xmax;
   double ymax;
   double zmax;

   VerdeBoolean used;

   VerdeVector centroid;
};

#endif
