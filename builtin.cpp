#include <ctype.h>
#include <errno.h>
#include "idr.hpp"
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "VerdeMessage.hpp"
//#include "RefEntityFactory.hpp"

//#include "Model.hpp"
//#include "MRefVertex.hpp"
//#include "VerdeNode.hpp"
//#include "MRefEdge.hpp"
//#include "RefEntityName.hpp"

// [added by MBS for MeshToolProxy]
//#include "MTPEdge.hpp"

#ifndef streq
#define streq(a,b)  (0 == strcmp(a,b))
#endif
#ifndef RAND_MAX
#include <limits.h>
#define RAND_MAX INT_MAX
#endif

#ifndef PI
#define PI  3.141592653589793238462643
#endif

#if defined(VMS) || defined(_hpux_) || defined(sun)
#define HYPOT(x,y) hypot(x,y)
#else
#define HYPOT(x,y) do_hypot(x,y, 0.0, 0.0)
#endif

#define d2r(x)  ((x)*PI/180.)
#define r2d(x)  ((x)*180./PI)

#ifndef max
#define max(x,y) (x) > (y) ? (x) : (y)
#define min(x,y) (x) < (y) ? (x) : (y)
#endif

#ifndef abs
#define abs(x)   ((x) < 0 ? -(x): (x))
#endif

#if defined(sun)
#define LOG1P(x)	log1p(x)
#else
#define LOG1P(x)	log(1.0 + (x))
#endif

#undef EXTERN
#define EXTERN 
#include "builtin.h"
#undef EXTERN

static void inner_tolower(char* string)
{
  char *p = string;
  while (*p != '\0')
  {
    *p = tolower (*p);
    p++;
  }
}

static void inner_toupper(char* string)
{
  char *p = string;
  while (*p != '\0')
  {
    *p = toupper (*p);
    p++;
  }
}

/* DO_INT:  Calculate integer nearest to zero from value */
double do_int (double x, double, double, double)
{
  double temp;
  errno = 0;
  temp = (double) (x < 0 ? -floor (-(x)) : floor (x));
  MATH_ERROR ("int");
  return (temp);
}

/* DO_DIST: Calculate distance between point 1 at (x1,y1) and
 *          point 2 at (x2,y2).
 */
double do_dist (double x1, double y1, double x2, double y2)
{
  double temp;
  errno = 0;
  temp = HYPOT ((x1 - x2), (y1 - y2));
  MATH_ERROR ("hypot");
  return (temp);
}

/* DO_ANGLE: Calculate angle (radians) between vector 1 at (0,0; x1,y1) and
 *          vector 2 at (0,0; x2,y2).
 */
double do_angle (double x1, double y1, double x2, double y2)
{
  double temp;
  temp = ((x1 * x2) + (y1 * y2)) / (HYPOT (x1, y1) * HYPOT (x2, y2));
  errno = 0;
  temp = acos (temp);
  MATH_ERROR ("angle");
  return (temp);
}

/* DO_ANGLE: Calculate angle (degrees) between vector 1 at (0,0; x1,y1) and
 *          vector 2 at (0,0; x2,y2).
 */
double do_angled (double x1, double y1, double x2, double y2)
{
  double temp;
  temp = ((x1 * x2) + (y1 * y2)) / (HYPOT (x1, y1) * HYPOT (x2, y2));
  errno = 0;
  temp = r2d (acos (temp));
  MATH_ERROR ("angled");
  return (temp);
}

/* DO_HYPOT: calcluate sqrt(p^2 + q^2)     */
/* Algorithm from "More Programming Pearls," Jon Bentley */
/* Accuracy: 6.5 digits after 2 iterations,
 *           20  digits after 3 iterations,
 *           62  digits after 4 iterations.
 */

double do_hypot (double x, double y, double, double)
{
  double r;
  int i;

  x = abs (x);
  y = abs (y);
  if (x < y)
    {
      r = y;
      y = x;
      x = r;
    }
  if (x == 0.0)
    return (y);

  for (i = 0; i < 3; i++)
    {
      r = y / x;
      r *= r;
      r /= (4.0 + r);
      x += (2.0 * r * x);
      y *= r;
    }
  return (x);
}

double do_max (double x, double y, double, double)
{
  double temp;
  errno = 0;
  temp = max (x, y);
  MATH_ERROR ("max");
  return (temp);
}

double do_min (double x, double y, double, double)
{
  double temp;
  errno = 0;
  temp = min (x, y);
  MATH_ERROR ("min");
  return (temp);
}

double do_d2r (double x, double, double, double)
{
  return (d2r (x));
}

double do_r2d (double x, double, double, double)
{
  return (r2d (x));
}

double do_sind (double x, double, double, double)
{
  double temp;
  errno = 0;
  temp = sin (d2r (x));
  MATH_ERROR ("sind");
  return (temp);
}

double do_sin (double x, double, double, double)
{
  double temp;
  errno = 0;
  temp = sin (x);
  MATH_ERROR ("sin");
  return (temp);
}

double do_cosd (double x, double, double, double)
{
  double temp;
  errno = 0;
  temp = cos (d2r (x));
  MATH_ERROR ("cosd");
  return (temp);
}

double do_cos (double x, double, double, double)
{
  double temp;
  errno = 0;
  temp = cos (x);
  MATH_ERROR ("cos");
  return (temp);
}

double do_tand (double x, double, double, double)
{
  double temp;
  errno = 0;
  temp = tan (d2r (x));
  MATH_ERROR ("tand");
  return (temp);
}

double do_tan (double x, double, double, double)
{
  double temp;
  errno = 0;
  temp = tan (x);
  MATH_ERROR ("tan");
  return (temp);
}

double do_atan2d (double x, double y, double, double)
{
  double temp;
  errno = 0;
  temp = r2d (atan2 (x, y));
  MATH_ERROR ("atan2d");
  return (temp);
}

double do_atan2 (double x, double y, double, double)
{
  double temp;
  errno = 0;
  temp = atan2 (x, y);
  MATH_ERROR ("atan2");
  return (temp);
}

double do_atand (double x, double, double, double)
{
  double temp;
  errno = 0;
  temp = r2d (atan (x));
  MATH_ERROR ("atand");
  return (temp);
}

double do_atan (double x, double, double, double)
{
  double temp;
  errno = 0;
  temp = atan (x);
  MATH_ERROR ("atan");
  return (temp);
}

double 
do_asind (double x, double, double, double)
{
  double temp;
  errno = 0;
  temp = r2d (asin (x));
  MATH_ERROR ("asind");
  return (temp);
}

double 
do_asin (double x, double, double, double)
{
  double temp;
  errno = 0;
  temp = asin (x);
  MATH_ERROR ("asin");
  return (temp);
}

double 
do_acosd (double x, double, double, double)
{
  double temp;
  errno = 0;
  temp = r2d (acos (x));
  MATH_ERROR ("acosd");
  return (temp);
}

double 
do_acos (double x, double, double, double)
{
  double temp;
  errno = 0;
  temp = acos (x);
  MATH_ERROR ("acos");
  return (temp);
}

/* do_rand(double x) returns a random double in the range 0<= do_rand <= x */
double 
do_rand (double xl, double xh, double, double)
{
  double temp;
  errno = 0;
  temp = xl + (xh - xl) * ((double) rand () / (double) RAND_MAX);
  MATH_ERROR ("rand");
  return (temp);
}

double 
do_sign (double x, double y, double, double)
{
  double temp;
  errno = 0;
  temp = (y) >= 0 ? fabs (x) : -fabs (x);
  MATH_ERROR ("sign");
  return (temp);
}

double 
do_dim (double x, double y, double, double)
{
  double temp;
  errno = 0;
  temp = x - (min (x, y));
  MATH_ERROR ("dim");
  return (temp);
}

double 
do_fabs (double x, double, double, double)
{
  double temp;
  errno = 0;
  temp = fabs (x);
  MATH_ERROR ("fabs");
  return (temp);
}

double 
do_ceil (double x, double, double, double)
{
  double temp;
  errno = 0;
  temp = ceil (x);
  MATH_ERROR ("ceil");
  return (temp);
}

double do_cosh (double x, double, double, double)
{
  double temp;
  errno = 0;
  temp = cosh (x);
  MATH_ERROR ("cosh");
  return (temp);
}

double do_exp (double x, double, double, double)
{
  double temp;
  errno = 0;
  temp = exp (x);
  MATH_ERROR ("exp");
  return (temp);
}

double do_floor (double x, double, double, double)
{
  double temp;
  errno = 0;
  temp = floor (x);
  MATH_ERROR ("floor");
  return (temp);
}

double 
do_fmod (double x, double y, double, double)
{
  double temp;
  errno = 0;
  temp = fmod (x, y);
  MATH_ERROR ("fmod");
  return (temp);
}

double 
do_log (double x, double, double, double)
{
  double temp;
  errno = 0;
  temp = log (x);
  MATH_ERROR ("log");
  return (temp);
}

double 
do_log10 (double x, double, double, double)
{
  double temp;
  errno = 0;
  temp = log10 (x);
  MATH_ERROR ("log10");
  return (temp);
}

double 
do_sinh (double x, double, double, double)
{
  double temp;
  errno = 0;
  temp = sinh (x);
  MATH_ERROR ("sinh");
  return (temp);
}

double 
do_sqrt (double x, double, double, double)
{
  double temp;
  errno = 0;
  temp = sqrt (x);
  MATH_ERROR ("sqrt");
  return (temp);
}

double 
do_tanh (double x, double, double, double)
{
  double temp;
  errno = 0;
  temp = tanh (x);
  MATH_ERROR ("tanh");
  return (temp);
}

double 
do_polarX (double rad, double ang, double, double)
{
  return (rad * cos (d2r (ang)));
}

double 
do_polarY (double rad, double ang, double, double)
{
  return (rad * sin (d2r (ang)));
}

static double cof[] =
{76.18009173, -86.50532033, 24.01409822,
 -1.231739516, 0.120858003e-2, -0.536382e-5};
double 
do_lgamma (double val, double, double, double)
{
#define STP	2.50662827465
  double x, tmp, ser;
  int j;

  x = val - 1.0;
  tmp = x + 5.5;
  tmp = (x + 0.5) * log (tmp) - tmp;
  ser = 1.0;
  for (j = 0; j < 6; j++)
    {
      x += 1.0;
      ser += (cof[j] / x);
    }
  return (tmp + log (STP * ser));
}

double 
do_juldayhms (double mon, double day, double year,
	      double h, double mi, double se)
{
  long m = (long)mon, d = (long)day, y = (long)year;
  long c, ya, j;
  double seconds = h * 3600.0 + mi * 60 + se;

  if (m > 2)
    m -= 3;
  else
    {
      m += 9;
      --y;
    }
  c = y / 100L;
  ya = y - (100L * c);
  j = (146097L * c) / 4L + (1461L * ya) / 4L + (153L * m + 2L) / 5L + d + 1721119L;
  if (seconds < 12 * 3600.0)
    {
      j--;
      seconds += 12.0 * 3600.0;
    }
  else
    {
      seconds = seconds - 12.0 * 3600.0;
    }
  return (j + (seconds / 3600.0) / 24.0);
}

double 
do_julday (double mon, double day, double year, double)
{
  return do_juldayhms (mon, day, year, 0.0, 0.0, 0.0);
}

double 
do_log1p (double x, double, double, double)
{
  return LOG1P (x);
}

double 
do_acosh (double x, double, double, double)
{
  double t;
  if (x > 1.0e20)
    return (LOG1P (x) + log (2.0));
  else
    {
      t = sqrt (x - 1.0);
      return (LOG1P (t * (t + sqrt (x + 1))));
    }
}
double 
do_asinh (double x, double, double, double)
{
  double s, t;
  if (1.0 + x * x == 1.0)
    return (x);
  if (sqrt (1.0 + x * x) == 1.0)
    return (do_sign (1.0, x, 0.0, 0.0) * (LOG1P (x) + log (2.0)));
  else
    {
      t = abs (x);
      s = 1.0 / t;
      return (do_sign (1.0, x, 0.0, 0.0) *
	      LOG1P (t + t / (s + sqrt (1 + s * s))));
    }
}
double 
do_atanh (double x, double, double, double)
{
  double z;
  z = do_sign (0.5, x, 0.0, 0.0);
  x = do_sign (x, 1.0, 0.0, 0.0);
  x = x / (1.0 - x);
  return (z * LOG1P (x + x));
}


/*   -- commented out for verde

double do_Vx(int vertex_id)
{
  MRefVertex *vertex = Model::instance()->get_mref_vertex(vertex_id);
  if (!vertex)
  {
    PRINT_ERROR("Vertex %d does not exist.\n",  vertex_id  );
    return 0.0;
  }
  return vertex->coordinates().x();
}

double do_Vy(int vertex_id)
{
  MRefVertex *vertex = Model::instance()->get_mref_vertex(vertex_id);
  if (!vertex)
  {
    PRINT_ERROR("Vertex %d does not exist.\n",  vertex_id  );
    return 0.0;
  }
  return vertex->coordinates().y();
}

double do_Vz(int vertex_id)
{
  MRefVertex *vertex = Model::instance()->get_mref_vertex(vertex_id);
  if (!vertex)
    {
      PRINT_ERROR("Vertex %d does not exist.\n",  vertex_id  );
      return 0.0;
    }
  return vertex->coordinates().z();
}

double do_Nx(int node_id)
{
  VerdeNode *entityPtr = VerdeNode::find_node(node_id);
  if (!entityPtr)
    {
      PRINT_ERROR("Node %d does not exist.\n", node_id  );
      return 0.0;
    }
return entityPtr->node_x();
}

double do_Ny(int node_id)
{
  VerdeNode *entityPtr = VerdeNode::find_node(node_id);
  if (!entityPtr)
    {
      PRINT_ERROR("Node %d does not exist.\n", node_id  );
      return 0.0;
    }
return entityPtr->node_y();
}

double do_Nz(int node_id)
{
  VerdeNode *entityPtr = VerdeNode::find_node(node_id);
  if (!entityPtr)
    {
      PRINT_ERROR("Node %d does not exist.\n", node_id  );
      return 0.0;
    }
return entityPtr->node_z();
}

double do_IntNum (int edge_id)
{
  MRefEdge *entityPtr = Model::instance()->get_mref_edge(edge_id);
  if (!entityPtr)
  {
    PRINT_ERROR("Curve %d does not exist.\n",  edge_id  );
    return 0.0;
  }
  return entityPtr->mesh_tool()->interval_count();
}

double do_IntSize (int edge_id)
{
  MRefEdge *entityPtr = Model::instance()->get_mref_edge(edge_id);
  if (!entityPtr)
  {
    PRINT_ERROR("Curve %d does not exist.\n",  edge_id  );
    return 0.0;
  }
  return entityPtr->mesh_tool()->interval_size();
}

double do_Length (int edge_id)
{
  MRefEdge *entityPtr = Model::instance()->get_mref_edge(edge_id);
  if (!entityPtr)
  {
    PRINT_ERROR("Curve %d does not exist.\n",  edge_id  );
    return 0.0;
  }
  return entityPtr->get_arc_length();
}

double do_Radius (int edge_id)
{
  double rad = 0.0;
  MRefEdge *ref_edge = Model::instance()->get_mref_edge(edge_id);
  if (!ref_edge)
  {
    PRINT_ERROR("Curve %d does not exist.\n",  edge_id  );
  }
  else
  {
    if (ref_edge->geometry_type() != STRAIGHT_CURVE_TYPE)
    {
        //Find the mid_point and calc the curvatur.
      VerdeVector mid_point, closest_location;
      VerdeVector curvature;
      ref_edge->mid_point(mid_point);
      ref_edge->closest_point(mid_point, closest_location,
                              NULL, &curvature );
      rad = curvature.length();
      
      if ( rad <= VERDE_RESABS && rad >= -VERDE_RESABS )
      {
          //no curvature...
        rad = 0.0;
      }
      else
      {
        rad = 1.0/rad;
      }
    }
  }
  return rad;
}



double do_get_error_count(int)
{
  return VerdeMessage::instance()->error_count();
}

double do_set_error_count(int value)
{
  return VerdeMessage::instance()->reset_error_count(value);
}

double do_get_warning_count(int)
{
  return VerdeMessage::instance()->warning_count();
}

double do_set_warning_count(int value)
{
  return VerdeMessage::instance()->reset_warning_count(value);
}


double do_Id(const char *string, const char*)
{
  double rv = 0;
    // See if the passed in string is an entity name
  const char *class_name = RefEntityName::instance()->
    get_refentity_type(string);
  if (class_name != NULL)
  {
    rv = RefEntityName::instance()->get_refentity_id(string);
  }
  else
  {
    char* new_str;
    NEWSTR(string, new_str);
    inner_tolower(new_str);
    if (streq("body", new_str))
      rv = (double)RefEntityFactory::instance()->current_body_id();
    else if (streq("volume", new_str))
      rv = (double)RefEntityFactory::instance()->current_volume_id();
    else if (streq("surface", new_str))
      rv = (double)RefEntityFactory::instance()->current_face_id();
    else if (streq("curve", new_str))
      rv = (double)RefEntityFactory::instance()->current_edge_id();
    else if (streq("vertex", new_str))
      rv = (double)RefEntityFactory::instance()->current_vertex_id();
    else if (streq("group", new_str))
      rv = (double)RefEntityFactory::instance()->current_group_id();
    else
    {
      PRINT_WARNING("Unrecognized option to Id function: '%s'\n",
                    string);
    }
    FREESTR(new_str);
  }
  return rv;
}

*/


//  --------------------------STRING FUNCTIONS------------------------
char *do_tolower (const char *string)
{
  char* rv;
  if (string)
  {
    NEWSTR(string, rv);
    inner_tolower(rv);
  }
  else
  {
    NEWSTR("", rv);
  }
  return rv;
}

char *do_toupper (const char *string)
{
  char* rv;
  if (string)
  {
    NEWSTR(string, rv);
    inner_toupper(rv);
  }
  else
  {
    NEWSTR("", rv);
  }
  return rv;
}

char *do_tostring (double x, const char *, const char *)
{
  char *tmp;
  static char tmpstr[128];
  if (x == 0.0)
  {
    NEWSTR ("0", tmp);
    return (tmp);
  }
  else
  {
    symrec *format;
    format = getsym ("_FORMAT");
    (void) sprintf (tmpstr, format->value.svar, x);
    NEWSTR (tmpstr, tmp);
    return (tmp);
  }
}

char *do_getenv (const char *env)
{
  char *tmp;
  char *ret_string;
  tmp = (char *)getenv(env);
  if (tmp != NULL)
  {
    NEWSTR (tmp, ret_string);
  }
  else
  {
    NEWSTR("", ret_string);
  }
  return ret_string;
}

char *do_error(const char *error_string)
{
  PRINT_ERROR("%s\n", error_string);
  exit(1);
  /* NOTREACHED */
  char* tmp;
  NEWSTR("", tmp);
  return tmp;
}

/*

char *do_Type(const char *string)
{
    // See if the passed in string is an entity name
  char *ret_string;
  const char *class_name = RefEntityName::instance()->
    get_refentity_type(string);
  if (class_name != NULL)
  {
    NEWSTR(class_name, ret_string);
  }
  else
  {
    PRINT_WARNING("'%s' is not an entity name.\n", string);
    NEWSTR("", ret_string);
  }
  return ret_string;
}

*/

double do_word_count (const char *string, const char *delm)
{
  char *temp;
  double i = 0;
  
  NEWSTR(string, temp);
  if(strtok(temp,delm))
  {
    i++;
    while(strtok(NULL,delm))
    {
      i++;
    }
  }
  FREESTR(temp);
  return i;
}

char *do_get_word (double n, const char *string, const char *delm)
{
  char *temp, *token, *word;
  int i;
  
  NEWSTR(string, temp);
  token = strtok(temp,delm);
  if( n == 1 )
  {
    NEWSTR(token,word);
    FREESTR(temp);
    return(word);
  }
  for(i=1; i<n; i++)
  {
    if( (token = strtok(NULL,delm)) == NULL )
    {
      FREESTR(temp);
      return(NULL);
    }
  }
  NEWSTR(token,word);
  FREESTR(temp);
  return(word);
}

double do_strtod (const char *string, const char *)
{
  double x;
  errno = 0;
  x = atof(string);
  MATH_ERROR("strtod");
  return x;
}

