// Simon St James (ssjgz) - 2019-12-30
// 
// Solution to: https://www.codechef.com/problems/MOVIEWKN
//
#include <iostream>
#include <vector>
#include <algorithm>

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

        int movieIndex = 1;
        for (auto& movie : movies)
        {
            movie.length = read<int>();
            movie.index = movieIndex;
            movieIndex++;
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
                // Prefer *minimal* index, hence the ">".
                return lhsMovie.index > rhsMovie.index;
            });

        cout << bestMovie.index << endl;

    }

    assert(cin);
}
