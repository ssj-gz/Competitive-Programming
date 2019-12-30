// Simon St James (ssjgz) - 2019-12-30
// 
// Solution to: https://www.codechef.com/problems/MOVIEWKN
//
//#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <algorithm>

#include <cassert>

#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!

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
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        // TODO - generate randomised test.
        //const int T = rand() % 100 + 1;
        const int T = 1;
        cout << T << endl;

        for (int t = 0; t < T; t++)
        {
        }

        return 0;
    }
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        struct Movie
        {
            int length = -1;
            int rating = -1;
            int index = -1;
        };

        const int numMovies = read<int>();

        vector<Movie> movies(numMovies);

        int index = 1;
        for (auto& movie : movies)
        {
            movie.length = read<int>();
            movie.index = index;
            index++;
        }

        for (auto& movie : movies)
        {
            movie.rating = read<int>();
        }

        const auto& bestMovie = *std::max_element(movies.begin(),
                                                    movies.end(),
            [](const auto& lhsMovie, const auto& rhsMovie)
            {
                if (lhsMovie.rating * lhsMovie.length != rhsMovie.rating * rhsMovie.length)
                {
                    return lhsMovie.rating * lhsMovie.length < rhsMovie.rating * rhsMovie.length;
                }
                if (lhsMovie.rating != rhsMovie.rating)
                {
                    return lhsMovie.rating < rhsMovie.rating;
                }
                    return lhsMovie.index > rhsMovie.index;
            });

        cout << bestMovie.index << endl;

    }

    assert(cin);
}
