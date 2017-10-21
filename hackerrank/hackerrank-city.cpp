// Simon St James (ssjgz) - 2017-10-12 11:42
#include <iostream>
#include <vector>

using namespace std;

const int64_t modulus = 1'000'000'007ULL;

class ModNum
{
    public:
        ModNum(int64_t n)
            : m_n{n}
        {
        }
        ModNum& operator+=(const ModNum& other)
        {
            m_n = (m_n + other.m_n) % ::modulus;
            return *this;
        }
        ModNum& operator*=(const ModNum& other)
        {
            m_n = (m_n * other.m_n) % ::modulus;
            return *this;
        }
        int64_t value() const { return m_n; };
    private:
        int64_t m_n;
};

ModNum operator+(const ModNum& lhs, const ModNum& rhs)
{
    ModNum result(lhs);
    result += rhs;
    return result;
}

ModNum operator*(const ModNum& lhs, const ModNum& rhs)
{
    ModNum result(lhs);
    result *= rhs;
    return result;
}

ostream& operator<<(ostream& os, const ModNum& toPrint)
{
    os << toPrint.value();
    return os;
}

int main()
{
    // Oh boy - that was embarrassing :)
    // It's fundamentally a fairly simple recurrence relation, but you have to be careful with the algebra.  Then
    // you have to be careful, if you change the modulus for debugging purposes, to set it back to the correct
    // value when you're done XD
    //
    // So: let G(N) be the graph after the nth step of the process.  It will look like this:
    //
    //  C1  tXXXX     XXXXX C2
    //      XXXXX     XXXXX
    //      XXXXX     XXXXX
    //      XXXXX     XXXXX
    //      XXXX1     2XXXX
    //          |     |
    //          A-----B
    //          |     |
    //  C3  XXXX3     4XXXX C4
    //      XXXXX     XXXXX
    //      XXXXX     XXXXX
    //      XXXXX     XXXXX
    //      XXXXX     XXXXc
    //
    // Each of the Ci is a copy of G(N-1).  The vertex c is the bottom-right vertex of component C4.  The 1, 2, 3 and 4
    // vertices are the bottom-right, bottom left, top right and top left of C1, C2, C3 and C4, respectively: they would
    // each have been copies of the "c" vertex of G(N-1).  A and B are new vertices; A is connected to B by an edge of
    // length A[N] and also to vertices 1 & 3, also by edges of length A[N]; similarly B is connected to vertices 2 and 4,
    // again by edges of length A[N].
    //
    // Let Sum(N) be the sum of edge lengths over all paths of G(N) i.e. sum [{u, v} u in G(N), v in G(N)] { len(u->v) }.  Note
    // that if we count the sum of the edge lengths in the path from u->v, we do *not* count the reverse path v->u.
    //
    // How do we calculate Sum(N)?
    //
    // There are a few choices for u and v:
    //
    //   1) u and v belong to the same component Ci.
    //
    // It's easy to see that since each Ci is a copy of G(N-1), the contribution of 1) is 4 * Sum(N - 1).
    //
    //   2) u belongs to some Ci; v = A.
    //
    // For C1, any such path can be split into two paths: first from u to 1, then from 1 to A.  The contribution is then:
    //
    //   sum [ u in C1 ] { len(u->1->A} =
    //   sum [ u in C1 ] { len(u->1) + len(1->A)} =
    //   sum [ u in C1 ] { len(u->1) }  +sum [u in C1] { len(1->A) } =
    //   sum [ u in C1 ] { len(u->1) } + sum [u in C1] { A[N] } =
    //
    // For the graph G(N), let SumToCorner(N) = sum [ u in G(N) ] { len(u->c) }.   Let NumNodes(N) be the number of vertices in G(N).
    //
    // Since C1 = G(N-1) and 1 is the c for G(N-1), this becomes
    //
    //   SumToCorner(N-1) + sum [u in C1] { A[N] } =
    //   SumToCorner(N-1) + NumNodes(N-1) * A[N]
    //
    // Similarly, for C2, the path can be decomposed as u->2, 2->B, B->A, and using identical reasoning to the above, we have:
    //  
    //   sum [ u in C2 ] { len(u->2->B->A} =
    //   sum [ u in C2 ] { len(u->2} + len(2->B) + len(B->A)} =
    //   sum [ u in C2 ] { len(u->2} + A[N] + A[N]} =
    //   sum [ u in C2 ] { len(u->2} + 2 * A[N] } =
    //   sum [ u in C2 ] { len(u->2} + sum [u in C2 ]{ 2 * A[N] } =
    //   SumToCorner(N-1) + sum [u in C2 ]{ 2 * A[N] } =
    //   SumToCorner(N-1) + 2 * NumNodes(N-1) * A[N]
    //
    // For C3, the path can be decomposed as u->3, 3->A, and we see, using reasoning identical to the C1 case that it contributes
    //
    //   SumToCorner(N-1) + NumNodes(N-1) * A[N]
    //
    // C4 is identical to C2 and contributes 
    //
    //   SumToCorner(N-1) + 2 * NumNodes(N-1) * A[N]
    //
    // The total contribution from 2) then is:
    //
    //   2 * (SumToCorner(N-1) + NumNodes(N-1) * A[N]) + 2 * (SumToCorner(N-1) + 2 * NumNodes(N-1) * A[N]) = 
    //   2 * SumToCorner(N-1) + 2 * NumNodes(N-1) * A[N]) + 2 * SumToCorner(N-1) + 4 * NumNodes(N-1) * A[N] = 
    //   4 * SumToCorner(N-1) + 6 * NumNodes(N-1) * A[N]
    //
    //  3) u belongs to Ci; v = B.
    // 
    // Using identical reasoning to 2), we see that the contribution of 3) is again
    //
    //   4 * SumToCorner(N-1) + 6 * NumNodes(N-1) * A[N]
    // 
    // so the contribution of cases 2) and 3) combined is: 
    //
    //   8 * SumToCorner(N-1) + 12 * NumNodes(N-1) * A[N] =
    //   4 * (2 * SumToCorner(N-1) + 3 * NumNodes(N-1) * A[N])
    //  
    //  4) u belongs to Ci, v belongs to Cj, j > i.
    //
    // (The condition j > i ensures that we don't count the same path twice).
    //
    // There are 6 ways of choosing i and j; let's look at i = 1, j = 2; the others are much the
    // same.
    //
    // Such a path can be decomposed as u->1, 1->A, A->B, B->2, 2->u.  The contribution is:
    //
    //   sum [ u in C1, v in C2 ] { len(u->1->A->B->2->u } =
    //   sum [ u in C1, v in C2 ] { len(u->1) + len(1->A) + len(A->B) + len(B->2) + len(2->u) } =
    //   sum [ u in C1, v in C2 ] { len(u->1) + A[N] + A[N] + A[N] + len(2->u) } =
    //   sum [ u in C1, v in C2 ] { len(u->1) +  3 * A[N] + len(2->u) } =
    //   sum [ u in C1, v in C2 ] { len(u->1) } +  sum [ u in C1, v in C2 ] {3 * A[N]} + sum [ u in C1, v in C2 ]{ len(2->u) } =
    //
    // Since there are NumNodes(N-1) nodes in C1 and in C2, this becomes:
    //
    //   NumNodes(N-1) * sum [ u in C1 ] { len(u->1) } +  NumNodes(N-1) * NumNodes(N-1) * 3 * A[N]} + NumNodes(N-1) * sum [ v in C2 ]{ len(2->u) }
    //
    // Since len(2->u) == len(u->2), this becomes:
    //
    //   NumNodes(N-1) * sum [ u in C1 ] { len(u->1) } +  NumNodes(N-1) * NumNodes(N-1) * 3 * A[N]} + NumNodes(N-1) * sum [ v in C2 ]{ len(u->2) } = 
    //   NumNodes(N-1) * SumToCorner(N-1) +  NumNodes(N-1) * NumNodes(N-1) * 3 * A[N] + NumNodes(N-1) * SumToCorner(N-1) = 
    //   2 * NumNodes(N-1) * SumToCorner(N-1) +  3 * NumNodes(N-1) * NumNodes(N-1) * A[N]
    //
    //  Similarly, for i = 1, j = 3, any path can be decomposed as u->1, 1->A, A->3, 3->V, which contributes:
    //   sum [ u in C1, v in C3 ] { len(u->1->A->2->u } =
    //   sum [ u in C1, v in C3 ] { len(u->1) + len(1->A) + len(A->3) + len(3->u) } =
    //   sum [ u in C1, v in C3 ] { len(u->1) + A[N] + A[N] + len(3->u) } =
    //   sum [ u in C1, v in C3 ] { len(u->1) +  2 * A[N] + len(2->u) } =
    //   sum [ u in C1, v in C3 ] { len(u->1) } +  sum [ u in C1, v in C3 ] {2 * A[N]} + sum [ u in C1, v in C3 ]{ len(3->u) } =
    //   NumNodes(N-1) * sum [ u in C1 ] { len(u->1) } +  NumNodes(N-1) * NumNodes(N-1) * 2 * A[N] + NumNodes(N-1) * sum [ v in C3 ]{ len(u->3) }
    //   NumNodes(N-1) * SumToCorner(N-1) +  NumNodes(N-1) * NumNodes(N-1) * 2 * A[N] + NumNodes(N-1) * SumToCorner(N-1)
    //   2 * NumNodes(N-1) * SumToCorner(N-1) +  2 * NumNodes(N-1) * NumNodes(N-1) * A[N]
    //
    // In general, we see that (i,j) in { (1,2), (1,4), (2,3), (3,4) } each contribute
    //
    //  2 * NumNodes(N-1) * SumToCorner(N-1) +  3 * NumNodes(N-1) * NumNodes(N-1) * A[N]
    //
    // and (i,j) in { (1,3) and (2,4) each contribute
    //
    //  2 * NumNodes(N-1) * SumToCorner(N-1) +  2 * NumNodes(N-1) * NumNodes(N-1) * A[N]
    //
    // for a total contribution of 
    //
    //  6 * 2 * NumNodes(N-1) * SumToCorner(N-1) + NumNodes(N-1) * NumNodes(N-1) * A[N] * (3 + 3 + 3 + 3 + 2 + 2) =
    //  12 * NumNodes(N-1) * SumToCorner(N-1) + 16 * NumNodes(N-1) * NumNodes(N-1) * A[N] 
    //
    //  5) u = A, v = B.
    //
    // Contribution is A[N] :) So we have:
    //
    // Sum(N) = 4 * Sum(N-1) +  
    //   4 * (2 * SumToCorner(N-1) + 3 * NumNodes(N-1) * A[N]) +
    //  12 * NumNodes(N-1) * SumToCorner(N-1) + 16 * NumNodes(N-1) * NumNodes(N-1) * A[N] 
    //
    // Almost there! What are NumNodes(N) and SumToCorner(N)? NumNodes(N) is easy: at each step, the number of nodes
    // is multiplied by 4, and then has two added to it: thus
    //
    //  NumNodes(N) = 4 * NumNodes(N-1) + 2
    //
    // SumToCorner(N) is a little harder: it is
    //
    //   sum [ u in G(N) ] { len(u->c) }
    //
    // As with Sum(N), there are a few cases to consider:
    //
    //  1) u is in C4.
    //
    // The contribution of this is simply SumToCorner(N-1) since C4 is a copy of G(N-1) with c of G(N) also being the c of G(N-1).
    //
    //  2) u = B
    //
    //  The path can be decomposed as B->4, 4->c, so the contribution is len(B->4) + len(4->c) = A[N] + len(4->c).  Let SumCornerToCorner(N)
    // be the sum of edge lengths of the path from the top left corner of G(N) to the bottom right corner of G(N); then we see that
    // len(4->c) = SumCornerToCorner(N-1), and the contribution for this case is 
    //
    //  A[N] + SumCornerToCorner(N-1)
    //
    //  3) u = A
    //
    //  The path can be decomposed as A->B, B->4, 4->c, so it contributes len(A->B) + len(B->4) + len(4->c) = 
    //
    //  A[N] + A[N] + SumCornerToCorner(N-1) =
    //  2 * A[N] + SumCornerToCorner(N-1)
    //
    //  4) u is in Ci, i != 4.
    //
    // Let's consider the case i = 1.  Then any such path can be decomposed into u->1, 1->A, A->B, B->4, 4->c, and the contribution is
    //
    //  sum [u in C1 ] { len(u->1) + len(1->A) + len(A->B) + len(B->4) + len(4->c) } =
    //  sum [u in C1 ] { len(u->1) + A[N] + A[N] + A[N] + SumCornerToCorner[N-1] } =
    //  sum [u in C1 ] { len(u->1) + 3 * A[N] + SumCornerToCorner[N-1] } =
    //  sum [u in C1 ] { len(u->1) } + sum [u in C1] {3 * A[N] + SumCornerToCorner[N-1] } =
    //  SumToCorner(N-1) + NumNodes(N-1) * 3 * A[N] + NumNodes(N-1) * SumCornerToCorner[N-1] =
    //  SumToCorner(N-1) + 3 * NumNodes(N-1) * A[N] + NumNodes(N-1) * SumCornerToCorner[N-1]
    //
    // For i = 2, any such path can be decomposed into u->2, 2->B, B->4, 4->c, and the contribution is
    //
    //  sum [u in C2 ] { len(u->2) + len(2->B) + len(B->4) + len(4->c) } =
    //  sum [u in C2 ] { len(u->2) + A[N] + A[N] + SumCornerToCorner[N-1] } =
    //  sum [u in C2 ] { len(u->2) + 2 * A[N] + SumCornerToCorner[N-1] } =
    //  sum [u in C2 ] { len(u->2) } + sum [u in C2] {2 * A[N] + SumCornerToCorner[N-1] } =
    //  SumToCorner(N-1) + NumNodes(N-1) * 2 * A[N] + NumNodes(N-1) * SumCornerToCorner[N-1] =
    //  SumToCorner(N-1) + 2 * NumNodes(N-1) * A[N] + NumNodes(N-1) * SumCornerToCorner[N-1]
    //
    // For i = 3, we end up with the same contribution of i = 1:
    //
    //  SumToCorner(N-1) + 3 * NumNodes(N-1) * A[N] + NumNodes(N-1) * SumCornerToCorner[N-1]
    // 
    // Thus, the total contribution of case 4 is:
    //
    // 3 * SumToCorner(N-1) + 3 * NumNodes(N-1) * SumCornerToCorner(N-1) + (3 + 2 + 3) * A[N] * NumNodes(N-1) =
    // 3 * SumToCorner(N-1) + 3 * NumNodes(N-1) * SumCornerToCorner(N-1) + 8 * A[N] * NumNodes(N-1) =
    // 3 * SumToCorner(N-1) + NumNodes(N-1) * (8 * A[N] + 3 * SumCornerToCorner(N-1)) =
    //
    // And that's almost it: we have 
    //
    //  SumToCorner(N-1) = SumToCorner(N-1) +
    //    A[N] + SumCornerToCorner(N-1) +
    //    2 * A[N] + SumCornerToCorner(N-1) +
    //    3 * SumToCorner(N-1) + NumNodes(N-1) * (8 * A[N] + 3 * SumCornerToCorner(N-1)) =
    //    4 * SumToCorner(N-1) + 3 * A[N] + 2 * SumCornerToCorner(N-1) + NumNodes(N-1) * (8 * A[N] + 3 * SumCornerToCorner(N-1)) 
    //
    // The final piece of the puzzle is finding SumCornerToCorner(N), and this is easy: it's the sum of the single path from
    // the top left of G(N) (t) to the bottom right (c).  This path can be decomposed as t->1, 1->A, A->B, B->4, 4->c, so
    // its length is:
    //
    //  len(t->1) + len(1->A) + len(A->B) + len(B->4) + len(4->c) =
    //  len(t->1) + A[N] + A[N] + A[N] + len(4->c) =
    //  len(t->1) + 3 * A[N] + len(4->c)
    //
    // Since len(t->1) == len(4->c) == SumCornerToCorner(N-1), we finally have:
    // 
    //  SumCornerToCorner(N) = 2 * SumCornerToCorner(N-1) + 3 * A[N-1]
    //
    // This gives us everything we need to compute Sum(N) :)

    int N;
    cin >> N;

    vector<int64_t> A(N);
    for (int i = 0; i < N; i++)
    {
        cin >> A[i];
    }

    ModNum numNodes = 1;
    // No edges to begin with, so all sums of paths are 0.
    ModNum sumOfAllPaths = 0;
    ModNum sumOfPathsToCorner = 0;
    ModNum sumCornerToCorner = 0;

    for (int i = 0; i < N; i++)
    {
        const ModNum newNumNodes = 4 * numNodes + 2;
        const ModNum newSumOfAllPaths = 4 * sumOfAllPaths + 
            4 * (3 * numNodes * A[i] + 2 * sumOfPathsToCorner) + 
            12 * numNodes * sumOfPathsToCorner + 
            16 * (A[i] * numNodes * numNodes) +
            A[i];
        const ModNum newSumOfPathsToCorner = 4 * sumOfPathsToCorner + 3 * A[i] + 2 * sumCornerToCorner + numNodes * (8 * A[i] + 3 * sumCornerToCorner);
        const ModNum newSumCornerToCorner = 2 * sumCornerToCorner + 3 * A[i];

        numNodes = newNumNodes;
        sumOfAllPaths = newSumOfAllPaths;
        sumOfPathsToCorner = newSumOfPathsToCorner;
        sumCornerToCorner = newSumCornerToCorner;
    }

    cout << sumOfAllPaths << endl; 

}
