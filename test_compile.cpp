#include <iostream>
#include <cstdlib>

int main() {
    int res = system("C:\\Users\\lucas\\AppData\\Local\\Programs\\CLion\\bin\\mingw\\bin\\g++.exe @cmake-build-release/CMakeFiles/CC_Rogue_Like_Game.dir/includes_CXX.rsp -O3 -DNDEBUG -std=gnu++20 -c src/main.cpp > out.txt 2>&1");
    std::cout << "Result: " << res << "\n";
    return 0;
}
