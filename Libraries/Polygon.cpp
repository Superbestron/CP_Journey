#include <bits/stdc++.h>
using namespace std;
typedef pair<int, int> ii;
typedef vector<ii> vii;
typedef long long ll;
typedef long double ld;

const double EPS = 1e-9;

double DEG_to_RAD(double d) { return d * M_PI / 180.0; }

double RAD_to_DEG(double r) { return r * 180.0 / M_PI; }

// struct point_i { int x, y; };                 // minimalist form
struct point_i {
  int x, y;                                      // default
  point_i() { x = y = 0; }                       // default
  point_i(int _x, int _y) : x(_x), y(_y) {}      // user-defined
};

struct point {
  double x, y;                                   // only used if more precision is needed
  point() { x = y = 0.0; }                       // default constructor
  point(double _x, double _y) : x(_x), y(_y) {}  // user-defined
  bool operator==(point other) const {
    return (fabs(x - other.x) < EPS && (fabs(y - other.y) < EPS));
  }
  bool operator<(const point &p) const {
    return x < p.x || (abs(x - p.x) < EPS && y < p.y);
  }
};

struct vec {
  double x, y;  // name: 'vec' is different from STL vector
  vec(double _x, double _y) : x(_x), y(_y) {}
  vec(point p1, point p2) : x(p2.x - p1.x), y(p2.y - p1.y) {}
};

vec toVec(point a, point b) {       // convert 2 points to vector a->b
  return {b.x - a.x, b.y - a.y};
}

double dist(point p1, point p2) {                // Euclidean distance
  return hypot(p1.x - p2.x, p1.y - p2.y);
}               // return double

// returns the perimeter of polygon P, which is the sum of
// Euclidian distances of consecutive line segments (polygon edges)
double perimeter(const vector<point> &P) {       // by ref for efficiency
  double ans = 0.0;
  for (int i = 0; i < (int) P.size() - 1; ++i)      // note: P[n-1] = P[0]
    ans += dist(P[i], P[i + 1]);                   // as we duplicate P[0]
  return ans;
}

// returns the area of polygon P
double area(const vector<point> &P) {
  double ans = 0.0;
  for (int i = 0; i < (int) P.size() - 1; ++i)      // Shoelace formula
    ans += (P[i].x * P[i + 1].y - P[i + 1].x * P[i].y);
  return fabs(ans) / 2.0;                          // only do / 2.0 here
}

double dot(vec a, vec b) { return (a.x * b.x + a.y * b.y); }

double norm_sq(vec v) { return v.x * v.x + v.y * v.y; }

double angle(point a, point o, point b) {  // returns angle aob in rad
  vec oa = toVec(o, a), ob = toVec(o, b);
  return acos(dot(oa, ob) / sqrt(norm_sq(oa) * norm_sq(ob)));
}

double cross(vec a, vec b) { return a.x * b.y - a.y * b.x; }

// returns the area of polygon P, which is half the cross products
// of vectors defined by edge endpoints
double area_alternative(const vector<point> &P) {
  double ans = 0.0;
  point O(0.0, 0.0);           // O = the Origin
  for (int i = 0; i < (int) P.size() - 1; ++i)      // sum of signed areas
    ans += cross(toVec(O, P[i]), toVec(O, P[i + 1]));
  return fabs(ans) / 2.0;
}

// note: to accept collinear points, we have to change the '> 0'
// returns true if point r is on the left side of line pq
bool ccw(point p, point q, point r) {
  return cross(toVec(p, q), toVec(p, r)) >= 0;
}

// returns true if point r is on the same line as the line pq
bool collinear(point p, point q, point r) {
  return fabs(cross(toVec(p, q), toVec(p, r))) < EPS;
}

// returns true if we always make the same turn
// while examining all the edges of the polygon one by one
bool isConvex(const vector<point> &P) {
  int n = (int) P.size();
  // a point/sz=2 or a line/sz=3 is not convex  
  if (n <= 3) return false;
  bool firstTurn = ccw(P[0], P[1], P[2]);        // remember one result,
  for (int i = 1; i < n - 1; ++i)                 // compare with the others
    if (ccw(P[i], P[i + 1], P[(i + 2) == n ? 1 : i + 2]) != firstTurn)
      return false;                              // different -> concave
  return true;                                   // otherwise -> convex
}

// returns 1/0/-1 if point p is inside/on (vertex/edge)/outside of
// either convex/concave polygon P
int insidePolygon(point pt, const vector<point> &P) {
  int n = (int) P.size();
  if (n <= 3) return -1;                         // avoid point or line
  bool on_polygon = false;
  for (int i = 0; i < n - 1; ++i)                  // on vertex/edge?
    if (fabs(dist(P[i], pt) + dist(pt, P[i + 1]) - dist(P[i], P[i + 1])) < EPS)
      on_polygon = true;
  if (on_polygon) return 0;                      // pt is on polygon
  double sum = 0.0;                              // first = last point
  for (int i = 0; i < n - 1; ++i) {
    if (ccw(pt, P[i], P[i + 1]))
      sum += angle(P[i], pt, P[i + 1]);            // left turn/ccw
    else
      sum -= angle(P[i], pt, P[i + 1]);            // right turn/cw
  }
  return fabs(sum) > M_PI ? 1 : -1;              // 360d->in, 0d->out
}

// compute the intersection point between line segment p-q and line A-B
point lineIntersectSeg(point p, point q, point A, point B) {
  double a = B.y - A.y, b = A.x - B.x, c = B.x * A.y - A.x * B.y;
  double u = fabs(a * p.x + b * p.y + c);
  double v = fabs(a * q.x + b * q.y + c);
  return {(p.x * v + q.x * u) / (u + v), (p.y * v + q.y * u) / (u + v)};
}

// cuts polygon Q along the line formed by point A->point B (order matters)
// (note: the last point must be the same as the first point)
vector<point> cutPolygon(point A, point B, const vector<point> &Q) {
  vector<point> P;
  for (int i = 0; i < (int) Q.size(); ++i) {
    double left1 = cross(toVec(A, B), toVec(A, Q[i])), left2 = 0;
    if (i != (int) Q.size() - 1) left2 = cross(toVec(A, B), toVec(A, Q[i + 1]));
    if (left1 > -EPS) P.push_back(Q[i]);         // Q[i] is on the left
    if (left1 * left2 < -EPS)                      // crosses line AB
      P.push_back(lineIntersectSeg(Q[i], Q[i + 1], A, B));
  }
  if (!P.empty() && !(P.back() == P.front()))
    P.push_back(P.front());                      // wrap around
  return P;
}

vector<point> CH_Graham(vector<point> &Pts) {    // overall O(n log n)
  vector<point> P(Pts);                          // copy all points
  int n = (int) P.size();
  if (n <= 3) {                                  // point/line/triangle
    if (!(P[0] == P[n - 1])) P.push_back(P[0]);    // corner case
    return P;                                    // the CH is P itself
  }

  // first, find P0 = point with lowest Y and if tie: rightmost X
  int P0 = min_element(P.begin(), P.end()) - P.begin();
  swap(P[0], P[P0]);                             // swap P[P0] with P[0]

  // second, sort points by angle around P0, O(n log n) for this sort
  sort(++P.begin(), P.end(), [&](point a, point b) {
    return ccw(P[0], a, b);                      // use P[0] as the pivot
  });

  // third, the ccw tests, although complex, it is just O(n)
  vector<point> S({P[n - 1], P[0], P[1]});         // initial S
  int i = 2;                                     // then, we check the rest
  while (i < n) {                                // n > 3, O(n)
    int j = (int) S.size() - 1;
    if (ccw(S[j - 1], S[j], P[i]))                 // CCW turn
      S.push_back(P[i++]);                       // accept this point
    else                                         // CW turn
      S.pop_back();                              // pop until a CCW turn
  }
  return S;                                      // return the result
}

// Last point does not have to be first point
vector<point> CH_Andrew(vector<point> &Pts) {    // overall O(n log n)
  int n = Pts.size(), k = 0;
  vector<point> H(2 * n);
  sort(Pts.begin(), Pts.end());                  // sort the points by x/y
  for (int i = 0; i < n; ++i) {                  // build lower hull
    while ((k >= 2) && !ccw(H[k - 2], H[k - 1], Pts[i])) --k;
    H[k++] = Pts[i];
  }
  for (int i = n - 2, t = k + 1; i >= 0; --i) {       // build upper hull
    while ((k >= t) && !ccw(H[k - 2], H[k - 1], Pts[i])) --k;
    H[k++] = Pts[i];
  }
  H.resize(k);
  return H;
}

double cal_tri_area(point &a, point &b, point &c) {
  return fabs(cross(vec(a, b), vec(a, c)) / 2.0);
}

// Rotating Caliper Algorithm to Get All AntiPodal Points
// Here P has no repeated vertex at the back
vii ShamosAlgorithm(vector<point> &P) {
  int n = P.size();

  if (n == 1) return {};
  if (n == 2) return {{0, 1}};

  vii ans;
  int i0 = n - 1;
  int i = 0;
  int j = i + 1;
  int j0 = j;
  while (cal_tri_area(P[i], P[i + 1], P[j + 1]) > cal_tri_area(P[i], P[i + 1], P[j])) {
    j = j + 1;
    j0 = j;
  }
  ans.emplace_back(i, j);
  while (j != i0) {
    i = i + 1;
    ans.emplace_back(i, j);

    while (cal_tri_area(P[i], P[i + 1], P[j + 1]) > cal_tri_area(P[i], P[i + 1], P[j])) {
      j = j + 1;
      if ((i != j0 && j != i0))
        ans.emplace_back(i, j);
      else
        return ans;
    }
    if (cal_tri_area(P[j], P[i + 1], P[j + 1]) == cal_tri_area(P[i], P[i + 1], P[j])) {
      if (i != j0 && j != i0)
        ans.emplace_back(i, j + 1);
      else
        ans.emplace_back(i + 1, j);
    }
  }
  return ans;
}

// last point doesn't have to be first point
pair<point, point> closest_pair(vector<point> &P, int n) {
  sort(P.begin(), P.end());
  set<point> s;
  pair<point, point> ans;

  double best_dist = 1e18;
  for (int i = 0, j = 0; i < n; ++i) {
    double d = best_dist;
    while (fabs(P[i].x - P[j].x) - d > EPS) {
      s.erase({P[j].y, P[j].x});
      j++;
    }

    auto it1 = s.lower_bound({P[i].y - d, P[i].x});
    auto it2 = s.upper_bound({P[i].y + d, P[i].x});

    for (auto it = it1; it != it2; ++it) {
      double dx = P[i].x - it->y;
      double dy = P[i].y - it->x;
      double dist = hypot(dx, dy);
      if (best_dist > dist) {
        best_dist = dist;
        ans = {point(it->y, it->x), P[i]};
      }
    }
    s.emplace(P[i].y, P[i].x);
  }
  return ans;
}

ld area_of_largest_triangle(vector<point> &P) {
  vector<point> ans(CH_Andrew(P));
  ans.pop_back();
  int n = ans.size();

  auto cal_tri_area = [&](int a_idx, int b_idx, int c_idx) {
    point &a = ans[a_idx % n];
    point &b = ans[b_idx % n];
    point &c = ans[c_idx % n];
    return abs(cross(vec(a, b), vec(a, c)));
  };

  ll A = 0;
  for (int a = 0; a < n; a++) {
    int b = a + 1;
    for (int k = 0; k < n - 2; k++) {
      int c = a + k + 2;
      ll area = cal_tri_area(a, b, c);
      while (b + 1 < c) {
        b++;
        ll newarea = cal_tri_area(a, b, c);
        if (newarea <= area) break;
        area = newarea;
      }
      b = max(a + 1, b - 1);
      A = max(A, area);
    }
  }
  return A / 2.0;
}

int main() {
  // 6(+1) points, entered in counter clockwise order, 0-based indexing
  vector<point> P;
  P.emplace_back(1, 1);                          // P0
  P.emplace_back(3, 3);                          // P1
  P.emplace_back(9, 1);                          // P2
  P.emplace_back(12, 4);                         // P3
  P.emplace_back(9, 7);                          // P4
  P.emplace_back(1, 7);                          // P5
  P.push_back(P[0]);                             // loop back, P6 = P0

  printf("Perimeter = %.2lf\n", perimeter(P));   // 31.64
  printf("Area = %.2lf\n", area(P));             // 49.00
  printf("Area = %.2lf\n", area_alternative(P)); // also 49.00
  printf("Is convex = %d\n", isConvex(P));       // 0 (false)

  //// the positions of P_out, P_on, P_in with respect to the polygon
  //7 P5------P_on----P4
  //6 |                  \
  //5 |                    \
  //4 |   P_in              P3
  //3 |   P1___            /
  //2 | / P_out \ ___    /
  //1 P0              P2
  //0 1 2 3 4 5 6 7 8 9 101112

  point p_out(3, 2); // outside this (concave) polygon
  printf("P_out is inside = %d\n", insidePolygon(p_out, P)); // -1
  printf("P1 is inside = %d\n", insidePolygon(P[1], P)); // 0
  point p_on(5, 7); // on this (concave) polygon
  printf("P_on is inside = %d\n", insidePolygon(p_on, P)); // 0
  point p_in(3, 4); // inside this (concave) polygon
  printf("P_in is inside = %d\n", insidePolygon(p_in, P)); // 1

  // cutting the original polygon based on line P[2] -> P[4] (get the left side)
  //7 P5--------------P4
  //6 |               |  \
  //5 |               |    \
  //4 |               |     P3
  //3 |   P1___       |    /
  //2 | /       \ ___ |  /
  //1 P0              P2
  //0 1 2 3 4 5 6 7 8 9 101112
  // new polygon (notice the index are different now):
  //7 P4--------------P3
  //6 |               |
  //5 |               |
  //4 |               |
  //3 |   P1___       |
  //2 | /       \ ___ |
  //1 P0              P2
  //0 1 2 3 4 5 6 7 8 9

  P = cutPolygon(P[2], P[4], P);
  printf("Perimeter = %.2lf\n", perimeter(P));   // smaller now, 29.15
  printf("Area = %.2lf\n", area(P));             // 40.00

  // running convex hull of the resulting polygon (index changes again)
  //7 P3--------------P2
  //6 |               |
  //5 |               |
  //4 |   P_out       |
  //3 |               |
  //2 |   P_in        |
  //1 P0--------------P1
  //0 1 2 3 4 5 6 7 8 9

  P = CH_Graham(P);                              // now this is a rectangle
  printf("Perimeter = %.2lf\n", perimeter(P));   // precisely 28.00
  printf("Area = %.2lf\n", area(P));             // precisely 48.00
  printf("Is convex = %d\n", isConvex(P));       // true
  printf("P_out is inside = %d\n", insidePolygon(p_out, P)); // 1
  printf("P_in is inside = %d\n", insidePolygon(p_in, P)); // 1

  return 0;
}
