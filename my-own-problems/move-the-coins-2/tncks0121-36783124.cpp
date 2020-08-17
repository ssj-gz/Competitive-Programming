    #pragma GCC optimize("Ofast")
     
    #include <cstdio>
    #include <vector>
    #include <iostream>
    using namespace std;
     
    const int N_ = 300500;
    int C[N_];
    int L[N_], R[N_];
    int dep[N_];
    std::vector<int> gph[N_];
     
    int TIME;
    int arr[N_];
    int dfs (int u, int p) {
        if (C[u] % 2 == 1) {
            arr[++TIME] = dep[u];
            L[u] = TIME;
        }
        for (int v : gph[u]) if(v != p) {
            dep[v] = dep[u] + 1;
            int c = dfs(v, u);
            if (!L[u]) L[u] = c;
        }
        if(L[u]) R[u] = TIME;
        return L[u];
    }
     
    int pref[N_];
     
    const int MOD = (int)1e9 + 7;
     
    void solve() {
        int N; scanf("%d", &N);
        for (int i = 1; i <= N-1; i++) {
            int a, b; scanf("%d%d", &a, &b);
            gph[a].push_back(b);
            gph[b].push_back(a);
        }
        for (int i = 1; i <= N; i++) {
            scanf("%d", &C[i]);
        }
     
        TIME = 0;
        dfs(1, -1);
     
        for (int i = 1; i <= TIME; i++) {
            pref[i] = pref[i-1] ^ arr[i];
        }
     
        int Q; scanf("%d", &Q);
        int ans = 0;
        int64_t blah = 0;
        for (int qid = 1, qv = 2; qid <= Q; qid++, qv = (qv * 2) % MOD) {
            int u, v; scanf("%d%d", &u, &v);
            int grundy = pref[TIME];
            if (L[u]) {
                grundy ^= pref[L[u] - 1] ^ pref[R[u]];
                int diff = -dep[u] + dep[v] + 1;
                const int l = L[u], r = R[u];
                //cout << "qid: " << qid << " l: " << l << " r: " << r << endl;
                blah += r - l;
sausage:
                for (int i = l; i <= r; i++) { // arr[i] == depth of node i if that node has coin; else 0.
                    grundy ^= arr[i] + diff;
                }
            }
            if (grundy == 0) {
                (ans += qv) %= MOD;
            }
        }
     
        printf("%d\n", ans);
        //cout << "blah: " << blah << endl;
     
     
        for (int i = 1; i <= TIME; i++) pref[i] = 0, arr[i] = 0;
        for (int i = 1; i <= N; i++) gph[i].clear(), L[i] = R[i] = dep[i] = 0;
    }
     
    int main() {
    #ifdef IN_MY_COMPUTER
        freopen("naive.in" ,"r", stdin);
    #endif
        int T; scanf("%d", &T);
        while(T--) solve();
        return 0;
    } 
