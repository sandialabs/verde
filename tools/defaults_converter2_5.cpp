
#include <fstream.h>
#include <iostream.h>
#include <string.h>


// character arrays olddefaults and newdefaults map 1:1

const char* olddefaults[] = {
#include "olddefaults.h"
};

const char* newdefaults[] = {
#include "newdefaults.h"
};


// Usage:
// -o output_file defaults_file


char* output_filename;
char* defaults_filename;


// function that processes the arguements 
// and sets the output_filename
// and defaults_filename
bool process_arguments(int argc, char** argv);

int main(int argc, char** argv)
{

  // Process arguements
  if(process_arguments(argc, argv) == false)
    return 1;


  // try to open the input file
  ifstream infile(defaults_filename);
  if(!infile)
  {
    cerr << "Couldn't open " << defaults_filename << endl;
    return 1;
  }

  // try to open the output file
  ofstream outfile(output_filename);
  if(!outfile)
  {
    cerr << "Couldn't open " << output_filename << endl;
    return 1;
  }

  char token[301];
  char flag[20];

  while( infile >> token )
  {
    // if we have a comment
    if(token[0] == '#')
    {
      // put the comment in the file
      outfile << token;
      infile.getline(token, 300);
      outfile << token << endl;
      continue;
    }

    // get the flag that goes with the token
    infile >> flag;
    
    // find the new replacement
    for(int i=0; olddefaults[i]!=NULL ;i++)
    {
      if(strcmp(olddefaults[i], token) == 0)
      {
        // put the replacement in the output file
        outfile << newdefaults[i] << " " << flag << endl;
      }
    }
  }

  return 0;
}

bool process_arguments(int argc, char** argv)
{

  // if we have all arguments  
  if(argc == 4 && strcmp(argv[1], "-o") == 0)
  {
    // set the filenames
    output_filename = argv[2];
    defaults_filename = argv[3];

    // check for duplicate filenames
    if(strcmp(output_filename, defaults_filename) == 0)
    {
      cerr << "filenames must be different" << endl;
      return false;
    }

    return true;
  }
  else
  {
    // print the help
    cerr << "Usage: -o output_file input_file" << endl;        
    return false;
  }
}






