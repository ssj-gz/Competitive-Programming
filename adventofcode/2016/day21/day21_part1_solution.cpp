#include <iostream>
#include <regex>
#include <algorithm>

#include <cassert>

using namespace std;

int main()
{
    std::regex instructionRegex(R"(^(swap position (\d+) with position (\d+))|(swap letter (.) with letter (.))|(rotate (left|right) (\d+) step[s]?)|(rotate based on position of letter (.))|(reverse positions (\d+) through (\d+))|(move position (\d+) to position (\d+))$)");
    string instruction;
    string password = "abcdefgh"; // TODO - reinstate this!
    //string password = "abcde";
    const int passwordLen = static_cast<int>(password.size());
    while (getline(cin, instruction))
    {
        std::cout << "instruction: " << instruction << std::endl;
        std::smatch instructionMatch;
        const bool matchSuccessful = std::regex_match(instruction, instructionMatch, instructionRegex);
        assert(matchSuccessful);
        if (!instructionMatch.str(1).empty())
        {
            // Swap position.
            std::cout << "Swap position" << std::endl;
            const int position1 = std::stoi(instructionMatch[2]);
            const int position2 = std::stoi(instructionMatch[3]);
            swap(password[position1], password[position2]);
        }
        else if (!instructionMatch.str(4).empty())
        {
            // Swap letters.
            std::cout << "Swap letters: " << instructionMatch.str(4) << std::endl;
            const char letter1 = instructionMatch[5].str().front();
            std::cout << "letter1: " << letter1 << std::endl;
            auto letter1Iter = std::find(password.begin(), password.end(), letter1);
            assert(letter1Iter != password.end());

            const char letter2 = instructionMatch[6].str().front();
            auto letter2Iter = std::find(password.begin(), password.end(), letter2);
            assert(letter2Iter != password.end());

            iter_swap(letter1Iter, letter2Iter);
        }
        else if (!instructionMatch.str(7).empty())
        {
            // Rotate by some number of steps.
            std::cout << "Rotate steps" << std::endl;
            const auto directionStr = instructionMatch[8];
            const int numSteps = std::stoi(instructionMatch[9]);
            string rotated;
            if (directionStr == "left")
            {
                int pos = numSteps % passwordLen;
                while (static_cast<int>(rotated.size()) < passwordLen)
                {
                    rotated += password[pos];
                    pos = (pos + 1) % passwordLen;
                }
            }
            else if (directionStr == "right")
            {
                int pos = (passwordLen - numSteps) % passwordLen;
                assert(pos >= 0);
                while (static_cast<int>(rotated.size()) < passwordLen)
                {
                    rotated += password[pos];
                    pos = (pos + 1) % passwordLen;
                }
            }
            else
                assert(false);
            password = rotated;
        }
        else if (!instructionMatch.str(10).empty())
        {
            // Rotate based on position of letter.
            std::cout << "Rotate position of letter" << std::endl;
            const char letter = instructionMatch[11].str().front();
            const int index = std::find(password.begin(), password.end(), letter) - password.begin();
            const int numSteps = (index + 1 + (index >= 4 ? 1 : 0)) % passwordLen;
            int pos = (passwordLen - numSteps) % passwordLen;
            assert(pos >= 0);
            string rotated;
            while (static_cast<int>(rotated.size()) < passwordLen)
            {
                rotated += password[pos];
                pos = (pos + 1) % passwordLen;
            }
            password = rotated;
        }
        else if (!instructionMatch.str(12).empty())
        {
            // Reverse positions.
            std::cout << "Reverse positions" << std::endl;
            int position1 = std::stoi(instructionMatch[13]);
            int position2 = std::stoi(instructionMatch[14]);
            if (position1 > position2)
                swap(position1, position2);
            assert(position1 >= 0 && position1 < passwordLen);
            assert(position2 >= 0 && position2 < passwordLen);
            std::reverse(password.begin() + position1, password.begin() + position2 + 1);
        }
        else if (!instructionMatch.str(15).empty())
        {
            // Move position.
            std::cout << "Move positions" << std::endl;
            int position1 = std::stoi(instructionMatch[16]);
            int position2 = std::stoi(instructionMatch[17]);
            assert(position1 >= 0 && position1 < passwordLen);
            assert(position2 >= 0 && position2 < passwordLen);
            const auto letterToMove = password[position1];
            password.erase(password.begin() + position1);
            password.insert(password.begin() + position2, letterToMove); 
        }
        else
            assert(false);
        std::cout << "New password: " << password << std::endl;
    }
    return 0;
}
