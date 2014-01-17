/*-----------------------------------------------------------------------------
	UCLA CS 111 Lab 1. "Time Travel Shell"
	Professor Peter Reiher
	Winter 2014

	FILE:   stack.c
	DESCR:  Simple stack implementation repurposed for making command trees
	
	NOTES:	Max items is currently 4 (should not need more for this purpose)
			Remember to set num_items on initialization.

	AUTHOR(s):
	Alan Kha        904030522 akhahaha@gmail.com
	Braden Anderson 203744563 bradencanderson@gmail.com

-----------------------------------------------------------------------------*/

/*
 * watch out for segfaults
 * max items is 4
 * remember to set num_items before use
 */

typedef struct stack stack;
struct stack
{
	command_t commands[4];
	int num_items;
};

command_t peek (stack *s)
{
	return s->commands[s->num_items - 1];
}

command_t pop (stack *s)
{
	s->num_items--;
	return s->commands[s->num_items];
}

command_t push (stack *s, command_t cmd)
{
	s->commands[s->num_items] = cmd;
	s->num_items++;
	
	return cmd;
}

int size (stack *s)
{
	return s->num_items;
}

int is_empty (stack *s)
{
	return s->num_items == 0;
}