//-------------------------------------------------------------------------
// Filename      : ErrorTool.cpp
//
// Purpose       : Attempts to predict maximum error for the mesh
// Special Notes : 
//
// Creator       : Ray Meyers
//
// Date          : 09/01/00
//
// Owner         : Ray Meyers
//-------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>


#include "ErrorTool.hpp"

#include "ElementBlock.hpp"
#include "ElemRef.hpp"
#include "VerdeMessage.hpp"
#include "VerdeUtil.hpp"
#include "VerdeApp.hpp"
#include "Mesh.hpp"
#include "TetRef.hpp"
#include "HexRef.hpp"
#include "HexMetric.hpp"
#include "TetMetric.hpp"
#include "VerdeMatrix.hpp"

#include "exodusII.h"

enum ArgType { VERDE_NULL, VERDE_LOGICAL, VERDE_INTEGER, VERDE_REAL,
               VERDE_STRING };

enum ElementType { VERDE_HEX, VERDE_TET };


struct
{
   const char *token;
   ArgType args[3];
}
errorCommandArray[] = {
  {"WAVEFORM_TYPE", {VERDE_STRING, VERDE_NULL, VERDE_NULL} },
  {"WAVEFORM_ORIGIN", {VERDE_REAL, VERDE_REAL, VERDE_REAL} },
  {"WAVEFORM_VARIABLE_ALPHA", {VERDE_REAL, VERDE_NULL, VERDE_NULL} },
  {"WAVEFORM_VARIABLE_BETA", {VERDE_REAL, VERDE_NULL, VERDE_NULL} },
  {"TRAVELING_WAVE_TYPE", {VERDE_STRING, VERDE_NULL, VERDE_NULL} },
  {"WAVE_VELOCITY", {VERDE_REAL, VERDE_NULL, VERDE_NULL} },
  {"START_TIME", {VERDE_REAL, VERDE_NULL, VERDE_NULL} },
  {"END_TIME", {VERDE_REAL, VERDE_NULL, VERDE_NULL} },
  {"TIME_STEPS", {VERDE_INTEGER, VERDE_NULL, VERDE_NULL} },
  {"PLANE_WAVE_DIRECTION", {VERDE_REAL, VERDE_REAL, VERDE_REAL} },
  {"CYLINDER_DIRECTION", {VERDE_REAL, VERDE_REAL, VERDE_REAL} },
  {"APPROXIMATION_TYPE", {VERDE_STRING, VERDE_NULL, VERDE_NULL} },
  {"CALCULATE_ERROR", {VERDE_NULL, VERDE_NULL, VERDE_NULL} },
  {NULL, {VERDE_NULL, VERDE_NULL, VERDE_NULL}}
};


const char * ErrorTool::waveTypeName[] = {
  "NONE",
  "SMOOTH_SHOCK",
  "PULSE",
  "DELTA",
  "LINEAR",
  "SPHERE",
  "CYLINDER",
  "PLANE" 
};

const char * ErrorTool::solutionTypeName[] = {
  "NONE",
  "LINEAR_BASIS"
};


ErrorTool *ErrorTool::instance_ = NULL;

double ErrorTool::UNDEFINED_VALUE = -777777.0;

ErrorTool *ErrorTool::instance()
{
   if (!instance_)
      instance_ = new ErrorTool();
   return instance_;
}

ErrorTool::ErrorTool()
{
  controlFileName = "";
  controlFile = NULL;
  doError = VERDE_FALSE;
  exactSolution = NULL;
  exactGradientMagnitude = NULL;
  approxSolution = NULL;
  approxSolutionSize = 0;
  waveFormType = NONE;
  travelingWaveType = NONE;
  waveFormAlpha = UNDEFINED_VALUE;
  waveFormBeta = UNDEFINED_VALUE;
  waveFormOrigin.set(UNDEFINED_VALUE,UNDEFINED_VALUE,UNDEFINED_VALUE);
  waveVelocity = UNDEFINED_VALUE;
  startTime = UNDEFINED_VALUE;
  endTime = UNDEFINED_VALUE;
  numberTimeSteps = (int)UNDEFINED_VALUE;
  planeWaveNormal.set(UNDEFINED_VALUE,UNDEFINED_VALUE,UNDEFINED_VALUE);
  solutionType = NO_SOLUTION_TYPE;
 
  e2Max = -1.0;
  eInfMax = -1.0;
  e2GradMax = -1.0;
  eInfGradMax = -1.0;
  
}

ErrorTool::~ErrorTool()
{
  ex_close(outputFile);
}

int ErrorTool::read_control_file()
{
  // if there is a file name set, read it

  if (controlFileName == "")
    return VERDE_FAILURE;
  

  // if file is open, read more commands

  if (!controlFile)
  {
    return VERDE_FAILURE;
  }
  
  
  // read values from the file

  int overall_success = VERDE_SUCCESS;
  const int MAX_CHARS = 256;   
  char input_line[MAX_CHARS];
  char original_line[MAX_CHARS];
   
  parseSuccess success;
  double double_value;
  int integer_value;
  
  while (fgets(input_line, MAX_CHARS, controlFile))
  {
    success = SUCCESS;
    // get first token

    strcpy(original_line,input_line);
    char *token = strtok(input_line,"\n ");

    // switch on the token
      
    if (!token || strlen(token) <= 1 || token[0] == '#')
    {
      continue;
    }

    // set waveform type
      
    else if (strcmp(token,"WAVEFORM_TYPE") == 0)
    {
      token = strtok(NULL,"\n ");     
      if (token)
      {
        if (VerdeUtil::strncmp_case_insensitive(token,"SMOOTH",6) == 0 ||
            VerdeUtil::strncmp_case_insensitive(token,"SHOCK",5) == 0 )
        {
          waveFormType = SMOOTH_SHOCK;
        }     
        else if (VerdeUtil::strncmp_case_insensitive(token,"PULSE",5) == 0)
        {
          waveFormType = PULSE;
        }     
        else if (VerdeUtil::strncmp_case_insensitive(token,"DELTA",5) == 0)
        {
          waveFormType = DELTA;
        }
        else if (VerdeUtil::strncmp_case_insensitive(token,"LINEAR",6) == 0)
        {
          waveFormType = LINEAR;
        }
        else
        {
          success = BAD_VALUE;
        }
      }
    }
      
    else if (strcmp(token,"TRAVELING_WAVE_TYPE") == 0)
    {
      token = strtok(NULL,"\n ");     
      if (token)
      {
        if (VerdeUtil::strncmp_case_insensitive(token,"SPHERE",6) == 0)
        {
          travelingWaveType = SPHERE;
        }     
        else if (VerdeUtil::strncmp_case_insensitive(token,"CYLINDER",8) == 0)
        {
          travelingWaveType = CYLINDER;
        }     
        else if (VerdeUtil::strncmp_case_insensitive(token,"PLANE",5) == 0)
        {
          travelingWaveType = PLANE;
        }     
        else
        {
          success = BAD_VALUE;
        }
      }
    }
      
    // set approx solution type
      
    else if (strcmp(token,"APPROXIMATION_TYPE") == 0)
    {
      token = strtok(NULL,"\n ");     
      if (token)
      {
        if (VerdeUtil::strncmp_case_insensitive(token,"LINEAR_BASIS",13) 
            == 0  )
        {
          solutionType = LINEAR_BASIS;
        }
        else
        {
          success = BAD_VALUE;
        }
      }
    }

    // set alpha and beta for waveform equations
      
    else if (strcmp(token,"WAVEFORM_VARIABLE_ALPHA") == 0)
    {
      token = strtok(NULL,"\n ");     
      if (token && VerdeUtil::is_double(token,double_value))
      {
        waveFormAlpha = double_value;
      }
      else
      {
        success = BAD_VALUE;
      }
    }
    else if (strcmp(token,"WAVEFORM_VARIABLE_BETA") == 0)
    {
      token = strtok(NULL,"\n ");     
      if (token && VerdeUtil::is_double(token,double_value))
      {
        waveFormBeta = double_value;
      }     
      else
      {
        success = BAD_VALUE;
      }
    }
      
    else if (strcmp(token,"WAVE_VELOCITY") == 0)
    {
      token = strtok(NULL,"\n ");     
      if (token && VerdeUtil::is_double(token,double_value))
      {
        waveVelocity = double_value;
      }     
      else
      {
        success = BAD_VALUE;
      }
    }
      
    else if (strcmp(token,"START_TIME") == 0)
    {
      token = strtok(NULL,"\n ");     
      if (token && VerdeUtil::is_double(token,double_value))
      {
        startTime = double_value;
      }     
      else
      {
        success = BAD_VALUE;
      }
    }
      
    else if (strcmp(token,"END_TIME") == 0)
    {
      token = strtok(NULL,"\n ");     
      if (token && VerdeUtil::is_double(token,double_value))
      {
        endTime = double_value;
      }     
      else
      {
        success = BAD_VALUE;
      }
    }
      
    else if (strcmp(token,"TIME_STEPS") == 0)
    {
      token = strtok(NULL,"\n ");     
      if (token && VerdeUtil::is_integer(token,integer_value))
      {
        numberTimeSteps = integer_value;
      }     
      else
      {
        success = BAD_VALUE;
      }
    }
      
    else if (strcmp(token,"WAVEFORM_ORIGIN") == 0)
    {
      // first value
      token = strtok(NULL,"\n ");     
      if (token && VerdeUtil::is_double(token,double_value))
      {
        token = strtok(NULL,"\n ");     
        double value_2;
        if (token && VerdeUtil::is_double(token,value_2))
        {
          token = strtok(NULL,"\n ");     
          double value_3;
          if (token && VerdeUtil::is_double(token,value_3))
          {
            waveFormOrigin.set(double_value,value_2,value_3);
          }
          else
          {
            success = BAD_VALUE;
          }
        }
        else
        {
          success = BAD_VALUE;
        }
      }
      else
      {
        success = BAD_VALUE;
      }
    }

    else if (strcmp(token,"PLANE_WAVE_DIRECTION") == 0 ||
             strcmp(token,"CYLINDER_AXIS") == 0 )
    {
      // first value
      token = strtok(NULL,"\n ");     
      if (token && VerdeUtil::is_double(token,double_value))
      {
        token = strtok(NULL,"\n ");     
        double value_2;
        if (token && VerdeUtil::is_double(token,value_2))
        {
          token = strtok(NULL,"\n ");     
          double value_3;
          if (token && VerdeUtil::is_double(token,value_3))
          {
            planeWaveNormal.set(double_value,value_2,value_3);
            planeWaveNormal.normalize();
          }
          else
          {
            success = BAD_VALUE;
          }
        }
        else
        {
          success = BAD_VALUE;
        }
      }
      else
      {
        success = BAD_VALUE;
      }
    }

    else if (strcmp(token,"CALCULATE_ERROR") == 0 )
    {
      return overall_success;
    }
      
    else
    {
      success = BAD_TOKEN;
    }

    // write errors here so they are consistent
      
    if (success == SUCCESS)
    {
      continue;
    }
    else if (success == BAD_TOKEN)
    {
      PRINT_INFO("Error Control File Error: %s"
                 "   Token %s not found...\n",
                 original_line, token);
      overall_success = VERDE_FAILURE;
    }
    else if (success == BAD_VALUE)
    {
      PRINT_INFO("Error Control File Error: %s"
                 "   Incorrect/Missing Value...\n",
                 original_line);
      overall_success = VERDE_FAILURE;
    }
  }

  // close input file

  fclose(controlFile);
  controlFile = NULL;
  return overall_success;
  
}

void ErrorTool::set_control_file_name(const char *name)
{
  // set input_file name
   
  controlFileName = name;
  doError = VERDE_TRUE;

  // open control file here
  // open the file

    
  controlFile = fopen(controlFileName.c_str(), "r");
  
  if (!controlFile)
  {
    PRINT_ERROR("Cannot open error control file %s...\n",
                controlFileName.c_str());
  }
}

void ErrorTool::list_error_commands()
{
   // list each command from command array

   PRINT_INFO("\nAvailable error estimation commands:\n\n");

   int i;
   for ( i = 0; errorCommandArray[i].token != NULL; i++)
   {
      PRINT_INFO("%s",errorCommandArray[i].token);
      for(int j = 0; j < 3; j++)
      {
        if (errorCommandArray[i].args[j] == VERDE_REAL)
        {
          PRINT_INFO("\treal_value");
        }
        else if (errorCommandArray[i].args[j] == VERDE_INTEGER)
        {
          PRINT_INFO(" \tinteger_value");
        }
        else if (errorCommandArray[i].args[j] == VERDE_LOGICAL)
        {
          PRINT_INFO(" \tlogical_value");
        }
        else if (errorCommandArray[i].args[j] == VERDE_STRING)
        {
          PRINT_INFO(" \tstring_value");
        }
      }
      PRINT_INFO("\n");
   }

   // print a final return

   PRINT_INFO("\n");

}


void ErrorTool::calculate_exact_solution(double time)
{
  MeshContainer *container = ElemRef::node_container();
  int number_nodes = container->number_nodes();

  // set aside memory for exactSolution if necessary

  if (!exactSolution)
  {
    exactSolution = new double[number_nodes];
  }
  if (!exactGradientMagnitude)
  {
    exactGradientMagnitude = new double[number_nodes];
  }
  

  // calculate the solution at each node

  VerdeVector coords, gradient;
  
  //PRINT_INFO("Exact Solutions:\n");
  
  for(int i = 0; i < number_nodes; i++)
  {
    // get the coordinates of the node

    coords = container->coordinates(i+1);
    
    // calculate the function and store in array

    exactSolution[i] = exact_solution(coords, time, gradient);
    //if (exactSolution[i] > 2.0)
    //  PRINT_WARNING(" exactSolution = %lf\n",exactSolution[i]);
    
    exactGradientMagnitude[i] = gradient.length();
    
    //PRINT_INFO("   Node %d:\t%lf\n",i+1,exactSolution[i]);
  }
}

double ErrorTool::exact_solution(VerdeVector &coords, double &time,
                                 VerdeVector &gradient)
{

  // calculate distance to waveform

  double distance = distance_from_origin(coords, time, gradient);
  
  double gradient_magnitude;
  
  // calculate based on waveForm


  switch(waveFormType)
  {
    case SMOOTH_SHOCK:
      gradient_magnitude = cosh(waveFormBeta*distance);
      gradient_magnitude = - waveFormAlpha * waveFormBeta /
        (gradient_magnitude*gradient_magnitude);
      gradient *= gradient_magnitude;
      return (- waveFormAlpha * tanh(waveFormBeta*distance));

    case PULSE:
      gradient_magnitude = - waveFormAlpha * waveFormBeta *
        tanh(waveFormBeta*distance) / cosh(waveFormBeta*distance);
      gradient *= gradient_magnitude;
      return (waveFormAlpha / cosh(waveFormBeta*distance));

    case DELTA:
      gradient_magnitude = -2.0 * waveFormBeta * waveFormBeta *
              waveFormBeta * distance *
              exp(- (waveFormBeta*waveFormBeta*distance*distance));
      gradient *= gradient_magnitude;
      return (waveFormBeta * 
              exp(- (waveFormBeta*waveFormBeta*distance*distance)));

    case LINEAR:
      gradient_magnitude = -0.5*waveFormAlpha*waveFormBeta;
      gradient *= gradient_magnitude;
      return waveFormAlpha *(1.0 - 0.5*distance*waveFormBeta);    

    default:
      break;

  }
  return UNDEFINED_VALUE;
}

double ErrorTool::distance_from_origin(VerdeVector &coords, double &time,
                                       VerdeVector &unit_gradient)
{
  double value=0;
  // now, modify to account for the traveling wave

  switch (travelingWaveType)
  {
    case SPHERE:
      unit_gradient = coords - waveFormOrigin;
      value = unit_gradient.length() - waveVelocity * time;
      unit_gradient.normalize();
      break;
    case CYLINDER:
      value = cylinder_distance(coords,time,unit_gradient);
      break;
    case PLANE:
      value = planeWaveNormal.x() * (coords.x() - waveFormOrigin.x()) +
              planeWaveNormal.y() * (coords.y() - waveFormOrigin.y()) +
              planeWaveNormal.z() * (coords.z() - waveFormOrigin.z())  -
              waveVelocity * time;
      unit_gradient = planeWaveNormal;
      break;
    case NONE:
      unit_gradient = waveFormOrigin - coords;
      value = unit_gradient.length();      
      unit_gradient.normalize();
      break;

    default:
      break;
  }
  return value;
}


void ErrorTool::calculate()
{
  // return if not doing errors

  if (doError == VERDE_FALSE)
    return;

  // read control file and calculate until done

  int success = VERDE_SUCCESS;
  int calc_number = 0;
  
  while(success == VERDE_SUCCESS)
  {

    PRINT_INFO("\n-------------------------\n"
               "Error Prediction Information\n"
               "-------------------------\n");

    // get input file and set output file name

    outputFileName = verde_app->input_file();

    // set output name = input_name_out.gen

    calc_number++;
    
    size_t pos = outputFileName.find(".");
    if (calc_number == 1)
      outputFileName = outputFileName.substr(0,pos) + "_err.gen";
    else
      outputFileName = outputFileName.substr(0,pos) + "_err" + 
        calc_number + ".gen";


    if (echo_input() == VERDE_FAILURE)
      return;

    // check for no traveling wave case

    if (travelingWaveType == NONE)
      numberTimeSteps = 0;
  
  
    // first, copy the entire original file so that output variables
    // can be added to it

    copy_input_file();
    init_exodus_output();

    // calculate error for each time step

    double current_time = startTime;
    double delta_time;
    if (numberTimeSteps == 0)
      delta_time = 0;
    else
      delta_time = (endTime - startTime) / (double) (numberTimeSteps);

    print_error_table_header();
    
    for (int i = 0; i <= numberTimeSteps; i++)
    {
    
      // calculate solution

      calculate_exact_solution(current_time);

      calculate_approx_solution(current_time,i);

      // write out to file

      write_exact_solution(i, current_time);
      current_time += delta_time;
    }

    // write out summary line of error table

    print_summary_error();

    success = read_control_file();
    
  }
  
}

int ErrorTool::copy_input_file()
{
  int error;


   // open output file
   
  int CPU_WORD_SIZE = sizeof(double);  // With ExodusII version 2, all floats
  int IO_WORD_SIZE = sizeof(double);               // should be changed to doubles

  if( (outputFile = ex_create(outputFileName.c_str(), EX_CLOBBER,
                              &CPU_WORD_SIZE, &IO_WORD_SIZE) ) < 0 )
  {
    PRINT_INFO("\nVerde:: problem opening error output genesis file %s\n",
               outputFileName.c_str());
    return VERDE_FAILURE;
  }

  error = ex_copy (verde_app->mesh()->exodus_file(),outputFile);
  if ( error == -1 )
  {
    PRINT_INFO("\nVerde:: error copying error output file...\n\n");
    return VERDE_FAILURE;
  }

  return VERDE_SUCCESS;
}

void ErrorTool::init_exodus_output()
{
  // specify nodal variable name and parameters

  int num_nodal_variables = 2;
  int num_elem_variables = 1;
  const char *nodal_var_names[2] = {"EXACT", "GRADIENT"};
  const char *elem_var_names[1] = {"APPROX"};
  int error = ex_put_var_param(outputFile, "n", num_nodal_variables);
  error = ex_put_var_names(outputFile,"n", num_nodal_variables, 
                           (char**) nodal_var_names);
  error = ex_put_var_param(outputFile, "e", num_elem_variables);
  error = ex_put_var_names(outputFile,"e", num_elem_variables, 
                           (char**) elem_var_names);

}

void ErrorTool::write_exact_solution(int time_step, double time )
{
  // increment time to be 1-based
  time_step++;
  
  // specify time

  int error = ex_put_time(outputFile, time_step, &time);

  // write the solution vector to the file

  MeshContainer *container = ElemRef::node_container();
  int number_nodes = container->number_nodes();
  int variable_index = 1;

  error = ex_put_nodal_var (outputFile, time_step,
                                variable_index, number_nodes,
                                exactSolution);
  variable_index = 2;
  error = ex_put_nodal_var (outputFile, time_step,
                                variable_index, number_nodes,
                            exactGradientMagnitude);

  // debug stuff

  //double max_exact = 0.0;
  
  //for(int i = 0; i < number_nodes; i++)
  //{
  //  if (exactSolution[i] > max_exact)
  //    max_exact = exactSolution[i];
  //}
  //PRINT_INFO("\nTime Step %d: Max Exact: %lf\n",time_step, max_exact);
  
                                
}  

void ErrorTool::write_approx_solution(int time_step, int block_id,
                                      int number_elements)
{
  // increment step to be 1 based

  time_step++;
  
  // write the solution vector to the file

  int variable_index = 1;

  /*int error =*/ ex_put_elem_var (outputFile, time_step,
                               variable_index, block_id,
                               number_elements, approxSolution);
                                
}  


double ErrorTool::cylinder_distance(VerdeVector &coords,
                                    double &time, VerdeVector &unit_gradient)
{
  // We have a cylinder defined by an origin waveFormOrigin and
  // a axis vector given by planeWaveNormal.  We will find the
  // point along the axis closest to the coords point, and the
  // vector from that point to the coords is outward from the
  // cylinder and is the vector we need to calculate

  VerdeVector closest_point = coords - waveFormOrigin;
  double distance = closest_point % planeWaveNormal;
  
  // locate the closest point on the axis

  closest_point = waveFormOrigin + distance * planeWaveNormal;
  unit_gradient = coords - closest_point;
  distance = unit_gradient.length() - time *waveVelocity;
  unit_gradient.normalize();
  return distance;
}

VerdeStatus ErrorTool::echo_input()
{
  // check data for current options to see if they are set

  VerdeStatus status = VERDE_SUCCESS;
  

  // Print out and check waveForm data

  PRINT_INFO("  WAVE_FORM_TYPE:\t%s\n",wave_type_name(waveFormType));

  if (waveFormType != SMOOTH_SHOCK &&
      waveFormType != PULSE &&
      waveFormType != DELTA &&
      waveFormType != LINEAR)
  {
    status = VERDE_FAILURE;
  }

  // check origin information

  if (waveFormType == SMOOTH_SHOCK ||
      waveFormType == PULSE ||
      waveFormType == DELTA)
  {
    if (waveFormOrigin.x() == UNDEFINED_VALUE)
    {
      PRINT_INFO("  WAVE_FORM_ORIGIN:\t UNDEFINED\n");
      status = VERDE_FAILURE;
    }
    else
    {
      PRINT_INFO("  WAVE_FORM_ORIGIN:\t%lf %lf %lf\n", waveFormOrigin.x(),
                 waveFormOrigin.y(),waveFormOrigin.z());
    }
    
    // do alpha and beta

    if ( waveFormType != DELTA)
    {
      if (waveFormAlpha == UNDEFINED_VALUE)
      {
        PRINT_INFO("  WAVE_FORM_ALPHA:\t UNDEFINED\n");
        status = VERDE_FAILURE;
      }
      else
      {
        PRINT_INFO("  WAVE_FORM_ALPHA:\t%lf\n",waveFormAlpha);
      }
    }
    

    if (waveFormBeta == UNDEFINED_VALUE)
    {
      PRINT_INFO("  WAVE_FORM_BETA:\t UNDEFINED\n");
      status = VERDE_FAILURE;
    }
    else
    {
      PRINT_INFO("  WAVE_FORM_BETA:\t%lf\n",waveFormBeta);
    }
  }
  
   

  // now, check for traveling wave information
   
  PRINT_INFO("  TRAVELING_WAVE_TYPE:\t%s\n",
             wave_type_name(travelingWaveType));
 
  if (travelingWaveType != SPHERE && 
      travelingWaveType != CYLINDER &&
      travelingWaveType != PLANE)
  {
    status = VERDE_FAILURE;
  }
  
  // output plane normal / cylinder axis

  if (travelingWaveType == SPHERE || 
      travelingWaveType == CYLINDER ||
      travelingWaveType == PLANE)
  {

    if ( travelingWaveType == CYLINDER)
    {
      if (planeWaveNormal.x() == UNDEFINED_VALUE)
      {
        PRINT_INFO("  CYLINDER_AXIS:\t UNDEFINED\n");
        status = VERDE_FAILURE;
      }
      else
      {
        PRINT_INFO("  CYLINDER_AXIS:\t%lf %lf %lf\n", planeWaveNormal.x(),
                   planeWaveNormal.y(),planeWaveNormal.z());
      }
    }
    
    else if ( travelingWaveType == PLANE)
    {
      if (planeWaveNormal.x() == UNDEFINED_VALUE)
      {
        PRINT_INFO("  WAVE_PLANE_DIRECTION:\t UNDEFINED\n");
        status = VERDE_FAILURE;
      }
      else
      {
        PRINT_INFO("  WAVE_PLANE_DIRECTION:\t%lf %lf %lf\n", 
                   planeWaveNormal.x(),
                   planeWaveNormal.y(),planeWaveNormal.z());
      }
    }

    // wave velocity

    if (waveVelocity == UNDEFINED_VALUE)
    {
      PRINT_INFO("  WAVE_VELOCITY:\tUNDEFINED\n");
      status = VERDE_FAILURE;
    }
    else
    {
      PRINT_INFO("  WAVE_VELOCITY:\t%lf\n",waveVelocity);
    }

    // start time

    if (startTime == UNDEFINED_VALUE)
    {
      PRINT_INFO("  START_TIME:\t\tUNDEFINED\n");
      status = VERDE_FAILURE;
    }
    else
    {
      PRINT_INFO("  START_TIME:\t\t%lf\n",startTime);
    }


    // end time

    if (endTime == UNDEFINED_VALUE)
    {
      PRINT_INFO("  END_TIME:\t\tUNDEFINED\n");
      status = VERDE_FAILURE;
    }
    else
    {
      PRINT_INFO("  END_TIME:\t\t%lf\n",endTime);
    }


    if (numberTimeSteps == UNDEFINED_VALUE)
    {
      PRINT_INFO("  TIME_STEPS:\t\tUNDEFINED\n");
      status = VERDE_FAILURE;
    }
    else
    {
      PRINT_INFO("  TIME_STEPS:\t\t%d\n",numberTimeSteps);
    }    
  }

  // echo approximation information

  PRINT_INFO("  APPROXIMATION_TYPE:\t%s\n",
             solution_type_name(solutionType));

  PRINT_INFO("  Output File Name:\t%s\n",outputFileName.c_str());

  return status;
}

const char *ErrorTool::wave_type_name(WaveType type)
{
  return waveTypeName[type];
}

const char *ErrorTool::solution_type_name(SolutionType type)
{
  return solutionTypeName[type];
}

VerdeBoolean ErrorTool::do_error()
{
  return doError;
}

double ErrorTool::approx_solution(TetRef &tet, VerdeVector &gradient)
{
  // calculate the approximate solution for the tet based on the
  // the solution operator chosen

  double approx = 0.0;
  
  switch (solutionType)
  {
    case LINEAR_BASIS:
      approx = simple_linear_approx(tet, gradient);
      break;

    default:
      break;
  }
  return approx;
}

double ErrorTool::approx_solution(HexRef &hex, VerdeVector &gradient)
{
  // calculate the approximate solution for the hex based on the
  // the solution operator chosen

  double approx = 0.0;
  
  switch (solutionType)
  {
    case LINEAR_BASIS:
      approx = simple_linear_approx(hex, gradient);
      break;
      
    default:
      break;;
  }
  return approx;
}


double ErrorTool::simple_linear_approx(TetRef &tet, VerdeVector &gradient)
{
  // make a simple FE linear approximation of the function at the
  // element centroid

  // first, get vectors of each node position of the tet

  VerdeVector coords[4];
  int i;
  
  for (i = 0; i < 4; i++)
  {
    coords[i] = tet.node_coordinates(i);
  }
  
  // calculate approx(x,y,z) = (c0 + c1*x + c2*y + c3*z) / 6*V

  // first, do c0

  double c0 = 0.0;
  double exact[4];
  double coeff = -1.0;
  
  for (i = 0; i < 4; i++)
  {
    exact[i] = exactSolution[tet.node_id(i)-1];
    coeff *= -1.0;
    c0 += coeff * exact[i] * (coords[(i+1)%4] %
                           (coords[(i+2)%4] * coords[(i+3)%4]));
  }
  
  
  // now, do c1, c2, c3

  VerdeVector cross;
  double c1 = 0.0;
  double c2 = 0.0;
  double c3 = 0.0;
  coeff = -1.0;
  
  for(i = 0; i < 4; i++)
  {
    cross = coords[(i+2)%4] * coords[(i+1)%4] +
      coords[(i+1)%4] * coords[(i+3)%4] +
      coords[(i+3)%4] * coords[(i+2)%4];
    coeff *= -1.0;
    c1 += coeff * exact[i] * cross.x();
    c2 += coeff * exact[i] * cross.y();
    c3 += coeff * exact[i] * cross.z();
  }

  
  
  
  // now, calculate approx at the centroid

  double one_over_six_vol = 1.0 / (TetMetric::instance()->volume(tet) * 6.0);
  VerdeVector center = tet.centroid(4);
  
  
  double approx = (c0 + c1*center.x() + c2*center.y() + c3 * center.z())
    * one_over_six_vol;

  gradient.set(c1,c2,c3);
  gradient *= one_over_six_vol;
  
  
  // check average of four exacts

  //double check = exact[0] + exact[1] + exact[2] + exact[3];
  //check *= 0.25;
  //PRINT_INFO("Approx: %lf\tAverage: %lf\tDiff: %lf\n",approx,check,
  //           approx-check);
  
  // as a temp test, check approx at each node

  //double test;
  //for(i = 0; i < 4; i++)
  //{
  //  test =  (c0 + c1*coords[i].x() + c2*coords[i].y() + c3 * coords[i].z())
  //    / (6.0 * volume);
  //  approx = test;
  //}
  //approx = (c0 + c1*center.x() + c2*center.y() + c3 * center.z())
  //  / (6.0 * volume);
  
  return approx;
}

double ErrorTool::simple_linear_approx(HexRef &hex, VerdeVector &gradient)
{
  // make a simple FE linear approximation of the function at the
  // element centroid

  // for linear hex case, average the function from each node

  double approx = 0.0;
  
  VerdeVector coords[8];
  double exact[8];
  
  int i;
  
  for(i = 0; i < 8; i++)
  {
    coords[i] = hex.node_coordinates(i);
    exact[i] = exactSolution[hex.node_id(i)-1];
    approx += exact[i];
    
  }
  approx *= 0.125;


  // now calculate the gradient using 

  // now build the M matrix

  VerdeVector B1 = - coords[0] + coords[1] + coords[2] - coords[3]
                   - coords[4] + coords[5] + coords[6] - coords[7];
  
  VerdeVector B2 = - coords[0] - coords[1] + coords[2] + coords[3]
                   - coords[4] - coords[5] + coords[6] + coords[7];
  
  VerdeVector B3 = - coords[0] - coords[1] - coords[2] - coords[3]
                   + coords[4] + coords[5] + coords[6] + coords[7];

  VerdeMatrix M(B1, B2, B3);
  
  // take the inverse, transpose

  M = M.inverse();
  M = M.transpose();
  
  // get the v matrix


  //a1 = -u0 +u1 +u2 -u3 -u4 +u5 +u6 -u7
  // a2 = -u0 -u1 +u2 +u3 -u4 -u5 +u6 +u7
  // a3 = -u0 -u1 -u2 -u3 +u4 +u5 +u6 +u7

  double A1 = - exact[0] + exact[1] + exact[2] - exact[3]
              - exact[4] + exact[5] + exact[6] - exact[7];
  
  double A2 = - exact[0] - exact[1] + exact[2] + exact[3]
              - exact[4] - exact[5] + exact[6] + exact[7];
  
  double A3 = - exact[0] - exact[1] - exact[2] - exact[3]
              + exact[4] + exact[5] + exact[6] + exact[7];

  VerdeVector V(A1, A2, A3);
  
  // put gradient in B1 and return
  
  gradient = M * V;

  return approx;
  
}


  

void ErrorTool::calculate_approx_solution(double time,int time_step)
{


  // go through and calculate the approx solution for each element at
  // the current time

  std::deque<ElementBlock*>block_list;
  verde_app->mesh()->element_blocks(block_list);

  double e2 = 0.0, e2_grad = 0.0;;
  double e_inf = -1.0, e_inf_grad = -1.0;
  int e_inf_id=0,e_inf_grad_id=0;
  double sum_volume = 0.0;
  double elem_volume=0.0;
  //double e2_max = -1.0;  
  //double e_inf_max = -1.0;    
  //double e2_grad_max = -1.0;    
  //double e_inf_grad_max = -1.0;
  //these above numbers never get used
 
  VerdeVector gradient;
  int element_number = 0;
  
  for(unsigned int i=0; i<block_list.size(); i++)
  {
    ElementBlock *block = block_list[i];
    MeshContainer *mesh_container = block->mesh_container();
    VerdeString element_type_string = mesh_container->element_type();
    int number_elements = mesh_container->number_elements();
    
    // allocate memory for approx solution if required

    if (number_elements > approxSolutionSize)
    {
      if (approxSolution)
      {
        delete approxSolution;
      }
      approxSolution = new double[number_elements];
      approxSolutionSize = number_elements;
    }
  
    ElementType element_type = VERDE_HEX;
    if (strncmp(element_type_string.c_str(),"HEX",3) == 0)
    {
      element_type = VERDE_HEX;
    }
    else if (strncmp(element_type_string.c_str(),"TET",3) == 0)
    {
      element_type = VERDE_TET;
    }

    // process each element

    VerdeVector centroid;
        
    for(int j = 0; j < number_elements; j++)
    {
      element_number++;
      if (element_type == VERDE_HEX)
      {
        HexRef hex(*mesh_container,j+1);
        approxSolution[j] = approx_solution(hex, gradient);
        centroid = hex.centroid(8);
        elem_volume = HexMetric::instance()->volume(hex);
      }
      else if (element_type == VERDE_TET)
      {
        TetRef tet(*mesh_container,j+1);
        approxSolution[j] = approx_solution(tet, gradient);
        centroid = tet.centroid(4);
        elem_volume = TetMetric::instance()->volume(tet);
      }

      sum_volume += elem_volume;
      
      // calculate exact and exact gradient and centroid

      VerdeVector exact_gradient;
      double exact = exact_solution(centroid,time,exact_gradient);

      // calculate e_inf and summation portion of e2

      double diff = fabs(exact - approxSolution[j]);
      if (e_inf < diff)
      {
        e_inf = diff;
        e_inf_id = element_number;
      }

      e2 += diff*diff*elem_volume;
      
      // calculate e_inf_grad and summation portion of e2_grad

      diff = (exact_gradient - gradient).length();
      if (e_inf_grad < diff)
      {
        e_inf_grad = diff;
        e_inf_grad_id = element_number;
      }
      
      e2_grad += diff*diff*elem_volume;      

    }

    // write elem solution for time step and block

    write_approx_solution(time_step, block->block_id(),number_elements);

    // calculate final e2's and n2's

    e2 = sqrt( e2/sum_volume );
    //double n2 = e2 / waveFormAlpha;
    //this number does not get used
    
    e2_grad = sqrt (e2_grad/sum_volume );
    //double n2_grad = e2_grad / waveFormAlpha;
    //this number does not get used

    // print out line to error table for this time step

    print_time_step_error(time_step, e_inf, e2, e_inf_id,
                          e_inf_grad, e2_grad, e_inf_grad_id);
     
    // calculate max's for all time steps

    if (e2 > e2Max)
    {
      e2Max = e2;
    }
    
    if (e_inf > eInfMax)
    {
      eInfMax = e_inf;
      eInfMaxId = e_inf_id;
    }

    if (e2_grad > e2GradMax)
    {
      e2GradMax = e2_grad;
    }
    
    if (e_inf_grad > eInfGradMax)
    {
      eInfGradMax = e_inf_grad;
      eInfGradMaxId = e_inf_grad_id;
    }
  }
}

void ErrorTool::print_time_step_error(int time_step,
                                      double e_inf, double e2, 
                                      int e_inf_id,
                                      double e_inf_grad, double e2_grad,
                                      int e_inf_grad_id)
{
  // calc normalized values

  double n_inf = e_inf/waveFormAlpha;
  double n2 = e2/waveFormAlpha;
  double n_inf_grad = e_inf_grad/waveFormAlpha;
  double n2_grad = e2_grad/waveFormAlpha;

  // print out line
  
  PRINT_INFO("|%3d |%8.6lf %8.6lf %8.6lf %8.6lf (%3d)"
             "|%8.6lf %8.6lf %8.6lf %8.6lf (%3d)|\n",
             time_step, e2, n2, e_inf, n_inf,e_inf_id,
             e2_grad, n2_grad, e_inf_grad, n_inf_grad, e_inf_grad_id);
             
}

void ErrorTool::print_summary_error()
{
  // calc normalized values

  double n_inf = eInfMax/waveFormAlpha;
  double n2 = e2Max/waveFormAlpha;
  double n_inf_grad = eInfGradMax/waveFormAlpha;
  double n2_grad = e2GradMax/waveFormAlpha;

  // print out line
  
  PRINT_INFO("|----|--------------------------------------"
             "---|-----------------------------------------|\n"

             "|MAX |%8.6lf %8.6lf %8.6lf %8.6lf (%3d)"
             "|%8.6lf %8.6lf %8.6lf %8.6lf (%3d)|\n",
             e2Max, n2, eInfMax, n_inf,eInfMaxId,
             e2GradMax, n2_grad, eInfGradMax, n_inf_grad, eInfGradMaxId);
             
}


void ErrorTool::print_error_table_header()
{
  PRINT_INFO("\n\n                          "
             "          TABLE OF ERROR MEASURES\n"
             "|----|--------------------------------------"
             "---|-----------------------------------------|\n"
             "|    |             FUNCTION ERROR          "
             "    |             GRADIENT ERROR              |\n"
             "|----|-----------------------------------------"
             "|-----------------------------------------|\n"
             "|Step|   e2       n2      e_inf    n_inf  (id) "
             "|   e2       n2      e_inf    n_inf  (id) |\n");
}
