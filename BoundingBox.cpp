//-------------------------------------------------------------------------
// Filename      : BoundBox.cc
//
// Purpose       : Determines if  x, y, or z coordinate are > xmax, ymax, zmax or < xmin, ymin, zmin
//                 and if so, replaces that max or min value....
//
// Special Notes :
//
// Creator       : Corey Ernst 
//
// Date          : 1/21/02 
//
// Owner         : Corey Ernst 
//-------------------------------------------------------------------------


#include "BoundingBox.hpp"
#include "ElemRef.hpp"
#include "VerdeVector.hpp"
#include "QuadElem.hpp"
#include "TriElem.hpp"
#include "EdgeElem.hpp"


BoundingBox::BoundingBox()
{
  used = VERDE_FALSE;   

  xmin = 0;
  ymin = 0;
  zmin = 0;

  xmax = 0;
  ymax = 0;
  zmax = 0;
   
  centroid.set(0,0,0);
}

BoundingBox::~BoundingBox()
{

}

void BoundingBox::reset()
{
  used = VERDE_FALSE;   

  xmin = 0;
  ymin = 0;
  zmin = 0;

  xmax = 0;
  ymax = 0;
  zmax = 0;

  centroid.set(0,0,0);
}


void BoundingBox::add_vector( VerdeVector vec )
{
   
  if(!used)
  {
    xmin = xmax = vec.x(); 
    ymin = ymax = vec.y(); 
    zmin = zmax = vec.z(); 
    used = VERDE_TRUE;   
  }

 
  // mins
  if( vec.x() < xmin )
    xmin = vec.x();
 
  if( vec.y() < ymin )
    ymin = vec.y();

  if( vec.z() < zmin )
    zmin = vec.z();

  // maxs
  if( vec.x() > xmax )
    xmax = vec.x();
 
  if( vec.y() > ymax )
    ymax = vec.y();

  if( vec.z() > zmax )
    zmax = vec.z();

}

VerdeVector BoundingBox::get_max()
{
  VerdeVector max_vec(xmax, ymax, zmax);

  return max_vec;
}

VerdeVector BoundingBox::get_min()
{
  VerdeVector min_vec(xmin, ymin, zmin);

  return min_vec;
}



void BoundingBox::add_edge( EdgeElem *elem )
{
  VerdeVector node;
   
  if(!used)
  {
    node = elem->coordinates(0);
    xmin = xmax = node.x(); 
    ymin = ymax = node.y(); 
    zmin = zmax = node.z(); 
    used = VERDE_TRUE;   
  }

 
  for(int i=2; i--; )
  {
    node = elem->coordinates(i);      

    // mins
    if( node.x() < xmin )
      xmin = node.x();
 
    if( node.y() < ymin )
      ymin = node.y();

    if( node.z() < zmin )
      zmin = node.z();

    // maxs
    if( node.x() > xmax )
      xmax = node.x();
 
    if( node.y() > ymax )
      ymax = node.y();

    if( node.z() > zmax )
      zmax = node.z();
  }

}

void BoundingBox::add_quad( QuadElem *elem )
{
  VerdeVector node;
   
  if(!used)
  {
    node = elem->coordinates(0);
    xmin = xmax = node.x(); 
    ymin = ymax = node.y(); 
    zmin = zmax = node.z(); 
    used = VERDE_TRUE;   
  }

 
  for(int i=4; i--; )
  {
    node = elem->coordinates(i);      

    // mins
    if( node.x() < xmin )
      xmin = node.x();
 
    if( node.y() < ymin )
      ymin = node.y();

    if( node.z() < zmin )
      zmin = node.z();

    // maxs
    if( node.x() > xmax )
      xmax = node.x();
 
    if( node.y() > ymax )
      ymax = node.y();

    if( node.z() > zmax )
      zmax = node.z();
  }

}

void BoundingBox::add_tri( TriElem *elem )
{
  VerdeVector node;
   
  if(!used)
  {
    node = elem->coordinates(0);
    xmin = xmax = node.x(); 
    ymin = ymax = node.y(); 
    zmin = zmax = node.z(); 
    used = VERDE_TRUE;   
  }

 
  for(int i=3; i--; )
  {
    node = elem->coordinates(i);      

    // mins
    if( node.x() < xmin )
      xmin = node.x();
 
    if( node.y() < ymin )
      ymin = node.y();

    if( node.z() < zmin )
      zmin = node.z();

    // maxs
    if( node.x() > xmax )
      xmax = node.x();
 
    if( node.y() > ymax )
      ymax = node.y();

    if( node.z() > zmax )
      zmax = node.z();
  }

}


void BoundingBox::add_element( ElemRef& element, int num_nodes)
{
  VerdeVector node;
  if(!used)
  {
    node = element.node_coordinates(0);
    xmin = xmax = node.x(); 
    ymin = ymax = node.y(); 
    zmin = zmax = node.z(); 
    used = VERDE_TRUE;   
  }
  for(int i=0; i<num_nodes; i++)
  {
    node = element.node_coordinates(i);
    
    // mins
    if( node.x() < xmin )
      xmin = node.x();
 
    if( node.y() < ymin )
      ymin = node.y();

    if( node.z() < zmin )
      zmin = node.z();

    // maxs
    if( node.x() > xmax )
      xmax = node.x();
 
    if( node.y() > ymax )
      ymax = node.y();

    if( node.z() > zmax )
      zmax = node.z();
  }

}




VerdeVector BoundingBox::get_centroid()
{
  centroid.x( (xmax+xmin)/2 );
  centroid.y( (ymax+ymin)/2 );
  centroid.z( (zmax+zmin)/2 );
   
  return centroid;
}
