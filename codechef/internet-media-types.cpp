// Simon St James (ssjgz) - 2019-12-26
// 
// Solution to: https://www.codechef.com/problems/MIME2
//
#include <iostream>
#include <vector>
#include <map>

#include <cassert>

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);

    const int N = read<int>();
    const int Q = read<int>();

    map<string, string> mediaTypeForExtension;
    for (int i = 0; i < N; i++)
    {
        const auto extension = read<string>();
        const auto mediaType = read<string>();
        mediaTypeForExtension[extension] = mediaType;
    }

    for (int i = 0; i < Q; i++)
    {
        const auto filename = read<string>();
        const auto lastDotPos = filename.rfind('.');
        string mediaType;
        if (lastDotPos != string::npos)
        {
            const auto extension = filename.substr(lastDotPos + 1);
            mediaType = mediaTypeForExtension[extension];
        }
        if (mediaType.empty())
            mediaType = "unknown";
        cout << mediaType << endl;
    }

    assert(cin);
}
