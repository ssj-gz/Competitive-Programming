#include <iostream>
#include <iomanip>

#include <openssl/md5.h>

#include <cassert>

using namespace std;

string md5AsHex(const std::string& input)
{
    unsigned char result[MD5_DIGEST_LENGTH];
    MD5((unsigned char*) input.data(), input.size(), result);
    ostringstream md5HexOut;
    for (auto byte : result)
    {
        md5HexOut << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(byte);
    }
    return md5HexOut.str();
}

int main()
{
    string secretkey;
    cin >> secretkey;
    assert(cin);

    int number = 1;
    while (md5AsHex(secretkey + std::to_string(number)).find("000000") != 0)
    {
        number++;
    }
    std::cout << "result: " << number << std::endl;
    return 0;
}
