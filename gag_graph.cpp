#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <stack>
#include <vector>

struct pair {
  int x, y;
  double angle;

  pair() {
    x = y = -1;
    angle = -1.0;
  }

  pair(int i, int j) {
    x = i;
    y = j;
    angle = -1.0;
  }

  std::ostream& operator<<(std::ostream& out) {
    out << '(' << x << ',' << y << ')' << std::endl;
    return out;
  }

  bool operator==(const pair& p) const {
    return x == p.x && y == p.y;
  }
};

typedef std::vector<int> vectorI;
typedef std::vector<pair> vectorP;
typedef std::stack<pair> stackP;

bool compareYs(pair, pair);
bool compareAngles(pair, pair);
bool colinear(pair, pair, pair);
int crossProduct(pair, pair, pair);
vectorI getNums(vectorP, vectorP, int, int);
vectorP findPairs(vectorP, vectorP);
stackP grahamsScan(vectorP);
pair bruteForceFindLine(vectorP, vectorP);
pair findLineSingle(vectorP, vectorP);

bool compareYs(pair p0, pair p1) {
  if (p0.y == p1.y) {
    return p0.x < p1.x;
  }
  else {
    return p0.y < p1.y;
  }
}

bool compareAngles(pair p0, pair p1) {
  return p0.angle < p1.angle;
}

bool colinear(pair p0, pair p1, pair p2) {
  int minx = (p0.x < p1.x) ? p0.x : p1.x;
  int miny = (p0.y < p1.y) ? p0.y : p1.y;
  int maxx = (p0.x > p1.x) ? p0.x : p1.x;
  int maxy = (p0.y > p1.y) ? p0.y : p1.y;

  return (minx <= p2.x) && (p2.x <= maxx) && (miny <= p2.y) && (p2.y <= maxy);
}

int crossProduct(pair p0, pair p1, pair p2) {
  return (p1.x - p0.x) * (p2.y - p0.y) - (p2.x - p0.x) * (p1.y - p0.y);
}

vectorI getNums(vectorP busters, vectorP ghosts, int bidx, int gidx) {
  unsigned int i;
  vectorI nums(4,0);

  for (i = 0; i < busters.size(); i++) {
    if (i != bidx) {
      if (crossProduct(busters[bidx], ghosts[gidx], busters[i]) > 0) {
        nums[0]++;
      }
      else {
        nums[2]++;
      }
    }
  }

  for (i = 0; i < ghosts.size(); i++) {
    if (i != gidx) {
      if (crossProduct(busters[bidx], ghosts[gidx], ghosts[i]) > 0) {
        nums[1]++;
      }
      else {
        nums[3]++;
      }
    }
  }

  return nums;
}

vectorP findPairs(vectorP busters, vectorP ghosts) {
  unsigned int i;
  vectorP pairs, recClockwise, bustersClockwise, ghostsClockwise,
          bustersCounter, ghostsCounter, recCounter;
  pair line, buster, ghost;

  if (busters.size() == 1) {
    pairs.push_back(busters[0]);
    pairs.push_back(ghosts[0]);
  }
  else {
    line = findLineSingle(busters, ghosts);
    pairs.push_back(busters[line.x]);
    pairs.push_back(ghosts[line.y]);

    if (busters.size() == 2) {
      line.x = (line.x + 1) % 2;
      line.y = (line.y + 1) % 2;
      pairs.push_back(busters[line.x]);
      pairs.push_back(ghosts[line.y]);
    }
    else {
      buster = busters[line.x];
      ghost = ghosts[line.y];

      busters.erase(busters.begin() + line.x);
      ghosts.erase(ghosts.begin() + line.y);

      for (i = 0; i < busters.size(); i++) {
        if (crossProduct(buster, ghost, busters[i]) < 0) {
          bustersClockwise.push_back(busters[i]);
        }
        else {
          bustersCounter.push_back(busters[i]);
        }
      }
      for (i = 0; i < ghosts.size(); i++) {
        if (crossProduct(buster, ghost, ghosts[i]) < 0) {
          ghostsClockwise.push_back(ghosts[i]);
        }
        else {
          ghostsCounter.push_back(ghosts[i]);
        }
      }

      if (bustersClockwise.size() != 0) {
        recClockwise = findPairs(bustersClockwise, ghostsClockwise);
      }
      if (bustersCounter.size() != 0) {
        recCounter = findPairs(bustersCounter, ghostsCounter);
      }

      for (i = 0; i < recClockwise.size(); i++) {
        pairs.push_back(recClockwise[i]);
      }
      for (i = 0; i < recCounter.size(); i++) {
        pairs.push_back(recCounter[i]);
      }
    }
  }

  return pairs;
}

stackP grahamsScan(vectorP q) {
  unsigned int i;
  double a;
  pair top, nextTop;
  stackP s, t;

  std::sort(q.begin(), q.end(), compareYs);

  for (i = 1; i < q.size(); i++) {
    if (q[i].x - q[0].x == 0) {
      a = 90.0;
    }
    else if (q[i].y - q[0].y == 0) {
      a = 0.0;
    }
    else {
      a = atan((double) (q[i].y - q[0].y) / (q[i].x - q[0].x));
      a = a * 180.0 / 3.14159;

      if (a < 0.0) {
        a += 180.0;
      }
    }

    q[i].angle = a;
  }

  std::sort(q.begin() + 1, q.end(), compareAngles);

  s.push(q[0]);
  s.push(q[1]);
  s.push(q[2]);

  for (i = 3; i < q.size(); i++) {
    t = stackP(s);
    top = s.top();
    t.pop();
    nextTop = t.top();

    while (crossProduct(nextTop, q[i], top) >= 0) {
      s.pop();
      t = stackP(s);
      top = s.top();
      t.pop();
      nextTop = t.top();
    }

    s.push(q[i]);
  }

  return s;
}

pair bruteForceFindLine(vectorP busters, vectorP ghosts) {
  unsigned int bidx, gidx;
  vectorI nums;
  pair res;

  for (bidx = 0; bidx < busters.size(); bidx++) {
    for (gidx = 0; gidx < ghosts.size(); gidx++) {
      nums = getNums(busters, ghosts, bidx, gidx);

      if (nums[0] == nums[1] || nums[2] == nums[3]) {
        res = pair(bidx, gidx);
        break;
      }
    }

    if (res.x != -1) {
      break;
    }
  }

  return res;
}

pair findLineSingle(vectorP busters, vectorP ghosts) {
  unsigned int i, j, k, x, y;
  pair res;
  stackP s;
  vectorI nums;
  vectorP comb;
  vectorP::iterator it0, it1;
  
  if (busters.size() == 1) {
    res = pair(0,0);
  }
  else {
    for (i = 0; i < busters.size(); i++) {
      comb.push_back(busters[i]);
    }
    for (i = 0; i < ghosts.size(); i++) {
      comb.push_back(ghosts[i]);
    }

    s = grahamsScan(comb);
    comb.clear();
    while (!s.empty()) {
      comb.push_back(s.top());
      s.pop();
    }

    for (i = 0; i < comb.size(); i++) {
      j = (i + 1) % comb.size();
      it0 = std::find(busters.begin(), busters.end(), comb[i]);
      it1 = std::find(ghosts.begin(), ghosts.end(), comb[j]);

      if (it0 != busters.end() && it1 != ghosts.end()) {
        for (k = 0; k < busters.size(); k++) {
          if (busters[k] == comb[i]) {
            x = k;
            break;
          }
        }

        for (k = 0; k < ghosts.size(); k++) {
          if (ghosts[k] == comb[j]) {
            y = k;
            break;
          }
        }

        res = pair(x,y);
        break;
      }

      it0 = std::find(ghosts.begin(), ghosts.end(), comb[i]);
      it1 = std::find(busters.begin(), busters.end(), comb[j]);

      if (it0 != ghosts.end() && it1 != busters.end()) {
        for (k = 0; k < ghosts.size(); k++) {
          if (ghosts[k] == comb[i]) {
            x = k;
            break;
          }
        }

        for (k = 0; k < busters.size(); k++) {
          if (busters[k] == comb[j]) {
            y = k;
            break;
          }
        }

        res = pair(y, x);
        break;
      }
    }

    if (res == pair()) {
      it0 = std::find(busters.begin(), busters.end(), comb[0]);
      if (it0 != busters.end()) {
        for (i = 0; i < busters.size(); i++) {
          if (busters[i] == comb[0]) {
            x = i;
            break;
          }
        }

        for (i = 0; i < ghosts.size(); i++) {
          nums = getNums(busters, ghosts, x, i);
          if (nums[0] == nums[1] || nums[2] == nums[3]) {
            res = pair(x,i);
            break;
          }
        }
      }
      else {
        for (i = 0; i < ghosts.size(); i++) {
          if (ghosts[i] == comb[0]) {
            y = i;
            break;
          }
        }

        for (i = 0; i < busters.size(); i++) {
          nums = getNums(busters, ghosts, i, y);
          if (nums[0] == nums[1] || nums[2] == nums[3]) {
            res = pair(i,y);
            break;
          }
        }
      }
    }
  }
  
  return res;
}
