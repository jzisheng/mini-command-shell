# Mini command shell 🖥
This project is a simplified version of a command shell (ffosh--for Five Fifty One SHell).
This program is written in C++ for this program, demonstrating command of object orientation.

--- 
# Capabilities
* Read a command name and run it(from path or local directory) with arguments
Wrote a program called "ffosh." Whenever the command shell expects input, it prints a prompt ("ffosh$") and can execute it.
```
  ffosh:/home/zjc4/ece551/mp_miniproject/commandShell$ /bin/ls -l
  total 392
  -rw-rw-r-- 1 zjc4 zjc4    128 Nov 25 13:31 Makefile
  -rw-rw-r-- 1 zjc4 zjc4   2073 Dec  7 21:40 TESTING.txt
```

* Directories and variables
Uses "cd" command to change the current directory by utilizing the chdir function. The prompt shows the current directory before the $.
```
  ffosh:/home/zjc4/ece551/mp_miniproject/commandShell$ set x "foo"
  ffosh:/home/zjc4/ece551/mp_miniproject/commandShell$ export x
  ffosh:/home/zjc4/ece551/mp_miniproject/commandShell$ env
  x=foo
```


# Running
The program is already compiled. Simply running the following command will start the program  
```
./ffosh
```

Test cases are included in the local directory `test/` folder
