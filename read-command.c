// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"

#include <error.h>

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */

// Linked list of command(tree)s
struct command_stream
{
  command* comm;
  command* next;
};

// Linked list of tokens
struct token_stream
{
  char* string;
  token* next;
};

command_stream_t
make_command_stream (int (*getbyte) (void *),
		     void *arg)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */

  int next = getbyte(arg)
  while (next != EOF)
  {
    if (next == '#') // if comment, skip to next line
    {
      do
      {
        next = getbyte(arg);
      } while (next != EOF || next != '\n');
    }

    // if nextline, process current tokens into a command stream
    
    printf(next);
  }

  error (1, 0, "command reading not yet implemented");
  return 0;
}

command_t
read_command_stream (command_stream_t s)
{
  /* FIXME: Replace this with your implementation too.  */
  error (1, 0, "command reading not yet implemented");
  return 0;
}
