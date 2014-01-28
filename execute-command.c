/*-----------------------------------------------------------------------------
	UCLA CS 111 Lab 1. "Time Travel Shell"
	Professor Peter Reiher
	Winter 2014

	FILE:   execute-command.c
	DESCR:  Executes commands from shell script.

	AUTHOR(s):
	Alan Kha        904030522	akhahaha@gmail.com
	Braden Anderson 203744563	bradencanderson@gmail.com

-----------------------------------------------------------------------------*/

#include "command.h"
#include "command-internals.h"

#include <error.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// Returns the status of command c.
int command_status (command_t c)
{
	return c->status;
}

// Executes command c. No idea what the time_travel arg is for.
void execute_command (command_t c, int time_travel)
{
	pid_t child;
	int status;
	
	switch (c->type)
	{
		case SIMPLE_COMMAND:
			child = fork();
			if (child == 0) // child process
			{
				// TODO: handle redirects
				
				// execute command
				execvp(c->u.word[0], c->u.word);
				error(3, 0, "Command execution error.");
			}
			else if (child > 0) // parent process
			{
				// wait for the child process
				waitpid(child, &status, 0);
				
				// set execution status
				c->status = status;
			}
			else
				error(1, 0, "Failed to create child process.");

			break;

		default:
			break;
	}

	int t_use = time_travel; // PLACEHOLDER arguments cannot be unused

	return ;
}
