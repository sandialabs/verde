//- Class:       CommandHandler
//- Description: Implementation code needed by all Command Interface classes.
//- Owner:       Bill Bohnhoff
//- Checked by:
//- Version: $Id: 

#ifdef CANT_USE_STD_IO
#include <strstream.h>
#else
#include <strstream>
#endif

#ifdef CANT_USE_STD_IO
#include <iostream.h>
#else
#include <iostream>
#endif

#ifdef CANT_USE_STD_IO
#include <iomanip.h>
#else
#include <iomanip>
#endif

#include <vector>

#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "idr.hpp"
#include "VerdeApp.hpp"
#include "GUI/VerdeGUIApp.hpp"

#include "VerdeUtil.hpp"
#include "CommandHandler.hpp"
#include "UserInterface.hpp"
#include "VerdeString.hpp"
#include "HexMetric.hpp"
#include "TetMetric.hpp"
#include "PyramidMetric.hpp"
#include "KnifeMetric.hpp"
#include "WedgeMetric.hpp"
#include "QuadMetric.hpp"
#include "EdgeMetric.hpp"
#include "TriMetric.hpp"
#include "ElementBlock.hpp"
#include "NodeBC.hpp" 
#include "ElementBC.hpp"
#include "Mesh.hpp"
#include "SkinTool.hpp"
#include "EdgeTool.hpp"

#include "ExodusTool.hpp"

#include "GUI/UserEventHandler.hpp"
#include "GUI/DrawingTool.hpp"
#include "GUI/GLWindow.hpp"


extern "C" {
  extern int yyparse();
}




// list of keywords
const char* const CommandHandler::keywords[] = {
#include "keywords.h"
    NULL };

// list of allwords
const char* const CommandHandler::allwords[] = {
#include "allwords.h"
    NULL };

const int screen_width = 80;

// Initialize static data members
int CommandHandler::validCommandStream = VERDE_TRUE;

// used by complete_word
char *CommandHandler::nextToken = NULL;

void CommandHandler::initialize_echo_stream ()
{
  validCommandStream = VERDE_TRUE;
}



void CommandHandler::parse_int_list( const FunctionData* parsed_data, int& current_keyword, std::vector<int>& list )
{
  bool cont = true;
  const int start_parse_pos = current_keyword;
  int last_id = 0;
  
  if(current_keyword < parsed_data->dataListLength)
  {
    do
    {
      cont = false;
     
      // todo: add support for ranges 
      if(VerdeUtil::compare(parsed_data->keywordIdentifier[current_keyword], "to" ) ||
         VerdeUtil::compare(parsed_data->keywordIdentifier[current_keyword], "through") ||
         VerdeUtil::compare(parsed_data->keywordIdentifier[current_keyword], "thru"))
      {
        // we got an error
        if(start_parse_pos == current_keyword || last_id <= 0)
          return;

        int to_int_id = parsed_data->integerData[current_keyword];
        for(int i= last_id+1; i<=to_int_id; i++)
        {
          list.push_back(i);
        }
        current_keyword++;
        cont = true;
        last_id = to_int_id;
      }
      else
      {
        if(parsed_data->integerData[current_keyword] != 0 )
        {
          if( start_parse_pos == current_keyword || (start_parse_pos != current_keyword &&
              VerdeUtil::compare(parsed_data->keywordIdentifier[current_keyword],"")))
          {
            list.push_back(parsed_data->integerData[current_keyword]);
            last_id = list.back();
            cont = true;
            current_keyword++;
          }
        }
      }

    } while(cont);
  }
}


void CommandHandler::command_wrapup(const FunctionData* /*parsed_data*/)
{
  if (validCommandStream)
  {
      // Get the journal string from UI
    VerdeString echo_string = UserInterface::instance()->
      journal_string();
    
    add_journal_entry(echo_string);
    
      // delete stream and use again fo the next command
    clear_echo_stream();
  }
  validCommandStream = VERDE_TRUE;
}

void CommandHandler::add_journal_entry(const VerdeString &string)
{

  if( UserInterface::instance()->command_is_recorded() ) {
    UserInterface::instance()->recording_stream() << string.c_str() << std::endl;
  }
  
  if( UserInterface::instance()->command_is_journaled() ) {
    UserInterface::instance()->journal_stream() << string.c_str() << std::endl;
  }
  
  if( UserInterface::instance()->command_is_echoed() ) {
    PRINT_INFO("Journaled Command: %s\n\n", string.c_str() );
  }
}

int CommandHandler::find_keyword (const char *keyword,
                                  const FunctionData* parsed_data, int start)
{
  for (int i = start; i < parsed_data->dataListLength; i++){
    if (VerdeUtil::compare(keyword, parsed_data->keywordIdentifier[i])) {
      return (i+1);
    }
  }
  return VERDE_FALSE;
}

void CommandHandler::clear_echo_stream ()
{
  validCommandStream = VERDE_FALSE;
}

void CommandHandler::help_handler (const FunctionData* parsed_data)
{
  help (parsed_data);
}

int CommandHandler::help (const FunctionData* parsed_data)
{
  const char* keyword = IDR::instance()->keyword_string(parsed_data->keywordId);
    // process the global help command  
  if ( VerdeUtil::compare (keyword, "help") && parsed_data->dataListLength == 0) {
    list_keywords();
    PRINT_INFO("Individual keyword help is available by "
               "typing 'help keyword'\n" );
    clear_echo_stream();
    return VERDE_TRUE;
  }
  
  if ( VerdeUtil::compare (keyword, "help") || find_keyword ("help", parsed_data) ) {
    print_help( parsed_data ); 
    clear_echo_stream();
    return VERDE_TRUE;
  }
  return VERDE_FALSE;
}


int CommandHandler::complete_word(char *input)
{

  int i;
  std::vector<int> match_list, exact_match_list;

  char *word = nextToken && !input ? nextToken : strtok(input, " \t");
  nextToken = NULL;
    
  if (word) {
    unsigned int len = strlen(word);
    for (i=0; allwords[i] != NULL; i++) {
      if (strncmp(word, allwords[i], len) == 0) 
      {
        match_list.push_back(i+1);
        if ( strlen( allwords[i] ) == len )
          exact_match_list.push_back( i+1 );
      }
    }
    int match = match_list.size();
    if (match == 0) {
         PRINT_INFO("No words match input token %s.\n", word );
      return -3;
    }

      // See if current plus next token match a compound keyword
    nextToken = strtok( NULL, " \t" );
    std::vector<int> compound_match_list, exact_compound_match_list, simple_list;
    
    for (i=0; i < match; i++) 
    {
        // if space in keyword matching first token
      const char* space = strchr( allwords[ match_list[i]-1 ], ' ');
      if (space)
      {
        const char* after_space = space;
        after_space++;
          // does compound keyword after space match the next token?
        if ( nextToken &&
             strncmp( after_space, nextToken, strlen( nextToken ) ) == 0 )
        {
          compound_match_list.push_back( match_list[i] );
          if ( strlen( nextToken ) == strlen(after_space) )
            exact_compound_match_list.push_back( match_list[i] );
        }
      }
      else
        simple_list.push_back( match_list[i] );
    }
      // if compound words matched the next token, use only those
    if ( compound_match_list.size() )
    {
      match_list =
        exact_compound_match_list.size() ? exact_compound_match_list : 
        compound_match_list;
      nextToken = NULL;
    }
      // if no compound words matched the next token, use any simple words
    else if ( simple_list.size() )
    {
      match_list = exact_match_list.size() ? exact_match_list : simple_list;
    }
      // if no compound words matched, and no simple words matched,
      // use original list
    
    match = match_list.size();
  
    if (match == 1)
    {
        // Found a single matching keyword
      return match_list[0]-1;
    }
    
      // Found multiple matches. List them all and return -1
    int maxlen = 0;
    int curlen;
    for(i=0; i < match; i++) 
      if ((curlen = strlen(allwords[match_list[i]-1])) > maxlen)
  maxlen = curlen;
    
    const int columns = (screen_width-1) / (maxlen+2);
    const int rows = (int) ceil( ((double)match ) / ((double) columns ) );
    PRINT_WARNING("Ambiguous word '%s'. Possible matches:\n ", word );
    for(i=0; i<rows; i++) {
      for (int j=i; j < match; j+=rows) {
        PRINT_INFO("%-*s", maxlen+2, allwords[match_list[j]-1] );
      }      
      PRINT_INFO("\n ");
    }
    PRINT_INFO("\n" );
    return -1;
  }
    // PRINT_INFO("end of input string\n");

  return -2;

}


void CommandHandler::keyword_help( const char* keyword, int start_only )
{
  const char *words_to_match[1];
  words_to_match[0] = keyword;
  print_parsed_help( words_to_match, 1, start_only, VERDE_FALSE );
}

    // The following words are commonly not explicitly in the help table,
    // and are often instead implied as acceptable as part of a "range" 
    // or "toggle" See VerdeDefines.h. These words are explicitly removed
    // so that adequate help is printed.
    // This array is outside the function scope to avoid HP compile problems.
const char *const ignore_words[] = 
{ 
///
///-------->  Comment out by SRS of Cat,  3/24/99 9:26:52 AM  <--------
///  "yes", "no", "true", "false", "all", "and", "by", "except", 
///  "expand", "in", "or", "out", "step", "through", "thru", "to", 
///  "" // zero-length singleton marking end of table
///-------->  End of Comment by SRS of Cat,  3/24/99 9:26:52 AM  <-----
/// Took out step, so help on STEP file import/export works
  "yes", "no", "true", "false", "all", "and", "by", "except", 
  "expand", "in", "or", "out", "through", "thru", "to", 
  "" // zero-length singleton marking end of table
};


void CommandHandler::print_raw_help( char *input_line, int order_dependent,
                                     int consecutive_dependent )
{
  const int max_words = 64;
  const char *matches[max_words];
  int num_matches = 0;
  int match;
  int should_list_keywords = VERDE_TRUE;
  
  int i, ignore_word; //flag, does current word match something in the table?
  
  char *input = input_line;
  do {
    match = complete_word(input);
    if ( match >= 0 ) {
      i = 0;
      ignore_word = VERDE_FALSE;
      while ( strlen( ignore_words[i] ) ) {
        if ( strcmp(ignore_words[i], allwords[match]) == 0 ) {
          ignore_word = VERDE_TRUE;
          PRINT_INFO("Ignoring minor word '%s'.\n", allwords[match] );
          break;
        }
        i++;
      }
      if (!ignore_word) 
      {
        matches[num_matches++] = allwords[match];
        should_list_keywords = VERDE_FALSE;
      }
    }
    else if (match == -1)
        should_list_keywords = VERDE_FALSE;
    input = NULL; // iterate 
  } while (match != -2 && num_matches < max_words);
  
    // print out help for the found words, or list the keywords if no matches
  if (num_matches)
      print_parsed_help( matches, num_matches, order_dependent, 
                         consecutive_dependent );
  if (should_list_keywords)
      list_keywords();
}


void CommandHandler::print_help( const FunctionData* parsed_data )
{
  
  char input_line[512];
  input_line[0] = '\0'; // Null character, zero length string
  
    // Find the words to search for.
  const int num_words = parsed_data->dataListLength;
    // There is always one less word than (datalistlength + 1 for keyword),
    // since we filter out one occurence of "help".
  int word_index = 0;
  int help_found = VERDE_FALSE; // filter out one instance of help
  
  const char *keyword = IDR::instance()->keyword_string(parsed_data->keywordId);
  if ( VerdeUtil::compare ( keyword, "help" ) )
      help_found = VERDE_TRUE;
  else {
    strcat( input_line, keyword );
    strcat( input_line, " " );
    word_index++;
  }
  int i;
    // words might be in characterdata or as identifiers
    // currently always in characterdata, but check both just in case
  for (i=0; i < num_words; i++ ) {
    const char * current_word = parsed_data->characterData[i];
    if (strlen(current_word) == 0)
        current_word = parsed_data->keywordIdentifier[i];
    if (strlen(current_word) > 0) {
        // support "help help"
      if (help_found || !VerdeUtil::compare( current_word, "help" ) ) {
        strcat( input_line, current_word );
        strcat( input_line, " " );
        word_index++;
      }
      else
        help_found = VERDE_TRUE;
    }
  }
  
    //not order dependent
  print_raw_help( input_line, VERDE_FALSE, VERDE_FALSE ); 
}


void CommandHandler::print_parsed_help( const char *compound_words_to_match[],
                                        int num_compound_words,
                                        int order_dependent,
                                        int consecutive_dependent )
{
    // split compound words 
  int i, j;
  int max_words = num_compound_words * 2 + 4;
  char **words_to_match = new char *[max_words];
  char **copied_words = new char *[max_words];
  char *s, *split_word;
  const char *orig_word;
  char space[2];
  space[0] = ' ';
  space[1] = '\0';
  for ( i = 0, j = 0; i < num_compound_words && j < max_words; i++ )
  {
      // copy word
    orig_word = compound_words_to_match[i];
    int len = strlen( orig_word ) + 1;
    s = new char [len];
    memcpy( s, orig_word, len );
    copied_words[i] = s;
      // split up
    while ( (split_word = strtok( s, space )) && j < max_words ) // = ok
    {
      words_to_match[j++] = split_word;
      s = NULL; // strtok expects a NULL
    }
  }
  int num_words = j;
  words_to_match[num_words] = NULL;
  

    // Print heading of what we're looking for.
  if ( order_dependent && consecutive_dependent )
    PRINT_INFO( "\nCompleting commands starting with: ");
  else 
  {
    PRINT_INFO( "\nHelp for %swords: ", order_dependent ? "ordered " : "" );
  }
  for (i=0; i<num_words; i++) 
  {
    PRINT_INFO("%s", words_to_match[i] );
    if (i < num_words-1 )
    {
      if ( order_dependent )
        PRINT_INFO(", ");
      else
        PRINT_INFO(" & ");
    }
  }
  PRINT_INFO(".\n\n");
  
    // for all help strings, see if it matches all the specified words
  int help_printed = VERDE_FALSE;
  int bad_string = VERDE_FALSE;
    //  for (i = 0; 0 != strcmp( helpStrings[i], "@"); i++ ) {
  for (i = 0; helpStrings[i] != NULL && !bad_string; i++ ) 
  {
    
       // get an all lower case copy of the help string
    char *lower_help = new char[ strlen(helpStrings[i]) + 1];
    for( unsigned int k = 0; k <= strlen(helpStrings[i]); k++ ) 
    {
        // use <= so as to copy the null terminator
      char help_char = helpStrings[i][k]; 
      lower_help[k] = tolower( help_char );
    }
      // does the lower-help-string match every word so far?
    int matches = VERDE_TRUE;
    
    if ( order_dependent && consecutive_dependent )
    {
      char *remaining_help = lower_help;
      int next_match = 0; //next word in words_to_match
        // make some stacks, of parenthesis, how many words have
        // been matched since the last open_paren, 
        // and if the next word must be a match.
    const int max_stack = 100;
      char parenthesis[max_stack];
      int skip_to_end_paren[max_stack];
      int num_matches[max_stack], next_must_match[max_stack];
      parenthesis[0] = '\0';
      num_matches[0] = 0;
      next_must_match[0] = VERDE_TRUE;
      skip_to_end_paren[0] = VERDE_FALSE;
      int stack_height = 1;

      while ( remaining_help && 
              remaining_help[0] != '\0' &&
              next_match < num_words && 
              !bad_string )
      {
        char c = remaining_help[0]; // shorthand

          // skip white space
        if ( isspace( c ) )
        {
          remaining_help++;
          continue;
        }
          // skip stuff between *** and ***
        if ( strncmp( remaining_help, "***", 3 ) == 0 )
        {
          remaining_help = &remaining_help[3];
          remaining_help = strstr( remaining_help, "***" );
          if ( remaining_help )
            remaining_help = &remaining_help[3];
          continue;
        }
          // skip stuff between < and >
        if ( c == '<' )
        {
          while ( remaining_help[0] != '>' && remaining_help[1] != '\0' )
            remaining_help++;
          remaining_help++;
            // count it as a match, for supporting e.g.,
            // " {<id_range>| all}"
            // " {vertex <id_range> | surface <id_range>}" will still *not* be 
            // match if neither vertex nor surface is matched.
          num_matches[stack_height-1]++;
          continue;
        }
          // skip stuff between ' and '
        if ( c == '\"' || c == '\'' )
        {
          while ( remaining_help[0] != '\"' && remaining_help[0] != '\'' 
                  && remaining_help[1] != '\0' )
            remaining_help++;
          remaining_help++;
            // count it as a match, for supporting e.g.,
            // " {<id_range>| all}"
            // " {vertex <id_range> | surface <id_range>}" will still *not* be 
            // match if neither vertex nor surface is matched.
          num_matches[stack_height-1]++;
          continue;
        }
          // open paren
        if ( c == '[' || c == '{' )
        {
          parenthesis[ stack_height ] = c;
          num_matches[ stack_height ] = 0;
          next_must_match[stack_height] = VERDE_FALSE;
          skip_to_end_paren[stack_height] = 
            skip_to_end_paren[stack_height-1];
          stack_height++;
          if ( stack_height >= max_stack )
            PRINT_ERROR( "Can't handle strings with more than %d "
                         "sets of parenthesis:\n%s\n", 
                         max_stack, lower_help );
          remaining_help++;
          continue;
        }
          // close paren
        if ( c == ']' || c == '}' )
        {
          if ( c == '}' && parenthesis[stack_height-1] != '{' ||
               c == ']' && parenthesis[stack_height-1] != '[' ||
               stack_height < 2 )
          {
            PRINT_ERROR( "bad help string parenthesis:\n%s\n", lower_help );
            bad_string = VERDE_TRUE;
            continue;
          }
            // make sure something was matched
          if ( c == '}' )
          {
            if ( num_matches[stack_height-1] == 0 )
            {
              matches = VERDE_FALSE;
              break;
            }
          }
          stack_height--;
          remaining_help++;
          continue;
        }
        if ( c == '|' )
        {
          next_must_match[stack_height-1] = VERDE_FALSE;
            // only skip to end paren if we're within an enclosing set
            // of braces that didn't match
          skip_to_end_paren[stack_height-1] = 
            ( stack_height > 1 )? 
            skip_to_end_paren[stack_height-2] : VERDE_FALSE;            
          remaining_help++;
          continue;
        }
          // if skip_to_end_paren, skip all real words
          // basically we're just counting parenthesis at this point.
        if ( skip_to_end_paren[stack_height-1] )
        {
          remaining_help++;
          continue;
        }
          // real word, matches
        if ( strncmp( remaining_help, words_to_match[next_match],
                      strlen(words_to_match[next_match]) ) == 0 )
        {
          for ( int kk = strlen(words_to_match[next_match]); kk--; )
            remaining_help++;
          next_match++;
          num_matches[ stack_height-1 ]++;
            // must continue matching until the next | or closing parenthesis
          next_must_match[stack_height-1] = VERDE_TRUE;
          continue;
        }
          // real word, doesn't match
        else
        {
            // needed to match
          if ( next_must_match[stack_height-1] )
          {
            matches = VERDE_FALSE;
            break;
          }
            // Didn't need to match, so proceed to next |, ], or }
            // at the same stack height.
          else
          {
            skip_to_end_paren[stack_height-1] = VERDE_TRUE;
              // below doesn't keep track of stack_height correctly
//            while ( c != '|' && c != ']' && c != '}' &&
//                    remaining_help[1] != '\0' )
//            {
//              remaining_help++;
//              c = remaining_help[0];
//            }
          }          
        }
      }
        // matched everything before hitting end
      matches = matches && next_match == num_words;  
    }
      // !( order_dependent && consecutive_dependent )
    else
    {
      
      char *remaining_help = lower_help;
      for ( j = 0; matches == VERDE_TRUE && j < num_words; j++) 
      {
        int matches_jth_word = VERDE_FALSE;
      
          // if order doesn't matter, search from the beginning of the string.
          // otherwise, search from the last match+1
        if ( !order_dependent )
          remaining_help = lower_help;

          // while I haven't searched to the end of the help string,
          // and I haven't already matched the word...
        while ( remaining_help != NULL && matches_jth_word == VERDE_FALSE ) 
        {
            // find the next occurence of the matching word. Advance
            // "remaining_help" to this position.
          remaining_help = strstr( remaining_help, words_to_match[j] );

          if (remaining_help != NULL) {
          
              // *** Make sure match is not in the middle of a larger word. ***
          
              // *** Check after ***
              // find character after the word_to_match in the help string
            char next_char = remaining_help[ strlen( words_to_match[j] ) ];
              // say its the end of the word if its not a letter or number or the
              // underscore character '_'
            matches_jth_word = !( isalnum( next_char ) || next_char == '_');
          
              // *** Check before ***
            if ( matches_jth_word ) {
                // find character before. If there is no character before, say its
                // preceeded by a space.    
              char prev_char = remaining_help == lower_help ? ' ' :
                remaining_help[-1];
              matches_jth_word = !( isalnum( prev_char ) || prev_char == '_');
            }          
              // *** Iterate, avoid re-matching in current place ***
            remaining_help = &remaining_help[1]; 
          
          }
        }
        matches = matches_jth_word;
      }
    }
    if (matches) 
    {
      PRINT_INFO("%s", helpStrings[i] );
      help_printed = VERDE_TRUE;
    }
    delete [] lower_help;
  }
  
    // no help has been found for this keyword
  if (!help_printed) 
      PRINT_INFO("Help not found for the specified word %s.\n",
                 order_dependent ? "order" : "combination");
  PRINT_INFO("\n");

  for ( i = 0; i < num_compound_words; i++ )
    delete [] copied_words[i];
  delete [] words_to_match;

}


void CommandHandler::list_keywords() 
{
  unsigned int maxlen = 0;
  unsigned int curlen;
  unsigned int i;
  for(i=0; i < IDR::instance()->number_keywords(); i++) 
      if ((curlen = strlen(IDR::instance()->keyword_string(i))) > maxlen)
          maxlen = curlen;
  
    // subtract 1, to leave room for the space at the beginning of the line.
  const unsigned int columns = (screen_width-1) / (maxlen+2);
  const unsigned int rows = (unsigned int) ceil( ((double) IDR::instance()->number_keywords()) /
                               ((double) columns) );
  
  PRINT_INFO("The following keywords are supported in VERDE:\n\n " );
  
  for (i=0; i<rows; i++) {
    for (unsigned int j=i; j<IDR::instance()->number_keywords(); j+=rows)
        PRINT_INFO("%-*s", maxlen+2, IDR::instance()->keyword_string(j));
    PRINT_INFO("\n ");
  }
  PRINT_INFO(" \n ");
}


/*  helpStrings should be formatted as follows:
  
  Layout:
  1. Commas separate strings. 
  2. Use new-lines between 80-char lines.
  3. Comments and explanatory text appear before the string, surrounded 
     by three '*'s, e.g. "*** xxx ***", and should NOT be followed by a ','.
  4. For helpStrings taking more than 80 characters, lines after the
     first should be indented 5 spaces.
  5. If there is a common and a detailed form for a command, either
     place the detailed form inside [] on the next line, or put the detailed
     command in a new string, depending on the complexity.

  Syntax:
  6. Keywords should have their first letter capitalized. Names
     describing the type of input should be all lower case.
  7. Optional parameters are surrounded by [].
  8. Numeric parameters are surrounded by <>.
     e.g. "MBody <body_id_range> [Copy] Scale <scale>\n"
  9. When one of several parameters are required, the parameters
     should be separated by '|' and surrounded by {}.
     e.g. "Delete Fine Mesh {Volume|Surface|Curve} <id_range> [Propagate]\n"
 10. Use '|' also when one of several parameters are optional.
     e.g. "[set] Debug <index> [on|off]\n"

  Defaults:
 11. For optional words, the default should be in ALL-CAPS.
 12. For numeric parameters, the default should be appear following 
     an equals sign inside the '<>' as follows:
     " set  Corner Weight <value=1>\n",
     Valid ranges can appear in explanatory text or inside the <> as well.

 */

const char * const CommandHandler::helpStrings [] =
{

  // note:  win32 - crashes sometimes if there are commas missing
  //                between strings
    
  "Quit\n",
  "Exit\n",

  "Analyize\n",

  // doens't work yet
  //"Axis {on|off}\n",
  
  "Block <id> {on|off}\n",

  "Calculate\n",

  "Display\n",

  "Draw { hex | tet | wedge | pyramid | knife | quad | tri | edge | node } <entity_list>\n",

  "Echo on|off\n",

  "Graphics Mode { Wireframe | Hiddenline | Smoothshade }\n",
  "Graphics Reset\n"
  "Highlight failed hexes metric { which_metric }\n",
  "Highlight failed tets metric { which_metric }\n",
  "Highlight failed tris metric { which_metric }\n",
  "Highlight failed quads metric { which_metric }\n",

  "Label {nodes|elements|failed|nodesets|sidesets|blocks|} {on|off}\n", 

  // file operations
  "Load Defaults '<filename>'\n",

  "Model Edges {on|off}\n",

  // put nodeset and sideset next to each other even though it isn't in ABC order.
  "Nodeset <id> {on|off}\n",  
  "Sideset <id> {on|off}\n",  

  "Normals {on|off}\n",

  "Open '<filename>' [add] [blocks <id_list>] [block_info]\n",
  
  "Playback '<journal file>'\n",
  
  // journal recording commands:
  "Record '<journal_filename>'\n",
  "Record Stop\n",

  "Save { Genesis | Exodus } '<filename>'\n", 
  
  // Command Line Flags:

  "Set Topology_calculations {on|off}\n",
  "Set Interface_calculations {on|off}\n",
  "Set Metric_calculations {on|off}\n",
  "Set Algebraic_calculations {on|off}\n",
  "Set Diagnostic_calculations {on|off}\n",
  "Set Traditional_calculations {on|off}\n",
  "Set Verbose_model {on|off}\n",
  "Set Individual_output {on|off}\n",
  
  // Command Line Settings:
  
  "Set Quads_share_three_nodes {none|exterior|all}\n",

  // Hex Metric failure criteria:

  "Set Hex Aspect Minimum {value}\n",
  "Set Hex Aspect Maximum {value}\n",
  "Set Hex Skew Minimum {value}\n",
  "Set Hex Skew Maximum {value}\n",
  "Set Hex Taper Minimum {value}\n",
  "Set Hex Taper Maximum {value}\n",
  "Set Hex Volume Minimum {value}\n",
  "Set Hex Volume Maximum {value}\n",
  "Set Hex Stretch Minimum {value}\n",
  "Set Hex Stretch Maximum {value}\n",
  "Set Hex Diagonal Minimum {value}\n",
  "Set Hex Diagonal Maximum {value}\n",
  "Set Hex Dimension Minimum {value}\n",
  "Set Hex Dimension Maximum {value}\n",
  "Set Hex Oddy Minimum {value}\n",
  "Set Hex Oddy Maximum {value}\n",
  "Set Hex Condition Minimum {value}\n",
  "Set Hex Condition Maximum {value}\n",
  "Set Hex Jacobian Minimum {value}\n",
  "Set Hex Jacobian Maximum {value}\n",
  "Set Hex Scaled Jacobian Minimum {value}\n",
  "Set Hex Scaled Jacobian Maximum {value}\n",
  "Set Hex Shear Minimum {value}\n",
  "Set Hex Shear Maximum {value}\n",
  "Set Hex Shape Minimum {value}\n",
  "Set Hex Shape Maximum {value}\n",
  "Set Hex Relative Size Minimum {value}\n",
  "Set Hex Relative Size Maximum {value}\n",
  "Set Hex Shape and Size Minimum {value}\n",
  "Set Hex Shape and Size Maximum {value}\n",
  
  // Set point size
  "Set Point Size {value} min = 1, max = 10\n",
        
  // Tet Metrics failure criteria:

  "Set Tet Aspect Minimum {value}\n",
  "Set Tet Aspect Maximum {value}\n",
  "Set Tet Aspect Gamma Minimum {value}\n",
  "Set Tet Aspect Gamma Maximum {value}\n",
  "Set Tet Volume Minimum {value}\n",
  "Set Tet Volume Maximum {value}\n",
  "Set Tet Condition Minimum {value}\n",
  "Set Tet Condition Maximum {value}\n",
  "Set Tet Jacobian Minimum {value}\n",
  "Set Tet Jacobian Maximum {value}\n",
  "Set Tet Scaled Jacobian Minimum {value}\n",
  "Set Tet Scaled Jacobian Maximum {value}\n",
  "Set Tet Shear Minimum {value}\n",
  "Set Tet Shear Maximum {value}\n",
  "Set Tet Shape Minimum {value}\n",
  "Set Tet Shape Maximum {value}\n",
  "Set Tet Relative Size Minimum {value}\n",
  "Set Tet Relative Size Maximum {value}\n",
  "Set Tet Shape and Size Minimum {value}\n",
  "Set Tet Shape and Size Maximum {value}\n",

  // Pyramid Metrics failure criteria:

  "Set Pyramid Volume Minimum {value}\n",
  "Set Pyramid Volume Maximum {value}\n",


  // Wedge Metrics failure criteria:

  "Set Wedge Volume Minimum {value}\n",
  "Set Wedge Volume Maximum {value}\n",


  // Knife Metrics failure criteria:

  "Set Knife Volume Minimum {value}\n",
  "Set Knife Volume Maximum {value}\n",


  // Quad Metrics failure criteria:

  "Set Quad Aspect Minimum {value}\n",
  "Set Quad Aspect Maximum {value}\n",
  "Set Quad Skew Minimum {value}\n",
  "Set Quad Skew Maximum {value}\n",
  "Set Quad Taper Minimum {value}\n",
  "Set Quad Taper Maximum {value}\n",
  "Set Quad Warpage Minimum {value}\n",
  "Set Quad Warpage Maximum {value}\n",
  "Set Quad Area Minimum {value}\n",
  "Set Quad Area Maximum {value}\n",
  "Set Quad Stretch Minimum {value}\n",
  "Set Quad Stretch Maximum {value}\n",
  "Set Quad Smallest Angle Minimum {value}\n",
  "Set Quad Smallest Angle Maximum {value}\n",
  "Set Quad Largest Angle Minimum {value}\n",
  "Set Quad Largest Angle Maximum {value}\n",
  "Set Quad Oddy Minimum {value}\n",
  "Set Quad Oddy Maximum {value}\n",
  "Set Quad Condition Minimum {value}\n",
  "Set Quad Condition Maximum {value}\n",
  "Set Quad Jacobian Minimum {value}\n",
  "Set Quad Jacobian Maximum {value}\n",
  "Set Quad Scaled Jacobian Minimum {value}\n",
  "Set Quad Scaled Jacobain Maximum {value}\n",
  "Set Quad Shear Minimum {value}\n",
  "Set Quad Shear Maximum {value}\n",
  "Set Quad Shape Minimum {value}\n",
  "Set Quad Shape Maximum {value}\n",
  "Set Quad Relative Size Minimum {value}\n",
  "Set Quad Relative Size Maximum {value}\n",
  "Set Quad Shape and Size Minimum {value}\n",
  "Set Quad Shape and Size Maximum {value}\n",


  // Tri Metrics failure criteria:

  "Set Tri Aspect Minimum {value}\n",
  "Set Tri Aspect Maximum {value}\n",
  "Set Tri Area Minimum {value}\n",
  "Set Tri Area Maximum {value}\n",
  "Set Tri Largest Angle Minimum {value}\n",
  "Set Tri Largest Angle Maximum {value}\n",
  "Set Tri Smallest Angle Minimum {value}\n",
  "Set Tri Smallest Angle Maximum {value}\n",
  "Set Tri Condition Minimum {value}\n",
  "Set Tri Condition Maximum {value}\n",
  "Set Tri Scaled Jacobian Minimum {value}\n",
  "Set Tri Scaled Jacobian Maximum {value}\n",
  "Set Tri Shear Minimum {value}\n",
  "Set Tri Shear Maximum {value}\n",
  "Set Tri Shape Minimum {value}\n",
  "Set Tri Shape Maximum {value}\n",
  "Set Tri Relative Size Minimum {value}\n",
  "Set Tri Relative Size Maximum {value}\n",
  "Set Tri Shape and Size Minimum {value}\n",
  "Set Tri Shape and Size Maximum {value}\n",

  // Edge Metrics failure criteria

  "Set Edge Length Minimum {value}\n",
  "Set Edge Length Maximum {value}\n",

  "Skin {on|off}\n",


      // Last entry must be NULL, as a signal that its the last one.
    NULL  // DO NOT MOVE or REMOVE
};



void CommandHandler::Set(const FunctionData* parsed_data)
{
  
  int currkeyword = 0;
  
  do
  {
    
    if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "quads_share_three_nodes" ))
    {
      // set warning level value
      if(VerdeUtil::compare( parsed_data->keywordIdentifier[currkeyword+1], "none"))
        SkinTool::set_quads_sharing_three_nodes_completeness(0);
      if(VerdeUtil::compare( parsed_data->keywordIdentifier[currkeyword+1], "exterior"))
        SkinTool::set_quads_sharing_three_nodes_completeness(1);
      if(VerdeUtil::compare( parsed_data->keywordIdentifier[currkeyword+1], "all"))
        SkinTool::set_quads_sharing_three_nodes_completeness(2);
    }  
    
    else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "topology_calculations" )
        && VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword+1], "off") )
    {
      // set topology_calculations off
      verde_app->do_topology(VERDE_FALSE);
    }

    else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "topology_calculations" )
      && VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword+1], "on") )
    {
      // set topology_calculations on
      verde_app->do_topology(VERDE_TRUE);
    }

    else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "interface_calculations" )
      && VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword+1], "off") )
    {
      // set interface_calculations off
      verde_app->do_interface(VERDE_FALSE);
    }

    else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "interface_calculations" )
      && VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword+1], "on") )
    {
      // set interface_calculations on
      verde_app->do_interface(VERDE_TRUE);
    }
  
    else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "metric_calculations" )
      && VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword+1], "off") )
    {
      // set metric_calculations off
      verde_app->do_metrics(VERDE_FALSE);
    }
  
    else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "metric_calculations" )
      && VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword+1], "on") )
    {
      // set metric_calculations on
      verde_app->do_metrics(VERDE_TRUE);
    }

    else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "algebraic_calculations" )
      && VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword+1], "off") )
    {
      // set algebraic_calculations off
      Metric::do_algebraic(VERDE_FALSE);
    }
  
    else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "algebraic_calculations" )
      && VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword+1], "on") )
    {
      // set algebraic_calculations on
      Metric::do_algebraic(VERDE_TRUE);
    }

    else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "diagnostic_calculations" )
      && VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword+1], "off") )
    {
      // set diagnostic_calculations off
      Metric::do_diagnostic(VERDE_FALSE);
    }
  
    else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "diagnostic_calculations" )
      && VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword+1], "on") )
    {
      // set diagnostic_calculations on
      Metric::do_diagnostic(VERDE_TRUE);
    }

    else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "traditional_calculations" )
      && VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword+1], "off") )
    {
      // set traditional_calculations off
      Metric::do_traditional(VERDE_FALSE);
    }
  
    else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "traditional_calculations" )
      && VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword+1], "on") )
    {
      // set traditional_calculations on
      Metric::do_traditional(VERDE_TRUE);
    }

    else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "individual_output" )
      && VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword+1], "off") )
    {
      // set individual_output off
      verde_app->do_individual(VERDE_FALSE);
    }
  
    else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "individual_output" )
      && VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword+1], "on") )
    {
      // set individual_output on
      verde_app->do_individual(VERDE_TRUE);
    }
  
    else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "verbose_model" )
      && VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword+1], "off") )
    {
      // set verbose_model off
      verde_app->print_failed_elements(VERDE_FALSE);
    }
  
    else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "verbose_model" )
      && VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword+1], "on") )
    {
      // set verbose_model on
      verde_app->print_failed_elements(VERDE_TRUE);
    }
    else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "autofit" ) )
    {
      if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword+1], "on" ) )
      { 
          DrawingTool::instance()->toggle_autofit(1);
          UserEventHandler::instance()->post_message(UserMessage(Autofit, "1" ));
      }    
      else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword+1], "off" ) )
      {
          DrawingTool::instance()->toggle_autofit(0);
          UserEventHandler::instance()->post_message(UserMessage(Autofit));
      }
    }   
    else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "point size" ) )
    {
      GLWindow::first_instance()->set_point_size( parsed_data->realData[currkeyword] );
      GLWindow::first_instance()->updateGL();
    }
    else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "quick transform" ) )
    {
      if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword+1], "off" ) )
      {
        GLWindow::first_instance()->quick_transform(false);
        UserEventHandler::instance()->post_message(UserMessage(QuickTransform, "1"));
      }
      else
      {
        GLWindow::first_instance()->quick_transform(true);
        UserEventHandler::instance()->post_message(UserMessage(QuickTransform, 0));
      }
      
    }
  
    // if no success, print help
    else
    {
      PRINT_ERROR("Command not issued correctly\n");
      break;
    }
  
    currkeyword += 2;
    } while(!VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], ""));


  command_wrapup(parsed_data);

}

void CommandHandler::Calculate(const FunctionData* parsed_data)
{
  //changes cursor to hourglass  
  UserEventHandler::instance()->post_message(UserMessage(SetMouseToHourglass), TRUE);

  verde_app->verify_mesh();
  UserEventHandler::instance()->post_message(UserMessage(ClearHighlighted));
  UserEventHandler::instance()->post_message(UserMessage(UpdateGraphics));
  UserEventHandler::instance()->post_message(UserMessage(ShowFailedElements));
  
  //changes cursor back to arrow
  UserEventHandler::instance()->post_message(UserMessage(RestoreMouse));
  command_wrapup(parsed_data);

}

void CommandHandler::SetHex(const FunctionData* parsed_data)
{

  int currkeyword = 0;


  // pointer to the set_min or set_max function
  void (HexMetric::*minmax_func)(enum hexMetrics, double) = 0;


  do

    {

      minmax_func = 0;

  if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword+1], "minimum" ))
    minmax_func = &HexMetric::set_min;
  else if (VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword+1], "maximum"))
    minmax_func = &HexMetric::set_max;


  if(!minmax_func)
    {
      PRINT_ERROR("Value not specified\n");
      break;
    }

  
  // set hex aspect {min | max} value
  if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "aspect"))
    (HexMetric::instance()->*minmax_func)(HEX_ASPECT, parsed_data->realData[currkeyword+1]);
    
  // set hex skew {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "skew"))
    (HexMetric::instance()->*minmax_func)(HEX_SKEW, parsed_data->realData[currkeyword+1]);
    
  // set hex taper {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "taper"))
    (HexMetric::instance()->*minmax_func)(HEX_TAPER, parsed_data->realData[currkeyword+1]);

  // set hex volume {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "volume"))
    (HexMetric::instance()->*minmax_func)(HEX_VOLUME, parsed_data->realData[currkeyword+1]);

  // set hex stretch {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "stretch"))
    (HexMetric::instance()->*minmax_func)(HEX_STRETCH, parsed_data->realData[currkeyword+1]);

  // set hex diagonal {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "diagonal"))
    (HexMetric::instance()->*minmax_func)(HEX_DIAGONALS, parsed_data->realData[currkeyword+1]);

  // set hex dimension {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "dimension"))
    (HexMetric::instance()->*minmax_func)(HEX_CHARDIM, parsed_data->realData[currkeyword+1]);

  // set hex oddy {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "oddy"))
    (HexMetric::instance()->*minmax_func)(HEX_ODDY, parsed_data->realData[currkeyword+1]);
  
  // set hex condition {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "condition"))
    (HexMetric::instance()->*minmax_func)(HEX_CONDITION, parsed_data->realData[currkeyword+1]);

  // set hex jacobian {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "jacobian"))
    (HexMetric::instance()->*minmax_func)(HEX_JACOBIAN, parsed_data->realData[currkeyword+1]);

  // set hex scaled jacobian {max | min} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "scaled jacobian"))
    (HexMetric::instance()->*minmax_func)(HEX_NORM_JACOBIAN, parsed_data->realData[currkeyword+1]);

  // set hex shear {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "shear"))
    (HexMetric::instance()->*minmax_func)(HEX_SHEAR, parsed_data->realData[currkeyword+1]);

  // set hex shape {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "shape"))
    (HexMetric::instance()->*minmax_func)(HEX_SHAPE, parsed_data->realData[currkeyword+1]);

  // set hex relative size {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "relative size"))
    (HexMetric::instance()->*minmax_func)(HEX_RELSIZE, parsed_data->realData[currkeyword+1]);

  // set hex shape and size {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "shape and size"))
    (HexMetric::instance()->*minmax_func)(HEX_SHAPE_SIZE, parsed_data->realData[currkeyword+1]);

  // if we got this far, the command was not issued correctly
  else
    {
      PRINT_ERROR("Command not issued correctly\n");
      break;
    }

  currkeyword += 2;

    } while(!VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "" ));

  command_wrapup(parsed_data);

}

void CommandHandler::SetTet(const FunctionData* parsed_data)
{


  int currkeyword = 0;


  //pointer to the set_min or set_max function
  void (TetMetric::*minmax_func)(enum tetMetrics, double) = 0;

  do
    {
      minmax_func = 0;
  if(VerdeUtil::compare(parsed_data->keywordIdentifier[1], "minimum" ))
    minmax_func = &TetMetric::set_min;
  else if (VerdeUtil::compare(parsed_data->keywordIdentifier[1], "maximum"))
    minmax_func = &TetMetric::set_max;


  // check to see if a valid number was specified
  if(!minmax_func)
    {
      PRINT_ERROR("Value not specified\n");
      break;
    }

  
  // set tet aspect {min | max} value
  if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "aspect"))
    (TetMetric::instance()->*minmax_func)(ASPECT_RATIO, parsed_data->realData[currkeyword+1]);

  // set tet aspect gamma {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "aspect gamma"))
    (TetMetric::instance()->*minmax_func)(ASPECT_RATIO_GAMMA, parsed_data->realData[currkeyword+1]);

  // set tet volume {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "volume"))
    (TetMetric::instance()->*minmax_func)(TET_VOLUME, parsed_data->realData[currkeyword+1]);

  // set tet condition {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "condition"))
    (TetMetric::instance()->*minmax_func)(TET_CONDITION_NUMBER, parsed_data->realData[currkeyword+1]);

  // set tet jacobian {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "jacobian"))
    (TetMetric::instance()->*minmax_func)(TET_JACOBIAN, parsed_data->realData[currkeyword+1]);

  // set tet scaled jacobian {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "scaled jacobian"))
    (TetMetric::instance()->*minmax_func)(TET_NORM_JACOBIAN, parsed_data->realData[currkeyword+1]);

  // set tet shear {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "shear"))
    (TetMetric::instance()->*minmax_func)(TET_SHEAR, parsed_data->realData[currkeyword+1]);

  // set tet shape {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "shape"))
    (TetMetric::instance()->*minmax_func)(TET_SHAPE, parsed_data->realData[currkeyword+1]);

  // set tet relative size {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "relative size"))
    (TetMetric::instance()->*minmax_func)(TET_RELSIZE, parsed_data->realData[currkeyword+1]);

  // set tet shape and size {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "shape and size"))
    (TetMetric::instance()->*minmax_func)(TET_SHAPE_SIZE, parsed_data->realData[currkeyword+1]);

  // if we get this far, the command was not issued correctly
  else
    {
      PRINT_ERROR("Command not issued correctly\n");
      break;
    }
  
  currkeyword += 2;
    } while (!VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "" ));

  command_wrapup(parsed_data);

}

void CommandHandler::SetPyramid(const FunctionData* parsed_data)
{

  int currkeyword = 0;

  do
    {
  if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "volume") )
  {
    if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword+1], "minimum"))
      {
        // set pyramid volume min value
        PyramidMetric::instance()->set_min(PYRAMID_VOLUME, parsed_data->realData[currkeyword+1]);
      }
    
    else if(VerdeUtil::compare(parsed_data->keywordIdentifier[1], "maximum"))
      {
        // set pyramid volume max value
        PyramidMetric::instance()->set_max(PYRAMID_VOLUME, parsed_data->realData[currkeyword+1]);
      }
    else
        PRINT_ERROR("Minimum or Maximum not specified\n");
  }

  else
    {
      // if we make it this far, the command was not issued correctly
      PRINT_ERROR("Command not issued correctly\n");
      break;
    }

  currkeyword += 2;

    } while (!VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "" ));

  command_wrapup(parsed_data);

}

void CommandHandler::SetKnife(const FunctionData* parsed_data)
{

  int currkeyword = 0;

  do 
    {
  
  if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "volume") )
  {
      if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword+1], "minimum") )
      {
        // set knife volume min value
        KnifeMetric::instance()->set_min(KNIFE_VOLUME, parsed_data->realData[currkeyword+1] );
      }

      else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword+1], "maximum" ))
      {
        // set knife volume max value
        KnifeMetric::instance()->set_max(KNIFE_VOLUME, parsed_data->realData[currkeyword+1] );
      }
      else
        PRINT_ERROR("Minimum or Maximum not specified\n");
  }
  else
    {
      // if we make it this far, the command was not issued correctly
      PRINT_ERROR("Command not issued correctly\n");
      break;
    }

  currkeyword += 2;
    } while (!VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "" ));
  command_wrapup(parsed_data);
 
} 

void CommandHandler::SetQuad(const FunctionData* parsed_data)
{

  int currkeyword = 0;
  
  // pointer to the set_min or set_max function
  void (QuadMetric::*minmax_func)(enum quadMetrics, double) = 0;

  do
    {
      minmax_func = 0;
  if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword+1], "minimum" ))
    minmax_func = &QuadMetric::set_min;
  else if (VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword+1], "maximum"))
    minmax_func = &QuadMetric::set_max;
  else
  {
    // not correct syntax
    PRINT_ERROR("Command not issued correctly\n");
    break;
  }

  if(!minmax_func)
  {
    // invalid number
    PRINT_ERROR("Value not specified\n");
    break;
  }

  // set quad aspect {min | max} value
  if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "aspect"))
    (QuadMetric::instance()->*minmax_func)(QUAD_ASPECT, parsed_data->realData[currkeyword+1]);

  // set quad skew {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "skew"))
    (QuadMetric::instance()->*minmax_func)(QUAD_SKEW, parsed_data->realData[currkeyword+1]);

  // set quad taper {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "taper"))
    (QuadMetric::instance()->*minmax_func)(QUAD_TAPER, parsed_data->realData[currkeyword+1]);
  
  // set quad warpage {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "warpage"))
    (QuadMetric::instance()->*minmax_func)(QUAD_WARPAGE, parsed_data->realData[currkeyword+1]);

  // set quad area {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "area"))
    (QuadMetric::instance()->*minmax_func)(QUAD_AREA, parsed_data->realData[currkeyword+1]);

  // set quad stretch {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "stretch"))
    (QuadMetric::instance()->*minmax_func)(QUAD_STRETCH, parsed_data->realData[currkeyword+1]);

  // set quad smallest angle {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "smallest angle"))
    (QuadMetric::instance()->*minmax_func)(QUAD_MIN_ANGLE, parsed_data->realData[currkeyword+1]);

  // set quad largest angle {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "largest angle"))
    (QuadMetric::instance()->*minmax_func)(QUAD_ANGLE, parsed_data->realData[currkeyword+1]);

  // set quad oddy {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "oddy"))
    (QuadMetric::instance()->*minmax_func)(QUAD_MAX_ODDY, parsed_data->realData[currkeyword+1]);
  
  // set quad condition {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "condition"))
    (QuadMetric::instance()->*minmax_func)(QUAD_MAX_COND, parsed_data->realData[currkeyword+1]);

  // set quad jacobian {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "jacobian"))
    (QuadMetric::instance()->*minmax_func)(QUAD_MIN_JAC, parsed_data->realData[currkeyword+1]);
  
  // set quad scaled jacobian {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "scaled jacobian"))
    (QuadMetric::instance()->*minmax_func)(QUAD_MIN_SC_JAC, parsed_data->realData[currkeyword+1]);

  // set quad shear {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "shear"))
    (QuadMetric::instance()->*minmax_func)(QUAD_SHEAR, parsed_data->realData[currkeyword+1]);

  // set quad shape {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "shape"))
    (QuadMetric::instance()->*minmax_func)(QUAD_SHAPE, parsed_data->realData[currkeyword+1]);
  
  // set quad relative size {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "relative size"))
    (QuadMetric::instance()->*minmax_func)(QUAD_REL_SIZE, parsed_data->realData[currkeyword+1]);
  
  // set quad shape and size {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "shape and size"))
    (QuadMetric::instance()->*minmax_func)(QUAD_SHAPE_SIZE, parsed_data->realData[currkeyword+1]);


  // if we make it this far, the command was not issued correctly
  // print help
  else
    {
      PRINT_ERROR("Command not issued correctly\n");
      break;
    }

  currkeyword += 2;

    } while (!VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "" ));

  command_wrapup(parsed_data);
   
} 

void CommandHandler::SetTri(const FunctionData* parsed_data)
{

  int currkeyword = 0;



  // pointer to the set_min or set_max function
  void (TriMetric::*minmax_func)(enum triMetrics, double) = 0;


  do
    {
      minmax_func = 0;

  if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword+1], "minimum" ))
    minmax_func = &TriMetric::set_min;
  else if (VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword+1], "maximum"))
    minmax_func = &TriMetric::set_max;


  if(!minmax_func)
  {
    // invalid number
    PRINT_ERROR("Command not issued correctly\n");
    break;
  }

  // set tri aspect {min | max} value
  if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "aspect"))
    (TriMetric::instance()->*minmax_func)(TRI_ASPECT, parsed_data->realData[currkeyword+1]);
  
  // set tri area {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "area"))
    (TriMetric::instance()->*minmax_func)(TRI_AREA, parsed_data->realData[currkeyword+1]);

  // set tri smallest angle {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "smallest angle"))
    (TriMetric::instance()->*minmax_func)(TRI_MIN_ANGLE, parsed_data->realData[currkeyword+1]);
    
  // set tri largest angle {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "largest angle"))
    (TriMetric::instance()->*minmax_func)(TRI_ANGLE, parsed_data->realData[currkeyword+1]);
  
  // set tri condition {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "condition"))
    (TriMetric::instance()->*minmax_func)(TRI_CONDITION_NUMBER, parsed_data->realData[currkeyword+1]);
  
  // set tri scaled jacobian {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "scaled jacobian"))
    (TriMetric::instance()->*minmax_func)(TRI_MIN_SC_JAC, parsed_data->realData[currkeyword+1]);
  
  // set tri shear {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "shear"))
    (TriMetric::instance()->*minmax_func)(TRI_SHEAR, parsed_data->realData[currkeyword+1]);
  
  // set tri shape {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "shape"))
    (TriMetric::instance()->*minmax_func)(TRI_SHAPE, parsed_data->realData[currkeyword+1]);
  
  // set tri relative size {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "relative size"))
    (TriMetric::instance()->*minmax_func)(TRI_REL_SIZE, parsed_data->realData[currkeyword+1]);
  
  // set tri shape and size {min | max} value
  else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "shape and size"))
    (TriMetric::instance()->*minmax_func)(TRI_SHAPE_SIZE, parsed_data->realData[currkeyword+1]);
 

  // if we make it this far, the command was not issued correctly
  else
    {
      PRINT_ERROR("Command not issued correctly\n");
      break;
    }

  currkeyword += 2;
  
    } while (!VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "" ));


  command_wrapup(parsed_data);
 
} 

void CommandHandler::SetEdge(const FunctionData* parsed_data)
{

  int currkeyword = 0;

  do
    {

  if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "length") )
  {
      if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword+1], "minimum") )
      {
        // set edge length min value
        EdgeMetric::instance()->set_min(EDGE_LENGTH, parsed_data->realData[currkeyword+1] );
      }
      else if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword+1], "maximum" ))
      {
        // set edge length max value
        EdgeMetric::instance()->set_max(EDGE_LENGTH, parsed_data->realData[currkeyword+1] );
      }
      else
        {
          PRINT_ERROR("Command not issued correctly\n");
          break;
        }
  }
  else
    {
      PRINT_ERROR("Command not issued correctly\n");
      break;
    }

  currkeyword += 2;

    } while (!VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "" ));

  command_wrapup(parsed_data);


}

void CommandHandler::SetWedge(const FunctionData* parsed_data)
{
  
  int currkeyword = 0;

  

  
  // pointer to the set_min or set_max function
  void (WedgeMetric::*minmax_func)(enum wedgeMetrics, double) = 0;

  do 

    {

      minmax_func = 0;
  if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword+1], "minimum" ))
    minmax_func = &WedgeMetric::set_min;
  else if (VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword+1], "maximum"))
    minmax_func = &WedgeMetric::set_max;


  if(!minmax_func)
  {
    PRINT_ERROR("Command not issued correctly\n");
    break;
  }

  
  if(VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], "volume"))
  {
    // set wedge volume {min | max} value
    (WedgeMetric::instance()->*minmax_func)(WEDGE_VOLUME, parsed_data->realData[currkeyword+1]);
  }

  else
    {
      PRINT_ERROR("Command not issued correctly\n");
      break;
    }

  
  currkeyword += 2;

    } while (!VerdeUtil::compare(parsed_data->keywordIdentifier[currkeyword], ""));


  command_wrapup(parsed_data);
 
}   

void CommandHandler::open_mesh(const FunctionData* parsed_data)
{
  //changes cursor to hourglass
  UserEventHandler::instance()->post_message(UserMessage(SetMouseToHourglass), TRUE);

  if(strlen(parsed_data->characterData[0]) == 0)
  {
    PRINT_ERROR("No filename specified\n");
    return;
  }
  int curr_keyword = 1;
  
  if(verde_app->mesh() && VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "add"))
  {
    verde_app->set_add_mesh(1);
    curr_keyword++;
  }
  else
  {
    verde_app->set_add_mesh(0);
    ElementBlock::static_reset();
    ElementBC::static_reset();
    NodeBC::static_reset();
  }
  
  if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "block_info"))
  {
    SkinTool::clear_skin();
    EdgeTool::instance()->reset();
    verde_app->load_mesh(parsed_data->characterData[0], VERDE_FALSE);
    std::deque<int> empty_block_list;
    verde_app->mesh()->open_exodus_file(parsed_data->characterData[0], &empty_block_list);
  }
  else
    verde_app->load_mesh(parsed_data->characterData[0]);
  
  UserEventHandler::instance()->post_message(UserMessage(UpdateModelView));
  UserEventHandler::instance()->post_message(UserMessage(ClearOutModelResultView));
  UserEventHandler::instance()->post_message(UserMessage(NewMesh));
  
  //changes cursor back to arrow
  UserEventHandler::instance()->post_message(UserMessage(RestoreMouse));
  command_wrapup(parsed_data);
}

void CommandHandler::PlayJournal(const FunctionData* parsed_data)
{
  
  // UI will automatically parse the file
  // GUICommandHandler will check to see that the file is parsed
  UserInterface::instance()->add_input_file(parsed_data->characterData[0]);

  command_wrapup(parsed_data);
   
} 


void CommandHandler::record_handler(const FunctionData* parsed_data)
{
  if(VerdeUtil::compare("stop", parsed_data->keywordIdentifier[0]) ) 
  {
    UserInterface::instance()->close_recording_stream();
  }
  else
  {
    UserInterface::instance()->
      open_recording_stream(parsed_data->characterData[0]);
  }
  clear_echo_stream();  // Don't journal this command.
  command_wrapup(parsed_data);

}

void CommandHandler::echo_handler(const FunctionData* parsed_data)
{
  if(VerdeUtil::compare("off", parsed_data->keywordIdentifier[0]))
  {
    UserInterface::instance()->command_is_echoed(VERDE_FALSE);
  }
  else
  {
    UserInterface::instance()->command_is_echoed(VERDE_TRUE);
  }

}

void CommandHandler::load_defaults(const FunctionData* parsed_data)
{
  if(strlen(parsed_data->characterData[0]) > 0)
    UserInterface::instance()->process_defaults_file(parsed_data->characterData[0]);
  else
    UserInterface::instance()->process_defaults_file();

  command_wrapup(parsed_data);
}

void CommandHandler::toggle_blocks(const FunctionData* parsed_data)
{
 // process block active commands

  // First, make sure mesh is loaded

  Mesh *mesh = verde_app->mesh();
  if (!mesh)
  {
    PRINT_ERROR("No Mesh Loaded...\n");
    return;
  }

  const int num_words = parsed_data->dataListLength;

  // see if the specified block exists

  ElementBlock *block = mesh->get_element_block(parsed_data->integerData[0]);

  if(!block)
  {
    PRINT_ERROR("Block %d not found...\n",parsed_data->integerData[0]);
    return;
  }

  // see if on/off toggle was specified

  if(VerdeUtil::compare("on", parsed_data->keywordIdentifier[num_words - 1]))
  {
    for(int i=0; i<num_words-1; i++)
    {
      //get the next block
      block = mesh->get_element_block(parsed_data->integerData[i]);
      if(!block)
      {
        PRINT_ERROR("Block %d not found...\n",parsed_data->integerData[i]);
        return;
      }

      if(!block->is_active())
      {
        block->active(VERDE_TRUE);
        mesh->load_element_block(block);
   	block->skin_tool()->add_block( block );	

        char block_num[100];
        sprintf(block_num, "%i", parsed_data->integerData[i]);
        UserEventHandler::instance()->post_message(UserMessage(BlockActive, block_num));
      }
    }
    EdgeTool::instance()->reset();
    EdgeTool::instance()->find_exterior_edges();
    //NotifyGUI here
    UserEventHandler::instance()->post_message(UserMessage(UpdateSkin));

  }
  else if(VerdeUtil::compare("off", parsed_data->keywordIdentifier[num_words - 1]))
  {
    for(int i=0; i<num_words-1; i++)
    {
      block = mesh->get_element_block(parsed_data->integerData[i]);

      //get the next block
      if(!block)
      {
        PRINT_ERROR("Block %d not found...\n",parsed_data->integerData[i]);
        return;
      }

      if(block->is_active())
      {
        block->active(VERDE_FALSE);
        block->skin_tool()->remove_block( block );	

        char block_num[100];
        sprintf(block_num, "%i", parsed_data->integerData[i]);
        UserEventHandler::instance()->post_message(UserMessage(BlockInActive, block_num));
      }
    }

    EdgeTool::instance()->reset();
    EdgeTool::instance()->find_exterior_edges();
    //NotifyGUI here
    UserEventHandler::instance()->post_message(UserMessage(UpdateSkin));
    
  }
  else
  {
    PRINT_ERROR("Block command not correct...\n");
    return;
  }

  command_wrapup(parsed_data);
}

void CommandHandler::toggle_node_bcs(const FunctionData* parsed_data)
{

  // process nodebc active commands

  // First, make sure mesh is loaded

  Mesh *mesh = verde_app->mesh();
  if (!mesh)
  {
    PRINT_ERROR("No Mesh Loaded...\n");
    return;
  }

  // see if the specified node bc exists

  NodeBC *bc = mesh->get_node_bc(parsed_data->integerData[0]);

  if(!bc)
  {
    PRINT_ERROR("Nodal Boundary Condition %d not found...\n",
                parsed_data->integerData[0]);
    return;
  }

  // see if on/off toggle was specified
    
  if(VerdeUtil::compare("on", parsed_data->keywordIdentifier[1]))
  {
    if(!bc->is_active())
    {
      bc->active(VERDE_TRUE);
      //NotifyGUI here
      char bc_num[100];
      sprintf(bc_num, "%i", parsed_data->integerData[0]);
      UserEventHandler::instance()->post_message(UserMessage(NodeBCActive, bc_num));
      UserEventHandler::instance()->post_message(UserMessage(UpdateGraphics));
    }
  }
  else if(VerdeUtil::compare("off", parsed_data->keywordIdentifier[1]))
  {
    if(bc->is_active())
    {
      bc->active(VERDE_FALSE);
      //NotifyGUI here
      char bc_num[100];
      sprintf(bc_num, "%i", parsed_data->integerData[0]);
      UserEventHandler::instance()->post_message(UserMessage(NodeBCInActive, bc_num));
      UserEventHandler::instance()->post_message(UserMessage(UpdateGraphics));

    }
  }
  else
  {
    PRINT_ERROR("Block command not correct...\n");
    return;
  }

  command_wrapup(parsed_data);
}

void CommandHandler::toggle_element_bcs(const FunctionData* parsed_data)
{

  // process elementbc active commands

  // First, make sure mesh is loaded

  Mesh *mesh = verde_app->mesh();
  if (!mesh)
  {
    PRINT_ERROR("No Mesh Loaded...\n");
    return;
  }

  // see if the specified element bc exists

  ElementBC *bc = mesh->get_element_bc(parsed_data->integerData[0]);

  if(!bc)
  {
    PRINT_ERROR("Element Boundary Condition %d not found...\n",
                parsed_data->integerData[0]);
    return;
  }

  // see if on/off toggle was specified
    
  if(VerdeUtil::compare("on", parsed_data->keywordIdentifier[1]))
  {
    if(!bc->is_active())
    {
      bc->active(VERDE_TRUE);
      
      //NotifyGUI here
      char elem_num[100];
      sprintf(elem_num, "%i", parsed_data->integerData[0]);
      UserEventHandler::instance()->post_message(UserMessage(ElementBCActive, elem_num));
      UserEventHandler::instance()->post_message(UserMessage(UpdateGraphics));
    }
  }
  else if(VerdeUtil::compare("off", parsed_data->keywordIdentifier[1]))
  {
    if(bc->is_active())
    {
      bc->active(VERDE_FALSE);
      //NotifyGUI here
      char elem_num[100];
      sprintf(elem_num, "%i", parsed_data->integerData[0]);
      UserEventHandler::instance()->post_message(UserMessage(ElementBCInActive, elem_num));
      UserEventHandler::instance()->post_message(UserMessage(UpdateGraphics));
    }
  }
  else
  {
    PRINT_ERROR("Block command not correct...\n");
    return;
  }

  command_wrapup(parsed_data);
}

void CommandHandler::print_parsed_data( const FunctionData* parsed_data )
{
   int i;
   int n = parsed_data->dataListLength;
   PRINT_INFO( "\nContents of FunctionData:\n" );
   PRINT_INFO( "-------------------------\n" );
   PRINT_INFO( "keywordId = %d (%s)\n", parsed_data->keywordId,
      IDR::instance()->keyword_string(parsed_data->keywordId) );
   PRINT_INFO( "dataListLength = %d\n", parsed_data->dataListLength );
   PRINT_INFO( "integerData =\n" );
   for( i=0; i<n; i++ )
   {
      if( parsed_data->integerData[i] == VERDE_INT_MAX )
         PRINT_INFO( "   integerData[%d] = VERDE_INT_MAX\n", i );
      else
         PRINT_INFO( "   integerData[%d] = %d\n", i, parsed_data->integerData[i] );
   }
   PRINT_INFO( "realData =\n" );
   for( i=0; i<n; i++ )
   {
      if( parsed_data->realData[i] == VERDE_DBL_MAX )
         PRINT_INFO( "   realData[%d] = VERDE_DBL_MAX\n", i );
      else
         PRINT_INFO( "   realData[%d] = %.6lf\n", i, parsed_data->realData[i] );
   }
   PRINT_INFO( "characterData =\n" );
   for( i=0; i<n; i++ )
      PRINT_INFO( "   characterData[%d] = %s\n", i, parsed_data->characterData[i] );
   PRINT_INFO( "keywordIdentifier =\n" );
   for( i=0; i<n; i++ )
      PRINT_INFO( "   keywordIdentifier[%d] = %s\n", i, parsed_data->keywordIdentifier[i] );


   // Also dump to a file
   FILE *fp = fopen("parsed_data.dat", "w");
   fprintf( fp,  "\nContents of FunctionData:\n" );
   fprintf( fp,  "-------------------------\n" );
   fprintf( fp,  "keywordId = %d (%s)\n", parsed_data->keywordId,
      IDR::instance()->keyword_string(parsed_data->keywordId) );
   fprintf( fp,  "dataListLength = %d\n", parsed_data->dataListLength );
   fprintf( fp,  "integerData =\n" );
   for( i=0; i<n; i++ )
   {
      if( parsed_data->integerData[i] == VERDE_INT_MAX )
         fprintf( fp,  "   integerData[%d] = VERDE_INT_MAX\n", i );
      else
         fprintf( fp,  "   integerData[%d] = %d\n", i, parsed_data->integerData[i] );
   }
   fprintf( fp,  "realData =\n" );
   for( i=0; i<n; i++ )
   {
      if( parsed_data->realData[i] == VERDE_DBL_MAX )
         fprintf( fp,  "   realData[%d] = VERDE_DBL_MAX\n", i );
      else
         fprintf( fp,  "   realData[%d] = %.6e\n", i, parsed_data->realData[i] );
   }
   fprintf( fp,  "characterData =\n" );
   for( i=0; i<n; i++ )
      fprintf( fp,  "   characterData[%d] = %s\n", i, parsed_data->characterData[i] );
   fprintf( fp,  "keywordIdentifier =\n" );
   for( i=0; i<n; i++ )
      fprintf( fp, "   keywordIdentifier[%d] = %s\n", i, parsed_data->keywordIdentifier[i] );

   fclose( fp );

}



void CommandHandler::graphics_handler(const FunctionData* parsed_data)
{
    //Get out if you're in batch mode
    if(UserInterface::instance()->GetBatchMode())
      return;

   int curr_keyword = 0;
   while(!VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "") )
   {
      if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "mode") )
      {
         curr_keyword++;
         if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "smoothshade" ))
         {
            GLWindow::first_instance()->set_graphics_mode(SMOOTHSHADE);
            GLWindow::first_instance()->updateGL();
         }
         else if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "wireframe" ))
         {
            GLWindow::first_instance()->set_graphics_mode(WIREFRAME);
            GLWindow::first_instance()->updateGL();
         }
         else if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "hiddenline" ))
         {
            GLWindow::first_instance()->set_graphics_mode(HIDDENLINE);
            GLWindow::first_instance()->updateGL();
         }
      }
      else if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "perspective") )
      {
        curr_keyword++;
        if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "on") )
        {
          GLWindow::first_instance()->set_perspective(1);
          UserEventHandler::instance()->post_message(UserMessage(Perspective, "1" ));
        }
        else if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "off") )
        {
          GLWindow::first_instance()->set_perspective(0);
          UserEventHandler::instance()->post_message(UserMessage(Perspective));
        }

      }
      else if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "reset") )
      {
           GLWindow::first_instance()->set_draw_temp_only(false);
           GLWindow::first_instance()->reset();
           GLWindow::first_instance()->center_model();

           //DrawingTool::instance()->clear_highlighted();
	   UserEventHandler::instance()->post_message(UserMessage(ClearHighlighted), TRUE);
           DrawingTool::instance()->clear_temp_lists();
           GLWindow::first_instance()->updateGL();
      }
      curr_keyword++;
   }
}

void CommandHandler::graphics_highlight_handler(const FunctionData* parsed_data)
{
  int curr_keyword = 0;
  
  //Get out if you're in batch mode
  if(UserInterface::instance()->GetBatchMode())
  return;

  DrawingTool::instance()->clear_highlighted();
  
  while(!VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "") )
  {
    if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "failed") )
    {
      curr_keyword++;
      if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "hexes" ))
      {
        curr_keyword++;
        if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "metric" ))
        {
          std::deque<FailedHex*> failed_hexes;
          curr_keyword++;

          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "aspect" ))
            HexMetric::instance()->failed_hex_list(HEX_ASPECT, failed_hexes);
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "skew" ))
            HexMetric::instance()->failed_hex_list(HEX_SKEW, failed_hexes);
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "taper" ))
            HexMetric::instance()->failed_hex_list(HEX_TAPER, failed_hexes);
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "volume" ))
            HexMetric::instance()->failed_hex_list(HEX_VOLUME, failed_hexes);
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "stretch" ))
            HexMetric::instance()->failed_hex_list(HEX_STRETCH, failed_hexes);
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "diagonal" ))
            HexMetric::instance()->failed_hex_list(HEX_DIAGONALS, failed_hexes);
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "dimension" ))
            HexMetric::instance()->failed_hex_list(HEX_CHARDIM, failed_hexes);
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "oddy" ))
            HexMetric::instance()->failed_hex_list(HEX_ODDY, failed_hexes);
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "condition" ))
            HexMetric::instance()->failed_hex_list(HEX_CONDITION, failed_hexes);
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "jacobian" ))
            HexMetric::instance()->failed_hex_list(HEX_JACOBIAN, failed_hexes);
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "scaled jacobian" ))
            HexMetric::instance()->failed_hex_list(HEX_NORM_JACOBIAN, failed_hexes);
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "shear" ))
            HexMetric::instance()->failed_hex_list(HEX_SHEAR, failed_hexes);
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "shape" ))
            HexMetric::instance()->failed_hex_list(HEX_SHAPE, failed_hexes);
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "relative size" ))
            HexMetric::instance()->failed_hex_list(HEX_RELSIZE, failed_hexes);
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "shape and size" ))
            HexMetric::instance()->failed_hex_list(HEX_SHAPE_SIZE, failed_hexes);

          DrawingTool::instance()->highlight(failed_hexes);
          GLWindow::first_instance()->updateGL();
          
        }
      }
      
      if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "tets" ))
      {
        curr_keyword++;
        if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "metric" ))
        {
          std::deque<FailedTet*> failed_tets;
          curr_keyword++;
          
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "aspect" ))
            TetMetric::instance()->failed_tet_list(ASPECT_RATIO, failed_tets);
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "aspect gamma" ))
            TetMetric::instance()->failed_tet_list(ASPECT_RATIO_GAMMA, failed_tets);
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "volume" ))
            TetMetric::instance()->failed_tet_list(TET_VOLUME, failed_tets);
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "condition" ))
            TetMetric::instance()->failed_tet_list(TET_CONDITION_NUMBER, failed_tets);
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "jacobian" ))
            TetMetric::instance()->failed_tet_list(TET_JACOBIAN, failed_tets);
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "scaled jacobian" ))
            TetMetric::instance()->failed_tet_list(TET_NORM_JACOBIAN, failed_tets);
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "shear" ))
            TetMetric::instance()->failed_tet_list(TET_SHEAR, failed_tets);
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "shape" ))
            TetMetric::instance()->failed_tet_list(TET_SHAPE, failed_tets);
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "relative size" ))
            TetMetric::instance()->failed_tet_list(TET_RELSIZE, failed_tets);
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "shape and size" ))
            TetMetric::instance()->failed_tet_list(TET_SHAPE_SIZE, failed_tets);
          
          DrawingTool::instance()->highlight(failed_tets);
          GLWindow::first_instance()->updateGL();
        }
      }
      
      if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "quads" ))
      {
        curr_keyword++;
        if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "metric" ))
        {
          std::deque<FailedQuad*> failed_quads;
          curr_keyword++;
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "aspect" ))
          {
            QuadMetric::instance()->failed_quad_list(QUAD_ASPECT, failed_quads);
            DrawingTool::instance()->highlight(failed_quads);
            GLWindow::first_instance()->updateGL();
          }
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "skew" ))
          {
            QuadMetric::instance()->failed_quad_list(QUAD_SKEW, failed_quads);
            DrawingTool::instance()->highlight(failed_quads);
            GLWindow::first_instance()->updateGL();
          }
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "taper" ))
          {
            QuadMetric::instance()->failed_quad_list(QUAD_TAPER, failed_quads);
            DrawingTool::instance()->highlight(failed_quads);
            GLWindow::first_instance()->updateGL();
          }
          
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "warpage" ))
          {
            QuadMetric::instance()->failed_quad_list(QUAD_WARPAGE, failed_quads);
            DrawingTool::instance()->highlight(failed_quads);
            GLWindow::first_instance()->updateGL();
          }
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "area" ))
          {
            QuadMetric::instance()->failed_quad_list(QUAD_AREA, failed_quads);
            DrawingTool::instance()->highlight(failed_quads);
            GLWindow::first_instance()->updateGL();
          }
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "stretch" ))
          {
            QuadMetric::instance()->failed_quad_list(QUAD_STRETCH, failed_quads);
            DrawingTool::instance()->highlight(failed_quads);
            GLWindow::first_instance()->updateGL();
          }
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "largest angle" ))
          {
            QuadMetric::instance()->failed_quad_list(QUAD_ANGLE, failed_quads);
            DrawingTool::instance()->highlight(failed_quads);
            GLWindow::first_instance()->updateGL();
          }
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "smallest angle" ))
          {
            QuadMetric::instance()->failed_quad_list(QUAD_MIN_ANGLE, failed_quads);
            DrawingTool::instance()->highlight(failed_quads);
            GLWindow::first_instance()->updateGL();
          }
          
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "oddy" ))
          {
            QuadMetric::instance()->failed_quad_list(QUAD_MAX_ODDY, failed_quads);
            DrawingTool::instance()->highlight(failed_quads);
            GLWindow::first_instance()->updateGL();
          }
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "condition" ))
          {
            QuadMetric::instance()->failed_quad_list(QUAD_MAX_COND, failed_quads);
            DrawingTool::instance()->highlight(failed_quads);
            GLWindow::first_instance()->updateGL();
          }
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "jacobian" ))
          {
            QuadMetric::instance()->failed_quad_list(QUAD_MIN_JAC, failed_quads);
            DrawingTool::instance()->highlight(failed_quads);
            GLWindow::first_instance()->updateGL();
          }
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "scaled jacobian" ))
          {
            QuadMetric::instance()->failed_quad_list(QUAD_MIN_SC_JAC, failed_quads);
            DrawingTool::instance()->highlight(failed_quads);
            GLWindow::first_instance()->updateGL();
          }
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "shear" ))
          {
            QuadMetric::instance()->failed_quad_list(QUAD_SHEAR, failed_quads);
            DrawingTool::instance()->highlight(failed_quads);
            GLWindow::first_instance()->updateGL();
          }
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "shape" ))
          {
            QuadMetric::instance()->failed_quad_list(QUAD_SHAPE, failed_quads);
            DrawingTool::instance()->highlight(failed_quads);
            GLWindow::first_instance()->updateGL();
          }
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "relative size" ))
        {
          QuadMetric::instance()->failed_quad_list(QUAD_REL_SIZE, failed_quads);
          DrawingTool::instance()->highlight(failed_quads);
          GLWindow::first_instance()->updateGL();
        }
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "shape and size" ))
          {
            QuadMetric::instance()->failed_quad_list(QUAD_SHAPE_SIZE, failed_quads);
            DrawingTool::instance()->highlight(failed_quads);
            GLWindow::first_instance()->updateGL();
          }
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "coincident" ))
          {
            std::deque<QuadElem*>quad_list ;
            SkinTool::coincident_quads(quad_list);
            DrawingTool::instance()->highlight(quad_list);
            GLWindow::first_instance()->updateGL();
          }
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "share three nodes" ))
          {
            std::deque<QuadElem*>quad_list ;
            SkinTool::quads_share_three_nodes_list(quad_list);
            DrawingTool::instance()->highlight(quad_list);      
            GLWindow::first_instance()->updateGL();
          }
        }  
      }
      
      if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "tris" ))
      {    
        curr_keyword++;
        if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "metric" ))
        {
          std::deque<FailedTri*> failed_tris;
          std::deque<TriElem*> tri_list;
          curr_keyword++;
          const char* metric = parsed_data->keywordIdentifier[curr_keyword];

          if(VerdeUtil::compare(metric, "aspect" ))
            TriMetric::instance()->failed_tri_list(TRI_ASPECT, failed_tris);
          if(VerdeUtil::compare(metric, "area" ))
            TriMetric::instance()->failed_tri_list(TRI_AREA, failed_tris);
          if(VerdeUtil::compare(metric, "largest angle" ))
            TriMetric::instance()->failed_tri_list(TRI_ANGLE, failed_tris);
          if(VerdeUtil::compare(metric, "smallest angle" ))
            TriMetric::instance()->failed_tri_list(TRI_MIN_ANGLE, failed_tris);
          if(VerdeUtil::compare(metric, "condition" ))
            TriMetric::instance()->failed_tri_list(TRI_CONDITION_NUMBER, failed_tris);
          if(VerdeUtil::compare(metric, "scaled jacobian" ))
            TriMetric::instance()->failed_tri_list(TRI_MIN_SC_JAC, failed_tris);
          if(VerdeUtil::compare(metric, "shear" ))
            TriMetric::instance()->failed_tri_list(TRI_SHEAR, failed_tris);
          if(VerdeUtil::compare(metric, "shape" ))
            TriMetric::instance()->failed_tri_list(TRI_SHAPE, failed_tris);
          if(VerdeUtil::compare(metric, "relative size" ))
            TriMetric::instance()->failed_tri_list(TRI_REL_SIZE, failed_tris);
          if(VerdeUtil::compare(metric, "shape and size" ))
            TriMetric::instance()->failed_tri_list(TRI_SHAPE_SIZE, failed_tris);

          if(VerdeUtil::compare(metric, "coincident" ))
            SkinTool::coincident_tris(tri_list);
          if(VerdeUtil::compare(metric, "nonconformal" ))
            SkinTool::non_conformal_tri_list(tri_list);

          if(tri_list.size())
            DrawingTool::instance()->highlight(tri_list);
          else
            DrawingTool::instance()->highlight(failed_tris);

          GLWindow::first_instance()->updateGL();
        }  
      }
      
      if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "edges" ))
      {
        curr_keyword++;
        if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "metric" ))
        {
          curr_keyword++;
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "nonmanifold" ))
          {
            std::deque<EdgeElem *> edge_list;
            EdgeTool::instance()->non_manifold_edges(edge_list);
            DrawingTool::instance()->highlight(edge_list);
          }
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "length" ))
          {
            std::deque<FailedEdge*> failed_edges;
            EdgeMetric::instance()->failed_edge_list(EDGE_LENGTH, failed_edges);
            DrawingTool::instance()->highlight(failed_edges);
          }
        }
        GLWindow::first_instance()->updateGL();
      }
      
      if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "nodes" ))
      {
        curr_keyword++;
        if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "metric" ))
        {
          curr_keyword++;
          if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "coincident" ))
          {
            std::deque<int> node_ids;
            SkinTool::coincident_node_id_list(node_ids);
            DrawingTool::instance()->highlight(node_ids);
            GLWindow::first_instance()->updateGL();
          }
        }
      }
    }
    curr_keyword++;
  }
}

void CommandHandler::graphics_display_handler(const FunctionData* /*parsed_data*/)
{
    //Get out if you're in batch mode
    if(UserInterface::instance()->GetBatchMode())
      return;
    
    GLWindow::first_instance()->set_draw_temp_only(false);

    if( DrawingTool::instance()->get_autofit_toggle() )
      GLWindow::first_instance()->center_model();

    //if autofit is off and we're to set 'draw_temp_only' back to false 
    if( !DrawingTool::instance()->get_autofit_toggle() ) 
      GLWindow::first_instance()->size_clip_planes();

    DrawingTool::instance()->clear_highlighted();
    DrawingTool::instance()->clear_temp_lists();


    GLWindow::first_instance()->updateGL();
}

void CommandHandler::draw_normals(const FunctionData* parsed_data)
{
    int curr_keyword = 0;
    if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "on") )
    {
      UserEventHandler::instance()->post_message(UserMessage(Normals, "1" ));
    }
    else if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "off") )
    {
      UserEventHandler::instance()->post_message(UserMessage(Normals));
    }
}

void CommandHandler::draw_skin(const FunctionData* parsed_data)
{
    int curr_keyword = 0;
    if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "on") )
    {
      UserEventHandler::instance()->post_message(UserMessage(Skin, "1" ));
    }
    else if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "off") )
    {
      UserEventHandler::instance()->post_message(UserMessage(Skin));
    }
}

void CommandHandler::draw_model_edges(const FunctionData* parsed_data)
{
    int curr_keyword = 0;
    if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "on") )
    {
      UserEventHandler::instance()->post_message(UserMessage(Edges, "1" ));
    }
    else if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "off") )
    {
      UserEventHandler::instance()->post_message(UserMessage(Edges));
    }
}

void CommandHandler::draw_axis(const FunctionData* /*parsed_data*/)
{
/*
    int curr_keyword = 0;
    if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "on") )
    {
      UserEventHandler::instance()->post_message(UserMessage(Axis, "1" ));
    }
    else if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "off") )
    {
      UserEventHandler::instance()->post_message(UserMessage(Axis));
    }
*/
}


void CommandHandler::label(const FunctionData* parsed_data)
{
    int curr_keyword = 0;
    if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "failed") )
    {
      if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword+1], "on") )
        UserEventHandler::instance()->post_message(UserMessage(Label, "1 failed" ));
      else if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword+1], "off") )
        UserEventHandler::instance()->post_message(UserMessage(Label, "0 failed" ));
    }
    
    else if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "blocks") )
    {
      if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword+1], "on") )
        UserEventHandler::instance()->post_message(UserMessage(Label, "1 blocks" ));
      else if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword+1], "off") )
        UserEventHandler::instance()->post_message(UserMessage(Label, "0 blocks" ));
    }

    else if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "nodesets") )
    {
      if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword+1], "on") )
        UserEventHandler::instance()->post_message(UserMessage(Label, "1 nodesets" ));
      else if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword+1], "off") )
        UserEventHandler::instance()->post_message(UserMessage(Label, "0 nodesets" ));
    }

    else if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "sidesets") )
    {
      if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword+1], "on") )
        UserEventHandler::instance()->post_message(UserMessage(Label, "1 sidesets" ));
      else if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword+1], "off") )
        UserEventHandler::instance()->post_message(UserMessage(Label, "0 sidesets" ));
    }
    else if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "nodes") )
    {
      if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword+1], "on") )
        UserEventHandler::instance()->post_message(UserMessage(Label, "1 nodes" ));
      else if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword+1], "off") )
        UserEventHandler::instance()->post_message(UserMessage(Label, "0 nodes" ));
    }
    else if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword], "elements") )
    {
      if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword+1], "on") )
        UserEventHandler::instance()->post_message(UserMessage(Label, "1 elements" ));
      else if(VerdeUtil::compare(parsed_data->keywordIdentifier[curr_keyword+1], "off") )
        UserEventHandler::instance()->post_message(UserMessage(Label, "0 elements" ));
    }
}



void CommandHandler::save_mesh(const FunctionData* parsed_data)
{
   MeshOutput* meshoutput=0;

   // output an exodus mesh
   if(VerdeUtil::compare(parsed_data->keywordIdentifier[0], "exodus") ||
      VerdeUtil::compare(parsed_data->keywordIdentifier[0], "genesis") )
   {
     meshoutput = new ExodusTool;
   }

   // do the output if we can
   if(meshoutput)
   {
     if(meshoutput->output_mesh_file(verde_app->mesh(),
         parsed_data->characterData[0]) == VERDE_FAILURE)
     {
       PRINT_ERROR("Couldn't save mesh\n");
       delete meshoutput;
       return;
     }
     
   }
   else
   {
     PRINT_WARNING("mesh type not specified\n");
     delete meshoutput;
     return;
   }

   if(meshoutput)
      delete meshoutput;

   command_wrapup(parsed_data); 

}


void CommandHandler::draw_handler( const FunctionData* parsed_data)
{
  // if we are in batch mode, get out of here
  if(UserInterface::instance()->GetBatchMode())
    return;

  int current_keyword = 0;

  std::vector<int> entities;

  DrawingTool::instance()->clear_temp_lists();

  if( DrawingTool::instance()->get_autofit_toggle() )
    DrawingTool::instance()->reset_boundary(1);

  GLWindow::first_instance()->set_draw_temp_only(true);

  while( 0 == VerdeUtil::compare(parsed_data->keywordIdentifier[current_keyword], ""))
  {
    // clean out the list of entities
    entities.clear();

    if( VerdeUtil::compare(parsed_data->keywordIdentifier[current_keyword], "hex") )
    {
      parse_int_list(parsed_data, current_keyword, entities);
      DrawingTool::instance()->draw_hexes( entities );
    }
    else if( VerdeUtil::compare(parsed_data->keywordIdentifier[current_keyword], "tet") )
    {
      parse_int_list(parsed_data, current_keyword, entities);
      DrawingTool::instance()->draw_tets( entities );
    }
    else if( VerdeUtil::compare(parsed_data->keywordIdentifier[current_keyword], "knife") )
    {
      parse_int_list(parsed_data, current_keyword, entities);
      DrawingTool::instance()->draw_knives( entities );
    }
    else if( VerdeUtil::compare(parsed_data->keywordIdentifier[current_keyword], "wedge") )
    {
      parse_int_list(parsed_data, current_keyword, entities);
      DrawingTool::instance()->draw_wedges( entities );
    }
    else if( VerdeUtil::compare(parsed_data->keywordIdentifier[current_keyword], "pyramid") )
    {
      parse_int_list(parsed_data, current_keyword, entities);
      DrawingTool::instance()->draw_pyramids( entities );
    }
    else if( VerdeUtil::compare(parsed_data->keywordIdentifier[current_keyword], "quad") )
    {
      parse_int_list(parsed_data, current_keyword, entities);
      DrawingTool::instance()->draw_quads( entities );
    }
    else if( VerdeUtil::compare(parsed_data->keywordIdentifier[current_keyword], "tri") )
    {
      parse_int_list(parsed_data, current_keyword, entities);
      DrawingTool::instance()->draw_tris( entities );
    }
    else if( VerdeUtil::compare(parsed_data->keywordIdentifier[current_keyword], "edge") )
    {
      parse_int_list(parsed_data, current_keyword, entities);
      DrawingTool::instance()->draw_edges( entities );
    }
    else if( VerdeUtil::compare(parsed_data->keywordIdentifier[current_keyword], "node") )
    {
      parse_int_list(parsed_data, current_keyword, entities);
      DrawingTool::instance()->draw_nodes( entities );
    }

    if( DrawingTool::instance()->get_autofit_toggle() )
      GLWindow::first_instance()->center_model();

    GLWindow::first_instance()->updateGL();
  }
}


