Time Travel Shell
===================
	UCLA CS 111 Lab 1
	Professor Peter Reiher
	Winter 2014

	Alan Kha        904030522	akhahaha@gmail.com
	Braden Anderson 203744563	bradencanderson@gmail.com
-------------------------------------------------------------------------------
Summary
---------------
Time Travel Shell speeds up shell script executions by executing commands in 
parallel where possible.

Features
---------------
- Supports nested subshells.
	
Installation
---------------
1. Download and unpack tarball.
2. Run *make* to compile.
3. Execute using *./timetrash input.sh*. The *-p* option will print the 
	input script in tree form to standard output.

Limitations
---------------
- Input file (minus comments) is limited to INT_MAX bytes
- Multiline commands within subshells not handled properly.

Exit Codes
---------------
0. Success
1. Unimplemented function or memory allocation failure
2. Syntax error
3. Command execution error
4. Other error
