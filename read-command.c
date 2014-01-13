/*-----------------------------------------------------------------------------
  UCLA CS 111 Lab 1. "Time Travel Shell""
  Professor Peter Reiher
  Winter 2014

  FILE:   read-command.c
  DESCR:  Reads input shell script into commands.

  AUTHOR(s):
  Alan Kha        904030522 akhahaha@gmail.com
  Braden Anderson 203744563 bradencanderson@gmail.com

-----------------------------------------------------------------------------*/

#include "alloc.h"
#include "command.h"
#include "command-internals.h"

#include <error.h>
#include <stdio.h> // required to print diagnostic text
#include <string.h> // strings: scanning, finding, etc

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */

// Linked list of command(tree)s
struct command_stream
{
  command_t* comm;
  command_t* next;
};

// enumerated list of token types
enum token_type
{
	HEAD,	// used as a dummy header
	PIPE,
	AND,
	OR,
	WORD,
	SEMICOLON,
	SUBSHELL;
	LEFT,
	RIGHT
};

typedef struct token token_t;
struct token
{
	enum token_type type;
	char* content;
	token_t* next;
};

typedef struct token_stream token_stream_t;
struct token_stream
{
	token_t* head;
	token_stream_t* next;
}

// creates a new token with specified type and pointer to content string
token_t* new_token (enum token_type type, char* content)
{
	token_t* tok = (token_stream_t*) checked_malloc(sizeof(token_stream_t));

	tok->type = type;
	tok->content = content;
	tok->next = NULL;
	
	return tok;
}

token_stream* make_token_stream (char* script, size_t script_size)
{
	token_t* head_token = new_token(HEAD, NULL);
	token_t* curr_token = head_token;

	token_stream_t* head_stream = (token_stream_t*) checked_malloc(sizeof(token_stream_t));
	token_stream_t* curr_stream = head_stream;
	curr_stream->head = head_token;

	size_t index = 0;
	char c = *script;
	while (index < script_size)
	{
		// input decision tree, TODO: better as switch statement?
		if (c == '(') // SUBSHELL
		{
			int nested = 1;

			size_t count = 0;
			size_t subshell_size = 64;
			char* subshell = (char*) checked_malloc(subshell_size);

			// grab contents until subshell is closed
			while (nested > 0)
			{
				c++; index++;
				if (index == script_size)
				{
					error (2, 0, "Syntax error. EOF reached before subshell was closed.")
					// TODO force exit
				}

				if (c == '(') // count for nested subshells
					nested++;
				else if (c == ')') // close subshell
					nested--;

				// load into subshell buffer
				subshell[count] = c;
				count++;

				// expand subshell buffer if necessary
				if (count == subshell_size)
				{
					subshell_size = subshell_size * 2;
					subshell = checked_grow_alloc (subshell, &subshell_size);
				}
			}

			curr_token-> = new_token(SUBSHELL, subshell);
			curr_token = curr_token->next;
		}
		else if (c == ')') // CLOSE PARENS
		{
			error (2, 0, "Syntax error. Close parens found without matching open parens.");
			// TODO force exit?
		}
		else if (c == '<') // LEFT REDIRECT
		{
			curr_token-> = new_token(LEFT, NULL);
			curr_token = curr_token->next;
		}
		else if (c == '>') // RIGHT REDIRECT
		{
			curr_token-> = new_token(RIGHT, NULL);
			curr_token = curr_token->next;
		}
		else if (c == '&') // check & or &&
		{
			c++; index++;
			if (c == '&') // AND
			{
				curr_token->next = new_token(AND, NULL);
				curr_token = curr_token->next;
			}
			else // single & is illegal?
			{
				error (2, 0, "Syntax error. Single '&' not allowed.");
				// TODO force exit?
			}
		}
		else if (c == '|') // check | or ||, TODO: what about |||||?
		{
			c++; index++;
			if (c== '|') // OR
			{
				curr_token-> = new_token(OR, NULL);
				curr_token = curr_token->next;
			}
			else // PIPE
			{
				curr_token-> new_token(OR, NULL);
				curr_token = curr_token->next;
			}
		}
		else if (c == ';') // SEMICOLON
		{
			curr_token-> = new_token(SEMICOLON, NULL);
			curr_token = curr_token->next;
		}
		else if (c == ' ' || c == '\t') // WHITESPACE
		{
			// ignore
			c++; index++;
		}
		else if (c == '\n') // NEWLINE
		{
			// start next token_stream
			curr_stream->next = (token_stream_t*) checked_malloc(sizeof(token_stream_t));
			curr_stream = curr_stream->next;
			curr_stream->head = new_token(HEAD, NULL);
			curr_token = curr_stream->head;
		}
		// TODO process simple words
	}

	return head_stream;
}

command_stream_t
make_command_stream (int (*getbyte) (void *),
		     void *arg)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */

	size_t count = 0;
	size_t buffer_size = 1024; // initial buffer size is 1K chars

	char* buffer = (char *) checked_malloc(buffer_size*sizeof(char));

	int next;

	do
	{
		next = getbyte(arg);

		if (next == '#') // for comments: ignore everything until '\n' or EOF
		{
			do
			{
				next = getbyte(arg);
			} while (next != -1 && next != '\n');
		}

		// TODO: test for illegal chars

		if( next != -1 )
		{
			// load into buffer
			buffer[count] = next;
			count++;

			// expand buffer if necessary
			if (count == buffer_size)
			{
				buffer_size = buffer_size * 2; // TODO check for integer overflows necessary?
				buffer = checked_grow_alloc (buffer, &buffer_size);
			}
		}
		
		// // if current char ends complete command, process buf into commands
		// if(next == -1 || next == '\n' || next == ';')
		// {
		// 	buffer[count] = '\0';
		// 	token* head = tokenize_command(buffer);
		// 	// TODO: add head to forest, somehow.
			
		// 	/*
		// 	 * traverse across command list like so:
		// 	 *
		// 	 * while(head != NULL) // DIAGNOSTIC
		// 	 *{
		// 	 *	printf("%s ", head->word);
		// 	 *	head = head->next;
		// 	 *}
		// 	 */

		// 	// reset buffer
		// 	count = 0;
		// }

	} while(next != -1);

	// process buffer into token stream
	token* head = make_token_stream(buffer);
  

	// free(buffer); // free() not defined

	error (1, 0, "command reading not yet implemented");
	return 0;
}

command_t
read_command_stream (command_stream_t s)
{
  /* FIXME: Replace this with your implementation too.  */

  command_stream_t suse = s; // PLACEHOLDER arguments cannot be unused

  error (1, 0, "command reading not yet implemented");
  return 0;
}
