//- Class: VerdeMatrix
//- Description: This file defines the VerdeMatrix class.
//- Owner: Dan Goodrich
//- Checked by:

#include <assert.h>

#include "VerdeMatrix.hpp"
#include "VerdeVector.hpp"
#include "VerdeMessage.hpp"
#include "VerdeDefines.hpp"

VerdeMatrix::VerdeMatrix()
{
  numRows = 3;
  numCols = 3;
  
  matrixPtr = new double *[3];
  
  int ii;
  for( ii = 0; ii < 3; ii++ )
  {
    matrixPtr[ii] = new double [3];
  }
  
    // Initialize matrix to zeros.     
  for( ii = 0; ii < 3; ii++ )
     for( int jj = 0 ; jj < 3; jj++ )
        matrixPtr[ii][jj] = 0.0;
}

VerdeMatrix::VerdeMatrix( const int n, const int m )
{
  numRows = n;
  numCols = m;
  
  matrixPtr = new double *[n];
  
  int ii;
  for( ii = 0; ii < n; ii++ )
  {
    matrixPtr[ii] = new double [m];
  }
  
    // Initialize matrix to zeros.     
  for( ii = 0; ii < n; ii++ )
     for( int jj = 0 ; jj < m; jj++ )
        matrixPtr[ii][jj] = 0.0;
}


VerdeMatrix::VerdeMatrix( const int n )
{
  numRows = n;
  numCols = n;
  
  matrixPtr = new double *[n];
  
  int ii;
  for( ii = 0; ii < n; ii++ )
  {
    matrixPtr[ii] = new double [n];
  }
  
    // Initialize matrix to zeros.     
  for( ii = 0; ii < n; ii++ )
     for( int jj = 0 ; jj < n; jj++ )
        if ( ii == jj ) {
          matrixPtr[ii][jj] = 1.0;
        }
else {
          matrixPtr[ii][jj] = 0.0;
}
}


VerdeMatrix::VerdeMatrix( const VerdeVector vec1,
                          const VerdeVector vec2,
                          const VerdeVector vec3 )
{
  numRows = 3;
  numCols = 3;
  
  matrixPtr = new double *[3];
  
  for( int ii = 0; ii < 3; ii++ )
  {
    matrixPtr[ii] = new double [3];
  }
  
    // Initialize the matrix columns to the three vectors
  matrixPtr[0][0] = vec1.x();
  matrixPtr[1][0] = vec1.y();
  matrixPtr[2][0] = vec1.z();
  matrixPtr[0][1] = vec2.x();
  matrixPtr[1][1] = vec2.y();
  matrixPtr[2][1] = vec2.z();
  matrixPtr[0][2] = vec3.x();
  matrixPtr[1][2] = vec3.y();
  matrixPtr[2][2] = vec3.z();
}


VerdeMatrix::VerdeMatrix( const VerdeVector vec1,
                          const VerdeVector vec2 )
{             
  numRows = 3;
  numCols = 3;
  
  matrixPtr = new double *[3];
  
  for( int ii = 0; ii < 3; ii++ )
  {
    matrixPtr[ii] = new double [3];
  }
  
    // Initialize the matrix elements using otimes (outer product)
  matrixPtr[0][0] = vec1.x() * vec2.x();
  matrixPtr[1][0] = vec1.y() * vec2.x();
  matrixPtr[2][0] = vec1.z() * vec2.x();
  matrixPtr[0][1] = vec1.x() * vec2.y();
  matrixPtr[1][1] = vec1.y() * vec2.y();
  matrixPtr[2][1] = vec1.z() * vec2.y();
  matrixPtr[0][2] = vec1.x() * vec2.z();
  matrixPtr[1][2] = vec1.y() * vec2.z();
  matrixPtr[2][2] = vec1.z() * vec2.z();
}

VerdeMatrix::VerdeMatrix( const VerdeMatrix &matrix )
{
  numRows = matrix.num_rows();
  numCols = matrix.num_cols();
  
  matrixPtr = new double *[numRows];
  
  int ii;
  for( ii = 0; ii < numRows; ii++ )
  {
    matrixPtr[ii] = new double [numCols];
  }
  
  for( ii = 0; ii < numRows; ii++ )
  {
    for( int jj = 0; jj < numCols; jj++ )
       matrixPtr[ii][jj] = matrix.get( ii, jj );
  }
}



VerdeMatrix::~VerdeMatrix()
{
  for( int ii = 0; ii < numRows; ii++ )
  {
    delete [] ( matrixPtr[ii] );
  }
  delete [] ( matrixPtr );
}


void VerdeMatrix::print_matrix()
{
  for( int ii = 0; ii < numRows; ii++ )
  {
    for( int jj = 0; jj < numCols; jj++ )
       PRINT_INFO("%8.3lf", matrixPtr[ii][jj]);
    PRINT_INFO("\n");
  }
}


VerdeMatrix VerdeMatrix::operator=( VerdeMatrix matrix )
{
   for( int ii = 0; ii < matrix.num_rows(); ii++ )
   {
      for( int jj = 0; jj < matrix.num_cols(); jj++ )
      {
         matrixPtr[ii][jj] = matrix.get( ii, jj );
      }
   }
   
   return *this;
}


// Multiply this ( size NxM ) with the input matrix ( size MxL ).
// return matrix of size NxL
VerdeMatrix VerdeMatrix::operator*( VerdeMatrix matrix )
{
// Check that we can multiply them.
  assert( numCols == matrix.num_rows() );
  
  VerdeMatrix return_matrix( numRows, matrix.num_cols() );
  
  for( int ii = 0; ii < numRows; ii++ )
  {
    for( int jj = 0; jj < matrix.num_cols(); jj++ )
    {
      double temp = 0.0;
      for( int kk = 0; kk < numCols; kk++ )
      {
          //temp += matrixPtr[ii][kk] * matrix.get( kk, jj );
        temp += get( ii, kk ) * matrix.get( kk, jj );
      }
      return_matrix.set( ii, jj, temp );
    }
  }
  return return_matrix;
}



// multiply this times the input vector
VerdeVector VerdeMatrix::operator*( VerdeVector vector )
{
    // Check that we can multiply them.
  assert( numCols == 3 );
  
  double vec1[3];
  double vec2[3];
  
  vec2[0] = vector.x();
  vec2[1] = vector.y();
  vec2[2] = vector.z();
  
  for( int ii = 0; ii < numRows; ii++ )
  {
    vec1[ii] = 0.0;
    for( int jj = 0; jj < numCols; jj++ )
    {
      vec1[ii] += ( matrixPtr[ii][jj] * vec2[jj] );
    }
  }
  
  return VerdeVector( vec1[0],  vec1[1], vec1[2] );
}


// multiply this times the input scalar
VerdeMatrix VerdeMatrix::operator*( double val )
{
  VerdeMatrix matrix( numRows, numCols );
  
  for( int ii = 0; ii < numRows; ii++ )
  {
    for( int jj = 0; jj < numCols; jj++ )
    {
      matrix.set( ii, jj,( matrixPtr[ii][jj] * val ) );
    }
  }
  return matrix;
}

// multiply this times the input scalar
VerdeMatrix VerdeMatrix::operator/( double val )
{
  assert( val != 0 );
  VerdeMatrix matrix( numRows, numCols );
  
  for( int ii = 0; ii < numRows; ii++ )
  {
    for( int jj = 0; jj < numCols; jj++ )
    {
      matrix.set( ii, jj,( matrixPtr[ii][jj] / val ) );
    }
  }
  return matrix;
}


// subtract this ( size NxM ) with the input matrix ( size NxM ).
// return matrix of size NxM
VerdeMatrix VerdeMatrix::operator-( VerdeMatrix matrix )
{
  VerdeMatrix return_matrix( numRows, numCols );
  
  for( int ii = 0; ii < numRows; ii++ )
  {
    for( int jj = 0; jj < numCols; jj++ )
    {
      return_matrix.set( ii, jj, matrixPtr[ii][jj] -
                         matrix.get( ii, jj ));
    }
  }
  
  return return_matrix;
}

// add this ( size NxM ) with the input matrix ( size NxM ).
// return matrix of size NxM
VerdeMatrix VerdeMatrix::operator+( VerdeMatrix matrix )
{
   VerdeMatrix return_matrix( numRows, numCols );
   
   for( int ii = 0; ii < numRows; ii++ )
   {
      for( int jj = 0; jj < numCols; jj++ )
      {
         return_matrix.set( ii, jj, matrixPtr[ii][jj] +
                            matrix.get( ii, jj ));
      }
   }

   return return_matrix;
}


VerdeMatrix& VerdeMatrix::operator+=( const VerdeMatrix &matrix )
{
  for( int ii = 0; ii < numRows; ii++ )
  {
    for( int jj = 0; jj < numCols; jj++ )
    {
      matrixPtr[ii][jj] += matrix.get( ii, jj );
    }
  }
  return *this;
}

VerdeMatrix& VerdeMatrix::operator*=(const double multiplier)
{
  for( int ii = 0; ii < numRows; ii++ )
  {
    for( int jj = 0; jj < numCols; jj++ )
    {
      matrixPtr[ii][jj] *= multiplier;
    }
  }
  return *this;
}

// Inverts this matrix, if it is of size NxN, and a 3x3 or
// smaller.
VerdeMatrix VerdeMatrix::inverse()
{
  VerdeMatrix adj_matrix( numRows, numCols );
  double   det;
  
  if( numRows > 4 )
  {
//       rval = recipie_inverse();
//       return rval == VERDE_TRUE ? VERDE_TRUE : VERDE_FALSE;
    PRINT_INFO("Can't handle matrice's greater than 3x3 yet.\n");
  }
  
  det = determinant();
  assert( fabs(det) > VERDE_DBL_MIN );
  
  adj_matrix = adjoint();
  return adj_matrix * ( 1.0/det );
}

VerdeBoolean VerdeMatrix::diagonally_dominant()
{
  VerdeBoolean diag_dom = VERDE_TRUE;
  for( int ii = 0; ii < numRows; ii++ )
  {
    double sum=0.;
    for( int jj = 0; jj < numCols; jj++ )
    {
      if ( jj != ii ) {
         sum += fabs(  matrixPtr[ii][jj] );
      }
    }
    if ( fabs( matrixPtr[ii][ii] ) <= sum ) { 
       diag_dom = VERDE_FALSE;
       break;
    }
  }
  
  return diag_dom;
}

VerdeBoolean VerdeMatrix::positive_definite()
{

  if ( matrixPtr[0][0] <= 0. ) { return VERDE_FALSE; }

  double det2x2 = matrixPtr[0][0] * matrixPtr[1][1] - matrixPtr[1][0] * matrixPtr[0][1];
  if ( det2x2 <= 0. ) { return VERDE_FALSE; }

  if ( determinant() <= 0. ) { return VERDE_FALSE; }
  
  return VERDE_TRUE;
}

double VerdeMatrix::determinant()
{
  double det = 0.0;
  
  if( numRows == 1 )
     det = matrixPtr[0][0];
  else if( numRows == 2 )
     det = matrixPtr[0][0] * matrixPtr[1][1] - matrixPtr[0][1]
        * matrixPtr[1][0];
  else if (numRows == 3)
     det = matrixPtr[0][0] * matrixPtr[1][1] * matrixPtr[2][2] +
         matrixPtr[0][1] * matrixPtr[1][2] * matrixPtr[2][0] +
         matrixPtr[0][2] * matrixPtr[1][0] * matrixPtr[2][1] -
         matrixPtr[2][0] * matrixPtr[1][1] * matrixPtr[0][2] -
         matrixPtr[2][1] * matrixPtr[1][2] * matrixPtr[0][0] -
         matrixPtr[2][2] * matrixPtr[1][0] * matrixPtr[0][1];       
  else
  {
    for( int jj = 0; jj < numRows; jj++ )
    {
      det += ( matrixPtr[0][jj] * cofactor( 0, jj ) );
    }
  }
  return det;
}

double VerdeMatrix::cofactor( const int row, const int col )
{
  double c = 0.0;
  VerdeMatrix matrix_sub( numRows - 1, numCols -1 );
  
  sub_matrix( matrix_sub,row, col );
  
  c = matrix_sub.determinant();
  c = (row+col)%2 ? -1*c : c;
  
  return c;
}

VerdeMatrix VerdeMatrix::adjoint()
{
  VerdeMatrix matrix( numRows, numRows );
  
  for( int ii = 0; ii < numRows; ii++ )
  {
    for( int jj = 0; jj < numRows; jj++ )
    {
      matrix.set( ii, jj, cofactor( ii, jj ) );
    }
  }
  return matrix.transpose();
}


VerdeMatrix VerdeMatrix::transpose()
{
  VerdeMatrix return_matrix( numCols, numRows );
  
  for( int ii = 0; ii < numRows; ii++ )
  {
    for( int jj = 0; jj < numCols; jj++ )
    {
      return_matrix.set( jj, ii, matrixPtr[ii][jj] );
    }
  }
  
  return return_matrix;
}

void VerdeMatrix::sub_matrix( VerdeMatrix &matrix,
                                     const int row, const int col )
{
   // VerdeMatrix matrix( numRows - 1, numRows -1 );
  
  int kk = 0;
  
  for( int ii = 0; ii < numRows; ii++ )
  {
    if( ii != row )
    {
      int ll = 0;
      for( int jj = 0; jj < numCols; jj++ )
      {
        if( jj != col )
        {
          matrix.set( kk, ll, matrixPtr[ii][jj] );
          ll++;
        }
      }
      kk++;
    }
  }

  return;
  
  // return matrix;
}

double VerdeMatrix::inf_norm() 
{ 
    // infinity norm  = max_i sum_j  | A_ij |
  double matrix_norm = 0., row_norm, v;
  for ( int ii = 0; ii < numRows; ii++ ) {
    row_norm = 0.;
    for( int jj = 0; jj < numCols; jj++ )
    {
      v = fabs( get( ii, jj ) );
      row_norm += v;
    }
    if ( row_norm > matrix_norm )
       matrix_norm = row_norm;
  }
  return matrix_norm;
}

int VerdeMatrix::gauss_elim( VerdeVector &b )
{
    VerdeVector pivot;
    int ierr = factor( pivot );
    if ( ierr == 0 ) {  solve( b, pivot ); }
    return ierr;
}

int VerdeMatrix::factor( VerdeVector &pivot )
{
    double pvt[3];

    const int n=3;
    double s[3], tmp;

    int i,j;
    for ( i=0; i<n; i++ ) {
       s[i] = 0.0;
       for ( j=0; j<n; j++ ) {
          tmp = fabs( matrixPtr[i][j] );
          if ( tmp > s[i] ) {
             s[i] = tmp;
	  }
       }
       
       if ( s[i] == 0.0 ) { return(1); }
    
    }

    for ( int k=0; k<n-1; k++ ) {
       double ck = 0.0;
       int i0 = 0;
       for ( i=k; i<n; i++ ) {
          tmp = fabs( matrixPtr[i][k] / s[i] );
          if ( tmp > ck ) {
             ck = tmp;
             i0 = i;
	  }
       }

       pvt[k] = i0;
       if ( ck == 0.0 ) { return(1); }

       if ( i0 != k ) {
          for ( j=k; j<n; j++ ) {
             double swap = matrixPtr[i0][j];
             matrixPtr[i0][j] = matrixPtr[k][j];
             matrixPtr[k][j] = swap;
	  }
       }

       for ( i=k+1; i<n; i++ ) {
          double r = matrixPtr[i][k] / matrixPtr[k][k];
          matrixPtr[i][k] = r;
          for ( j=k+1; j<n; j++ ) {
             matrixPtr[i][j] -= r * matrixPtr[k][j];
	  }
       }

    }

    pivot.set( pvt[0], pvt[1], pvt[2] );
    return(0);
}

void VerdeMatrix::solve( VerdeVector &b, VerdeVector pivot )
{
  double rhs[3];
  rhs[0] = b.x();
  rhs[1] = b.y();
  rhs[2] = b.z();

  double pvt[3];
  pvt[0] = pivot.x();
  pvt[1] = pivot.y();
  pvt[2] = pivot.z();

  int j;
  const int n=3;
  for ( int k=0; k<n-1; k++ ) {
     j=(int)pvt[k];
     if ( j != k ) {
        double swap = rhs[k];
        rhs[k] = rhs[j];
        rhs[j] = swap;
     }

     for ( int i=k+1; i<n; i++ ) {
        rhs[i] -= matrixPtr[i][k] * rhs[k];
     }

  }

  rhs[n-1] /= matrixPtr[n-1][n-1];

  for ( int i=n-2; i>-1; i-- ) {
     double sum=0.;
     for ( j=i+1; j<n; j++ ) {
        sum += matrixPtr[i][j] * rhs[j];
     }
     rhs[i] = ( rhs[i] - sum ) / matrixPtr[i][i];
  }

  b.set( rhs[0], rhs[1], rhs[2] );

}






// Here is the recipe for inverting a NxM matrice.
// I did not spend the time trying to convert it to Verde style.
// Matrix is a double**
// Vector is a double*
// Scalar is a double
// int mxiRecipieInverse(Matrix M1, Matrix M2, int N)
// {
//   Matrix           M1_loc, M2_loc, M3_loc;
//   Vector           col, copycol;
//   Scalar           d;
//   int              i, j, *indx;

//   indx = ((int*)malloc((unsigned long)N*sizeof(int)))-1;

//   M1_loc  = mxInitMatrixR(1, N, 1, N);
//   M2_loc  = mxInitMatrixR(1, N, 1, N);
//   M3_loc  = mxInitMatrixR(1, N, 1, N);
//   col     = mxInitVectorR(1, N);
//   copycol = mxInitVectorR(1, N);
//   if (M1_loc == NULL || M2_loc == NULL || col == NULL || indx == NULL) 
//     return 0;
//   if (M3_loc == NULL || copycol == NULL)
//     printf("\n\nCannot use Improve function\n\n");

//             /* copy the input matrix */
//   for( i = 1; i <= N; i++ )
//     for( j = 1; j <= N; j++ ) {
//       M1_loc[i][j] = M1[i-1][j-1];
//       if (M3_loc != NULL)
//         M3_loc[i][j] = M1[i-1][j-1];
//       M2_loc[i][j] = 0.0;
//     }

//   if (!mxiLudcmp(M1_loc, N, indx, &d)) return 0;
//   for (j=1; j<=N; j++) {
//     for (i=1; i<=N; i++) {
//       col[i]=0.0;
//       if (copycol != NULL)
//         copycol[i] = 0.0;
//     }
//     col[j] = 1.0;
//     if (copycol != NULL) copycol[j] = 1.0;
//     mxiLubksb(M1_loc, N, indx, col);
//     if (copycol != NULL && M3_loc != NULL)
//       if (!mxiImprove(M3_loc, M1_loc, N, indx, copycol, col))
//         return 0;
//     for (i=1; i<=N; i++) M2_loc[i][j]=col[i];
//   }
//             /* copy the inverted matrix */
//   for( i = 1; i <= N; i++ )
//     for( j = 1; j <= N; j++ )
//       M2[i-1][j-1] = M2_loc[i][j];

//   mxFreeMatrixR(M1_loc, 1, N, 1, N);
//   mxFreeMatrixR(M2_loc, 1, N, 1, N);
//   mxFreeMatrixR(M3_loc, 1, N, 1, N);
//   mxFreeVectorR(col, 1, N);
//   mxFreeVectorR(copycol, 1, N);
//   free(indx+1);
//   return 1;
// } /* mxiRecipieInverse */

