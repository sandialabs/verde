//-------------------------------------------------------------------------
// Filename      : ErrorTool.hpp
//
// Purpose       : Attempts to predice maximum error for the mesh
// Special Notes : 
//
// Creator       : Ray Meyers
//
// Date          : 09/01/00
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------

#ifndef ERROR_TOOL_HPP
#define ERROR_TOOL_HPP

#include "VerdeString.hpp"
#include "VerdeDefines.hpp"
#include "VerdeVector.hpp"

class TetRef;
class HexRef;

//! Attempts to predict maximum error for the mesh
class ErrorTool
{
public:
   
  //! destructor
  ~ErrorTool();
   
  //! returns the singleton pointer
  static ErrorTool *instance();

  //! initializes parameters of the ErrorTool
  void initialize(int number_nodes);

  //! sets control file name
  void set_control_file_name(const char *name);

  //! reads variables in from control file
  int read_control_file();

  //! lists currently defined error commands
  void list_error_commands();

  //! calculates error for the mesh
  void calculate();

  /*! checks to see if all data is provided for options selected,
  //  and echos data to command line */
  VerdeStatus echo_input();
  
  //! returns the approx solution for the tet
  double approx_solution(TetRef &tet, VerdeVector &gradient);

  //! returns the approx solution for the hex
  double approx_solution(HexRef &hex, VerdeVector &gradient);

  //! returns do_error flag
  VerdeBoolean do_error();

  enum WaveType { NONE=0, SMOOTH_SHOCK, PULSE, DELTA, LINEAR,
                  SPHERE, CYLINDER, PLANE };

  enum SolutionType { NO_SOLUTION_TYPE=0, LINEAR_BASIS };
  
  
  enum parseSuccess {SUCCESS, BAD_VALUE, BAD_TOKEN};

private:

  //! singleton pointer
  static ErrorTool *instance_;

  //! filename of control file for error calcs
  VerdeString controlFileName;
  FILE *controlFile;
  

  //! set if error calcs should be done
  VerdeBoolean doError;

  static double UNDEFINED_VALUE;
  WaveType waveFormType;
  WaveType travelingWaveType;
  double waveFormAlpha;
  double waveFormBeta;
  VerdeVector waveFormOrigin;
  VerdeVector planeWaveNormal;
  double waveVelocity;
  double startTime;
  double endTime;
  int numberTimeSteps;
  SolutionType solutionType;
  
  
  //! control variables for wave form function

  //! array holding exact solution for each node for a single time
  double *exactSolution;
  double *exactGradientMagnitude;
  

  //! array holding approx solution for each element for a single time
  double *approxSolution;
  int approxSolutionSize;
  

  //! max values for error calc functions
  double e2Max;
  double eInfMax;
  double e2GradMax;
  double eInfGradMax;
  int eInfMaxId;
  int eInfGradMaxId;
  
  //! integer id of the exodus output file
  int outputFile; 

  //! error output file name
  VerdeString outputFileName;
  
  static const char *waveTypeName[];
  static const char *solutionTypeName[];

  //! Constructor
  ErrorTool();

  //! calculates the exact function solution at time for each node
  void calculate_exact_solution(double time);

  //! calculates the approx function solution at time for each elem
  void calculate_approx_solution(double time, int time_step);

  //! returns the exact solution at node with id node_id
  double exact_solution(VerdeVector &coords, double &time,
                        VerdeVector &gradient);

  //! returns the linear approx solution for the tet
  double simple_linear_approx(TetRef &tet, VerdeVector &gradient);

  //! returns the linear approx solution for the hex
  double simple_linear_approx(HexRef &hex, VerdeVector &gradient);

  double distance_from_origin(VerdeVector &coords, double &time,
                              VerdeVector &unit_gradient);

  //! copies input genesis file to a new output file 
  int copy_input_file();

  //! initializes the new variables to the output file
  void init_exodus_output();

  //! writes a time step of the exact solution to the output file
  void write_exact_solution(int time_step, double time);
  
  //! writes a time step of the approx solution to the output file
  void write_approx_solution(int time_step, int block_id,
                             int number_elements);
  
  //! find the effective distance to the point for cylindrical wave
  double cylinder_distance(VerdeVector &coords, double &time,
                           VerdeVector &unit_gradient);
  
  const char *wave_type_name(WaveType type);
  const char *solution_type_name(SolutionType type);  

  //! private functions to print error table
  void print_error_table_header();
  void print_summary_error();
  void print_time_step_error(int time_step,
                             double e_inf, double e2, 
                             int e_inf_id,
                             double e_inf_grad, double e2_grad,
                             int e_inf_grad_id);
  
    };

#endif
