/*
 *  Program: pgn-extract: a Portable Game Notation (PGN) extractor.
 *  Copyright (C) 1994-2013 David Barnes
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 1, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  David Barnes may be contacted as D.J.Barnes@kent.ac.uk
 *  http://www.cs.kent.ac.uk/people/staff/djb/
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "bool.h"
#include "defs.h"
#include "typedef.h"
#include "lines.h"
#include "taglist.h"
#include "tokens.h"
#include "lex.h"
#include "moves.h"
#include "end.h"
#include "eco.h"
#include "argsfile.h"
#include "apply.h"
#include "output.h"
#include "lists.h"
#include "mymalloc.h"

#define CURRENT_VERSION "v17-14"
#define URL "http://www.cs.kent.ac.uk/people/staff/djb/pgn-extract/"

/* The prefix of the arguments allowed in an argsfile.
 * The full format is:
 *         :-?
 * where ? is an argument letter.
 *
 * A line of the form:
 *         :filename
 * means use filename as a NORMALFILE source of games.
 *
 * A line with no leading colon character is taken to apply to the
 * move-reason argument line. Currently, this only applies to the
 *        -t -v -x -z
 * arguments.
 */
static const char argument_prefix[] = ":-";
static const int argument_prefix_len = sizeof(argument_prefix)-1;
static ArgType classify_arg(const char *line);

        /* Select the correct function according to operating system. */
static int
stringcompare(const char *s1, const char *s2)
{
#if defined(__unix__) || defined(__linux__) || defined(__APPLE__)
    return strcasecmp(s1,s2);
#else
    return _stricmp(s1,s2);
#endif
}

#if 0
        /* Return TRUE if str contains prefix as a prefix, FALSE otherwise. */
static Boolean
prefixMatch(const char *prefix, const char *str)
{
    size_t prefixLen = strlen(prefix);
    if(strlen(str) >= prefixLen) {
#if defined(__unix__) || defined(__linux__)
        return strncasecmp(prefix, str, prefixLen) == 0;
#else
        return _strnicmp(prefix, str, prefixLen) == 0;
#endif
    }
    else {
        return FALSE;
    }
}
#endif

    /* Skip over leading spaces from the string. */
static const char *skip_leading_spaces(const char *str)
{
    while(*str == ' ') {
        str++;
    }
    return str;
}

        /* Print a usage message, and exit. */
static void
usage_and_exit(void)
{
    const char *help_data[] = {
      "-7 -- output only the seven tag roster for each game. Other tags (apart",
      "      from FEN and possibly ECO) are discarded (See -e).",
      "-#num -- output num games per file, to files named 1.pgn, 2.pgn, etc.",

      "",

      "-aoutputfile -- append extracted games to outputfile. (See -o).",
      "-Aargsfile -- read the program's arguments from argsfile.",
      "-b[elu]num -- restricted bounds on the number of moves in a game.",
      "       lnum set a lower bound of `num' moves,",
      "       unum set an upper bound of `num' moves,",
      "       otherwise num (or enum) means equal-to `num' moves.",
      "-cfile[.pgn] -- Use file.pgn as a check-file for duplicates or",
      "      contents of file (no pgn suffix) as a list of check-file names.",
      "-C -- don't include comments in the output. Ordinarily these are retained.",
      "-dduplicates -- write duplicate games to the file duplicates.",
      "-D -- don't output duplicate games.",
      "-eECO_file -- perform ECO classification of games. The optional",
      "      ECO_file should contain a PGN format list of ECO lines",
      "      Default is to use eco.pgn from the current directory.",
      "-E[123 etc.] -- split output into separate files according to ECO.",
      "      E1 : Produce files from ECO letter, A.pgn, B.pgn, ...",
      "      E2 : Produce files from ECO letter and first digit, A0.pgn, ...",
      "      E3 : Produce files from full ECO code, A00.pgn, A01.pgn, ...",
      "      Further digits may be used to produce non-standard further",
      "      refined division of games.",
      "      All files are opened in append mode.",
      "-F -- output a FEN string comment of the final game position.",
      "-ffile_list  -- file_list contains the list of PGN source files, one per line.",
      "-h -- print details of the arguments.",
      "-llogfile  -- Save the diagnostics in logfile rather than using stderr.",
      "-Llogfile  -- Append all diagnostics to logfile, rather than overwriting.",
      "-M -- Match only games which end in checkmate.",
      "-noutputfile -- Write all valid games not otherwise output to outputfile.",
      "-N -- don't include NAGs in the output. Ordinarily these are retained.",
      "-ooutputfile -- write extracted games to outputfile (existing contents lost).",
      "-P -- don't match permutations of the textual variations (-v).",
      "-Rtagorder -- Use the tag ordering specified in the file tagorder.",
      "-r -- report any errors but don't extract.",
      "-S -- Use a simple soundex algorithm for some tag matches. If used",
      "      this option must precede the -t or -T options.",
      "-s -- silent mode: don't report each game as it is extracted.",
      "-ttagfile -- file of player, date, result or FEN extraction criteria.",
      "-Tcriterion -- player, date, or result extraction criterion.",
      "-U -- don't output games that only occur once. (See -d).",
      "-vvariations -- the file variations contains the textual lines of interest.",
      "-V -- don't include variations in the output. Ordinarily these are retained.",
      "-wwidth -- set width as an approximate line width for output.",
      "-W[cm|epd|halg|lalg|elalg|san] -- specify the output format to use.",
      "      Default is SAN.",
      "      -W means use the input format.",
      "      -Wcm is (a possibly obsolete) ChessMaster format.",
      "      -Wepd is EPD format.",
      "      -Wsan[PNBRQK] for language specific output.",
      "      -Whalg is hyphenated long algebraic.",
      "      -Wlalg is long algebraic.",
      "      -Welalg is enhanced long algebraic.",
      "      -Wuci is output compatible with the UCI protocol.",
      "-xvariations -- the file variations contains the lines resulting in",
      "                positions of interest.",
      "-zendings -- the file endings contains the end positions of interest.",
      "-Z -- use the file virtual.tmp as an external hash table for duplicates.",
      "      Use when MallocOrDie messages occur with big datasets.",

      "",

      "--append - see -a",
      "--checkfile - see -c",
      "--checkmate - see -M",
      "--duplicates - see -d",
      "--evaluation - include a position evaluation after each move",
      "--fencomments - include a FEN string after each move",
      "--fuzzydepth plies - positional duplicates match",
      "--help - see -h",
      "--linelength - see -w",
      "--markmatches - mark positional and material matches with a comment; see -t, -v, and -z",
      "--nochecks - don't output + and # after moves.",
      "--nocomments - see -C",
      "--noduplicates - see -D",
      "--nomovenumbers - don't output move numbers.",
      "--nonags - see -N",
      "--noresults - don't output results.",
      "--notags - don't output any tags.",
      "--nounique - see -U",
      "--novars - see -V",
      "--output - see -o",
      "--plylimit - limit the number of plies output.",
      "--seven - see -7",
      "--stalemate - only output games that end in stalemate.",
      "--version - print the current version number and exit.",

      /* Must be NULL terminated. */
      (char *)NULL,
  };

  const char **data = help_data;

  fprintf(GlobalState.logfile,
          "pgn-extract %s (%s): a Portable Game Notation (PGN) manipulator.\n",
          CURRENT_VERSION,__DATE__);
  fprintf(GlobalState.logfile,
          "Copyright (C) 1994-2013 David J. Barnes (d.j.barnes@kent.ac.uk)\n");
  fprintf(GlobalState.logfile,"%s\n\n",URL);
  fprintf(GlobalState.logfile,"Usage: pgn-extract [arguments] [file.pgn ...]\n");
  
  for(; *data != NULL; data++){
      fprintf(GlobalState.logfile,"%s\n",*data);
  }
  exit(1);
}

void
read_args_file(const char *infile)
{   char *line;
    FILE *fp = fopen(infile,"r");

    if(fp == NULL){
        fprintf(GlobalState.logfile,"Cannot open %s for reading.\n",infile);
        exit(1);
    }
    else{
        ArgType linetype = NO_ARGUMENT_MATCH;
        ArgType nexttype;
        while((line = read_line(fp)) != NULL){
            if(blank_line(line)){
                (void) free(line);
                continue;
            }
            nexttype = classify_arg(line);
            if(nexttype == NO_ARGUMENT_MATCH){
                if(*line == argument_prefix[0]){
                    /* Treat the line as a source file name. */
                    add_filename_to_source_list(&line[1],NORMALFILE);
                }
                else if(linetype != NO_ARGUMENT_MATCH){
                    /* Handle the line. */
                    switch(linetype){
                        case MOVES_ARGUMENT:
                            add_textual_variation_from_line(line);
                            break;
                        case POSITIONS_ARGUMENT:
                            add_positional_variation_from_line(line);
                            break;
                        case TAGS_ARGUMENT:
                            process_tag_line(infile,line);
                            break;
                        case TAG_ROSTER_ARGUMENT:
                            process_roster_line(line);
                            break;
                        case ENDINGS_ARGUMENT:
                            process_ending_line(line);
                            (void) free(line);
                            break;
                        default:
                            fprintf(GlobalState.logfile,
                                    "Internal error: unknown linetype %d in read_args_file\n",
                                    linetype);
                            (void) free(line);
                            exit(-1);
                    }
                }
                else{
                    /* It should have been a line applying to the
                     * current linetype.
                     */
                    fprintf(GlobalState.logfile,
                        "Missing argument type for line %s in the argument file.\n",
                        line);
                    exit(1);
                }
            }
            else{
                switch(nexttype){
                        /* Arguments with a possible additional
                         * argument value.
                         * All of these apply only to the current
                         * line in the argument file.
                         */
                    case WRITE_TO_OUTPUT_FILE_ARGUMENT:
                    case APPEND_TO_OUTPUT_FILE_ARGUMENT:
                    case WRITE_TO_LOG_FILE_ARGUMENT:
                    case APPEND_TO_LOG_FILE_ARGUMENT:
                    case DUPLICATES_FILE_ARGUMENT:
                    case USE_ECO_FILE_ARGUMENT:
                    case CHECK_FILE_ARGUMENT:
                    case FILE_OF_FILES_ARGUMENT:
                    case BOUNDS_ARGUMENT:
                    case GAMES_PER_FILE_ARGUMENT:
                    case ECO_OUTPUT_LEVEL_ARGUMENT:
                    case FILE_OF_ARGUMENTS_ARGUMENT:
                    case NON_MATCHING_GAMES_ARGUMENT:
                    case TAG_EXTRACTION_ARGUMENT:
                    case LINE_WIDTH_ARGUMENT:
                    case OUTPUT_FORMAT_ARGUMENT:
                        process_argument(line[argument_prefix_len],
                                         &line[argument_prefix_len+1]);
                        linetype = NO_ARGUMENT_MATCH;
                        break;
                    case LONG_FORM_ARGUMENT:
			{
			    char *arg = &line[argument_prefix_len+1];
			    char *space = strchr(arg, ' ');
			    if(space != NULL) {
				/* We need to drop an associated value from arg. */
				int arglen = space - arg;
				char *just_arg = (char *) MallocOrDie(arglen + 1);
				strncpy(just_arg, arg, arglen);
				just_arg[arglen] = '\0';
				process_long_form_argument(just_arg,
							   skip_leading_spaces(space));
			    }
			    else {
				process_long_form_argument(arg, "");
				linetype = NO_ARGUMENT_MATCH;
			    }
			}
                        break;

                        /* Arguments with no additional
                         * argument value.
                         * All of these apply only to the current
                         * line in the argument file.
                         */
                    case SEVEN_TAG_ROSTER_ARGUMENT:
                    case HELP_ARGUMENT:
                    case ALTERNATIVE_HELP_ARGUMENT:
                    case DONT_KEEP_COMMENTS_ARGUMENT:
                    case DONT_KEEP_DUPLICATES_ARGUMENT:
                    case DONT_MATCH_PERMUTATIONS_ARGUMENT:
                    case DONT_KEEP_NAGS_ARGUMENT:
                    case OUTPUT_FEN_STRING_ARGUMENT:
                    case CHECK_ONLY_ARGUMENT:
                    case KEEP_SILENT_ARGUMENT:
                    case USE_SOUNDEX_ARGUMENT:
                    case MATCH_CHECKMATE_ARGUMENT:
                    case SUPPRESS_ORIGINALS_ARGUMENT:
                    case DONT_KEEP_VARIATIONS_ARGUMENT:
                    case USE_VIRTUAL_HASH_TABLE_ARGUMENT:
                        process_argument(line[argument_prefix_len],"");
                        linetype = NO_ARGUMENT_MATCH;
                        break;

                        /* Arguments whose values persist beyond
                         * the current line.
                         */
                    case MOVES_ARGUMENT:
                    case POSITIONS_ARGUMENT:
                    case ENDINGS_ARGUMENT:
                    case TAGS_ARGUMENT:
                    case TAG_ROSTER_ARGUMENT:
                        process_argument(line[argument_prefix_len],
                                         &line[argument_prefix_len+1]);
                        /* Apply this type to subsequent lines. */
                        linetype = nexttype;
                        break;
                    default:
                        linetype = nexttype;
                        break;
                }
                (void) free(line);
            }
        }
        (void) fclose(fp);
    }
}

        /* Determine which (if any) type of argument is
         * indicated by the contents of the current line.
         * Arguments are assumed to be surrounded by
         * square brackets.
         */
static ArgType
classify_arg(const char *line)
{
    /* Valid arguments must have at least one character beyond
     * the prefix.
     */
    static const size_t min_argument_length = 1+sizeof(argument_prefix)-1;
    size_t line_length = strlen(line);

    /* Check for a line of the form:
     *            :-argument
     */
    if((strncmp(line,argument_prefix,argument_prefix_len) == 0) &&
                   (line_length >= min_argument_length)){
        char argument_letter = line[argument_prefix_len];
        switch(argument_letter){
            case TAGS_ARGUMENT:
            case MOVES_ARGUMENT:
            case POSITIONS_ARGUMENT:
            case ENDINGS_ARGUMENT:
            case TAG_EXTRACTION_ARGUMENT:
            case LINE_WIDTH_ARGUMENT:
            case OUTPUT_FORMAT_ARGUMENT:
            case SEVEN_TAG_ROSTER_ARGUMENT:
            case FILE_OF_ARGUMENTS_ARGUMENT:
            case NON_MATCHING_GAMES_ARGUMENT:
            case DONT_KEEP_COMMENTS_ARGUMENT:
            case DONT_KEEP_DUPLICATES_ARGUMENT:
            case DONT_KEEP_NAGS_ARGUMENT:
            case DONT_MATCH_PERMUTATIONS_ARGUMENT:
            case OUTPUT_FEN_STRING_ARGUMENT:
            case CHECK_ONLY_ARGUMENT:
            case KEEP_SILENT_ARGUMENT:
            case USE_SOUNDEX_ARGUMENT:
            case MATCH_CHECKMATE_ARGUMENT:
            case SUPPRESS_ORIGINALS_ARGUMENT:
            case DONT_KEEP_VARIATIONS_ARGUMENT:
            case WRITE_TO_OUTPUT_FILE_ARGUMENT:
            case WRITE_TO_LOG_FILE_ARGUMENT:
            case APPEND_TO_LOG_FILE_ARGUMENT:
            case APPEND_TO_OUTPUT_FILE_ARGUMENT:
            case DUPLICATES_FILE_ARGUMENT:
            case USE_ECO_FILE_ARGUMENT:
            case CHECK_FILE_ARGUMENT:
            case FILE_OF_FILES_ARGUMENT:
            case BOUNDS_ARGUMENT:
            case GAMES_PER_FILE_ARGUMENT:
            case ECO_OUTPUT_LEVEL_ARGUMENT:
            case HELP_ARGUMENT:
            case ALTERNATIVE_HELP_ARGUMENT:
            case TAG_ROSTER_ARGUMENT:
            case LONG_FORM_ARGUMENT:
                return (ArgType) argument_letter;
            default:
                fprintf(GlobalState.logfile,
                        "Unrecognized argument: %s in the argument file.\n",
                        line);
                exit(1);
                return NO_ARGUMENT_MATCH;
        }
    }
    else{
        return NO_ARGUMENT_MATCH;
    }
}

    /* Process the argument character and its associated value.
     * This function processes arguments from the command line and
     * from an argument file associated with the -A argument.
     *
     * An argument -ofile.pgn would be passed in as:
     *                'o' and "file.pgn".
     * A zero-length string for associated_value is not necessarily
     * an error, e.g. -e has an optional following filename.
     * NB: If the associated_value is to be used beyond this function,
     * it must be copied.
     */
void
process_argument(char arg_letter,const char *associated_value)
{   /* Provide an alias for associated_value because it will
     * often represent a file name.
     */
    const char *filename = skip_leading_spaces(associated_value);

    switch(arg_letter){
        case WRITE_TO_OUTPUT_FILE_ARGUMENT:
        case APPEND_TO_OUTPUT_FILE_ARGUMENT:
          if(GlobalState.ECO_level > 0){
              fprintf(GlobalState.logfile,"-%c conflicts with -E\n",
                      arg_letter);
          }
          else if(GlobalState.games_per_file > 0){
              fprintf(GlobalState.logfile,"-%c conflicts with -#\n",
                      arg_letter);
          }
          else if(GlobalState.output_filename != NULL){
              fprintf(GlobalState.logfile,
                      "-%c: File %s has already been selected for output.\n",
                      arg_letter,GlobalState.output_filename);
              exit(1);
          }
          else if(*filename == '\0'){
              fprintf(GlobalState.logfile,"Usage: -%cfilename.\n",arg_letter);
              exit(1);
          }
          else{
              if(GlobalState.outputfile != NULL){
                  (void) fclose(GlobalState.outputfile);
              }
              if(arg_letter == WRITE_TO_OUTPUT_FILE_ARGUMENT){
                  GlobalState.outputfile = MustOpen(filename,"w");
              }
              else{
                  GlobalState.outputfile = MustOpen(filename,"a");
              }
              GlobalState.output_filename = filename;
          }
          break;
        case WRITE_TO_LOG_FILE_ARGUMENT:
        case APPEND_TO_LOG_FILE_ARGUMENT:
          /* Take precautions against multiple log files. */
          if((GlobalState.logfile != stderr) && (GlobalState.logfile != NULL)){
                (void) fclose(GlobalState.logfile);
          }
          if(arg_letter == WRITE_TO_LOG_FILE_ARGUMENT){
                GlobalState.logfile = fopen(filename,"w");
          }
          else{
                GlobalState.logfile = fopen(filename,"a");
          }
          if(GlobalState.logfile == NULL){
                fprintf(stderr,"Unable to open %s for writing.\n",filename);
                GlobalState.logfile = stderr;
          }
          break;
        case DUPLICATES_FILE_ARGUMENT:
          if(*filename == '\0'){
              fprintf(GlobalState.logfile,"Usage: -%cfilename.\n",arg_letter);
              exit(1);
          }
          else if(GlobalState.suppress_duplicates){
              fprintf(GlobalState.logfile,
                      "-%c clashes with the -%c flag.\n",arg_letter,
                      DONT_KEEP_DUPLICATES_ARGUMENT);
              exit(1);
          }
          else{
              GlobalState.duplicate_file = MustOpen(filename,"w");
          }
          break;
        case USE_ECO_FILE_ARGUMENT:
          GlobalState.add_ECO = TRUE;
          if(*filename != '\0'){
              GlobalState.eco_file = StringCopy(filename);
          }
          else if((filename = getenv("ECO_FILE")) != NULL){
              GlobalState.eco_file = filename;
          }
          else{
              /* Use the default which is already set up. */
          }
          initEcoTable();
          break;
        case ECO_OUTPUT_LEVEL_ARGUMENT:
          {   unsigned level;

              if(GlobalState.output_filename != NULL){
                  fprintf(GlobalState.logfile,
                          "-%c: File %s has already been selected for output.\n",
                          arg_letter,
                          GlobalState.output_filename);
                  exit(1);
              }
              else if(GlobalState.games_per_file > 0){
                  fprintf(GlobalState.logfile,
                            "-%c conflicts with -#.\n",
                            arg_letter);
                  exit(1);
              }
              else if(sscanf(associated_value,"%u",&level) != 1){
                    fprintf(GlobalState.logfile,
                            "-%c requires a number attached, e.g., -%c1.\n",
                            arg_letter,arg_letter);
                     exit(1);
              }
              else if((level < MIN_ECO_LEVEL) || (level > MAX_ECO_LEVEL)){
                    fprintf(GlobalState.logfile,
                            "-%c level should be between %u and %u.\n",
                            MIN_ECO_LEVEL,MAX_ECO_LEVEL,arg_letter);
                    exit(1);
              }
              else{
                    GlobalState.ECO_level = level;
              }
          }
          break;
        case CHECK_FILE_ARGUMENT:
          if(*filename != '\0'){
              /* See if it is a single PGN file, or a list
               * of files.
               */
              size_t len = strlen(filename);
              /* Check for a .PGN suffix. */
              const char *suffix = output_file_suffix(SAN);

              if((len > strlen(suffix)) &&
                    (stringcompare(&filename[len-strlen(suffix)],
                                            suffix) == 0)){
                  add_filename_to_source_list(filename,CHECKFILE);
              }
              else{
                  FILE *fp = MustOpen(filename,"r");
                  add_filename_list_from_file(fp,CHECKFILE);
                  (void) fclose(fp);
              }
          }
          break;
        case FILE_OF_FILES_ARGUMENT:
          if(*filename != '\0'){
              FILE *fp = MustOpen(filename,"r");
              add_filename_list_from_file(fp,NORMALFILE);
              (void) fclose(fp);
          }
          else{
              fprintf(GlobalState.logfile,"Filename expected with -%c\n",
                      arg_letter);
          }
          break;
        case BOUNDS_ARGUMENT:
          {
              /* Bounds on the number of moves are to be found.
               * "l#" means less-than-or-equal-to.
               * "g#" means greater-than-or-equal-to.
               * Otherwise "#" (or "e#") means that number.
               */
              /* Equal by default. */
              char which = 'e';
              unsigned value;
              Boolean Ok = TRUE;
              const char *bound = associated_value;

              switch(*bound){
                case 'l':
                case 'u':
                case 'e':
                  which = *bound;
                  bound++;
                  break;
                default:
                  if(!isdigit((int) *bound)){
                      fprintf(GlobalState.logfile,
                              "-%c must be followed by e, l, or u.\n",
                              arg_letter);
                      Ok = FALSE;
                  }
                  break;
              }
              if(Ok && (sscanf(bound,"%u",&value) == 1)){
                GlobalState.check_move_bounds = TRUE;
                switch(which){
                  case 'e':
                        GlobalState.lower_move_bound = value; 
                        GlobalState.upper_move_bound = value; 
                        break;
                  case 'l':
                    if(value <= GlobalState.upper_move_bound){
                        GlobalState.lower_move_bound = value; 
                    }
                    else{
                        fprintf(GlobalState.logfile,
                           "Lower bound is greater than the upper bound; -%c ignored.\n",
                           arg_letter);
                        Ok = FALSE;
                    }
                    break;
                  case 'u':
                    if(value >= GlobalState.lower_move_bound){
                        GlobalState.upper_move_bound = value; 
                    }
                    else{
                        fprintf(GlobalState.logfile,
                           "Upper bound is smaller than the lower bound; -%c ignored.\n",
                           arg_letter);
                        Ok = FALSE;
                    }
                    break;
                }
             }
             else{
               fprintf(GlobalState.logfile,
                       "-%c should be in the form -%c[elu]number.\n",
                       arg_letter,arg_letter);
               Ok = FALSE;
             }
             if(!Ok){
                exit(1);
             }
         }
         break;
        case GAMES_PER_FILE_ARGUMENT:
          if(GlobalState.ECO_level > 0){
              fprintf(GlobalState.logfile,
                      "-%c conflicts with -E.\n",arg_letter);
              exit(1);
          }
          else if(GlobalState.output_filename != NULL){
              fprintf(GlobalState.logfile,
                        "-%c: File %s has already been selected for output.\n",
                        arg_letter,
                        GlobalState.output_filename);
              exit(1);
          }
          else if(sscanf(associated_value,"%u",
                       &GlobalState.games_per_file) != 1){
            fprintf(GlobalState.logfile,
                    "-%c should be followed by an unsigned integer.\n",
                    arg_letter);
            exit(1);
          }
          else{
            /* Value set. */
          }
          break;
        case FILE_OF_ARGUMENTS_ARGUMENT:
          if(*filename != '\0'){
              /* @@@ Potentially recursive call. Is this safe? */
              read_args_file(filename);
          }
          else{
              fprintf(GlobalState.logfile,"Usage: -%cfilename.\n",
                      arg_letter);
          }
          break;
        case NON_MATCHING_GAMES_ARGUMENT:
          if(*filename != '\0'){
              if(GlobalState.non_matching_file != NULL){
                  (void) fclose(GlobalState.non_matching_file);
              }
              GlobalState.non_matching_file = MustOpen(filename,"w");
          }
          else{
              fprintf(GlobalState.logfile,"Usage: -%cfilename.\n",arg_letter);
              exit(1);
          }
          break;
        case TAG_EXTRACTION_ARGUMENT:
            /* A single tag extraction criterion. */
            extract_tag_argument(associated_value);
            break;
        case LINE_WIDTH_ARGUMENT:
            { /* Specify an output line width. */
              unsigned length;

              if(sscanf(associated_value,"%u",&length) > 0){
                  set_output_line_length(length);
              }
              else{
                  fprintf(GlobalState.logfile,
                          "-%c should be followed by an unsigned integer.\n",
                          arg_letter);
                  exit(1);
              }
            }
            break;
        case HELP_ARGUMENT:
            usage_and_exit();
            break;
        case OUTPUT_FORMAT_ARGUMENT:
            /* Whether to use the source form of moves or
             * rewrite them into another format.
             */
	    {
		OutputFormat format = which_output_format(associated_value);
		if(format == UCI) {
		    /* Rewrite the game in a format suitable for input to
		     * a UCI-compatible engine.
		     * This is actually LALG but involves adjusting a lot of
		     * the other statuses, too.
		     */
		    GlobalState.keep_NAGs = FALSE;
		    GlobalState.keep_comments = FALSE;
		    GlobalState.keep_move_numbers = FALSE;
		    GlobalState.keep_checks = FALSE;
		    GlobalState.keep_variations = FALSE;
		    set_output_line_length(5000);
		    format = LALG;
		}
		GlobalState.output_format = format;
	    }
            break;
        case SEVEN_TAG_ROSTER_ARGUMENT:
            if(GlobalState.tag_output_format == ALL_TAGS ||
               GlobalState.tag_output_format == SEVEN_TAG_ROSTER) {
                GlobalState.tag_output_format = SEVEN_TAG_ROSTER;
            }
            else {
                fprintf(GlobalState.logfile,
                        "-%c clashes with another argument.\n",
                        SEVEN_TAG_ROSTER_ARGUMENT);
                exit(1);
            }
            break;
        case DONT_KEEP_COMMENTS_ARGUMENT:
            GlobalState.keep_comments = FALSE;
            break;
        case DONT_KEEP_DUPLICATES_ARGUMENT:
            /* Make sure that this doesn't clash with -d. */
            if(GlobalState.duplicate_file == NULL){
                GlobalState.suppress_duplicates = TRUE;
            }
            else{
                fprintf(GlobalState.logfile,
                            "-%c clashes with -%c flag.\n",
                            DONT_KEEP_DUPLICATES_ARGUMENT,
                            DUPLICATES_FILE_ARGUMENT);
                exit(1);
            }
            break;
        case DONT_MATCH_PERMUTATIONS_ARGUMENT:
            GlobalState.match_permutations = FALSE;
            break;
        case DONT_KEEP_NAGS_ARGUMENT:
            GlobalState.keep_NAGs = FALSE;
            break;
        case OUTPUT_FEN_STRING_ARGUMENT:
            /* Output a FEN string of the final position.
             * This is displayed in a comment.
             */
	    if(GlobalState.add_FEN_comments) {
		/* Already implied. */
	        GlobalState.output_FEN_string = FALSE;
	    }
	    else {
		GlobalState.output_FEN_string = TRUE;
	    }
            break;
        case CHECK_ONLY_ARGUMENT:
            /* Report errors, but don't convert. */
            GlobalState.check_only = TRUE;
            break;
        case KEEP_SILENT_ARGUMENT:
            /* Turn off progress reporting. */
            GlobalState.verbose = FALSE;
            break;
        case USE_SOUNDEX_ARGUMENT:
            /* Use soundex matches for player tags. */
            GlobalState.use_soundex = TRUE;
            break;
        case MATCH_CHECKMATE_ARGUMENT:
            /* Match only games that end in checkmate. */
            GlobalState.match_only_checkmate = TRUE;
            break;
        case SUPPRESS_ORIGINALS_ARGUMENT:
            GlobalState.suppress_originals = TRUE;
            break;
        case DONT_KEEP_VARIATIONS_ARGUMENT:
            GlobalState.keep_variations = FALSE;
            break;
        case USE_VIRTUAL_HASH_TABLE_ARGUMENT:
            GlobalState.use_virtual_hash_table = TRUE;
            break;

        case TAGS_ARGUMENT:
            if(*filename != '\0'){
                read_tag_file(filename);
            }
            break;
        case TAG_ROSTER_ARGUMENT:
            if(*filename != '\0'){
                read_tag_roster_file(filename);
            }
            break;
        case MOVES_ARGUMENT:
            if(*filename != '\0'){
                /* Where the list of variations of interest are kept. */
                FILE *variation_file = MustOpen(filename,"r");
                /* We wish to search for particular variations. */
                add_textual_variations_from_file(variation_file);
                fclose(variation_file);
            }
            break;
        case POSITIONS_ARGUMENT:
            if(*filename != '\0'){
                FILE *variation_file = MustOpen(filename,"r");
                /* We wish to search for positional variations. */
                add_positional_variations_from_file(variation_file);
                fclose(variation_file);
            }
            break;
        case ENDINGS_ARGUMENT:
            if(*filename != '\0'){
                if(!build_endings(filename)){
                    exit(1);
                }
            }
            break;
        default:
            fprintf(GlobalState.logfile,
                    "Unrecognized argument -%c\n", arg_letter);
    }
}

    /* The argument has been expressed in a long-form, i.e. prefixed
     * by --
     * Decode and act on the argument.
     * The associated_value will only be required by some arguments.
     * Return whether one or both were required.
     */
int
process_long_form_argument(const char *argument, const char *associated_value)
{
    if(stringcompare(argument, "append") == 0) {
        process_argument(APPEND_TO_OUTPUT_FILE_ARGUMENT, associated_value);
        return 2;
    }
    else if(stringcompare(argument, "checkfile") == 0) {
        process_argument(CHECK_FILE_ARGUMENT, associated_value);
        return 2;
    }
    else if(stringcompare(argument, "checkmate") == 0) {
        process_argument(MATCH_CHECKMATE_ARGUMENT, "");
        return 1;
    }
    else if(stringcompare(argument, "duplicates") == 0) {
        process_argument(DUPLICATES_FILE_ARGUMENT, associated_value);
        return 2;
    }
    else if(stringcompare(argument, "evaluation") == 0) {
        /* Output an evaluation is required with each move. */
        GlobalState.output_evaluation = TRUE;
        return 1;
    }
    else if(stringcompare(argument, "fencomments") == 0) {
        /* Output an evaluation is required with each move. */
        GlobalState.add_FEN_comments = TRUE;
	/* Turn off any separate setting of output_FEN_comment. */
	GlobalState.output_FEN_string = FALSE;
        return 1;
    }
    else if(stringcompare(argument, "help") == 0) {
        process_argument(HELP_ARGUMENT, "");
        return 1;
    }
    else if(stringcompare(argument, "fuzzydepth") == 0) {
        /* Extract the depth. */
        int depth = 0;

        if(sscanf(associated_value, "%d",&depth) == 1){
            if(depth >= 0) {
		GlobalState.fuzzy_match_duplicates = TRUE;
                GlobalState.fuzzy_match_depth = depth;
            }
            else {
                fprintf(GlobalState.logfile,
                        "--%s requires a number greater than or equal to zero.\n", argument);
                exit(1);
            }
        }
        else {
            fprintf(GlobalState.logfile,
                    "--%s requires a number following it.\n", argument);
            exit(1);
        }
        return 2;
    }
    else if(stringcompare(argument, "linelength") == 0) {
        process_argument(LINE_WIDTH_ARGUMENT,
                         associated_value);
        return 2;
    }
    else if(stringcompare(argument, "markmatches") == 0) {
        if(*associated_value != '\0') {
	    GlobalState.add_position_match_comments = TRUE;
	    GlobalState.position_match_comment = StringCopy(associated_value);
	}
	else {
              fprintf(GlobalState.logfile,
                        "--markmatches requires a comment string following it.\n");
              exit(1);
	}
	return 2;
    }
    else if(stringcompare(argument, "nochecks") == 0) {
        GlobalState.keep_checks = FALSE;
        return 1;
    }
    else if(stringcompare(argument, "nocomments") == 0) {
        process_argument(DONT_KEEP_COMMENTS_ARGUMENT, "");
        return 1;
    }
    else if(stringcompare(argument, "noduplicates") == 0) {
        process_argument(DONT_KEEP_DUPLICATES_ARGUMENT, "");
        return 1;
    }
    else if(stringcompare(argument, "nomovenumbers") == 0) {
        GlobalState.keep_move_numbers = FALSE;
        return 1;
    }
    else if(stringcompare(argument, "nonags") == 0) {
        process_argument(DONT_KEEP_NAGS_ARGUMENT, "");
        return 1;
    }
    else if(stringcompare(argument, "noresults") == 0) {
        GlobalState.keep_results = FALSE;
        return 1;
    }
    else if(stringcompare(argument, "notags") == 0) {
        if(GlobalState.tag_output_format == ALL_TAGS ||
           GlobalState.tag_output_format == NO_TAGS) {
            GlobalState.tag_output_format = NO_TAGS;
        }
        else {
            fprintf(GlobalState.logfile,
                    "--notags clashes with another argument.\n");
            exit(1);
        }
        return 1;
    }
    else if(stringcompare(argument, "nounique") == 0) {
        process_argument(SUPPRESS_ORIGINALS_ARGUMENT, "");
        return 1;
    }
    else if(stringcompare(argument, "novars") == 0) {
        process_argument(DONT_KEEP_VARIATIONS_ARGUMENT, "");
        return 1;
    }
    else if(stringcompare(argument, "output") == 0) {
        process_argument(WRITE_TO_OUTPUT_FILE_ARGUMENT, associated_value);
        return 2;
    }
    else if(stringcompare(argument, "plylimit") == 0) {
          int limit = 0;

          /* Extract the limit. */
          if(sscanf(associated_value, "%d",&limit) == 1){
              if(limit >= 0) {
                  GlobalState.output_ply_limit = limit;
              }
              else {
                  fprintf(GlobalState.logfile,
                        "--plylimit requires a number greater than or equal to zero.\n");
                  exit(1);
              }
          }
          else {
              fprintf(GlobalState.logfile,
                        "--plylimit requires a number following it.\n");
              exit(1);
          }
          return 2;
    }
    else if(stringcompare(argument, "seven") == 0) {
        process_argument(SEVEN_TAG_ROSTER_ARGUMENT, "");
        return 1;
    }
    else if(stringcompare(argument, "stalemate") == 0) {
        GlobalState.match_only_stalemate = TRUE;
        return 1;
    }
    else if(stringcompare(argument, "version") == 0) {
        fprintf(GlobalState.logfile, "pgn-extract %s\n", CURRENT_VERSION);
	exit(0);
        return 1;
    }
    else {
        fprintf(GlobalState.logfile,
                "Unrecognised long-form argument: --%s\n",
                argument);
        exit(1);
        return 1;
    }
}
