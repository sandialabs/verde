/*! Class:       CommandHandler
//  Description: Abstract base class to provide common data members and
//               functions required by all Command Interface classes.
//  Owner:       Bill Bohnhoff
//  Checked by:  Mark Whiteley
//  Version: $Id: 
*/

#ifndef COMMANDHANDLER_HPP
#define COMMANDHANDLER_HPP

#include <string>
#ifdef CANT_USE_STD_IO
#include <strstream.h>
#else
#include <strstream>
#endif

#include <vector>

#include "VerdeDefines.hpp"

struct FunctionData;
class  VerdeString;
class VerdeVector;
class RefVertex;
class RefEdge; 
class RefFace;
class MRefEdge; 
class MRefGroup;
class VerdeVector;
class RefEntity;
class MeshEntity;
class MRefEntity;
class GenesisEntity;

//! common data members and functions required by all Command Interface classes.
class CommandHandler
{
public:
	static void SetWarningLevel(const FunctionData* parsed_data);
	static void Set(const FunctionData* parsed_data); 
	static void Help(const FunctionData* parsed_data);
	static void Calculate(const FunctionData* parsed_data);

	static void SetHex(const FunctionData* parsed_data); 
        static void SetTet(const FunctionData* parsed_data);
	static void SetPyramid(const FunctionData* parsed_data);
	static void SetKnife(const FunctionData* parsed_data); 
        static void SetQuad(const FunctionData* parsed_data);  
        static void SetTri(const FunctionData* parsed_data);  
        static void SetEdge(const FunctionData* parsed_data);
	static void SetWedge(const FunctionData* parsed_data);
	static void open_mesh(const FunctionData* parsed_data);
	static void PlayJournal(const FunctionData* parsed_data);
	static void record_handler(const FunctionData* parsed_data);
	static void echo_handler(const FunctionData* parsed_data);
	static void load_defaults(const FunctionData* parsed_data);
	static void toggle_blocks(const FunctionData* parsed_data);
	static void toggle_node_bcs(const FunctionData* parsed_data);
	static void toggle_element_bcs(const FunctionData* parsed_data);
	static void save_mesh(const FunctionData* parsed_data);
	static void save_gui_settings(const FunctionData* parsed_data);


	static void graphics_handler(const FunctionData* parsed_data);
	static void graphics_highlight_handler(const FunctionData* parsed_data);
	static void graphics_display_handler(const FunctionData* parsed_data);
	static void draw_normals(const FunctionData* parsed_data);
	static void draw_axis(const FunctionData* parsed_data);
	static void draw_skin(const FunctionData* parsed_data);
	static void draw_model_edges(const FunctionData* parsed_data);
        static void label(const FunctionData* parsed_data);
	static void draw_handler(const FunctionData* parsed_data);



  //! initializes the echoStream
  static void initialize_echo_stream ();

  /*! provides help on all commands using the keyword to print only
  // those help formats - returns true if the help message was printed
  */
  static void help_handler ( const FunctionData* parsed_data );

  /*! returns help on the specified keyword that *starts* a command,
  // or appears anywhere in a command. */
  static void keyword_help ( const char* keyword, int start_only );

  /*! provides help on all commands using the keyword to print only
  // those help formats - returns true if the help message was printed
  // Since anything can follow help, calls second function below. */
  static int help ( const FunctionData* parsed_data );
  
  /*! Parses free form input line, matches all words, and prints help
  // on matched words. */
  static void print_raw_help( char *input_line, int order_dependent,
                              int consecutive_dependent );
  
  //! list all command keywords in a column layout
  static void list_keywords();

  /*! Returns VERDE_TRUE if the input 'word' exactly matches
  // a valid identifier (listed in allwords array).
  // Otherwise returns VERDE_FALSE */
  static VerdeBoolean is_identifier(const char *word);

protected:

  //! writes out the journal file entry
  static void command_wrapup(const FunctionData* parsed_data);

  /*! writes {string} to the journal file. 
  // Use to journal commands generated internally */
  static void add_journal_entry(const VerdeString &string);

  /*! parses {parsed_data} to find the range associated with the
  // keyword {keyword}. Returns the range in {id_min}, {id_max},
  // and {id_step} if the keyword is found. Returns {VERDE_TRUE} if 
  // keyword found, returns {VERDE_FALSE} if not. */
  static int find_int_range (const char* keyword,
                             const FunctionData* parsed_data,
                             int& id_min, int& id_max, int& id_step);

  /*!parses to find the values of the next three doubles
  //increments the current keyword
  //returns VERDE_FAILURE or VERDE_SUCCESS */
  static int get_3_doubles (int&, const FunctionData*,
             double&, double&, double& );

  /*! determines whether the specified keyword exists in {parsed_data}
  // Returns its (index+1) or 0 if not found */
  static int find_keyword ( const char *keyword,
                            const FunctionData* parsed_data, int start=0);

  //! clears the echoStream
  static void clear_echo_stream ();

  //! Internal flag, True if command should be journalled, false otherwise.
  static int validCommandStream;
  
  //! Debug function, prints contents of parsed_data
  static void print_parsed_data( const FunctionData* parsed_data );

  //! parses and integer list
  static void parse_int_list( const FunctionData* parsed_data, int& curr_kywd, std::vector<int>& list );

private:

  // Heading: private functions
  
  //! Trick to make the class abstract
  virtual void dummy() = 0;
  
  static void generate_journal_entry(const FunctionData *parsed_data,
      std::ostrstream& journal_stream);

  /*! Returns help on a list of words.
  // All the other help functions eventually call this one. */
  static void print_parsed_help( const char *words_to_match[], int num_words,
                                 int order_dependent,
                                 int consecutive_dependent );

  /*! Take parsed_data and construct an input line so that
  // print_raw_help can be called. Note that since anything matches
  // help, the parsed_data is just a list of tokens, and is not really parsed. */
  static void print_help( const FunctionData* parsed_data );
  
  //! used when handling compound keywords.
  static char *nextToken; 
  
  /*! try to match the input line tokens with words.  First time
  // call with input == input line, subsequent times on the same line
  // with input == NULL. Returns the index >=0 of a word if a match
  // is found, returns -2 if the end of the input line is reached,
  // returns -1 and prints some help if multiple matches were
  // found, returns -3 if no match was found. */
  static int complete_word(char *input);

  /*! parses to find a range of values; increments the current_keyword
  // index to reflect the next location to search for a keyword and also
  // write the range to the journal file entry.
  // Returns VERDE_SUCCESS if valid range found.
  // Returns VERDE_FAILURE if problems parsing range. Function tries to 
  // still return a valid range that may be usable in some situations. */
  static int range_specifier ( const FunctionData* parsed_data,
			       int& current_keyword,
			       int& id_min, int& id_max,
			       int& id_step,
			       int &all_qualifier);
  //- Heading: private member data
  
  //! array of strings to print out as help messages for various words 
  static const char * const helpStrings[];

  //! array of keywords.
  static const char * const keywords[];
  //! array of keywords+identifiers.
  static const char * const allwords[];

}; 

#endif


