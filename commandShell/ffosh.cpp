#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <algorithm>
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>

#include "src/args.h"
#include "src/shell.h"

int main(void) {
  // This main function calls the shell class
  Shell shell = Shell();
  shell.runShell();
}
