#include "../../json/json.hpp"
#include <string>
#include <iostream>
#include <cstring>

int main(int argc, char const *argv[])
{
    using json = nlohmann::json;
    json data = json::parse(R"({"happy": true, "pi":3.141})");
    std::string s = data.dump();
    std::cout << s << std::endl;
    const char *data_chr = s.c_str();
    std::cout << strlen(data_chr) << std::endl;
    return 0;
}
