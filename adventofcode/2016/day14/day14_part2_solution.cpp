#include <iostream>
#include <sstream>
#include <iomanip>
#include <map>
#include <set>

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
    string salt;
    cin >> salt;
    assert(cin);
    std::cout << "salt: " << salt << std::endl;

    int index = 0;
    int numKeysFound = 0;
    std::map<int, string> indexHashLookup;
    while (true)
    {
        std::cout << "index: " << index << std::endl;
        auto hashIndex = [&indexHashLookup, &salt](int index)
        {
            if (indexHashLookup.contains(index))
                return indexHashLookup[index];
            auto hash = md5AsHex(salt + std::to_string(index));
            for (int i = 0; i < 2016; i++)
            {
                hash = md5AsHex(hash);
            }
            indexHashLookup[index] = hash;
            return hash;
        };
        const auto hash = hashIndex(index);

        // Do we have any triples?
        char triplyRepeated = '\0';
        for (string::size_type pos = 0; pos + 3 <= hash.size(); pos++)
        {
            if (hash[pos] == hash[pos + 1] && hash[pos] == hash[pos + 2])
            {
                //std::cout << "First triple repetition in " << hash << " is " << hash[pos] << " at position: " << pos << std::endl;
                if (pos  + 3 == hash.size() || hash[pos + 3] != hash[pos])
                {
                    assert(triplyRepeated == '\0');
                    triplyRepeated = hash[pos];
                    break;
                }
            }
        }

        for (int inNext1000Index = index + 1; inNext1000Index <= index + 1000; inNext1000Index++)
        {
            if (hashIndex(inNext1000Index).find(string(5, triplyRepeated)) != string::npos)
            {
                std::cout << "woo: " << index << std::endl;
                numKeysFound++;
                if (numKeysFound == 64)
                {
                    std::cout << "result: " << index << std::endl;
                    return 0;
                }
            }
        }

        index++;
    }
    return 0;
}
