#Rshell
##The mandatory brainchild of all UCR cs-100 students.

###What is rshell? 
Rshell is a simple shell for unix systems that students in the UCR CS100 course are required to create.
It is essentially an unfinished, more limited version of Bash.

###Current Features
* Executes commands (e.g. ls, mkdir, etc.)
* Supports comments (commands entered in as a comment will not be executed)
* handles "exit".
* can also be terminated using ctrl+d.
* prints a "Goodbye" message when terminated.
* A syntax error message will be printed when one is detected.

###Known bugs
(that need to, and will, be fixed)
* Any command entered that begins with a semicolon or forward slash causes an error. (e.g. ;ls)
* Unable to run rshell within rshell; (bin/rshell gives a permission denied message)
* Some memory leaks occur. 


