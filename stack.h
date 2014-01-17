/*-----------------------------------------------------------------------------
	UCLA CS 111 Lab 1. "Time Travel Shell"
	Professor Peter Reiher
	Winter 2014

	FILE:   stack.h
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

command_t peek (stack *s);

command_t pop (stack *s);

command_t push (stack *s, command_t cmd);

int size (stack *s);

int is_empty (stack *s);
