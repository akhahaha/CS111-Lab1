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
	token_t* tok;
	token_stream_t* next;
}

// creates a new token with specified type and pointer to content string
token_t* new_token (enum token_type type, char* content)
{
	token_t* tok = (token_stream_t*) checked_malloc(sizeof(token_stream_t));
	
	if (tok == NULL)
	{
		printf("Token creation failed.\n");
	}
	else
	{
		tok->type = type;
		tok->content = content;
		tok->next = NULL;
	}
	
	return tok;
}

// converts a string representing a complete command into tokens.
token_t* tokenize_command (char* cmd_buf)
{
	token_t* head = NULL;
	token_t* curr = head;
	
	char* tkn = strtok(cmd_buf, " ");
	char* prev_tkn = NULL;
	while(tkn != NULL)
	{
		if(head == NULL)
		{
			head = (token_t*)checked_malloc(sizeof(token_t));
			curr = head;
		}
		else
		{
			curr->next = (token_t*)checked_malloc(sizeof(token_t));
			curr = curr->next;
		}
		curr->word = (char *) checked_malloc(strlen(tkn));;
		curr->next = NULL;
		
		strcpy(curr->word, tkn);
		
		prev_tkn = tkn;
		tkn = strtok(NULL, " \n");
		
		// adjacent whitespace should be skipped over
		if(tkn != NULL && tkn - prev_tkn <= 1)
		{
			prev_tkn = tkn;
			tkn = strtok(NULL, " ");
		}
	}
	
	return head;
}

token_stream* make_token_stream (char* script, size_t script_size)
{	
	token_stream_t* curr_stream = (token_stream_t*) checked_malloc(sizeof(token_stream_t));
	token_stream_t* head_stream = curr_stream;

	token_t* curr_token = (token_t*) checked_malloc(sizeof(token_t));
	token_t* head_token = NULL;

	size_t index = 0;
	char c = *script;
	while (index < script_size)
	{
		// check & or &&
		if (c == '&')
		{
			c++; index++;
			if (c == '&')
			{
				curr_token->word = (char*) checked_malloc(2);
				curr_token->word = "&&";
				curr_token->next = (token_t*) checked_malloc(sizeof(token_t));
				curr_token = curr_token->next;
			}
			else
			{
				curr_token->word = (char*) checked_malloc(|);
				curr_token->word = "|";
				curr_token->next = (token_t*) checked_malloc(sizeof(token_t));
				curr_token = curr_token->next;
			}
		}
		// check | or ||
		else if (c == )
		// process < > or ;
		// process subshells
		// process whitespace
		// process simple words
	}
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
			buffer[count] = next;
			count++;

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
