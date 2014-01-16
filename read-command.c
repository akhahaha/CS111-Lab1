/*-----------------------------------------------------------------------------
	UCLA CS 111 Lab 1. "Time Travel Shell"
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

#include <ctype.h>		// required to perform isalnum()
#include <error.h>
#include <stdbool.h>	// required for boolean functions
#include <stdio.h>		// required to print diagnostic text
#include <stdlib.h>		// required to free memory
#include <string.h>		// required for strchr()

// Linked list of command(tree)s
struct command_stream
{
	command_t comm;
	command_stream_t next;
};

// Enumerated token types
enum token_type
{
	HEAD,		// used for dummy head of token lists
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

command_stream_t head_command;

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

// DIAGNOSTIC FUNCTION: Outputs all tokens of a token stream to stdout
void output_token_stream (token_stream_t* head_stream)
{
	token_stream_t* curr_stream = head_stream;

	int count = 1;
	while (curr_stream->head->next != NULL)
	{
		printf("TOKEN STREAM %i:\n", count);

		token_t* curr = curr_stream->head->next; // skips dummy header

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
		curr_stream = curr_stream->next;
		count++;
	}

	return;
}

// Deallocates all allocated memory associated with a token stream
void free_tokens (token_stream_t* head_stream)
{
	token_stream_t* curr_stream = head_stream;
	token_stream_t* prev_stream;

	while (curr_stream != NULL)
	{
		token_t* curr = curr_stream->head;
		token_t* prev;

		while (curr != NULL)
		{
			if (curr->content != NULL)
				free(curr->content);

			prev = curr;
			curr = curr->next;
			free(prev);
		}

		prev_stream = curr_stream;
		curr_stream = curr_stream->next;
		free(prev_stream);
	}

	return;
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
				{
					nested--;

					if (nested == 0) // break if outermost subshell is closed
					{
						script++; index++; c = *script; // consume last close parens
						break;
					}
				}

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
			script++; index++; c = *script; // check next char

			if (c == '&') // AND
			{
				curr_token->next = new_token(AND, NULL);
				curr_token = curr_token->next;

				script++; index++; c = *script;
			}
			else // single & is illegal?
			{
				error(2, 0, "Syntax error. Single '&' not allowed.");
				return head_stream; // TODO force exit?
			}
		}
		else if (c == '|') // check | or ||, TODO: what about |||||?
		{
			script++; index++; c = *script; // check next char

			if (c== '|') // OR
			{
				curr_token->next = new_token(OR, NULL);
				curr_token = curr_token->next;

				script++; index++; c = *script;
			}
			else // PIPE
			{
				curr_token->next = new_token(PIPE, NULL);
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
			// start next token_stream only if current stream has been used
			if (curr_token->type != HEAD)
			{
				curr_stream->next = checked_malloc(sizeof(token_stream_t));
				curr_stream = curr_stream->next;
				curr_stream->head = new_token(HEAD, NULL);
				curr_token = curr_stream->head;
			}

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
			error(2, 0, "Syntax error. Unrecognized character in script.");
			return head_stream; // TODO force exit?
		}
	}

	return head_stream;
}

command_t construct_complete_command (token_t* head_tok)
{
	token_t* ctok = head_tok;
	command_t root = NULL;
	if(!ctok)
	{
		printf("Attempting to construct complete command on NULL token!\n");
		return NULL;
	}
	
	command_t prev_cmd = NULL;
	command_t prev_word = NULL;
	command_t cmd = NULL;
	
	// for support for infix format: [word] [operator] [word]
	// just set waiting_for_input = operator_cmd
	command_t waiting_for_input = NULL;

	do
	{
		// make new command
		cmd = (command_t) checked_malloc(sizeof( struct command ));
		if(!root)
		{
			root = cmd;
		}
		
		switch (ctok->type)
		{
			case SUBSHELL:
				cmd->type = SUBSHELL_COMMAND;
			//	printf(ctok->content);
				putchar('(');
				// make new command
				// cmd -> u.subshell_cmd = construct_complete_cmd(
				//		make_token_stream (ctok->content, strlen(ctok->content)) -> head
				// );
				cmd -> u.subshell_command = construct_complete_command(
					make_token_stream (ctok->content, strlen(ctok->content)) -> head
				);
				
				prev_word = cmd; // for redirection support
				if(prev_cmd)
				{
					prev_cmd->u.command[1] = cmd;
				}
				
				putchar(')');
				// TODO: make_token_stream() might return multiple HEADs. Maybe use a while loop
				// and a subshell cmd would have N children (N = # of heads)
			//	root = cmd;
				
				// TODO: treat this like a word
				break;
			case LEFT:
				// next token should be word (assumption)
				ctok = ctok->next;
				//assert(ctok->type == WORD)
				prev_word->input = ctok->content;
				
			//	root = cmd;
				break;
			case RIGHT:
				// next token should be word (assumption)
				ctok = ctok->next;
				//assert(ctok->type == WORD)
				prev_word->output = ctok->content;
			//	root = cmd;
				break;
			case AND: printf("&&");
				cmd->type = AND_COMMAND;
			//	if(waiting_for_input != NULL)
			//	{
			//		waiting_for_input->u.command[1] = cmd;
			//	}
			//	if(!prev_cmd && prev_cmd->type != SIMPLE_COMMAND)
			//		prev_cmd->u.command[1] = cmd;
				
				cmd->u.command[0] = root; //(prev_cmd ? prev_cmd : prev_word);
			//	waiting_for_input = cmd;
				root = cmd;
				break;
			case OR: printf("||");
				cmd->type = OR_COMMAND;
			/*	if(waiting_for_input != NULL)
				{
					waiting_for_input->u.command[1] = cmd;
				}*/
				
				cmd->u.command[0] = root; //(prev_cmd ? prev_cmd : prev_word);
			//	waiting_for_input = cmd;
				root = cmd;
				break;
			case PIPE:
				cmd->type = PIPE_COMMAND;
				printf("|");
			/*	if(waiting_for_input != NULL)
				{
					waiting_for_input->u.command[1] = cmd;
				}*/
				
				cmd->u.command[0] = root; //(prev_cmd ? prev_cmd : prev_word);
			//	waiting_for_input = cmd;
				root = cmd;
				break;
			case SEMICOLON:
				cmd->type = SEQUENCE_COMMAND;
				printf(";");
			/*	if(waiting_for_input != NULL)
				{
					waiting_for_input->u.command[1] = cmd;
				}*/
				
				cmd->u.command[0] = root; //(prev_cmd ? prev_cmd : prev_word);
			//	waiting_for_input = cmd;
				root = cmd;
				break;
			case WORD:
				cmd->type = SIMPLE_COMMAND;
				putchar('[');
				size_t i = 0;
				token_t* ct = ctok;
				while(1)
				{
					i++;
					if(ct->next == NULL || ct->next->type != WORD)
						break;
					else
						ct = ct->next;
				}
				cmd->u.word = (char**) checked_malloc((i+1) * sizeof(char*));

				cmd->u.word[0] = ctok->content;
				printf("%s",cmd->u.word[0]);
				size_t j;
				for(j=1; j != i; j++)
				{
					putchar(' ');
					ctok = ctok->next;
					cmd->u.word[j] = ctok->content;
					printf("%s",cmd->u.word[j]);
				}
				// set last word pointer to NULL
				cmd->u.word[j] = NULL;
				putchar(']');
				
			/*	if(waiting_for_input != NULL)
				{
					waiting_for_input->u.command[1] = cmd;
				}
				
				waiting_for_input = NULL;*/
				prev_word = cmd; // for redirection support
			/*	if(prev_cmd)
				{
					prev_cmd->u.command[1] = cmd;
				}*/
				root->u.command[1] = cmd;
				break;
			default: break;
		};
		if(cmd->type != SIMPLE_COMMAND && cmd->type != SUBSHELL_COMMAND)
			prev_cmd = cmd;
		
	} while( ctok != NULL && (ctok = ctok->next) != NULL );
	
	return root;

}

// TODO: better function name
command_stream_t construct_command_stream (token_stream_t* tok_head_stream)
{
//	token_stream_t* head_stream

	token_stream_t* tok_curr_stream = tok_head_stream;
	
	head_command = NULL;
	command_stream_t curr_command = head_command;

	int count = 1;
	while (tok_curr_stream->head->next != NULL)
	{
		printf("PROCESSING TOKEN STREAM %i:\n", count);

		token_t* curr = tok_curr_stream->head->next; // skips dummy header
		command_t cmd = construct_complete_command (curr);	// root of command tree
		
		// if this is the first complete command
		if(!head_command)
		{
			head_command = (command_stream_t) checked_malloc(sizeof(struct command_stream));
			head_command->comm = cmd;
			curr_command = head_command;
		}
		else
		{
			curr_command->next = (command_stream_t) checked_malloc(sizeof(struct command_stream));
			curr_command = curr_command->next;
			curr_command->comm = cmd;
		}

		putchar('\n');
		tok_curr_stream = tok_curr_stream->next;
		count++;
	}

	return head_command;
}


command_stream_t
make_command_stream (int (*getbyte) (void *),
		     void *arg)
{
	size_t count = 0;
	size_t buffer_size = 1024;
	char* buffer = checked_malloc(buffer_size);

	int next;

	// create buffer of input with comments stripped out
	do
	{
		next = getbyte(arg);

		if (next == '#') // for comments: ignore everything until '\n' or EOF
		{
			do
			{
				next = getbyte(arg);
			} while (next != -1 && next != EOF && next != '\n');
		}

		if (next != -1 && next != EOF)
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

	printf("Buffer loaded...\n"); // DIAGNOSTIC
	printf(buffer); // DIAGNOSTIC

	// process buffer into token stream
	token_stream_t* head = make_token_stream(buffer, count);

	printf("Token streams created...\n"); // DIAGNOSTIC
	output_token_stream(head); // DIAGNOSTIC
	
	printf("\n\n\n");

	/* TODO: parse token streams into command streams
		- subshells should be run through make_token_stream 
			and the command stream converter recursively?
		- handle EOF token?
	*/
	
	command_stream_t command_stream = construct_command_stream(head);

	// TODO: deallocate memory
	free(buffer);
//	free_tokens(head); TODO: determine a better way to clean up memory (?)

//	error(1, 0, "command making not yet implemented"); // TODO: delete this
	return command_stream;
}

command_t
read_command_stream (command_stream_t s)
{
	/* FIXME: Replace this with your implementation too.  */
	int i;
	command_stream_t suse = head_command;

	// pop off head of command stream
//	if(s == head_command
	if(head_command)
		head_command = head_command->next;
	
	
//	error(1, 0, "command reading not yet implemented");
	return (suse ? suse->comm : NULL);
}
