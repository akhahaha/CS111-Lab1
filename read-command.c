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

#include "command.h"
#include "command-internals.h"

#include <error.h>
#include <stdio.h> // required to print diagnostic text

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

// Linked list of tokens? needs to be typedefed
typedef struct token_stream
{
   char* token;
   token_stream_t* next;
};

command_stream_t
make_command_stream (int (*getbyte) (void *),
		     void *arg)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */

  size_t count = 0;
  size_t buffer_size = 1024;

  char* buffer = (char *) checked_malloc(buffer_size);

  int next;

  do
  {
    next = getbyte(arg);

    if (next == '#') // if comment, skip to next line
    {
      do
      {
        next = getbyte(arg);
      } while (next != -1 && next != '\n');
    }

    if (next != -1) // if not comment/EOF, load into buffer
    {
      buffer[count] = next;
      count++;

      // double buffer when filled
      if (count == buffer_size)
      {
        buffer_size = buffer_size * 2;
        buffer = checked_grow_alloc (buffer, &buffer_size);
      }
    }
  } while (next != -1);

  // DIAGNOSTIC print out buffer
  size_t i;
  for (i = 0; i < count; i++)
  {
    putchar(buffer[i]);
  }

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
