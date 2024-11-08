#include <iostream>

#include <cassert>

using namespace std;

int main()
{
    string quotedString;
    int totalEncodedChars = 0;
    int totalOriginalChars = 0;
    while (std::getline(cin, quotedString))
    {
        std::cout << "quotedString: " << quotedString << std::endl;
        string encoded = "\"";
        for (string::size_type pos = 0; pos < quotedString.size(); pos++)
        {
            const auto character = quotedString[pos];
            if (character == '"')
            {
                encoded += "\\\"";
            }
            else if (character == '\\')
            {
                encoded += "\\\\";
            }
            else
                encoded += character;
        }
        encoded += '"';
        std::cout << "encoded: " << encoded << std::endl;
        totalEncodedChars += encoded.size();
        totalOriginalChars += quotedString.size();
    };
    std::cout << "totalOriginalChars: " << totalOriginalChars << " totalEncodedChars: " << totalEncodedChars << std::endl;
    std::cout << "result: " << (totalEncodedChars - totalOriginalChars) << std::endl;
    return 0;
}
