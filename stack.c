/*-----------------------------------------------------------------------------
	UCLA CS 111 Lab 1. "Time Travel Shell"
	Professor Peter Reiher
	Winter 2014

	FILE:   stack.c
	DESCR:  Simple stack implementation repurposed for making command trees
	
	NOTES:	Max items is currently 4 (should not need more for this purpose)
			Remember to set num_items on initialization.
			
	SOURCE:	"Notes from C++ Introduction" (1997) by Jonathan Levene @ MIT
			http://groups.csail.mit.edu/graphics/classes/6.837/F04/cpp_notes/
			c++_notes.html

	AUTHOR(s):
	Alan Kha        904030522 akhahaha@gmail.com
	Braden Anderson 203744563 bradencanderson@gmail.com

-----------------------------------------------------------------------------*/

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
