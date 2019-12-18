// Simon St James (ssjgz) - 2019-12-18
// 
// Solution to: https://www.codechef.com/problems/BOOKCHEF
//
//#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <set>
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

        const int numSpecialFriends = 1 + rand() % 100;
        const int numPosts = 1 + rand() % 100;

        const int maxPopularity = 1 + rand() % 100;
        const int maxSpecialFriendId = numSpecialFriends + rand() % 100;
        const int maxPostFriendId = 1 + rand() % 100;

        cout << numSpecialFriends << " " << numPosts << endl;
        vector<int> specialFriendIds;
        for (int i = 1; i <= maxSpecialFriendId; i++)
        {
            specialFriendIds.push_back(i);
        }
        random_shuffle(specialFriendIds.begin(), specialFriendIds.end());
        for (int i = 0; i < numSpecialFriends; i++)
        {
            cout << specialFriendIds[i];
            if (i != numSpecialFriends - 1)
            {
                cout << " ";
            }
        }
        cout << endl;
        vector<int> postPopularities;
        for (int i = 1; i <= maxPopularity; i++)
        {
            postPopularities.push_back(i);
        }
        random_shuffle(postPopularities.begin(), postPopularities.end());
        for (int i = 0; i < numPosts; i++)
        {
            string s;
            const int strLen = 1 + rand() % 10;
            for (int j = 0; j < strLen; j++)
            {
                s += 'a' + rand() % 26;
            }
            cout << (1 + rand() % maxPostFriendId) << " " << postPopularities[i] << s << endl;
        }

        return 0;
    }

    struct Post
    {
        bool isSpecialFriend = false;
        int popularity = -1;
        string content;
    };
    set<int> specialFriendIds;

    const int numSpecialFriends = read<int>();
    const int numPosts = read<int>();

    for (int i = 0; i < numSpecialFriends; i++)
    {
        const int specialFriendId = read<int>();
        specialFriendIds.insert(specialFriendId);
    }

    vector<Post> posts(numPosts);
    for (auto& post : posts)
    {
        const int friendId = read<int>();
        post.isSpecialFriend = (specialFriendIds.find(friendId) != specialFriendIds.end());
        post.popularity = read<int>();
        post.content = read<string>();
    }

    sort(posts.begin(), posts.end(), [](const auto& lhsPost, const auto& rhsPost)
            {
            if (lhsPost.isSpecialFriend == rhsPost.isSpecialFriend)
            return lhsPost.popularity > rhsPost.popularity;
            return (lhsPost.isSpecialFriend);
            });

    for (const auto& post : posts)
    {
        cout << post.content << endl;
    }

    assert(cin);
}
