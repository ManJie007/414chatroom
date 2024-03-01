#include "../../json/json.hpp"
#include <string>
#include <iostream>
#include <cstring>

int main(int argc, char const *argv[])
{
    using json = nlohmann::json;
    json data = json::parse(R"({"happy": true, "pi":3.141})");
    // json data = json::parse(R"({"happy": true, "pi":3.141})");
    std::string s = data.dump();
    std::cout << s << std::endl;
    const char *data_chr = s.c_str();
    std::cout << strlen(data_chr) << std::endl;
    auto data2 = json::parse(s);
    bool b = data2["happy"];
    if (b)
    {
        std::cout << b << std::endl;
    }
    
    std::cout << data2["happy"] << std::endl;
    std::cout << data2["pi"] << std::endl;
    return 0;
}
