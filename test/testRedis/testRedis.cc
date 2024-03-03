#include "sw/redis++/redis++.h"
#include <iostream>

using namespace std;

int main(int argc, char const *argv[])
{
    using namespace sw::redis;

    try
    {
        {
            auto redis = Redis("tcp://127.00.1:6379");

            // std::vector<std::string> to Redis LIST.
            std::vector<std::string> vec = {"a", "b", "c"};
            redis.rpush("list", vec.begin(), vec.end());
            vec.clear();
        }

        {
            auto redis = Redis("tcp://127.00.1:6379");
            vector<std::string> vec;
            redis.lrange("list", 0, -1, std::back_inserter(vec));

            for (auto &str : vec)
            {
                std::cout << str << std::endl;
            }
        }
    }
    catch (const Error &e)
    {
        // Error handling.
    }
    return 0;
}
