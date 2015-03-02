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

/* Define a type to hold a list of input files. */
typedef struct {
    /* The list should have a (char *)NULL on the end. */
    const char **files;
    SourceFileType *file_type;
    /* Keep track of the number of files in the list. */
    unsigned num_files;
    /* Keep track of how many the list may hold. */
    unsigned max_files;
} FILE_LIST;

#if 1
#define RUSSIAN_KNIGHT_OR_KING (0x008a)
#define RUSSIAN_KING_SECOND_LETTER (0x00e0)
#define RUSSIAN_QUEEN (0x0094)
#define RUSSIAN_ROOK (0x008b)
#define RUSSIAN_BISHOP (0x0091)
#define RUSSIAN_PAWN (0x00af)
#else
#define RUSSIAN_KNIGHT_OR_KING (0x00eb)
#define RUSSIAN_KING_SECOND_LETTER (0x00d2)
#define RUSSIAN_QUEEN (0x00e6)
#define RUSSIAN_ROOK (0x00ec)
#define RUSSIAN_BISHOP (0x00f3)
#endif
/* Define a macro for checking the value of a char against
 * one of the Russian piece letters.
 */
#define RUSSIAN_PIECE_CHECK(c) ((c) & 0x00ff)

    /* Shared prototypes for the non-static functions in the
     * lexical analyser.
     */
void yyerror(const char *s);
void save_assessment(const char *assess);
void restart_lex_for_new_game(void);
void free_move_list(Move *move_list);
void read_tag_file(const char *TagFile);
void read_tag_roster_file(const char *RosterFile);
Boolean process_tag_line(const char *TagFile,char *line);
Boolean process_roster_line(char *line);
void print_error_context(FILE *fp);
void init_lex_tables(void);
TokenType next_token(void);
TokenType skip_to_next_game(TokenType token);
const char *tag_header_string(TagName tag);
Boolean open_first_file(void);
const char *input_file_name(int file_number);
int current_file_number(void);
Boolean open_eco_file(const char *eco_file);
int yywrap(void);
void add_filename_to_source_list(const char *filename,SourceFileType file_type);
void add_filename_list_from_file(FILE *fp,SourceFileType file_type);
void reset_line_number(void);
