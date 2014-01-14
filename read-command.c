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

#include <ctype.h>
#include <error.h>
#include <stdbool.h>
#include <stdio.h> // required to print diagnostic text
#include <stdlib.h>
#include <string.h> // strings: scanning, finding, etc

// Linked list of command(tree)s
struct command_stream
{
  command_t* comm;
  command_t* next;
};

// Enumerated token types
enum token_type
{
  HEAD,	// used for dummy head of token lists
  SUBSHELL,
  LEFT,
  RIGHT,
  AND,
  OR,
  PIPE,
  SEMICOLON,
  WORD
};

// Linked list of tokens. Stores type and content (for subshells and words)
typedef struct token token_t;
struct token
{
	enum token_type type;
	char* content;
	token_t* next;
};

// Linked list of token-lists.
typedef struct token_stream token_stream_t;
struct token_stream
{
	token_t* head;
	token_stream_t* next;
};

// Creates a new token with specified type and pointer to content string
token_t* new_token (enum token_type type, char* content)
{
	token_t* tok = checked_malloc(sizeof(token_t));

	tok->type = type;
	tok->content = content;
	tok->next = NULL;
	
	return tok;
}

// Determines if a character can be part of a simple word
bool is_word (char c)
{
  if (isalnum(c) || strchr("!%+,-./:@^_", c) != NULL)
    return true;
  
  return false;
}

// DIAGNOSTIC FUNCTION Outputs all tokens of a token stream to stdout.
bool output_token_stream (token_stream_t* head_stream)
{
  token_stream_t* curr_stream = head_stream;
  int count = 1;
  while (curr_stream != NULL)
  {
    printf("TOKEN "); putchar(count); putchar ('\n');

    token_t* curr = curr_stream->head->next; // next to skip the dummy header

    while (curr != NULL)
    {
      switch (curr->type)
      {
        case SUBSHELL: printf(curr->content); putchar('+'); break;
        case LEFT: printf("LEFT+"); break;
        case RIGHT: printf("RIGHT+"); break;
        case AND: printf("AND+"); break;
        case OR: printf("OR+"); break;
        case PIPE: printf("PIPE+"); break;
        case SEMICOLON: printf("SEMICOLON+"); break;
        case WORD: printf(curr->content); putchar('+'); break;
        default: break;
      };

      curr = curr->next;
    }

    putchar('\n');
  }

  return true;
}

// Converts an input script into a token stream
token_stream_t* make_token_stream (char* script, size_t script_size)
{
	token_t* head_token = new_token(HEAD, NULL);
	token_t* curr_token = head_token;

	token_stream_t* head_stream = checked_malloc(sizeof(token_stream_t));
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
			char* subshell = checked_malloc(subshell_size);

			// grab contents until subshell is closed
			while (nested > 0)
			{
				script++; index++; c = *script;
				if (index == script_size)
				{
					error(2, 0, "Syntax error. EOF reached before subshell was closed.");
					return head_stream; // TODO force exit
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

      // create subshell token
			curr_token->next = new_token(SUBSHELL, subshell);
			curr_token = curr_token->next;
		}
		else if (c == ')') // CLOSE PARENS
		{
			error(2, 0, "Syntax error. Close parens found without matching open parens.");
			return head_stream; // TODO force exit?
		}
		else if (c == '<') // LEFT REDIRECT
		{
			curr_token->next = new_token(LEFT, NULL);
			curr_token = curr_token->next;

      script++; index++; c = *script;
		}
		else if (c == '>') // RIGHT REDIRECT
		{
			curr_token->next = new_token(RIGHT, NULL);
			curr_token = curr_token->next;

      script++; index++; c = *script;
		}
		else if (c == '&') // check & or &&
		{
			script++; index++; c = *script;

			if (c == '&') // AND
			{
				curr_token->next = new_token(AND, NULL);
				curr_token = curr_token->next;
			}
			else // single & is illegal?
			{
				error(2, 0, "Syntax error. Single '&' not allowed.");
				return head_stream; // TODO force exit?
			}
		}
		else if (c == '|') // check | or ||, TODO: what about |||||?
		{
			script++; index++; c = *script;

			if (c== '|') // OR
			{
				curr_token->next = new_token(OR, NULL);
				curr_token = curr_token->next;
			}
			else // PIPE
			{
				curr_token->next = new_token(OR, NULL);
				curr_token = curr_token->next;
			}
		}
		else if (c == ';') // SEMICOLON
		{
			curr_token->next = new_token(SEMICOLON, NULL);
			curr_token = curr_token->next;

      script++; index++; c = *script;
		}
		else if (c == ' ' || c == '\t') // WHITESPACE
		{
			// do nothing
			script++; index++; c = *script;
		}
		else if (c == '\n') // NEWLINE
		{
      printf("Making new token stream...\n"); // DIAGNOSTIC

			// start next token_stream
			curr_stream->next = checked_malloc(sizeof(token_stream_t));
			curr_stream = curr_stream->next;
			curr_stream->head = new_token(HEAD, NULL);
			curr_token = curr_stream->head;

      script++; index++; c = *script;
		}
    else if (is_word(c)) // WORD
    {
      size_t count = 0;
      size_t word_size = 16;
      char* word = checked_malloc(word_size);

      do
      {
        // load into word buffer
        word[count] = c;
        count++;

        // expand word buffer if necessary
        if (count == word_size)
        {
          word_size = word_size * 2;
          word = checked_grow_alloc(word, &word_size);
        }

        script++; index++; c = *script;
      } while (is_word(c) && index < script_size);

      // create word token
      curr_token->next = new_token(WORD, word);
      curr_token = curr_token->next;
    }
    else // UNRECOGNIZED CHARACTER
    {
      putchar('+'); putchar(c); error(2, 0, "Syntax error. Unrecognized character in script.");
      return head_stream; // TODO force exit?
    }
	}

	return head_stream;
}

command_stream_t
make_command_stream (int (*getbyte) (void *),
		     void *arg)
{
	size_t count = 0;
	size_t buffer_size = 1024;
	char* buffer = checked_malloc(buffer_size);

	int next;

  // create cstring buffer of input with comments stripped out
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
	} while(next != -1);

  printf(buffer);
  printf("Buffer loaded...\n"); // DIAGNOSTIC

	// process buffer into token stream
	token_stream_t* head = make_token_stream(buffer, count);

  printf("Token streams created...\n"); // DIAGNOSTIC
  
  output_token_stream(head); // DIAGNOSTIC

	free(buffer);

	error(1, 0, "command reading not yet implemented");
	return 0;
}

command_t
read_command_stream (command_stream_t s)
{
  /* FIXME: Replace this with your implementation too.  */

  command_stream_t suse = s; // PLACEHOLDER arguments cannot be unused

  error(1, 0, "command reading not yet implemented");
  return 0;
}
