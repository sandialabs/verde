//- Class: VerdeMessage
//- Description: VerdeMessage class - used for reporting messages to
//-              the user
//- Owner: Tim Tautges
//- Checked By:
//- Version:

#ifndef VERDEMESSAGE_HPP
#define VERDEMESSAGE_HPP

// *****************
// Class Declaration
// *****************

#include <stdarg.h>

// VERDE debug flags; see VerdeMessage.cc for a definition of each flag

#define VERDE_ERROR                        0
#define VERDE_WARNING                      1
#define VERDE_INFO                         2
#define VERDE_DIAGNOSTIC                   3

#define VERDE_DEBUG_1                     11
#define VERDE_DEBUG_2                     12
#define VERDE_DEBUG_3                     13
#define VERDE_DEBUG_4                     14
#define VERDE_DEBUG_5                     15
#define VERDE_DEBUG_6                     16
#define VERDE_DEBUG_7                     17
#define VERDE_DEBUG_8                     18
#define VERDE_DEBUG_9                     19
#define VERDE_DEBUG_10                    20
#define VERDE_DEBUG_11                    21
#define VERDE_DEBUG_12                    22
#define VERDE_DEBUG_13                    23
#define VERDE_DEBUG_14                    24
#define VERDE_DEBUG_15                    25
#define VERDE_DEBUG_16                    26
#define VERDE_DEBUG_17                    27
#define VERDE_DEBUG_18                    28
#define VERDE_DEBUG_19                    29
#define VERDE_DEBUG_20                    30
#define VERDE_DEBUG_21                    31
#define VERDE_DEBUG_22                    32
#define VERDE_DEBUG_23                    33
#define VERDE_DEBUG_24                    34
#define VERDE_DEBUG_25                    35
#define VERDE_DEBUG_26                    36
#define VERDE_DEBUG_27                    37
#define VERDE_DEBUG_28                    38
#define VERDE_DEBUG_29                    39
#define VERDE_DEBUG_30                    40
#define VERDE_DEBUG_31                    41
#define VERDE_DEBUG_32                    42
#define VERDE_DEBUG_33                    43
#define VERDE_DEBUG_34                    44
#define VERDE_DEBUG_35                    45
#define VERDE_DEBUG_36                    46
#define VERDE_DEBUG_37                    47
#define VERDE_DEBUG_38                    48
#define VERDE_DEBUG_39                    49
#define VERDE_DEBUG_40                    50
#define VERDE_DEBUG_41                    51
#define VERDE_DEBUG_42                    52
#define VERDE_DEBUG_43                    53
#define VERDE_DEBUG_44                    54
#define VERDE_DEBUG_45                    55
#define VERDE_DEBUG_46                    56
#define VERDE_DEBUG_47                    57
#define VERDE_DEBUG_48                    58
#define VERDE_DEBUG_49                    59
#define VERDE_DEBUG_50                    60
#define VERDE_DEBUG_51                    61
#define VERDE_DEBUG_52                    62
#define VERDE_DEBUG_53                    63
#define VERDE_DEBUG_54                    64
#define VERDE_DEBUG_55                    65
#define VERDE_DEBUG_56                    66
#define VERDE_DEBUG_57                    67
#define VERDE_DEBUG_58                    68
#define VERDE_DEBUG_59                    69
#define VERDE_DEBUG_60                    70
#define VERDE_DEBUG_61                    71
#define VERDE_DEBUG_62                    72
#define VERDE_DEBUG_63                    73
#define VERDE_DEBUG_64                    74
#define VERDE_DEBUG_65                    75
#define VERDE_DEBUG_66                    76
#define VERDE_DEBUG_67                    77
#define VERDE_DEBUG_68                    78
#define VERDE_DEBUG_69                    79
#define VERDE_DEBUG_70                    80
#define VERDE_DEBUG_71                    81
#define VERDE_DEBUG_72                    82
#define VERDE_DEBUG_73                    83
#define VERDE_DEBUG_74                    84
#define VERDE_DEBUG_75                    85
#define VERDE_DEBUG_76                    86
#define VERDE_DEBUG_77                    87
#define VERDE_DEBUG_78                    88
#define VERDE_DEBUG_79                    89
#define VERDE_DEBUG_80                    90
#define VERDE_DEBUG_81                    91
#define VERDE_DEBUG_82                    92
#define VERDE_DEBUG_83                    93
#define VERDE_DEBUG_84                    94
#define VERDE_DEBUG_85                    95
#define VERDE_DEBUG_86                    96
#define VERDE_DEBUG_87                    97
#define VERDE_DEBUG_88                    98
#define VERDE_DEBUG_89                    99
#define VERDE_DEBUG_90                   100
#define VERDE_DEBUG_91                   101
#define VERDE_DEBUG_92                   102
#define VERDE_DEBUG_93                   103
#define VERDE_DEBUG_94                   104
#define VERDE_DEBUG_95                   105
#define VERDE_DEBUG_96                   106
#define VERDE_DEBUG_97                   107
#define VERDE_DEBUG_98                   108
#define VERDE_DEBUG_99                   109 


#define PRINT_ERROR VerdeMessage::instance()->print_error
#define PRINT_WARNING VerdeMessage::instance()->print_warning
#define PRINT_WARNING_HIGH VerdeMessage::instance()->print_warning
#define PRINT_WARNING_LOW VerdeMessage::instance()->print_warning_low
#define PRINT_INFO VerdeMessage::instance()->print_info
#define PRINT_DEBUG VerdeMessage::instance()->print_debug
#define DIAGNOSTIC VerdeMessage::instance()->print_diagnostic
#define DIAGNOSTIC_FLAG VerdeMessage::instance()->diagnostic_flag
#define DEBUG_FLAG VerdeMessage::instance()->debug_flag
#define INFO_FLAG VerdeMessage::instance()->info_flag
#define WARNING_FLAG VerdeMessage::instance()->warning_flag

#ifdef CANT_USE_STD_IO
#include <fstream.h>
#else
#include <fstream>
#endif

class VerdeString;

//! reports messages to the user
class MessageFlag {
public:
   
   //! Constructor
   MessageFlag();
   
   //! Constructor
   MessageFlag(int flag_number, const char *desc);
   
   //! Destructor
   ~MessageFlag();
 
   //! prints info. when turning debug flag on
   void output();

   //! debug flag number
   int flagNumber;

   //! setting--on or off
   int setting;
   
   //! description of the debug flag
   const char *description;

   //! filename
   VerdeString *filename;

   //! where to print to
   std::ofstream *outputStream;
};

class VerdeMessage
{
private:
  
   //! static pointer to unique instance of this class
   static VerdeMessage* instance_;
  
   //! debug flag, used with internal_error
   static MessageFlag debugFlag[];

   //! info flag, used with internal_error
   int infoFlag;

   //! warning flag, used with internal_error
   int warningFlag;
 
   //! diagnostic flag, used with internal_error
   int diagnosticFlag;

   //! Stores the number of times PRINT_ERROR was called.
   //! Nominally, this can be used as the programs exit status.
   int errorCount;

   //! What level of warning messages do we print out?
   int warningLevelFlag;
  
   std::ofstream *loggingStream;
  
   //! Stream pointer for logging of internal_error messages.
   //! If NULL, output goes to terminal only (except for debug)
   //! If Non-NULL, output goes to both terminal and stream.
   VerdeString *loggingFile;

   //! Increments the errorCount variable. Keep private (GDS).
   void add_to_error_count();

   //! Set the output stream for this debug flag to output_stream.
   void set_debug_stream(const int index, std::ofstream *output_stream);
   
   int count_stream_users(const std::ofstream *stream);

   //! Close and delete the stream if only one use.
   void remove_debug_stream(const int index);

   int find_file_use(const VerdeString &filename);


protected:
  VerdeMessage ();
    //! Class Constructor. (Not callable by user code. Class is constructed
    //! by the {instance()} member function.

public:

   //! Controlled access and creation of the sole instance of this class.
   static VerdeMessage* instance();

   //! Class Destructor.
   ~VerdeMessage();

   //! sets the logging file
   void set_logging_file(const VerdeString &filename);

   //! sets file to where debug info should go 
   void set_debug_file(const int index, const VerdeString &filename);

   //! debug flag, used with internal_error
   int  debug_flag(const int index);
   
   //! debug flag, used with internal_error
   void debug_flag(const int index, const int flag);

   //! returns number of debug flags
   int  number_of_debug_flags();

   //! returns info flag number
   int info_flag();

   //! info flag, used with internal_error
   void info_flag(int flag);

   //! warning flag, used with internal_error
   int warning_flag();

   //! sets warning_flag
   void warning_flag(int flag);

   //! warning level flag, controls what warnings are printed
   int warning_level_flag();

   //! sets warning_level_flag
   void warning_level_flag(int flag);
 
   //! diagnostic flag, used with internal_error
   int diagnostic_flag();

   //! sets diagnostic flag
   void diagnostic_flag(int flag);

   //! write out a debug/info/error/warning message 
   void internal_error(const int message_type, std::ofstream *output_stream,
                      const char *format, va_list &args);

   //! write out an error message 
   int print_error(const char *format, ... );

   //! write out a warning message 
   int print_warning(const char *format, ... );

   //! write out a low-priority warning message 
   int print_warning_low(const char *format, ... );

   //! write out an info message 
   int print_info(const char *format, ... );

   //! write out a debug message 
   int print_debug(const int message_type, const char *format, ... );
 
   //! write out a diagnostic message
   void print_diagnostic(const char *format, ... );

   //! Sets the errorCount variable to 0;
   //! Returns current value of variable.
   int reset_error_count(int value = 0);

   //! Returns the value of the errorCount variable;
   //! This errorCount variable is incremented only if print_error is called;
   //! there is not a public  interface to only set the flag.
   //! My reasoning for that is
   //! that there should be some notification of an error so that the
   //! user can figure out why this function returns TRUE.
   int error_count();

   //! outputs debug information
   void output_debug_information(int from=1, int to=-1, int step=1);

   //! outputs debug information
   void output_debug_information(VerdeString &match);

   //! outputs logging information
   void output_logging_information();

}; // End of Class VerdeMessage

inline int
VerdeMessage::debug_flag(const int index)
{return debugFlag[index].setting;}

inline void
VerdeMessage::debug_flag(const int index, const int flag)
{debugFlag[index].setting = flag;}

inline int
VerdeMessage::info_flag()
{return infoFlag;}

inline void
VerdeMessage::info_flag(int flag)
{infoFlag = flag;}

inline int
VerdeMessage::warning_flag()
{return warningFlag;}

inline void
VerdeMessage::warning_flag(int flag)
{warningFlag = flag;}
 
inline int
VerdeMessage::warning_level_flag()
{return warningLevelFlag;}

inline void
VerdeMessage::warning_level_flag(int flag)
{warningLevelFlag = flag;}
 
inline int
VerdeMessage::diagnostic_flag()
{return diagnosticFlag;}
 
inline void
VerdeMessage::diagnostic_flag(int flag)
{diagnosticFlag = flag;}

#endif

