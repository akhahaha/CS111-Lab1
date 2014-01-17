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
#include "stack.c"

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
	int line;
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
token_t* new_token (enum token_type type, char* content, int line)
{
	token_t* tok = checked_malloc(sizeof(token_t));

	tok->type = type;
	tok->content = content;
	tok->line = line;
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
	token_t* head_token = new_token(HEAD, NULL, -1);
	token_t* curr_token = head_token;
	token_t* prev_token = NULL;

	token_stream_t* head_stream = checked_malloc(sizeof(token_stream_t));
	token_stream_t* curr_stream = head_stream;
	curr_stream->head = head_token;

	int line = 1;
	size_t index = 0;
	char c = *script;
	while (index < script_size)
	{
		// input decision tree, TODO: better as switch statement?
		if (c == '(') // SUBSHELL
		{
			int subshell_line = line;
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
					error(2, 0, "Line %d: Syntax error. EOF reached before subshell was closed.", subshell_line);
					return NULL;
				}

				if (c == '\n')
					line++;
				else if (c == '(') // count for nested subshells
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
			curr_token->next = new_token(SUBSHELL, subshell, subshell_line); 
			curr_token = curr_token->next;
		}
		else if (c == ')') // CLOSE PARENS
		{
			error(2, 0, "Line %d: Syntax error. Close parens found without matching open parens.", line);
			return NULL;
		}
		else if (c == '<') // LEFT REDIRECT
		{
			curr_token->next = new_token(LEFT, NULL, line);
			curr_token = curr_token->next;

			script++; index++; c = *script;
		}
		else if (c == '>') // RIGHT REDIRECT
		{
			curr_token->next = new_token(RIGHT, NULL, line);
			curr_token = curr_token->next;

			script++; index++; c = *script;
		}
		else if (c == '&') // check & or &&
		{
			script++; index++; c = *script; // check next char

			if (c == '&') // AND
			{
				curr_token->next = new_token(AND, NULL, line);
				curr_token = curr_token->next;

				script++; index++; c = *script;
			}
			else // single & is illegal?
			{
				error(2, 0, "Line %d: Syntax error. Single '&' not allowed.", line);
				return NULL;
			}
		}
		else if (c == '|') // check | or ||, TODO: what about |||||?
		{
			script++; index++; c = *script; // check next char

			if (c== '|') // OR
			{
				curr_token->next = new_token(OR, NULL, line);
				curr_token = curr_token->next;

				script++; index++; c = *script;
			}
			else // PIPE
			{
				curr_token->next = new_token(PIPE, NULL, line);
				curr_token = curr_token->next;
			}
		}
		else if (c == ';') // SEMICOLON
		{
			curr_token->next = new_token(SEMICOLON, NULL, line);
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
			line++;
			
			// start next token_stream only if current stream has been used
			if (curr_token->type != HEAD)
			{
				curr_stream->next = checked_malloc(sizeof(token_stream_t));
				curr_stream = curr_stream->next;
				curr_stream->head = new_token(HEAD, NULL, -1);
				curr_token = curr_stream->head;
			}

			script++; index++; c = *script;
		}
		else if (is_word(c)) // WORD
		{
			size_t count = 0;
			size_t word_size = 8;
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
			curr_token->next = new_token(WORD, word, line);
			curr_token = curr_token->next;
		}
		else // UNRECOGNIZED CHARACTER
		{
			error(2, 0, "Line %d: Syntax error. Unrecognized character in script.", line);
			return NULL;
		}
	}

	return head_stream;
}

// Pops one op and two operands and reinserts the branch into operand stack
// Returns true if successful
bool make_branch (stack* ops, stack* operands)
{
	if (size(operands) < 2)
		return false;
	
	// pop twice from operands
	command_t right_child = pop(operands);
	command_t left_child = pop(operands);
	
	// pop once from ops
	command_t new_cmd = pop(ops);
	
	new_cmd->u.command[0] = left_child;
	new_cmd->u.command[1] = right_child;
	
	// push new tree onto operands
	push(operands, new_cmd);
	
	return true;
}

// Converts a list of tokens into a command tree
command_t make_command_tree (token_t* head_tok)
{
	token_t* curr_tok = head_tok;

	if (!curr_tok)
	{
		error(3, 0, "Line 0: NULL pointer passed to make_command_tree()");
		return NULL;
	}

	// initialize stacks
	stack* ops = checked_malloc(sizeof(stack));	ops->num_items = 0;
	stack* operands = checked_malloc(sizeof(stack)); operands->num_items = 0;

	command_t prev_cmd = NULL;
	command_t curr_cmd;

	// process tokens
	do
	{
		if( !(curr_tok->type == LEFT || curr_tok->type == RIGHT) )
		{
			// make new command
			curr_cmd = checked_malloc(sizeof( struct command ));
		}
		
		switch (curr_tok->type)
		{
			case SUBSHELL:
				curr_cmd->type = SUBSHELL_COMMAND;

				// process subshell command tree
				curr_cmd->u.subshell_command = make_command_tree(
					make_token_stream(curr_tok->content, strlen(curr_tok->content))->head);

				// TODO: multiline subshells?
				// requires changes to command struct definitino

				// push SUBSHELL tree to operands
				push(operands, curr_cmd);
				break;
				
			case LEFT:
				// check that previous command is a subshell or word
				if (prev_cmd == NULL || !(prev_cmd->type == SIMPLE_COMMAND || prev_cmd->type == SUBSHELL_COMMAND))
				{
					error(2, 0, "Line %d: Syntax error. Redirects can only follow words or subshells.", curr_tok->line);
					return NULL; // TODO: EH
				}
				else if (prev_cmd->output != NULL)
				{
					error(2, 0, "Line %d: Syntax error. Previous command already has output. ", curr_tok->line);
				}
				else if (prev_cmd->input != NULL)
				{
					error(2, 0, "Line %d: Syntax error. Previous command already has input.", curr_tok->line);
				}
				
				curr_tok = curr_tok->next;
				if (curr_tok->type == WORD) // followed by a word
				{
					prev_cmd->input = curr_tok->content;
				}
				else
				{
					error(2, 0, "Line %d: Syntax error. Redirects must be followed by words.", curr_tok->line);
					return NULL; // TODO: EH
				}
				
				// no pushing required				
				break;
				
			case RIGHT:
				// check that previous command is a subshell or word
				if (prev_cmd == NULL || !(prev_cmd->type == SIMPLE_COMMAND || prev_cmd->type == SUBSHELL_COMMAND))
				{
					error(2, 0, "Line %d: Syntax error. Redirects can only follow words or subshells.", curr_tok->line);
					return NULL; // TODO: EH
				}
				else if (prev_cmd->output != NULL)
				{
					error(2, 0, "Line %d: Syntax error. Previous command already has output.", curr_tok->line);
					return NULL;
				}
				
				curr_tok = curr_tok->next;
				if (curr_tok->type == WORD) // followed by a word
				{
					prev_cmd->output = curr_tok->content;
				}
				else
				{
					error(2, 0, "Line %d: Syntax error. Redirects must be followed by words", curr_tok->line);
					return NULL; // TODO: EH
				}
				
				// no pushing required				
				break;
				
			case AND:
				curr_cmd->type = AND_COMMAND;

				// if AND has <= priority to operands, pop
				if (	!is_empty(ops) &&
						(	peek(ops)->type == PIPE_COMMAND ||
							peek(ops)->type == OR_COMMAND ||
							peek(ops)->type == AND_COMMAND
						)
					)
				{
					if (!make_branch(ops, operands))
					{
						error(2, 0, "Line %d: Syntax error. Not enough children to create new tree.", curr_tok->line);
						return NULL;
					}
				}
				
				// push AND to ops
				push(ops, curr_cmd);				
				break;
				
			case OR:
				curr_cmd->type = OR_COMMAND;

				// if OR has <= priority to operands, pop
				if (	!is_empty(ops) &&
						(	peek(ops)->type == PIPE_COMMAND ||
							peek(ops)->type == OR_COMMAND ||
							peek(ops)->type == AND_COMMAND
						)
					)
				{
					if (!make_branch(ops, operands))
					{
						error(2, 0, "Line %d: Syntax error. Not enough children to create new tree.", curr_tok->line);
						return NULL;
					}
				}
				
				// push OR to ops
				push(ops, curr_cmd);				
				break;
				
			case PIPE:
				curr_cmd->type = PIPE_COMMAND;

				// if PIPE has <= priority to operands, pop
				if (!is_empty(ops) && peek(ops)->type == PIPE_COMMAND)
				{
					if (!make_branch(ops, operands))
					{
						error(2, 0, "Line %d: Syntax error. Not enough children to create new tree.", curr_tok->line);
						return NULL;
					}
				}
				
				// push PIPE to ops
				push(ops, curr_cmd);				
				break;
				
			case SEMICOLON:
				curr_cmd->type = SEQUENCE_COMMAND;

				// always pop since SEMICOLON <= all ops
				if (!is_empty(ops))
				{
					if (!make_branch(ops, operands))
					{
						error(2, 0, "Line %d: Syntax error. Not enough children to create new tree.", curr_tok->line);
						return NULL;
					}
				}
				
				// push SEMICOLON to ops
				push(ops, curr_cmd);				
				break;
				
			case WORD:
				curr_cmd->type = SIMPLE_COMMAND;
				
				// gather up all following words
				size_t i = 0;
				token_t* ct = curr_tok;
				while(1)
				{
					i++;
					if (ct->next == NULL || ct->next->type != WORD)
						break;
					else
						ct = ct->next;
				}
				curr_cmd->u.word = (char**) checked_malloc((i+1) * sizeof(char*));

				curr_cmd->u.word[0] = curr_tok->content;
				size_t j;
				for(j=1; j != i; j++)
				{
					curr_tok = curr_tok->next;
					curr_cmd->u.word[j] = curr_tok->content;
				}
				
				// set last word pointer to NULL
				curr_cmd->u.word[j] = NULL;
				
				// push SIMPLE COMMAND to operands
				push(operands, curr_cmd);
				break;
			default: break;
		};

		prev_cmd = curr_cmd; // save previous command for operand checks		
	} while(curr_tok != NULL && (curr_tok = curr_tok->next) != NULL );

	// finish tree from existing stack
	while(size(ops) > 0)
	{
		make_branch(ops, operands);
	}
	
	// check for single root
	if (size(operands) != 1)
	{
		error(2, 0, "Line %d: Syntax error. Tree did not converge into single root.", curr_tok->line);
		return NULL;
	}

	return pop(operands); // the root should be the final tree left in operands
}

// TODO: better function name
command_stream_t make_command_forest (token_stream_t* tok_head_stream)
{
	// token_stream_t* head_stream

	token_stream_t* tok_curr_stream = tok_head_stream;

	head_command = NULL;
	command_stream_t curr_command = head_command;

	int count = 1;
	while (tok_curr_stream->head->next != NULL)
	{
		token_t* curr = tok_curr_stream->head->next; // skips dummy header
		command_t cmd = make_command_tree (curr);	// root of command tree

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

		tok_curr_stream = tok_curr_stream->next;
		count++;
	}

	return head_command;
}

command_stream_t make_command_stream (int (*getbyte) (void *), void *arg)
{
	size_t count = 0;
	size_t buffer_size = 1024;
	char* buffer = checked_malloc(buffer_size);

	char next;

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
				buffer_size = buffer_size * 2;
				buffer = checked_grow_alloc (buffer, &buffer_size);
			}
		}
	} while(next != -1);

	// process buffer into token stream
	token_stream_t* head = make_token_stream(buffer, count);	
	// process token stream into command forest
	command_stream_t command_stream = make_command_forest(head);

	// TODO: deallocate memory
	free(buffer);
	// free_tokens(head); TODO: determine a better way to clean up memory (?)
	
	return command_stream;
}

command_t read_command_stream (command_stream_t s)
{
	/* FIXME: Replace this with your implementation too.  */
	int i;
	command_stream_t suse = head_command;

	// pop off head of command stream
	if(head_command)
		head_command = head_command->next;

	return (suse ? suse->comm : NULL);
}
