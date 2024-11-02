#include <iostream>
#include <sstream>
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
    string doorId;
    cin >> doorId;

    int index = 0;
    string password(8, '-');
    int numCharactersRemaining = password.size();
    while (numCharactersRemaining > 0)
    {
        const auto md5 = md5AsHex(doorId + std::to_string(index));
        if (md5.substr(0, 5) == "00000")
        {
            const char offsetChar = md5[5];
            if (offsetChar >= '0' && offsetChar <= '8')
            {
                const int offset = offsetChar - '0';
                if (password[offset] == '-')
                {
                    password[offset] =  md5[6];
                    numCharactersRemaining--;
                }

            }
        }

        index++;
    }
    std::cout << "password: " << password << std::endl;

    return 0;
}
