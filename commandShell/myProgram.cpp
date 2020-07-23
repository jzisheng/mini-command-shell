#include <algorithm>
#include <iostream>
#include <iterator>

int main(int argc, char * argv[]) {
  if (argc > 1) {
    std::cout << "there are " << argc << " arguments\n";
    std::copy(
        argv + 1, argv + argc, std::ostream_iterator<const char *>(std::cout, "\n"));
  }
}
