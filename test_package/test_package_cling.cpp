#ifdef __GLIBCXX__
  #error \"Using libstdc++\"
#else
  #warning \"Using libc++\"
#endif

#include <iostream>
#include <string>

int main(int argc, char** argv) {
  const std::string str = "Hello world!";
  std::cout << str << std::endl;
  return 0;
}
