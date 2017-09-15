// Simon St James (ssjgz) - 2017-09-15 18:53
#include <iostream>
#include <vector>

using namespace std;

const int numRows = 10;
const int numCols = 10;

enum Direction { LeftToRight, TopToBottom };

bool canFitWord(const vector<string>& crossword, const string& word, int startRow, int startCol, Direction direction)
{
    if (direction == LeftToRight)
    {
        if (startCol + word.length() > numCols)
            return false;
    }
    else
    {
        if (startRow + word.length() > numRows)
            return false;
    }
    for (int letterIndex = 0; letterIndex < word.size(); letterIndex++)
    {
        const auto letterInCrossword =  direction == LeftToRight ? crossword[startRow][startCol + letterIndex]
                                                                 : crossword[startRow + letterIndex][startCol];
        if (letterInCrossword != '-' && letterInCrossword != word[letterIndex])
        {
            return false;
        }
    }
    return true;
}

vector<string> addWord(const vector<string>& crossword, const string& word, int startRow, int startCol, Direction direction)
{
    auto newCrossword = crossword;
    for (int letterIndex = 0; letterIndex < word.size(); letterIndex++)
    {
        auto& letterInCrossword =  direction == LeftToRight ? newCrossword[startRow][startCol + letterIndex]
                                                                 : newCrossword[startRow + letterIndex][startCol];
        letterInCrossword = word[letterIndex];
    }
    return newCrossword;
}

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
    const string word = *nextWordIter;
    for (int startRow = 0; startRow < numRows; startRow++)
    {
        for (int startCol = 0; startCol < numCols; startCol++)
        {
            if (canFitWord(crossword, word, startRow, startCol, LeftToRight))
            {
                const auto newCrossword = addWord(crossword, word, startRow, startCol, LeftToRight);
                printCrosswordSolution(newCrossword, words, nextWordIter + 1);
            }
            if (canFitWord(crossword, word, startRow, startCol, TopToBottom))
            {
                const auto newCrossword = addWord(crossword, word, startRow, startCol, TopToBottom);
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
