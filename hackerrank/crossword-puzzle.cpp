// Simon St James (ssjgz) - 2017-09-15 18:53
#include <iostream>
#include <vector>

using namespace std;

const int numRows = 10;
const int numCols = 10;

void printCrosswordSolution(const vector<string>& crossword, const vector<string>& words, vector<string>::const_iterator nextWordIter)
{
    if (nextWordIter == words.cend())
    {
        for (const auto& row : crossword)
        {
            cout << row << endl;
        }
        return;
    }
    const string nextWord = *nextWordIter;
    // Left-to-right.
    for (int row = 0; row < numRows; row++)
    {
        for (int startCol = 0; startCol < numCols - nextWord.size() + 1; startCol++)
        {
            bool wordFitsHere = true;
            for (int letterIndex = 0; letterIndex < nextWord.size(); letterIndex++)
            {
                const auto letterInCrossword = crossword[row][startCol + letterIndex];
                if (letterInCrossword != '-' && letterInCrossword != nextWord[letterIndex])
                {
                    wordFitsHere = false;
                    break;
                }
            }
            if (wordFitsHere)
            {
                // Found a place.
                vector<string> newCrossword = crossword;
                for (int letterIndex = 0; letterIndex < nextWord.size(); letterIndex++)
                {
                    newCrossword[row][startCol + letterIndex] = nextWord[letterIndex];
                }
                printCrosswordSolution(newCrossword, words, nextWordIter + 1);
            }
        }
    }
    // Top-to-bottom.
    for (int col = 0; col < numCols; col++)
    {
        for (int startRow = 0; startRow < numRows - nextWord.length() + 1; startRow++)
        {
            bool wordFitsHere = true;
            for (int letterIndex = 0; letterIndex < nextWord.size(); letterIndex++)
            {
                const auto letterInCrossword = crossword[startRow + letterIndex][col];
                if (letterInCrossword != '-' && letterInCrossword != nextWord[letterIndex])
                {
                    wordFitsHere = false;
                    break;
                }
            }
            if (wordFitsHere)
            {
                // Found a place.
                vector<string> newCrossword = crossword;
                for (int letterIndex = 0; letterIndex < nextWord.size(); letterIndex++)
                {
                    newCrossword[startRow + letterIndex][col] = nextWord[letterIndex];
                }
                printCrosswordSolution(newCrossword, words, nextWordIter + 1);
            }
        }
    }
}

int main()
{
    vector<string> crossword(numRows, string(numCols, '\0'));

    for (int row = 0; row < numRows; row++)
    {
        cin >> crossword[row];
    }

    string stringList;
    cin >> stringList;

    vector<string> words;
    string::size_type wordBeginPos = 0;
    while (stringList.find(';', wordBeginPos) != string::npos)
    {
        const string::size_type wordEndPos = stringList.find(';', wordBeginPos) - 1;
        const string word = stringList.substr(wordBeginPos, wordEndPos - wordBeginPos + 1);
        words.push_back(word);
        wordBeginPos = wordEndPos + 2;
    }
    words.push_back(stringList.substr(wordBeginPos));

    printCrosswordSolution(crossword, words, words.cbegin());
}
