//- Class: VerdeVector
//-
//- Description: This file defines the VerdeVector class which is a
//- standard three-dimensional vector. All relevant arithmetic
//- operators are overloaded so VerdeVectors can be used similar to
//- built-in types.
//-
//- Owner: Greg Sjaardema
//- Checked by: Randy Lober, January 94
//- Version: $Id: 

#ifndef VERDEVECTOR_HPP
#define VERDEVECTOR_HPP

//#include "GeometryDefines.hpp"
#include "VerdeDefines.hpp"

//#include <iostream>

class VerdeNode;
class RefVertex;
class RefEdge;

class VerdeVector;
// a pointer to some function that transforms the point, taking a double parameter
// e.g. blow_out, rotate, and scale_angle
typedef void ( VerdeVector::*transform_function )( double gamma,
						   double gamma2);

//! standard three-dimensional vector class
class VerdeVector {

public:
  
   //! default constructor.
   VerdeVector();  //! Default constructor.
  
   //! constructor: create vector from three components
   VerdeVector(const double x, const double y, const double z);

   //! constructor: create vector from tuple
   VerdeVector( const double xyz[3] );

#ifndef TESTVECTOR
   //! constructor: create vector from coordinates of node {nodePtr}
   VerdeVector(const VerdeNode *nodePtr);
  
   //! constructor: create vector from node {nodePtr1} to node {nodePtr2}
   VerdeVector(const VerdeNode *nodePtr1, const VerdeNode* nodePtr2);
#endif
  
   //! copy Constructor
   VerdeVector(const VerdeVector& copy_from);  //! Copy Constructor
  
   //! constructor: create vector from coordinates of a vertex {vertex_ptr}
   VerdeVector(const RefVertex *vertex_ptr);
  
   //! constructor: create vector from vertex {vertexPtr1} to vertex {vertexPtr2}
   VerdeVector(const RefVertex *vertexPtr1, const RefVertex *vertexPtr2);

   //! change vector components to {x}, {y}, and {z}
   void set(const double x, const double y, const double z);

   //! change vector components to xyz[0], xyz[1], xyz[2]
   void set( const double xyz[3] );
  
#ifndef TESTVECTOR
   //! change vector coordinates to the coordinates of node {nodePtr}
   void set(const VerdeNode *nodePtr);
  
   //! change vector coordinates to the vector from node {nodePtr1} to
   //! node {nodePtr2}
   void set(const VerdeNode *nodePtr1, const VerdeNode* nodePtr2);

#endif
  
   //! change vector coordinates to the coordinates of a vertex {vertex_ptr}
   void set(const RefVertex *vertex_ptr);
  
   //! return x component of vector
   double x() const;
    
   //! return y component of vector
   double y() const;
  
   //! return z component of vector
   double z() const;

   //! get x, y, z components
   void get_xyz( double &x, double &y, double &z );

   //! get xyz tuple
   void get_xyz( double xyz[3] );

   //! return r component of vector, if (r,theta) format
   double &r();

   //! return theta component of vector, if (r,theta) format
   double &theta();
  
   //! set x component of vector
   void x( const double x ); 

   //! set y component of vector
   void y( const double y );

   //! set z component of vector
   void z( const double z ); 

   //! set r component of vector, if (r,theta) format
   void r( const double x ); 
  
   //! set theta component of vector, if (r,theta) format
   void theta( const double y ); 

   //! convert from cartesian to polar coordinates, just 2d for now
   //! theta is in [0,2 PI)
   void xy_to_rtheta();
  
   //! convert from  polar to cartesian coordinates, just 2d for now
   void rtheta_to_xy();

   //! tranform_function.
   //! transform  (x,y) to (r,theta) to (r,gamma*theta) to (x',y')
   //! plus some additional scaling so long chords won't cross short ones
   void scale_angle(double gamma, double );

   //! transform_function
   //! blow points radially away from the origin, 
   void blow_out(double gamma, double gamma2 = 0.0);
  
   //! transform function.
   //! transform  (x,y) to (r,theta) to (r,theta+angle) to (x',y')
   void rotate(double angle, double );

   //! dummy argument to make it a transform function
   void reflect_about_xaxis(double dummy, double );

   //! print the vector.  Uses printf, output formatted as:
   //! {<xcomp ycomp zcomp>}.
   void print() const;
  
   //! print the vector to stream.  Output formatted as:
   //! {<xcompi +/- ycompj +/- zcompk> } (no newline, space at end).
   //friend std::ostream& operator<<(std::ostream& o, const VerdeVector& vector);
  
   //! normalize (set magnitude equal to 1) vector
   void normalize();
  
   //! change length of vector to {new_length}. Can be used to move a
   //! location a specified distance from the origin in the current
   //! orientation.
   VerdeVector& length(const double new_length);
  
   //! calculate the length of the vector.
   //! Use {length_squared()} if only comparing lengths, not adding.
   double length() const;

   //! calculate the distance from the head of one vector
   //  to the head of the test_vector.
   double distance_between(const VerdeVector& test_vector);

   //! calculate the distance from the head of one vector
   //  to the head of the test_vector along the RefEdge.
   double distance_between(const VerdeVector& test_vector, RefEdge* test_edge);

   //! Calculate the squared length of the vector.
   //! Faster than {length()} since it eliminates the square root if
   //! only comparing other lengths.
   double length_squared() const;

   //! calculate the interior angle: acos((a%b)/(|a||b|))
   //! Returns angle in degrees.
   double interior_angle(const VerdeVector &otherVector);

   //! calculate the interior angle between the projections of
   //! {vec1} and {vec2} onto the plane defined by the {this} vector.
   //! The angle returned is the right-handed angle around the {this}
   //! vector from {vec1} to {vec2}. Angle is in radians.
   double vector_angle_quick(const VerdeVector& vec1, const VerdeVector& vec2);

   //! compute the angle between the projections of {vector1} and {vector2}
   //! onto the plane defined by *this. The angle is the
   //! right-hand angle, in radians, about *this from {vector1} to {vector2}.
   double vector_angle( const VerdeVector &vector1,
		       const VerdeVector &vector2) const;

   //! transform this vector to a perpendicular one, leaving
   //! z-component alone. Rotates clockwise about the z-axis by pi/2.
   void perpendicular_z();

   //! finds 2 (arbitrary) vectors that are orthogonal to this one
   void orthogonal_vectors( VerdeVector &vector2, VerdeVector &vector3 );
   
   //! finds the next point in space based on *this* point (starting point), 
   //! a direction and the distance to extend in the direction. The
   //! direction vector need not be a unit vector.  The out_point can be
   //! "*this" (i.e., modify point in place).
   void next_point( const VerdeVector &direction, double distance, 
                   VerdeVector& out_point );

   //! compare two vectors to see if they are spatially equal.  The default
   //! tolerance is for geometry.  But other tolerances and factors may be
   //! sent in.  The GEOMETRY_RESABS currently equals 1.0E-6 and the
   //! GEOMETRY_TOLERANCE_FACTORE currently equals 500.000.  If the vectors
   //! are not geometrically realated, VERDE_RESABS should be used for the
   //! tolerance.
   VerdeBoolean compare( const VerdeVector &vectorPtr2,
                        double tolerance,
                        double tolerance_factor ) const;
  

#if 0
   //! assignment
   VerdeVector& operator=(const VerdeVector &vec);
#endif
  
   //! compound Assignment: addition: {this = this + vec}
   VerdeVector&  operator+=(const VerdeVector &vec);
  
   //! compound Assignment: subtraction: {this = this - vec}
   VerdeVector& operator-=(const VerdeVector &vec);

   //! compound Assignment: cross product: {this = this * vec},
   //! non-commutative
   VerdeVector& operator*=(const VerdeVector &vec);

   //! compound Assignment: multiplication: {this = this * scalar}
   VerdeVector& operator*=(const double scalar);
  
   //! compound Assignment: division: {this = this / scalar}
   VerdeVector& operator/=(const double scalar);
  
   //! unary negation.
   VerdeVector operator-();
  
   //! normalize. Returns a new vector which is a copy of {vec},
   //! scaled such that {|vec|=1}. Uses overloaded bitwise NOT operator.
   friend VerdeVector operator~(const VerdeVector &vec);
  
   //! vector addition
   friend VerdeVector operator+(const VerdeVector &v1, 
				     const VerdeVector &v2);
  
   //! vector subtraction
   friend VerdeVector operator-(const VerdeVector &v1, 
				     const VerdeVector &v2);
  
   //! vector cross product, non-commutative
   friend VerdeVector operator*(const VerdeVector &v1, 
				     const VerdeVector &v2);
  
   //! vector * scalar
   friend VerdeVector operator*(const VerdeVector &v1, const double sclr);
  
   //! scalar * vector
   friend VerdeVector operator*(const double sclr, const VerdeVector &v1);
  
   //! dot product
   friend double operator%(const VerdeVector &v1, const VerdeVector &v2);
  
   //! vector / scalar
   friend VerdeVector operator/(const VerdeVector &v1, const double sclr);
  
   //! equality operator
   friend int operator==(const VerdeVector &v1, const VerdeVector &v2);
   
   //! inequality operator
   friend int operator!=(const VerdeVector &v1, const VerdeVector &v2);
  
   //! interpolate between two vectors. Returns (1-param)*v1 + param*v2
   friend VerdeVector interpolate(const double param, const VerdeVector &v1,
				 const VerdeVector &v2);
  
private:
  
   //! x component of vector.
   double xVal; 

   //! y component of vector.
   double yVal;

   //! z component of vector.
   double zVal;
};

   //! A new coordinate system is created with the xy plane corresponding
   //! to the plane normal to {normalAxis}, and the x axis corresponding to
   //! the projection of {referenceAxis} onto the normal plane.  The normal
   //! plane is the tangent plane at the root point.  A unit vector is
   //! constructed along the local x axis and then rotated by the given
   //! ccw angle to form the new point.  The new point, then is a unit
   //! distance from the global origin in the tangent plane.
   //! {angle} is in radians.
   VerdeVector vectorRotate(const double angle, 
			 const VerdeVector &normalAxis,
			 const VerdeVector &referenceAxis);



inline double VerdeVector::x() const { return xVal; }
inline double VerdeVector::y() const { return yVal; }
inline double VerdeVector::z() const { return zVal; }
inline void VerdeVector::get_xyz(double xyz[3])
{
   xyz[0] = xVal;
   xyz[1] = yVal;
   xyz[2] = zVal;
}
inline void VerdeVector::get_xyz(double &x, double &y, double &z)
{
   x = xVal; 
   y = yVal; 
   z = zVal;
}
inline double &VerdeVector::r() { return xVal; }
inline double &VerdeVector::theta() { return yVal; }
inline void VerdeVector::x( const double x ) { xVal = x; }
inline void VerdeVector::y( const double y ) { yVal = y; }
inline void VerdeVector::z( const double z ) { zVal = z; }
inline void VerdeVector::r( const double x ) { xVal = x; }
inline void VerdeVector::theta( const double y ) { yVal = y; }
inline VerdeVector&
VerdeVector::operator+=(const VerdeVector &vector)
{
    xVal += vector.xVal;
    yVal += vector.yVal;
    zVal += vector.zVal;
    return *this;
}

inline VerdeVector&
VerdeVector::operator-=(const VerdeVector &vector)
{
    xVal -= vector.xVal;
    yVal -= vector.yVal;
    zVal -= vector.zVal;
    return *this;
}

inline VerdeVector&
VerdeVector::operator*=(const VerdeVector &vector)
{
   double xcross, ycross, zcross;
   xcross = yVal * vector.zVal - zVal * vector.yVal;
   ycross = zVal * vector.xVal - xVal * vector.zVal;
   zcross = xVal * vector.yVal - yVal * vector.xVal;
   xVal = xcross;
   yVal = ycross;
   zVal = zcross;
   return *this;
}

inline VerdeVector::VerdeVector(const VerdeVector& copy_from)
{xVal=copy_from.xVal; yVal=copy_from.yVal; zVal= copy_from.zVal;}

inline VerdeVector::VerdeVector() : xVal(0), yVal(0), zVal(0) {}

inline VerdeVector::VerdeVector(const double xIn,
				const double yIn,
				const double zIn)
{ xVal=xIn; yVal=yIn; zVal=zIn;}

inline void VerdeVector::perpendicular_z()
{
  double temp = x();
  x( y() );
  y( -temp );
}

#endif

