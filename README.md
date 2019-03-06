# C Shell
This is a C shell operating in linux bash, contains the following functions:
1. Outer command such as ls, firefox, etc.
2. Directory manage functiions such as cd, dirs, pishd, popd
3. history
4. !string, !!
5. wildcard
6. prompt
7. alias, unalias

# Proram Operating Flow
1. Print the prompt
2. Get the command line
3. Store the line in history
4. Parse the line
5. Store the parsed line in args array
6. (args[0]: command, args[1],args[2]….: other arguments)
7. Check if args contain alias command
8. Check if args contain wildcard command
9. Check if args contain “ ! ”
10. Make a child process
11. Execute args 
12. Go back to 1.


********** For more information, please refer to the pdf file ***************************
