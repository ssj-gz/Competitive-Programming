// Simon St James (ssjgz) - 2019-12-18
// 
// Solution to: https://www.codechef.com/problems/BOOKCHEF
//
#include <iostream>
#include <vector>
#include <set>
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
    // Easy one - just do what the question says!
    // This version should work even if Posts can have the 
    // same popularity (the question itself guarantees the
    // easier situation where they do not).
    ios::sync_with_stdio(false);

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
