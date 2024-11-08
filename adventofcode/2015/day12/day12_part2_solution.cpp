#include <iostream>

#include <boost/json/src.hpp>

#include <cassert>

using namespace std;

int64_t calcNumbersSum(boost::json::value value)
{
    switch(value.kind())
    {
        case boost::json::kind::int64:
            return value.as_int64();
        case boost::json::kind::uint64:
            return value.as_uint64();
        case boost::json::kind::array:
            {
                auto& array = value.as_array();
                int64_t sum = 0;
                for (const auto& arrayElement : array)
                {
                    sum += calcNumbersSum(arrayElement);
                }
                return sum;
            }
            break;
        case boost::json::kind::object:
            {
                auto& object = value.as_object();
                int64_t sum = 0;
                for (const auto& [key, value] : object)
                {
                    if (value == "red")
                        return 0;
                    sum += calcNumbersSum(key);
                    sum += calcNumbersSum(value);
                }
                return sum;
            }
            break;
        default:
            break;
    }
    return 0;
}

int main()
{
    string json;
    getline(cin, json);
    assert(cin);
    auto jsonRoot = boost::json::parse(json);
    std::cout << "total:  " << calcNumbersSum(jsonRoot) << std::endl;
    return 0;
}
