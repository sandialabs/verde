//- Class: VerdeVector
//- Description: This file defines the VerdeVector class.
//- Owner: Greg Sjaardema
//- Checked by:

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef CANT_USE_STD_IO
#include <iostream.h>
#else
#include <iostream>
#endif

#ifdef CANT_USE_STD_IO
#include <iomanip.h>
#else
#include <iomanip>
#endif


#define TESTVECTOR

#include "VerdeVector.hpp"
#include "VerdeMessage.hpp"
//#include "GeometryTool.hpp"
#ifndef TESTVECTOR
#include "VerdeNode.hpp"
#include "RefVertex.hpp"
#include "VerdeMessage.hpp"
#include "RefEdge.hpp"
#endif

#undef PI
#ifdef M_PI
const double PI = M_PI;
#else
const double PI = 3.14159265358979323846;
#endif
const double TWO_PI = 2.0 * PI;


#ifndef TESTVECTOR
VerdeVector::VerdeVector(const VerdeNode* nodePtr1,
			 const VerdeNode* nodePtr2)
{
   xVal = nodePtr2->node_x() - nodePtr1->node_x();
   yVal = nodePtr2->node_y() - nodePtr1->node_y();
   zVal = nodePtr2->node_z() - nodePtr1->node_z();
}

VerdeVector::VerdeVector(const VerdeNode* node)
{
   xVal = node->node_x();
   yVal = node->node_y();
   zVal = node->node_z();
}

VerdeVector::VerdeVector(const RefVertex *vertex_ptr)
{
  VerdeVector coords = ((RefVertex*)vertex_ptr)->coordinates();
  xVal = coords.x();
  yVal = coords.y();
  zVal = coords.z();
}

VerdeVector::VerdeVector(const RefVertex* vertexPtr1,
			 const RefVertex* vertexPtr2)
{
  VerdeVector coords1 = ((RefVertex*)vertexPtr1)->coordinates();
  VerdeVector coords2 = ((RefVertex*)vertexPtr2)->coordinates();
  
  xVal = coords2.x() - coords1.x();
  yVal = coords2.y() - coords1.y();
  zVal = coords2.z() - coords1.z();
}
#endif

void VerdeVector::set(const double x,
		      const double y,
		      const double z)
{
   xVal = x;
   yVal = y;
   zVal = z;
}

void VerdeVector::set(const double xyz[3])
{
   xVal = xyz[0];
   yVal = xyz[1];
   zVal = xyz[2];
}


#ifndef TESTVECTOR
void VerdeVector::set(const VerdeNode* nodePtr1,
		      const VerdeNode* nodePtr2)
{
   xVal = nodePtr2->node_x() - nodePtr1->node_x();
   yVal = nodePtr2->node_y() - nodePtr1->node_y();
   zVal = nodePtr2->node_z() - nodePtr1->node_z();
}

void VerdeVector::set(const VerdeNode* node)
{
   xVal = node->node_x();
   yVal = node->node_y();
   zVal = node->node_z();
}

void VerdeVector::set(const RefVertex *vertex_ptr)
{
  VerdeVector coords = ((RefVertex*)vertex_ptr)->coordinates();
  xVal = coords.x();
  yVal = coords.y();
  zVal = coords.z();
}
#endif

void VerdeVector::print() const
{
  PRINT_INFO("<%10.6fi %10.6fj %10.6fk>",xVal, yVal, zVal);
}

/*
std::ostream& operator<<(std::ostream& o, const VerdeVector& vector)
{
	long old_format = o.setf(std::ios::scientific, std::ios::floatfield);
  int  old_precision = o.precision(4);
  o << "<"
    << std::setw(11) << vector.x() << "i  "
    << std::setw(11) << vector.y() << "j  "
    << std::setw(11) << vector.z() << "k> ";
  o.precision(old_precision);
  o.setf(old_format,  std::ios::floatfield);
  return o;
}
*/

void VerdeVector::normalize()
{
   double mag = sqrt(xVal*xVal + yVal*yVal + zVal*zVal);
   if (mag != 0)
   {
      xVal = xVal / mag;
      yVal = yVal / mag;
      zVal = zVal / mag;
   }
}

VerdeVector &VerdeVector::length(const double new_length)
{
  double length = this->length();
  xVal *= new_length / length;
  yVal *= new_length / length;
  zVal *= new_length / length;
  return *this;
}

double VerdeVector::length() const
{
   return( sqrt(xVal*xVal + yVal*yVal + zVal*zVal) );
}

double VerdeVector::distance_between(const VerdeVector& test_vector)
{
   double x = xVal - test_vector.xVal;
   double y = yVal - test_vector.yVal;
   double z = zVal - test_vector.zVal;
   
   return(sqrt(x*x + y*y + z*z));
}


double VerdeVector::length_squared() const
{
  return( xVal*xVal + yVal*yVal + zVal*zVal );
}

double operator%(const VerdeVector &vector1,
		 const VerdeVector &vector2)
{
   return( vector1.xVal * vector2.xVal +
           vector1.yVal * vector2.yVal +
           vector1.zVal * vector2.zVal );
}

double VerdeVector::interior_angle(const VerdeVector &otherVector)
{
   double cosAngle=0, angleRad=0, len1=0, len2=0;

   if (((len1 = this->length()) > 0) && ((len2 = otherVector.length()) > 0))
      cosAngle = (*this % otherVector)/(len1 * len2);
   else
   {
     // If we have a zero length, lets return a zero.
     // 
     return 0.0;
     //assert(len1 > 0);
     //assert(len2 > 0);
   }

   if ((cosAngle > 1.0) && (cosAngle < 1.0001))
   {
      cosAngle = 1.0;
      angleRad = acos(cosAngle);
   }
   else if (cosAngle < -1.0 && cosAngle > -1.0001)
   {
       cosAngle = -1.0;
       angleRad = acos(cosAngle);
   }
   else if (cosAngle >= -1.0 && cosAngle <= 1.0)
      angleRad = acos(cosAngle);
   else
   {
     assert(cosAngle < 1.0001 && cosAngle > -1.0001);
   }

   return( (angleRad * 180.) / PI );
}


//-  Vector assignment operator.

VerdeVector& VerdeVector::operator*=(const double multiplier)
{
    xVal *= multiplier;
    yVal *= multiplier;
    zVal *= multiplier;
    return *this;
}

VerdeVector& VerdeVector::operator/=(const double multiplier)
{
    xVal /= multiplier;
    yVal /= multiplier;
    zVal /= multiplier;
    return *this;
}

VerdeVector operator~(const VerdeVector &vec)
{
  double mag = sqrt(vec.xVal*vec.xVal +
		    vec.yVal*vec.yVal +
		    vec.zVal*vec.zVal);
  
  VerdeVector temp = vec;
  if (mag != 0.0)
    {
      temp /= mag;
    }
  return temp;
}


VerdeVector VerdeVector::operator-()
{
  return VerdeVector(-xVal, -yVal, -zVal);
}


VerdeVector operator+(const VerdeVector &vector1,
			    const VerdeVector &vector2)
{
  return VerdeVector(vector1) += vector2;
}

VerdeVector operator-(const VerdeVector &vector1,
			    const VerdeVector &vector2)
{
  return VerdeVector(vector1) -= vector2;
}

VerdeVector operator*(const VerdeVector &vector1,
			    const VerdeVector &vector2)
{
  return VerdeVector(vector1) *= vector2;
}

VerdeVector operator*(const VerdeVector &vector1,
			    const double scalar)
{
  return VerdeVector(vector1) *= scalar;
}

VerdeVector operator*(const double scalar,
			    const VerdeVector &vector1)
{
  return VerdeVector(vector1) *= scalar;
}

VerdeVector operator/(const VerdeVector &vector1,
			    const double scalar)
{
  return VerdeVector(vector1) /= scalar;
}

int operator==(const VerdeVector &v1, const VerdeVector &v2)
{
  return (v1.xVal == v2.xVal && v1.yVal == v2.yVal && v1.zVal == v2.zVal);
}

int operator!=(const VerdeVector &v1, const VerdeVector &v2)
{
  return (v1.xVal != v2.xVal || v1.yVal != v2.yVal || v1.zVal != v2.zVal);
}

VerdeVector interpolate(const double param, const VerdeVector &v1,
			const VerdeVector &v2)
{
  //- Interpolate between two vectors. Returns (1-param)*v1 + param*v2
  VerdeVector temp = (1.0 - param) * v1;
  temp += param * v2;
  return temp;
}

void VerdeVector::xy_to_rtheta()
{
  //careful about overwriting
  double r_ = length();
  double theta_ = atan2( y(), x() );
  if (theta_ < 0.0) 
    theta_ += TWO_PI;

  r( r_ );
  theta( theta_ );
}
  
void VerdeVector::rtheta_to_xy()
{
  //careful about overwriting
  double x_ =  r() * cos( theta() );
  double y_ =  r() * sin( theta() );

  x( x_ );
  y( y_ );
}

void VerdeVector::rotate(double angle, double )
{
  xy_to_rtheta();
  theta() += angle;
  rtheta_to_xy();
}


void VerdeVector::blow_out(double gamma, double rmin)
{
  // if gamma == 1, then 
  // map on a circle : r'^2 = sqrt( 1 - (1-r)^2 )
  // if gamma ==0, then map back to itself
  // in between, linearly interpolate
  xy_to_rtheta();
//  r() = sqrt( (2. - r()) * r() ) * gamma  + r() * (1-gamma);
  assert(gamma > 0.0);
  // the following limits should really be roundoff-based
  if (r() > rmin*1.001 && r() < 1.001) {
    r() = rmin + pow(r(), gamma) * (1.0 - rmin);
  }
  rtheta_to_xy();
}

void VerdeVector::reflect_about_xaxis(double, double )
{
  yVal = -yVal;
}

void VerdeVector::scale_angle(double gamma, double )
{
  const double r_factor = 0.3;
  const double theta_factor = 0.6;

  xy_to_rtheta();

  // if neary 2pi, treat as zero - some near zero stuff strays due to roundoff
  if (theta() > TWO_PI - 0.02)
    theta() = 0;
  // the above screws up on big sheets - need to overhaul at the sheet level
    

  if ( gamma < 1 ) {
    //squeeze together points of short radius so that
    //long chords won't cross them
    theta() += (PI-theta())*(1-gamma)*theta_factor*(1-r());

    //push away from center of circle, again so long chords won't cross
    r( (r_factor + r()) / (1 + r_factor) );

    //scale angle by gamma
    theta() *= gamma;
  }
  else {
    //scale angle by gamma, making sure points nearly 2pi are treated as zero
    double new_theta = theta() * gamma;
    if ( new_theta < 2.5 * PI || r() < 0.2) 
      theta( new_theta );
  }
  rtheta_to_xy();
}

double VerdeVector::vector_angle_quick(const VerdeVector& vec1, 
			       	 const VerdeVector& vec2)
{
  //- compute the angle between two vectors in the plane defined by this vector
  // build yAxis and xAxis such that xAxis is the projection of
  // vec1 onto the normal plane of this vector

  // NOTE: vec1 and vec2 are Vectors from the vertex of the angle along
  //       the two sides of the angle.
  //       The angle returned is the right-handed angle around this vector
  //       from vec1 to vec2.

  // NOTE: vector_angle_quick gives exactly the same answer as vector_angle below
  //       providing this vector is normalized.  It does so with two fewer
  //       cross-product evaluations and two fewer vector normalizations.
  //       This can be a substantial time savings if the function is called
  //       a significant number of times (e.g Hexer) ... (jrh 11/28/94)
  // NOTE: vector_angle() is much more robust. Do not use vector_angle_quick()
  //       unless you are very sure of the safety of your input vectors.

  VerdeVector ry = (*this) * vec1;
  VerdeVector rx = ry * (*this);

  double x = vec2 % rx;
  double y = vec2 % ry;

  double angle;
  assert(x != 0.0 || y != 0.0);

  angle = atan2(y, x);

  if (angle < 0.0)
  {
    angle += TWO_PI;
  }
  return angle;
}

VerdeVector vectorRotate(const double angle,
		         const VerdeVector &normalAxis,
		         const VerdeVector &referenceAxis)
{
  // A new coordinate system is created with the xy plane corresponding
  // to the plane normal to the normal axis, and the x axis corresponding to
  // the projection of the reference axis onto the normal plane.  The normal
  // plane is the tangent plane at the root point.  A unit vector is
  // constructed along the local x axis and then rotated by the given
  // ccw angle to form the new point.  The new point, then is a unit
  // distance from the global origin in the tangent plane.
  
  double x, y;
  
  // project a unit distance from root along reference axis
  
  VerdeVector yAxis = normalAxis * referenceAxis;
  VerdeVector xAxis = yAxis * normalAxis;
  yAxis.normalize();
  xAxis.normalize();
  
  x = cos(angle);
  y = sin(angle);
  
  xAxis *= x;
  yAxis *= y;
  return VerdeVector(xAxis + yAxis);
}

double VerdeVector::vector_angle(const VerdeVector &vector1,
				 const VerdeVector &vector2) const
{
  
  // This routine does not assume that any of the input vectors are of unit
  // length. This routine does not normalize the input vectors.
  // Special cases:
  //     If the normal vector is zero length:
  //         If a new one can be computed from vectors 1 & 2:
  //             the normal is replaced with the vector cross product
  //         else the two vectors are colinear and zero or 2PI is returned.
  //     If the normal is colinear with either (or both) vectors
  //         a new one is computed with the cross products (and checked again).


  // Check for zero length normal vector
  VerdeVector normal = *this;
  double normal_lensq = normal.length_squared();
  double len_tol = 0.0000001;
  if( normal_lensq <= len_tol )
  {
      // null normal - make it the normal to the plane defined by vector1
      // and vector2. If still null, the vectors are colinear so check
      // for zero or 180 angle.
      normal = vector1 * vector2;
      normal_lensq = normal.length_squared();
      if( normal_lensq <= len_tol )
      {
           double cosine = vector1 % vector2;
           if( cosine > 0.0 ) return 0.0;
           else               return PI;
      }
  }
  
  //Trap for normal vector colinear to one of the other vectors. If so,
  //use a normal defined by the two vectors.
  double dot_tol = 0.985;
  double dot = vector1 % normal;
  if( dot * dot >= vector1.length_squared() * normal_lensq * dot_tol )
  {
      normal = vector1 * vector2;
      normal_lensq = normal.length_squared();

      //Still problems if all three vectors were colinear
      if( normal_lensq <= len_tol )
      {
          double cosine = vector1 % vector2;
          if( cosine >= 0.0 ) return 0.0;
          else                return PI;
      }
  }
  else
  {
      //The normal and vector1 are not colinear, now check for vector2
      dot = vector2 % normal;
      if( dot * dot >= vector2.length_squared() * normal_lensq * dot_tol )
      {
          normal = vector1 * vector2;
      }
  }
 
  // Assume a plane such that the normal vector is the plane's normal.
  // Create yAxis perpendicular to both the normal and vector1. yAxis is
  // now in the plane. Create xAxis as the perpendicular to both yAxis and
  // the normal. xAxis is in the plane and is the projection of vector1
  // into the plane.
  
  normal.normalize();
  VerdeVector yAxis = normal;
  yAxis *= vector1;
  double y = vector2 % yAxis;
  //  yAxis memory slot will now be used for xAxis
  yAxis *= normal;
  double x = vector2 % yAxis;


//  assert(x != 0.0 || y != 0.0);
  if( x == 0.0 && y == 0.0 )
  {
      return 0.0;
  }
  double angle = atan2(y, x);

  if (angle < 0.0)
  {
      angle += TWO_PI;
  }
  return angle;
}
/*
VerdeBoolean VerdeVector::compare( const VerdeVector &vectorPtr2,
                                   double tolerance,
                                   double tolerance_factor ) const
{
  if (tolerance == VERDE_DBL_MAX )
    tolerance = GEOMETRY_RESABS;
  if ( tolerance_factor == VERDE_DBL_MAX )
    tolerance_factor = GeometryTool::get_geometry_factor();
  
  else 
  
   if ( 
       ( fabs (this->x() - vectorPtr2.x()) < 
                                     (tolerance * tolerance_factor) ) &&
       ( fabs (this->y() - vectorPtr2.y()) < 
                                     (tolerance * tolerance_factor) ) &&
       ( fabs (this->z() - vectorPtr2.z()) < 
                                     (tolerance * tolerance_factor) )
      )
   {
      return VERDE_TRUE;
   }
   
   return VERDE_FALSE;
}
*/
void VerdeVector::orthogonal_vectors( VerdeVector &vector2, 
                                     VerdeVector &vector3 )
{
   double x[3];
   unsigned short i=0;
   unsigned short imin=0;
   double rmin = 1.0E20;
   unsigned short iperm1[3];
   unsigned short iperm2[3];
   unsigned short cont_flag = 1;
   double vec1[3], vec2[3];
   double rmag;

   // Copy the input vector and normalize it
   VerdeVector vector1 = *this;
   vector1.normalize();
   
   // Initialize perm flags
   iperm1[0] = 1; iperm1[1] = 2; iperm1[2] = 0;
   iperm2[0] = 2; iperm2[1] = 0; iperm2[2] = 1;
   
   // Get into the array format we can work with
   vector1.get_xyz( vec1 );
   
   while (i<3 && cont_flag ) {
      if (fabs(vec1[i]) < 1e-6) {
         vec2[i] = 1.0;
         vec2[iperm1[i]] = 0.0;
         vec2[iperm2[i]] = 0.0;
         cont_flag = 0;
      }
      
      if (fabs(vec1[i]) < rmin) {
         imin = i;
         rmin = fabs(vec1[i]);
      }
      ++i;
   }
   
   if (cont_flag) {
      x[imin] = 1.0;
      x[iperm1[imin]] = 0.0;
      x[iperm2[imin]] = 0.0;
      
      // Determine cross product
      vec2[0] = vec1[1] * x[2] - vec1[2] * x[1];
      vec2[1] = vec1[2] * x[0] - vec1[0] * x[2];
      vec2[2] = vec1[0] * x[1] - vec1[1] * x[0];

      // Unitize
      rmag = sqrt(vec2[0]*vec2[0] + vec2[1]*vec2[1] + vec2[2]*vec2[2]);
      vec2[0] /= rmag;
      vec2[1] /= rmag;
      vec2[2] /= rmag;

   }

   // Copy 1st orthogonal vector into VerdeVector vector2
   vector2.set( vec2 );
   
   // Cross vectors to determine last orthogonal vector
   vector3 = vector1 * vector2;

   return;

}

//- Find next point from this point using a direction and distance
void VerdeVector::next_point( const VerdeVector &direction,
                             double distance, VerdeVector& out_point )
{
   VerdeVector my_direction = direction;
   my_direction.normalize();
   
   // Determine next point in space
   out_point.x( xVal + (distance * my_direction.x()) );     
   out_point.y( yVal + (distance * my_direction.y()) );     
   out_point.z( zVal + (distance * my_direction.z()) ); 
   
   return;
}

#ifdef TESTVECTOR
/*
void main()
{
  VerdeVector a(1.0, 1.0, 1.0);
  VerdeVector b = a;
  PRINT_INFO("\n a = "); a.print();
  PRINT_INFO("\n b = "); b.print();
  double len1 = a.length();
  PRINT_INFO("\n Length of "); a.print();
  PRINT_INFO(" = %f", len1);
  double len2 = b.length_squared();
  b.length(10);
  double len3 = b.length();
  PRINT_INFO("\n Length of "); b.print();
  PRINT_INFO(" = %f", len3);
}
*/
#endif
