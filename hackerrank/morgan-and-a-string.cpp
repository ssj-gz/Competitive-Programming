#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cassert>
using namespace std;

void swallowAs(string& result, const string& A, int& aPos, int upToPos)
{
    result += A.substr(aPos, upToPos - aPos);
    aPos = upToPos;
}
void swallowBs(string& result, const string& B, int& bPos, int upToPos)
{
    result += B.substr(bPos, upToPos - bPos);
    bPos = upToPos;
}


int main() {
    int T;
    cin >> T;
    for (int i = 0; i < T;  i++)
    {
        string A, B;    
        cin >> A >> B;

        string result;
        result.reserve(A.size() + B.size());

        int aPos = 0, bPos = 0;
        while (true)
        {
            if (aPos == A.size())
            {
                // Run out of A's - add rest of B's, and end.
                swallowBs(result, B, bPos,  B.size());
                break;
            }
            if (bPos == B.size())
            {
                // Run out of B's - add rest of A's, and end.
                swallowAs(result, A, aPos,  A.size());
                break;
            }
            if (A[aPos] < B[bPos])
            {
                result += A[aPos] ;
                aPos++;
            }
            else if (A[aPos] > B[bPos])
            {
                result += B[bPos] ;
                bPos++;
            }
            else
            {
                // Equal.  Find the position of the next characters, in A and B, that are 
                // not equal to the current one.
                int aDifferentCharPos = aPos;
                for (; aDifferentCharPos < A.size(); aDifferentCharPos++)
                {
                    if (A[aDifferentCharPos] != A[aPos])
                        break;
                }
                int bDifferentCharPos = bPos;
                for (; bDifferentCharPos < B.size(); bDifferentCharPos++)
                {
                    if (B[bDifferentCharPos] != B[bPos])
                        break;
                }
                if (aDifferentCharPos == A.size() && bDifferentCharPos == B.size())
                {
                    // All remaining chars - in both A and B - are the same.  Swallow them
                    // all, and end.
                    swallowAs(result, A, aPos, aDifferentCharPos);
                    swallowBs(result, B, bPos, bDifferentCharPos);
                    break;
                }

                if (aDifferentCharPos == A.size())
                {
                    // All remaining chars in A are the same.
                    if (B[bDifferentCharPos] > B[bPos])    
                    {
                        swallowAs(result, A, aPos, aDifferentCharPos);
                        continue; 
                    }
                    else
                    {
                        swallowBs(result, B, bPos, bDifferentCharPos);
                        continue; 
                    }
                }
                if (bDifferentCharPos == B.size())
                {
                    // All remaining chars in B are the same.
                    if (A[aDifferentCharPos] > A[aPos])    
                    {
                        swallowBs(result, B, bPos, bDifferentCharPos);
                        continue; 
                    }
                    else
                    {
                        swallowAs(result, A, aPos, aDifferentCharPos);
                        continue; 
                    }
                }
                // A and B eventually reach a char that is different to the current one(s).  Which one we choose
                // next depends on both the next different chars in A and B, and where they occur.
                if (A[aDifferentCharPos] < A[aPos] && (aDifferentCharPos - aPos < bDifferentCharPos - bPos))
                {
                    // e.g.
                    // DDDDDDCXXXXX <- A
                    // DDDDDDDXXXXX <- B
                    // Obviously, need to swallow all the D's from A first.
                    // (DDDDDDCDDDDDD < DDDDDDDDDDDDDC)
                    swallowAs(result, A, aPos, aDifferentCharPos);
                    continue; 
                }
                if (B[bDifferentCharPos] < B[bPos] && (bDifferentCharPos - bPos < aDifferentCharPos - aPos))
                {
                    // As above, but with A & B swapped!
                    swallowBs(result, B, bPos, bDifferentCharPos);
                    continue; 
                }
                if (A[aDifferentCharPos] < B[bDifferentCharPos])
                {
                    // e.g.
                    // DDDDDDEXX <- A
                    // DDDDDDDDDFXX <- B
                    // Swallow the D's from A first (note that this will force us to choose the D's from B next iteration as D < E).
                    // (DDDDDDDDDDDDDDDE < DDDDDDDDDDDDDDDF)
                    //
                    // Another example:
                    // DDDDDEXX <- A
                    // DDFXX <- A
                    // Swallow the D's from A first - again, this will force us to choose the D's from B next iteration.
                    // (DDDDDDDE < DDDDDDDF)
                    swallowAs(result, A, aPos, aDifferentCharPos);
                    continue; 
                }
                else if (A[aDifferentCharPos] > B[bDifferentCharPos])
                {
                    // As above, but with A & B swapped!
                    swallowBs(result, B, bPos, bDifferentCharPos);
                    continue;
                }
                else
                {
                    // Great - still the same e.g.
                    // DDDDDE <- A
                    // DDDE <- B
                    int aPos3 = aDifferentCharPos;
                    int bPos3 = bDifferentCharPos;

                    while (aPos3 < A.size() && bPos3 < B.size())
                    {
                        if (A[aPos3] < B[bPos3])
                        {
                            // Phew - broken stalemate!
                            swallowAs(result, A, aPos, aDifferentCharPos);
                            break;
                        }
                        else if (A[aPos3] > B[bPos3])
                        {
                            swallowBs(result, B, bPos, bDifferentCharPos);
                            break;
                        }
                        // Still the same! Keep going until we finally get a break e.g.
                        // DDDDDEEEBBBCCCZ <- A
                        // DDDEEEBBBCCCX <- B // Phew - finally, X != Z!
                        aPos3++; 
                        bPos3++; 
                        if (aPos3 == A.size())
                        {
                            // Never did break the stalemate - ran out of A's!
                            // Following B's leads to the lexographically smaller path.
                            swallowBs(result, B, bPos, bDifferentCharPos);
                            break;
                        }
                        if (bPos3 == B.size())
                        {
                            // As above, but with A & B swapped!
                            swallowAs(result, A, aPos, aDifferentCharPos);
                            break;
                        }
                    }
                    continue;


                }
            }
        }
        cout << result << endl;
    }
    return 0;
}



