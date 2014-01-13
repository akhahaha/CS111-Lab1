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

typedef struct token token;
struct token
{
	char* word;
	token* next;
};

// Linked list of tokens? needs to be typedefed
// typedef struct token_stream
// {
//    char* token;
//    token_stream_t* next;
// } token_stream_t;

// converts a string representing a complete command into tokens.
token* tokenize_command(char* cmd_buf)
{
	token* head = NULL;
	token* curr = head;
	
	char* tkn = strtok(cmd_buf, " ");
	char* prev_tkn = NULL;
	while(tkn != NULL)
	{
		if(head == NULL)
		{
			head = (token*)checked_malloc(sizeof(token));
			curr = head;
		}
		else
		{
			curr->next = (token*)checked_malloc(sizeof(token));
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
		
		// if current char ends complete command, process buf into commands
		if(next == -1 || next == '\n' || next == ';')
		{
			buffer[count] = '\0';
			token* head = tokenize_command(buffer);
			// TODO: add head to forest, somehow.
			
			/*
			 * traverse across command list like so:
			 *
			 * while(head != NULL)
			 *{
			 *	printf("%s ", head->word);
			 *	head = head->next;
			 *}
			 */

			// reset buffer
			count = 0;
		}

	} while(next != -1);
  

	free(buffer);

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
