// Simon St James (ssjgz) 2017-10-02
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main()
{
    // Fairly easy one, though the proof of why it works is fairly involved.
    // First, sort the numCandiesInPacket; then essentially it can be proven that 
    // given any distribution with minimum unfairness, if the distribution doesn't consist
    // of consecutive elements over the (sorted) numCandiesInPacket, then it can be
    // transformed into a distribution, also optimal, which does consist of consecutive
    // elements.  
    //
    // Let D = {p1, p2, ... pk} be a distribution with pi+1 > pi for i = 1 ... k-1. 
    // Let C = numCandiesInPacket (less typing :)). Then the unfairness of D, Unf(D) = 
    //
    //  sum [1 <= i < j <= k] {|C(pi) - C(pj)|}
    //   = sum [1 <= i < j <= k] {C(pj) - C(pi)}
    //
    // since pj > pi and C is in non-decreasing order.
    //
    // Lemma
    //
    // Given a distribution D = {p1, p2, ... pk}, if k > 2, then
    //  
    //  Unf(D) = (k - 1)(C(pk) - C(p1)) + Unf({p2, .... , pk-1 }).
    //
    // Proof.
    //
    //  Unf(D) = 
    //   sum [1 <= i < j <= k] {C(pj) - C(pi)} =
    //   sum [ 1 <= i < k] {C(pK) - C(pi)} + sum [ 1 <= i < j <= k-1 ] {C(pi)} =
    //   sum [ 1 <= i < k] {C(pK) - C(pi)} + sum [ 2 <= i < j <= k-1 ] {C(pi)} + sum [ 1 < i < k] {C(pi) - C(p1)} = 
    //   sum [ 1 <= i < k] {C(pK) - C(pi)} + Unf({p2, .... pk-1}) + sum [ 1 < i < k] {C(pi) - C(p1)} = 
    //   sum [ 1 < i < k] {C(pK) - C(pi)} + (C(pk) - c(p1)) + Unf({p2, .... pk-1}) + sum [ 1 < i < k] {C(pi) - C(p1)} = 
    //   (K - 2)C(pk) - sum [1 < i < k] {c(pi)} + (C(pk) - c(p1)) + Unf({p2, .... pk-1}) + sum [ 1 < i < k] {C(pi) - C(p1)} = 
    //   (K - 2)C(pk) - sum [1 < i < k] {c(pi)} + (C(pk) - c(p1)) + Unf({p2, .... pk-1}) + (k - 2)C(p1) + sum [ 1 < i < k] {C(pi)} = 
    //   (K - 1)C(pk) - sum [1 < i < k] {c(pi)} - C(p1) + Unf({p2, .... pk-1}) + (k - 2)C(p1) + sum [ 1 < i < k] {C(pi)} = 
    //   (K - 1)C(pk) - C(p1) + Unf({p2, .... pk-1}) + (k - 2)C(p1) = 
    //   (K - 1)(C(pk) - C(p1)) + Unf({p2, .... pk-1}).
    //
    // Corollary
    //   If D = {p1, p2, ... , pk}, then given p'1 = p2 - 1 and p'k = pk-1 + 1,  Unf({p'1, p2, p3, ... ,pk-1, pk-1 + 1}) <= Unf(D).
    //
    // Proof
    //
    // Immediate from the lemma; if p1 != p2 - 1, then p'1 > p1, and as we see from the lemma, increasing p1 cannot increase Unf(D).  Likewise,
    // if pk != pk - 1, then p'k < pk, and again from the lemma, decreasing pk cannot decrease Unf(D).
    //
    // Thus, given a distribution D, we can find one with unfairness <= Unf(D) and where the first two elements are consecutive, and the last two 
    // elements are consecutive.
    // 
    // Theorem
    //
    // If D = {p1, p2, ... , pk}, then there is a distribution D' = {p'1, p'2, ... p'k} with p'1 >= p1, p'k <= pk, and p'i+1 = pi + 1 for i = 1...k-1 and Unf(D') <= Unf(D).
    //
    // Proof.
    //
    // Proof is by induction on K.  From lemma,
    //
    //  Unf(D) = (k - 1)(C(pk) - C(p1)) + Unf({p2, p3, ... , pk-1}).
    //
    // By induction hypothesis, there are p'i with p'2 >= p2 and p'k-1 <= pk and p'i+1 = p'i + 1 for i = 2, ... k - 2 with Unf({p'2, ... p'k-1}) <= Unf(p2, ... pk-1).
    //
    // Thus, (k - 1)(C(pk) - C(p1)) + Unf{p'2, ... p'k-1} <= Unf(D).
    //
    // From the corrolary, setting p'1 = p'2 - 1 and p'k = p'k-1 + 1 cannot increase this expression i.e.

    // (k - 1)(C(p'k) - C(p'1)) + Unf{p'2, ... p'k-1} <= (k - 1)(C(pk) - C(p1)) + Unf{p'2, ... p'k-1} <= Unf(D).
    //
    // i.e. Unf({p'1, p'2 , ... p'k} < = Unf(D).
    //
    // Since p'1 >= p1 and p'k <= pk, we have our result.
    //
    // Thus, when trying to find the mininum unfairness, we need only consider consecutive elements.  Let D = {p1, p2, ... pk} with pi+1 = pi + 1 i = 1, ... k-1.
    // We can abbreviate this to D(l) i.e. D(l) = {l, l + 1, ... l + k - 1}. Let total(D(l)) = sum [ l <= i <= l + k - 1] {C(i)}
    //
    // Now, if we knew Unf(D(l)), how could we efficiently compute Unf(D(l+1))? (D(l + 1) = {l + 1, l + 2, ... l + k}).
    //
    //   Unf(D(l + 1)) = sum [ l + 1 <= i < j <= l + k ] {C(j) - C(i)}
    //  = sum [l + 1 <= i < j <= l + k - 1] {C(j) - C(i)} + sum [ l + 1 <= i <= l + k - 1] {C(l + k) - C(i)}
    //  = sum [l <= i < j <= l + k - 1] {C(j) - C(i)} - sum [ l + 1 <= i <= l + k - 1 ] {c(i) - c(l)}  + sum [ l + 1 <= i <= l + k - 1] {C(l + k) - C(i)}
    //  = Unf(D(l) - sum [ l + 1 <= i <= l + k - 1 ] {c(i) - c(l)}  + sum [ l + 1 <= i <= l + k - 1] {C(l + k) - C(i)}
    //  = Unf(D(l) + (k - 1)*c(l) - sum [ l + 1 <= i <= l + k - 1 ] {c(i)}  + sum [ l + 1 <= i <= l + k - 1] {C(l + k) - C(i)}
    //  = Unf(D(l) + (k - 1)*c(l) - sum [ l + 1 <= i <= l + k - 1 ] {c(i)}  + (k - 1) * C(l + k) - sum [ l + 1 <= i <= l + k - 1] {C(i)}
    //  = Unf(D(l) + (k - 1)*C(l) + (k - 1) * C(l + k) - 2 * sum [ l + 1 <= i <= l + k - 1] {C(i)}
    //  = Unf(D(l) + (k - 1)*C(l) + (k - 1) * C(l + k) - 2 * (sum [ l <= i <= l + k - 1] {C(i)} - C(l))
    //  = Unf(D(l) + (k - 1)*C(l) + (k - 1) * C(l + k) - 2 * (total(D(l) - C(l))
    //  = Unf(D(l) + (k - 1)*C(l) + 2 * C(l) - (k - 1) * C(l + k) - 2 * total(D(l))
    //  = Unf(D(l) + (k + 1)*C(l) + (k - 1) * C(l + k) - 2 * total(D(l))
    //
    // So, if numInPacketLost = C(l) and numInPacketGained = C(l + k) when transitioning from D(l) to D(l + 1), then:
    //
    //  Unf(D(l + 1) = Unf(D(l)) + (k + 1) * numInPacketLost + (k - 1) * numInPacketGained - 2 * total(D(l))
    //
    // and that's about it: the minimum unfairness over all possible distributions is the min [l = 1 ... n - k + 1] {Unf(D(l))}.
    int N, K;
    cin >> N >> K;

    vector<int64_t> numCandiesInPacket(N);

    for (int i = 0; i < N; i++)
    {
        cin >> numCandiesInPacket[i];
    }

    sort(numCandiesInPacket.begin(), numCandiesInPacket.end());

    // Calculate totalKSum/ totalUnfairness for initial (leftmost) block of K.
    int64_t totalKSum = 0;
    int64_t totalUnfairness = 0;
    for (int i = 0; i < K; i++)
    {
        totalUnfairness += min(i, K) * numCandiesInPacket[i] - totalKSum;
        totalKSum += numCandiesInPacket[i];
    }
    auto minUnfairness = totalUnfairness;

    // Calculate unfairness for remaining blocks of K.
    for (int i = K; i < N; i++)
    {
        const int64_t numInPacketLost = numCandiesInPacket[i - K];
        const int64_t numInPacketGained = numCandiesInPacket[i];
        const int64_t changeInUnfairness = (K + 1) * numInPacketLost + (K - 1) * numInPacketGained - 2 * totalKSum;
        totalUnfairness = totalUnfairness + changeInUnfairness;
        totalKSum -= numInPacketLost;
        totalKSum += numInPacketGained;

        minUnfairness = min(minUnfairness, totalUnfairness);
    }
    cout << minUnfairness << endl;
}
