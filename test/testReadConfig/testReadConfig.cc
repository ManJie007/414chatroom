#include "../../base/ConfigFileReader.h"
#include <iostream>

int main(int argc, char const *argv[])
{
    CConfigFileReader config("../../etc/chatserver.conf");

    auto config_map = config.getConfigMap();
    for (auto iter = config_map.cbegin(); iter != config_map.cend(); iter++)
    {
        std::cout << iter->first << " " << iter->second << std::endl;
    }
    
    return 0;
}
