#ifndef BUILTIN_H
#define BUILTIN_H

#ifndef EXTERN
#define EXTERN extern
#endif


EXTERN void check_math_error (double);

EXTERN double do_acos   (double x,  double, double, double);
EXTERN double do_acosd  (double x,  double, double, double);
EXTERN double do_acosh  (double x,  double, double, double);
EXTERN double do_angle  (double x1, double y1, double x2, double y2);
EXTERN double do_angled (double x1, double y1, double x2, double y2);
EXTERN double do_asin   (double x,  double, double, double);
EXTERN double do_asind  (double x,  double, double, double);
EXTERN double do_asinh  (double x,  double, double, double);
EXTERN double do_atan   (double x,  double, double, double);
EXTERN double do_atan2  (double x,  double y, double, double);
EXTERN double do_atan2d (double x,  double y, double, double);
EXTERN double do_atand  (double x,  double, double, double);
EXTERN double do_atanh  (double x,  double, double, double);
EXTERN double do_ceil   (double x,  double, double, double);
EXTERN double do_cos    (double x,  double, double, double);
EXTERN double do_cosd   (double x,  double, double, double);
EXTERN double do_cosh   (double x,  double, double, double);
EXTERN double do_d2r    (double x,  double, double, double);
EXTERN double do_dim    (double x,  double y, double, double);
EXTERN double do_dist   (double x1, double y1, double x2, double y2);
EXTERN double do_exp    (double x,  double, double, double);
EXTERN double do_fabs   (double x,  double, double, double);
EXTERN double do_floor  (double x,  double, double, double);
EXTERN double do_fmod   (double x,  double y, double, double);
EXTERN double do_hypot  (double x,  double y, double, double);
EXTERN double do_int    (double x,  double, double, double);
EXTERN double do_log    (double x,  double, double, double);
EXTERN double do_log10  (double x,  double, double, double);
EXTERN double do_log1p  (double x,  double, double, double);
EXTERN double do_max    (double x,  double y, double, double);
EXTERN double do_min    (double x,  double y, double, double);
EXTERN double do_r2d    (double x,  double, double, double);
EXTERN double do_rand   (double xl, double xh, double, double);
EXTERN double do_sign   (double x,  double y, double, double);
EXTERN double do_sin    (double x,  double, double, double);
EXTERN double do_sind   (double x,  double, double, double);
EXTERN double do_sinh   (double x,  double, double, double);
EXTERN double do_sqrt   (double x,  double, double, double);
EXTERN double do_tan    (double x,  double, double, double);
EXTERN double do_tand   (double x,  double, double, double);
EXTERN double do_tanh   (double x,  double, double, double);
EXTERN double do_polarX (double rad,double ang, double, double);
EXTERN double do_polarY (double rad,double ang, double, double);
EXTERN double do_lgamma (double val,double, double, double);
EXTERN double do_julday (double mon,double day, double year, double);
EXTERN double do_Vx     (int vertex_id);
EXTERN double do_Vy     (int vertex_id);
EXTERN double do_Vz     (int vertex_id);
EXTERN double do_Nx     (int node_id);
EXTERN double do_Ny     (int node_id);
EXTERN double do_Nz     (int node_id);
EXTERN double do_IntNum (int line_id);
EXTERN double do_IntSize(int line_id);
EXTERN double do_Length (int line_id);
EXTERN double do_Radius (int edge_id);
EXTERN double do_get_error_count(int);
EXTERN double do_set_error_count(int value);
EXTERN double do_get_warning_count(int);
EXTERN double do_set_warning_count(int value);

/* FNCTDC functions */
EXTERN double do_Id     (const char *string, const char*);
EXTERN double do_strtod (const char *string, const char*);
EXTERN double do_word_count (const char *string, const char *delimiter);

/* SFNCT functions */
EXTERN char *do_tolower (const char *string);
EXTERN char *do_toupper (const char *string);
EXTERN char *do_execute (const char *string);
EXTERN char *do_rescan  (const char *string);
EXTERN char *do_Units   (const char *string);
EXTERN char *do_dumpsym (const char *string);
EXTERN char *do_getenv  (const char *string);
EXTERN char *do_error   (const char *string);
EXTERN char *do_Type    (const char *string);

/* SDFNCT functions */
EXTERN char *do_tostring (double x, const char *, const char *);
EXTERN char *do_get_word(double n, const char *string, const char *delimiter);

#endif // BUILTIN_H

