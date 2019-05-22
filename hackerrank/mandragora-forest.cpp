// Simon St James (ssjgz) - 2019-05-21
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int64_t findBestExperience(const vector<int>& originalH)
{
    const int n = originalH.size();

    int64_t bestExperience = 0;
    auto sortedH = originalH;
    sort(sortedH.begin(), sortedH.end());

    vector<int64_t> sumOfRemainingHealths(n + 1);
    int64_t sumOfLastHealths = 0;
    for (const auto h : sortedH)
    {
        sumOfLastHealths += h;
    }
    for (int numRemaining = n; numRemaining >= 0; numRemaining--)
    {
        sumOfRemainingHealths[numRemaining] = sumOfLastHealths;
        if (n - numRemaining >= 0)
            sumOfLastHealths -= sortedH[n - numRemaining];
    }

    int64_t currentHealthIfOnlyEat = 1;
    const int64_t currentExperienceIfOnlyEat = 0;
    for (int i = 0; i < n; i++)
    {
        const int64_t currentExperience = currentHealthIfOnlyEat * sumOfRemainingHealths[n - i];
        bestExperience = max(bestExperience, currentExperience);
        currentHealthIfOnlyEat++;
    }
    return bestExperience;
}

int main(int argc, char* argv[])
{
    // Very easy one, as evidenced by the ~70% success rate :)
    //
    // So: our task is to a) choose the order of Mandragoras to
    // fight and b) choose whether to Eat or Battle each of these ordered Mandragoras
    // such that we get the largest possible experience.   There are n! ways
    // to order them, and for each order, 2**n ways to choose whether to
    // Eat or Battle each Mandragora.  Could there be a better solution that this O(n! x 2**n) one??
    //
    // Obviously, yes ;) It seems intuitively obvious that we might want to Eat the lowest-health
    // Mandragoras until we built up enough health to maximise the score from Battling the
    // highest-health ones, and this is indeed the case, as we will see.
    //
    // Observation: given an ordered list of k Mandragoras (H(j_1), H(j_2), ... , H(j_k)), the maximum
    // *gain* in experience from Eating/ Battling each in turn does not depend on the current
    // experience, though it *does* depend on our current health; thus if we currently have s health
    // it makes sense to define:
    //
    //   maxExpIncrease(s, (H(j_1), H(j_2), ... , H(j_k)))
    //
    // as the maximum increase in experience we can gain if our current health is s and we must
    // Eat/ Battle, in order, the Mandragoras with healths. H(j_1), H(j_2), ... , H(j_k).  To 
    // solve this challenge, we must compute:
    //
    //   initialExperience + maxExpIncrease(initialHealth, (H(i_1), H(i_2), ... , H(i_n)))
    //
    //   = maxExpIncrease(0, (H(i_1), H(i_2), ... , H(i_n)))
    //
    // across all permutations i_1, i_2, ... , i_n of 1, 2, ... , n.
    //
    // Theorem 1
    //
    // In a strategy that gives the largest experience gain, we may assume that the permutation 
    // i_1, i_2, ... , i_n of 1, 2, ... , n is the one that places the healths of the Mandragoras
    // we face into increase order i.e. H(i_1) <= H(i_2) <= ... <= H(i_n).
    //
    // Proof
    //
    // Pick any optimal strategy, and let i_1, i_2, ... , i_n of 1, 2, ... , n be the order of
    // Mandragora's chosen for this strategy.  If this sorts H, then we're done; otherwise, pick
    // the smallest j such that H(i_(j+1)) < H(i_j).
    //
    //   H(i_1) H(i_2) ... H(i_j) H(i_(j+1)) H(i_(j+2)) ... H(i_n)
    //
    // Let h be the current health and e the current experience at the moment after we dealt with 
    // the (j-1)th Mandragora.
    //
    // There are four possible scenarios for this strategy:
    //
    // i) We Battle H(i_j) and Battle H(i_(j+1)).
    //
    // The total experience for this strategy would then be:
    //
    //   e + h * H(i_j) + h * H(i_(j+1)) + maxExpIncrease(h, (H(i_(j+2)), H(i_(j+3)), ... , H(i_n))) (O1)
    //
    // Imagine if we changed this strategy so that H(i_j) and H(i_(j+1)) were swapped, but we still
    // Battled both H(i_j) and H(i_(j + 1)).
    //
    //   H(i_1) H(i_2) ... H(i_(j+1)) H(i_j) H(i_(j+2)) ... H(i_n)
    //                     ^Battle    ^Battle
    // 
    // The total experience from this modified strategy is then:
    //
    //   e + h * H(i_j) + h * H(i_(j+1)) + maxExpIncrease(h, (H(i_(j+2)), H(i_(j+3)), ... , H(i_n))) 
    //
    // i.e. exactly the same as that of the original strategy, (O1).  Thus, in this case, we can make the order of
    // Mandragora's "more ordered" with respect to health, but not lose any experience points from
    // doing so.
    //
    // ii) We Battle H(i_j), and Eat (H_(j+1)).
    //
    // The total experience for this strategy would then be:
    //
    //   e + h * H(i_j) + 0 + maxExpIncrease(h + 1, (H(i_(j+2)), H(i_(j+3)), ... , H(i_n))) (O2)
    //
    // Imagine if we changed this strategy so that H(i_j) and H(i_(j+1)) were swapped, but we still
    // Battle H(i_j) and Eat H(i_(j + 1)).
    //
    //   H(i_1) H(i_2) ... H(i_(j+1)) H(i_j) H(i_(j+2)) ... H(i_n)
    //                     ^Eat       ^Battle
    //
    // The total experience for this modified strategy would then be:
    //
    //   e + (h + 1) * H(i_j) + 0 + maxExpIncrease(h + 1, (H(i_(j+2)), H(i_(j+3)), ... , H(i_n))) 
    //
    // Subtracting (O2) from this gives:
    //  
    //   H(i_j)
    //
    // and since H(i_j) >= 1, this is strictly better than (O2).  Thus, in this case, we can make the order of
    // Mandragora's "more ordered" with respect to health, and actually get a better total experience from
    // doing so.
    //
    // iii) We Eat H(i_j) and Battle H(i_(j+1))
    //
    // The total experience for this strategy would then be:
    //
    //   e + 0 + (h + 1) * H(i_(j+1)) + maxExpIncrease(h + 1, (H(i_(j+2)), H(i_(j+3)), ... , H(i_n))) (O3)
    //
    // Imagine if we changed this strategy so that H(i_j) and H(i_(j+1)) were swapped, but we now
    // Battle H(i_j) and Eat H(i_(j + 1)).
    //
    //   H(i_1) H(i_2) ... H(i_(j+1)) H(i_j) H(i_(j+2)) ... H(i_n)
    //                     ^Eat       ^Battle
    //
    // The total experience for this modified strategy would then be:
    //
    //   e + (h + 1) * H(i_j) + 0 + maxExpIncrease(h + 1, (H(i_(j+2)), H(i_(j+3)), ... , H(i_n))) 
    //
    // Subtracting (O3) from this gives:
    //
    //   (h + 1) (H(i_j)) - H_(i_(j+1))
    //
    // and since H_(i_(j+1)) < H(i_j) by assumption, this is positive i.e. again in this case, we can make the order of
    // Mandragora's "more ordered" with respect to health, and actually get a better total experience from
    // doing so.
    //
    // iv) We Eat H(i_j) and Eat H(i_(j+1))
    //
    // The total experience for this strategy would then be:
    //
    //   e + 0 + 0 + maxExpIncrease(h + 2, (H(i_(j+2)), H(i_(j+3)), ... , H(i_n))) (O4)
    //
    // Imagine if we changed this strategy so that H(i_j) and H(i_(j+1)) were swapped, but we still
    // Eat H(i_j) and Eat H(i_(j + 1)).
    //
    // The total experience for this modified strategy would then be:
    //
    //   e + 0 + 0 + maxExpIncrease(h + 2, (H(i_(j+2)), H(i_(j+3)), ... , H(i_n))) 
    // 
    // i.e. identical to (O4).
    //
    // Thus, in all four cases, we can increase the index of the first disordered pair of healths and end
    // up with a total experience at least as good as the original; thus, by repeated application,
    // any optimal strategy that does not sort the Mandragora's in order of health can be transformed into
    // a strategy at least as good that does sort the Mandragora's in order of health.
    //
    // QED
    //
    // Theorem 2
    //
    // For any optimal strategy, we can assume that the Mandragora's are ordered in order of increasing health,
    // and that we initially exclusively Eat Mandragora's until a certain point, after which we exclusively
    // Battle the remaining Mandragora's.
    //
    // Proof
    //
    // We can assume that such a strategy has the Mandragora's ordered so that H(i_1) <= H(i_2) <= ... <= H(i_n)
    // by Theorem 1.
    //
    // If a chosen optimal strategy is not of the form "initially exclusively Eat Mandragora's until a certain point, 
    // after which we exclusively Battle the remaining Mandragora's",  then there is some lowest j such that
    // the strategy Battles H(i_j), but then Eats H(i_(j+1)).
    //
    // Again, let h and e be the health and experience after dealing with the (j-1)th Mandragora.  The total experience
    // gained from our chosen strategy is then:
    //
    //    e + h * H(i_j) + 0 + maxExpIncrease(h + 1, (H(i_(j+2)), H(i_(j+3)), ... , H(i_n))) (OBE)
    //
    // Imagine if we instead Eat H(i_j) and then Battle H(i_(j+1)).  Then the total experience from this modified strategy
    // is:
    //
    //    e + 0 + (h + 1) * H(i_(j+1)) + maxExpIncrease(h + 1, (H(i_(j+2)), H(i_(j+3)), ... , H(i_n)))
    //
    // Subtracting (OBE) from this gives:
    //
    //    (h + 1) * H(i_(j+1)) - h * H(i_j) >= (h + 1) * H(i_j) - h * H(i_j) (as H's are in increasing order)
    //                                      =   H(i_j)
    //                                      >=  1 (as all H's are >= 1)
    //                                      > 0
    //
    // i.e. our modified strategy is actually better than the original one, and is closer to the "exclusively Eat then
    // exclusively Battle".  Repeated application of this shows that we can turn any strategy into one at least
    // as good that exclusively Eats then exclusively Battles.    
    //
    // QED.
    //
    // So, almost there: we should sort our H's, then for each k = 0, 1, 2, ... n, compute:
    //
    //    experience from strategy that Eats first k, then Battles remaining.
    //
    // For any k, this is easily computed: after eating the first k, our health is 1 + k and our experience still 0  
    // The remaining (n - k) each give current health * Health of remaining Mandragora == (1 + k) * (sum of remaining
    // (n - k) healths).  With a simple lookup table (sumOfRemainingHealths), we can easy compute this for all
    // k and compute the best resulting experience in O(n).  Easy-peasy :)

    int T;
    cin >> T;

    for (int t = 0; t < T; t++)
    {
        int n;
        cin >> n;

        vector<int> H(n);
        for (int i = 0; i < n; i++)
        {
            cin >> H[i];
        }

        cout << findBestExperience(H) << endl;
    }
}
