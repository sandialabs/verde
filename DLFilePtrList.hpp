#ifndef VERDEINPUTFILE_HPP
#define VERDEINPUTFILE_HPP

#include <stdio.h>
#include "VerdeString.hpp"
#include "VerdeMessage.hpp"

#define FILE_NORMAL 1
#define FILE_FASTQ  2
#define FILE_TEMPORARY 3

//!Input file struct
struct VerdeInputFile {
  VerdeString filename;
  FILE*       filePointer;
  int         lineNumber;
  int         fileType;
  int         loopCount;

  VerdeInputFile(FILE *file, int type=FILE_NORMAL, int loop=1);
  VerdeInputFile(const char *fileName, int type=FILE_NORMAL, int loop=1,
		 char *default_path=NULL);
 ~VerdeInputFile();
};

inline VerdeInputFile::VerdeInputFile(FILE *file, int type, int loop)
{
  if (file) {
    if (file == stdin)
      filename    = "<stdin>";
    else
      filename    = "<unknown filename>";
    filePointer = file;
  }
  else {
    filename = "<Invalid File>";
    filePointer = NULL;
  }
  lineNumber  = 1;
  fileType = type;
  loopCount = --loop;
}

inline VerdeInputFile::VerdeInputFile(const char *fileName, int type,
				      int loop, char *includePath)
{
  VerdeString file_and_path;
  FILE *file = fopen(fileName, "r");

  if (!file && includePath) {
    file_and_path = includePath;
    file_and_path += "/";
    file_and_path += fileName;
    file = fopen(file_and_path.c_str(), "r");
  }

  if (file) {
    if (includePath)
      filename  = file_and_path;
    else
      filename  = fileName;
    filePointer = file;
  }
  else {
    filename = "<Invalid File>";
    filePointer = NULL;
    PRINT_WARNING("Could not open file: %s\n", fileName );
  }
  lineNumber  = 1;
  fileType = type;
  loopCount = --loop;
}

inline VerdeInputFile::~VerdeInputFile() {
  if (fileType == FILE_TEMPORARY)
    remove(filename.c_str());	/* Delete file if temporary */
}


#endif 

