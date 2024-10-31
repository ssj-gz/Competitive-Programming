#include <iostream>
#include <vector>
#include <algorithm>

#include <cassert>

using namespace std;

int main()
{
    int stepsToMove = -1;
    cin >> stepsToMove;

    vector<int> buffer;
    buffer.push_back(0);

    int currentPosition = 0;
    for (int count = 1; count <= 2'017; count++)
    {
        currentPosition = (currentPosition + stepsToMove) % static_cast<int>(buffer.size());
        buffer.insert(buffer.begin() + currentPosition + 1, count);
        currentPosition = (currentPosition + 1) % static_cast<int>(buffer.size());

        std::cout << "Inserted " << count << " - new buffer: " << std::endl;
        for (int i = 0; i < buffer.size(); i++)
        {
            if (i == currentPosition)
                std::cout << "(";
            cout << buffer[i];
            if (i == currentPosition)
                std::cout << ")";
            std::cout << " ";
        }
        std::cout << std::endl;
    }
    auto after2017Iter = std::find(buffer.begin(), buffer.end(), 2'017);
    after2017Iter++;
    if (after2017Iter == buffer.end())
        after2017Iter = buffer.begin();

    std::cout << "result: " << *after2017Iter << std::endl;
    return 0;
}
