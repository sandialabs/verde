//- Class: VerdeMatrix
//-
//- Description: This file defines the VerdeMatrix class which is a
//- standard NxM Matrix. All relevant arithmetic operators are
//- overloaded so VerdeMatrix's can be used similar to built-in types.
//-
//- Owner: Dan Goodrich
//- Checked by:


#ifndef VERDEMATRIX_HPP
#define VERDEMATRIX_HPP

#include "VerdeVector.hpp"

//! standard NxM matrix class
class VerdeMatrix {
public:
  
   //! Default constructor. 3x3 init to zeros.
   VerdeMatrix();
   
   //! Initializes NxM matrix to zeros.
   VerdeMatrix( const int n, const int m);
  
   //! Constructor: create 3x3 Matrix from three vector components
   //!  where each vector forms one column of the matrix.
   VerdeMatrix( const VerdeVector vec1, const VerdeVector vec2,
                const VerdeVector vec3);

   //! Constructor: create 3x3 Matrix from two vectors (otimes)
   //!  ( vec1 otimes vec2 )_ij = vec1_i * vec2_j
   VerdeMatrix( const VerdeVector vec1, const VerdeVector vec2 );

   //! Constructor: create n x n Identity matrix
   VerdeMatrix( const int n );

   //! Copy Constructor
   VerdeMatrix( const VerdeMatrix &matrix);
    
   //! destructor
   ~VerdeMatrix();
   
   //! returns number of Rows 
   int num_rows() const { return numRows; }
  
   //! returns number of Cols 
   int num_cols() const { return numCols; }
   
   //! Prints matrix
   void print_matrix();

   //! Change Matrix component row n, column m to val.
   void set(const int n, const int m, const double val)
    { matrixPtr[n][m] = val; }

   //! Gets the values of the matrix at position (n,m)
   double get( int n, int m ) const { return matrixPtr[n][m]; }
   
   //! overloaded = operator, assert if incompatible element sizes
   VerdeMatrix operator = ( VerdeMatrix matrix );
   //! overloaded * operator, assert if incompatible element sizes
   VerdeMatrix operator * ( VerdeMatrix matrix );
   //! overloaded * operator, assert if incompatible element sizes
   VerdeVector operator * ( VerdeVector vector );
   //! overloaded * operator, assert if incompatible element sizes
   VerdeMatrix operator * ( double val );
   //! overloaded / operator, assert if incompatible element sizes
   VerdeMatrix operator / ( double val );
   //! overloaded + operator, assert if incompatible element sizes
   VerdeMatrix operator + ( VerdeMatrix matrix );
   //! overloaded - operator, assert if incompatible element sizes
   VerdeMatrix operator - ( VerdeMatrix matrix );

   //! Scale the matrix by a linear factor: {this = this * multiplier},
   VerdeMatrix& operator+=( const VerdeMatrix &matrix );
   VerdeMatrix& operator*=(const double multiplier);
  
   //! inverts 1X1, 2X2, or 3X3 matrix, asserts if singular 
   VerdeMatrix inverse(); 
   
   //! returns if diagonally dominant
   VerdeBoolean diagonally_dominant(); 
   
   //! returns tru if matrix is positive definite 
   VerdeBoolean positive_definite(); 

   //! calculates the determinant
   double determinant();
  
   //! calculates the infinity norm 
   double      inf_norm(); 
   
   //! calculates the cofactor 
   double      cofactor( const int row, const int col );
   
   //! calculates the adjoint 
   VerdeMatrix adjoint();
   
   //! calculates the transpose
   VerdeMatrix transpose();
   
   //! calculates a sub matrix
   void sub_matrix( VerdeMatrix &matrix, const int row, const int col );

   //! routine to perform Gaussian elimination with pivoting and scaling
   //! on 3x3 matrix
   int gauss_elim( VerdeVector &b );

   //! routine to perform Gaussian elimination with pivoting and scaling
   //! on 3x3 matrix
   int factor( VerdeVector &pivot );

   //! routine to perform Gaussian elimination with pivoting and scaling
   //! on 3x3 matrix
   void solve( VerdeVector &b, VerdeVector pivot );  
      
private:

   double **matrixPtr;
   int numRows;
   int numCols;
   
};
#endif



