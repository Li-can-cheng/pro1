#include "server.h"
using namespace  std;
int main() {
    std::cout << "当前工作目录是: " << std::filesystem::current_path() << std::endl;

    Server server("0.0.0.0",8088);
    server.run();

    return 0;
}
