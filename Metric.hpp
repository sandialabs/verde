//-------------------------------------------------------------------------
// Filename      : Metric.hpp 
//
// Purpose       : Base Class for metric calculation classes in Verde.
//
// Special Notes : 
//
// Creator       : Ray Meyers
//
// Date          : 07/18/00
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------

#ifndef METRIC_HPP
#define METRIC_HPP

#include "VerdeVector.hpp"

//! base class for metric calculation classes in VERDE
class Metric
{  

public:

  //! destructor
  ~Metric () {};

  //! sets average size for the current element block being processed
  void set_average_size(double size);

  //! sets algebraic flag
  static void do_algebraic(VerdeBoolean value);
 
  //! sets diagnostic flag
  static void do_diagnostic (VerdeBoolean value);

  //! sets traditional flag
  static void do_traditional(VerdeBoolean value);

  //! gets algebraic flag
  static VerdeBoolean algebraic() { return doAlgebraic; }

  //! gets diagnostic flag
  static VerdeBoolean diagnostic() { return doDiagnostic; }

  //! gets traditional flag
  static VerdeBoolean traditional() { return doTraditional; }

 
protected:

  int elementShape;
  double aspectRatio;
  double elementSize;

  //- controls whether algebraic metrics are generated
  static VerdeBoolean doAlgebraic;

  //- controls whether diagnostic metrics are generated
  static VerdeBoolean doDiagnostic;

  //- controls whether traditional metrics are generated
  static VerdeBoolean doTraditional;

   //! Constructor
  Metric() { elementShape = 1; aspectRatio = 1.0; 
             elementSize = 0.0; metricTolerance = 1.0e-06; }

  //! tolerance used to decide if a value is outside of allowable range
  double metricTolerance;

  //! 2d common calc function
  double determinant( double m11, double m21, double m12, double m22 );
  //! 2d common calc function
  double norm_squared( double m11, double m21, double m12, double m22 );

  //! 2d common calc function
  void metric_matrix( double m11, double m21, double m12, double m22, 
                      double &g11, double &g12, double &g22 );

  //! 2d common calc function
  int skew_matrix( double g11, double g12, double g22, double det, 
                   double &q11, double &q21, double &q12, double &q22 );

  //! 2d common calc function
  void jacobian_matrix( double gm11, double gm12, double detm, 
                        double &am11, double &am21, double &am12, 
                        double &am22 );

  //! 2d common calc function
  void form_t( double m11, double m21, double m12, double m22, 
               double mw11, double mw21, double mw12, double mw22,
               double detmw, double &x11, double &x21, double &x12, 
               double &x22 );

  //! 3d common calc functions
  double determinant(VerdeVector &v1, VerdeVector &v2, VerdeVector &v3 );

  //! 3d common calc functions
  void form_q(VerdeVector a1, VerdeVector a2, VerdeVector a3,
              VerdeVector &q1, VerdeVector &q2, VerdeVector &q3);

  //! 3d common calc functions
  void form_t(VerdeVector a1, VerdeVector a2, VerdeVector a3,
              VerdeVector w1, VerdeVector w2, VerdeVector w3, 
              VerdeVector &t1, VerdeVector &t2, VerdeVector &t3);

  //! 3d common calc functions
  double skew_x(VerdeVector q1, VerdeVector q2, VerdeVector q3, 
                VerdeVector qw1, VerdeVector qw2, VerdeVector qw3);

  //! 3d common calc functions
  void inverse(VerdeVector x1, VerdeVector x2, VerdeVector x3, 
               VerdeVector &u1, VerdeVector &u2, VerdeVector &u3);

  //! 3d common calc functions
 void product(VerdeVector a1, VerdeVector a2, VerdeVector a3,
              VerdeVector b1, VerdeVector b2, VerdeVector b3,
              VerdeVector &c1, VerdeVector &c2, VerdeVector &c3);

  //! 3d common calc functions
 double norm_squared(VerdeVector x1, VerdeVector x2, VerdeVector x3);
   
};

// inline functions

inline void Metric::do_algebraic(VerdeBoolean value)
{
   doAlgebraic = value;
}

inline void Metric::do_diagnostic(VerdeBoolean value)
{
   doDiagnostic = value;
}

inline void Metric::do_traditional(VerdeBoolean value)
{
   doTraditional = value;
}

inline double Metric::determinant( double m11, double m21, double m12, double m22 )
{
   return m11*m22-m21*m12;
}

inline double Metric::norm_squared( double m11, double m21, double m12, double m22 ) 
{
    return m11*m11+m21*m21+m12*m12+m22*m22;
}

inline void Metric::jacobian_matrix( double gm11, double gm12, double detm, double &am11, double &am21, double &am12, double &am22 )
{
   double tmp = sqrt(gm11);
   if ( tmp <= VERDE_DBL_MIN ) {
      am11=0;
      am21=0;
      am12=0;
      am22=0;
   }
   else {
      am11=tmp;
      am21=0;
      am12=gm12/tmp;  
      am22=detm/tmp;
   }
}

inline void Metric::metric_matrix( double m11, double m21, double m12, double m22, double &gm11, double &gm12, double &gm22 )
{
   gm11 = m11*m11+m21*m21;
   gm12 = m11*m12+m21*m22;
   gm22 = m12*m12+m22*m22;
}

inline int Metric::skew_matrix( double gm11, double gm12, double gm22, double det, double &qm11, double &qm21, double &qm12, double &qm22 )
{
   double tmp = sqrt(gm11*gm22);
   if ( tmp <= VERDE_DBL_MIN ) { return VERDE_FAILURE; }

   qm11=1;
   qm21=0;
   qm12=gm12/tmp;
   qm22=det/tmp;
   return VERDE_SUCCESS;

}

inline void Metric::form_t( double m11, double m21, double m12, double m22, double mw11, double mw21, double mw12, double mw22, double detmw, double &xm11, double &xm21, double &xm12, double &xm22 )
{
  // Note: assumes detmw != 0
   xm11= (m11*mw22-m12*mw21)/detmw;
   xm21= (m21*mw22-m22*mw21)/detmw;
   xm12= (m12*mw11-m11*mw12)/detmw;
   xm22= (m22*mw11-m21*mw12)/detmw;
}

inline void Metric::set_average_size(double value)
{
  elementSize = value;
}

inline double Metric::determinant(VerdeVector &v1, VerdeVector &v2, 
                                  VerdeVector &v3 )
{
   return v1 % ( v2 * v3 );
}

inline void Metric::inverse(VerdeVector x1, VerdeVector x2, VerdeVector x3, VerdeVector &u1, VerdeVector &u2, VerdeVector &u3)
{
   double detx = determinant(x1,x2,x3);

   VerdeVector rx1, rx2, rx3;

   rx1.set(x1.x(),x2.x(),x3.x());
   rx2.set(x1.y(),x2.y(),x3.y());
   rx3.set(x1.z(),x2.z(),x3.z());

   u1 = rx2 * rx3;
   u2 = rx3 * rx1;
   u3 = rx1 * rx2;

   u1 /= detx;
   u2 /= detx;
   u3 /= detx;
}

inline void Metric::form_t(VerdeVector a1, VerdeVector a2, VerdeVector a3,
VerdeVector w1, VerdeVector w2, VerdeVector w3, VerdeVector &t1, VerdeVector &t2, VerdeVector &t3)
{
   VerdeVector x1, x2, x3;
   VerdeVector ra1, ra2, ra3;

   ra1.set(a1.x(),a2.x(),a3.x());
   ra2.set(a1.y(),a2.y(),a3.y());
   ra3.set(a1.z(),a2.z(),a3.z());

   inverse(w1,w2,w3,x1,x2,x3);

   t1.set( ra1 % x1, ra1 % x2, ra1 % x3 );
   t2.set( ra2 % x1, ra2 % x2, ra2 % x3 );
   t3.set( ra3 % x1, ra3 % x2, ra3 % x3 );

}

inline void Metric::form_q(VerdeVector a1, VerdeVector a2, VerdeVector a3,
VerdeVector &q1, VerdeVector &q2, VerdeVector &q3)
{
   double g11 = a1 % a1;
   double g12 = a1 % a2;
   double g13 = a1 % a3;
   double g22 = a2 % a2;
   double g23 = a2 % a3;
   double g33 = a3 % a3;

   double rtg11 = sqrt(g11);
   double rtg22 = sqrt(g22);
   double rtg33 = sqrt(g33);

   double cross = sqrt((a1 * a2) % (a1 * a2));

   double q11,q21,q31;
   double q12,q22,q32;
   double q13,q23,q33;

   q11=1;
   q21=0;
   q31=0;

   q12 = g12 / rtg11 / rtg22;
   q22 = cross / rtg11 / rtg22;
   q32 = 0;

   q13 = g13 / rtg11 / rtg33;
   q23 = ( g11*g23-g12*g13 )/ rtg11 / rtg33 / cross;
   q33 = ( a1 % ( a2 * a3 ) ) / rtg33 / cross;

   q1.set( q11, q21, q31 );
   q2.set( q12, q22, q32 );
   q3.set( q13, q23, q33 );

}

inline double Metric::norm_squared(VerdeVector x1, VerdeVector x2, VerdeVector x3)
{
   return x1%x1+x2%x2+x3%x3;
}

inline void Metric::product(VerdeVector a1, VerdeVector a2, VerdeVector a3,VerdeVector b1, VerdeVector b2, VerdeVector b3, VerdeVector &c1, VerdeVector &c2, VerdeVector &c3)
{
   VerdeVector x1, x2, x3;

   x1.set(a1.x(),a2.x(),a3.x());
   x2.set(a1.y(),a2.y(),a3.y());
   x3.set(a1.z(),a2.z(),a3.z());

   c1.set( x1 % b1, x2 % b1, x3 % b1 );
   c2.set( x1 % b2, x2 % b2, x3 % b2 );
   c3.set( x1 % b3, x2 % b3, x3 % b3 );
}

inline double Metric::skew_x(VerdeVector q1, VerdeVector q2, VerdeVector q3, VerdeVector qw1, VerdeVector qw2, VerdeVector qw3)
{
   double normsq1, normsq2, kappa;
   VerdeVector u1, u2, u3;
   VerdeVector x1, x2, x3;

   inverse(qw1,qw2,qw3,u1,u2,u3);
   product(q1,q2,q3,u1,u2,u3,x1,x2,x3);
   inverse(x1,x2,x3,u1,u2,u3);
   normsq1 = norm_squared(x1,x2,x3);
   normsq2 = norm_squared(u1,u2,u3);
   kappa = sqrt( normsq1 * normsq2 );

   double skew=0;
   if ( kappa > VERDE_DBL_MIN ) {
      skew = 3/kappa;
   }
   return skew;
}


#endif

