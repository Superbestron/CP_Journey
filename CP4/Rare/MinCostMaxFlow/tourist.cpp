#include <bits/stdc++.h>
using namespace std;

typedef long long ll;
typedef tuple<int, ll, ll, ll> edge;
typedef vector<int> vi;
typedef vector<ll> vll;

const ll INF = 1e18; // INF = 1e18, not 2^63-1 to avoid overflow

class min_cost_max_flow {
 private:
  int V;
  ll total_cost;
  vector<edge> EL;
  vector<vi> AL;
  vll d;
  vi last, vis;

  bool SPFA(int s, int t) { // SPFA to find augmenting path in residual graph
    d.assign(V, INF); d[s] = 0; vis[s] = 1;
    queue<int> q({s});
    while (!q.empty()) {
      int u = q.front(); q.pop(); vis[u] = 0;
      for (auto &idx : AL[u]) {                  // explore neighbors of u
        auto &[v, cap, flow, cost] = EL[idx];          // stored in EL[idx]
        if ((cap-flow > 0) && (d[v] > d[u] + cost)) {      // positive residual edge
          d[v] = d[u]+cost;
          if(!vis[v]) q.push(v), vis[v] = 1;
        }
      }
    }
    return d[t] != INF;                           // has an augmenting path
  }

  ll DFS(int u, int t, ll f = INF) {             // traverse from s->t
    if ((u == t) || (f == 0)) return f;
    vis[u] = 1;
    for (int &i = last[u]; i < (int)AL[u].size(); ++i) { // from last edge
      auto &[v, cap, flow, cost] = EL[AL[u][i]];
      if (!vis[v] && d[v] == d[u]+cost) {                      // in current layer graph
        if (ll pushed = DFS(v, t, min(f, cap-flow))) {
          total_cost += pushed * cost;
          flow += pushed;
          auto &[rv, rcap, rflow, rcost] = EL[AL[u][i]^1]; // back edge
          rflow -= pushed;
          vis[u] = 0;
          return pushed;
        }
      }
    }
    vis[u] = 0;
    return 0;
  }

 public:
  min_cost_max_flow(int initialV) : V(initialV), total_cost(0) {
    EL.clear();
    AL.assign(V, vi());
    vis.assign(V, 0);
  }

  // if you are adding a bidirectional edge u<->v with weight w into your
  // flow graph, set directed = false (default value is directed = true)
  void add_edge(int u, int v, ll w, ll c, bool directed = true) {
    if (u == v) return;                          // safeguard: no self loop
    EL.emplace_back(v, w, 0, c);                 // u->v, cap w, flow 0, cost c
    AL[u].push_back(EL.size()-1);                // remember this index
    EL.emplace_back(u, 0, 0, -c);                // back edge
    AL[v].push_back(EL.size()-1);                // remember this index
    if (!directed) add_edge(v, u, w, c);         // add again in reverse
  }

  pair<ll, ll> mcmf(int s, int t) {
    ll mf = 0;                                   // mf stands for max_flow
    while (SPFA(s, t)) {                          // an O(V^2*E) algorithm
      last.assign(V, 0);                         // important speedup
      while (ll f = DFS(s, t))                   // exhaust blocking flow
        mf += f;
    }
    return {mf, total_cost};
  }
};

int T, H, W;

int convertToIdx(int i, int j, bool btm) {
  return 2 * (i * W + j) + btm;
}

int main() {
  ios_base::sync_with_stdio(false);
  cin.tie(nullptr);
  cin >> T;
  while (T--) {
    cin >> W >> H;
    vector<string> arr(H);
    for (string& s : arr) cin >> s;
    min_cost_max_flow mf(2 * H * W);
    for (int i = 0; i < H; i++) {
      for (int j = 0; j < W; j++) {
        // construct right and down edges
        if (i + 1 != H)
          mf.add_edge(convertToIdx(i, j, true), convertToIdx(i + 1, j, false), 2, 0);
        if (j + 1 != W)
          mf.add_edge(convertToIdx(i, j, true), convertToIdx(i, j + 1, false), 2, 0);
        if (arr[i][j] == '*') {
          mf.add_edge(convertToIdx(i, j, false), convertToIdx(i, j, true), 1, 0);
          // one edge for benefit of 1
          mf.add_edge(convertToIdx(i, j, false), convertToIdx(i, j, true), 1, -1);
        } else if (arr[i][j] == '.') {
          mf.add_edge(convertToIdx(i, j, false), convertToIdx(i, j, true), 2, 0);
        }
      }
    }
    auto [max_flow, total_cost] = mf.mcmf(0, convertToIdx(H - 1, W - 1, true));
    cout << -total_cost << '\n';
  }
}