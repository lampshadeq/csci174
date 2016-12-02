#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <future>
#include <iostream>
#include <stack>
#include <thread>
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

typedef std::thread thread;
typedef std::vector<int> vectorI;
typedef std::vector<pair> vectorP;
typedef std::vector<thread> vectorT;
typedef std::stack<pair> stackP;
typedef std::promise<vectorI> promise;
typedef std::vector<promise> vectorR;
typedef std::future<vectorI> future;
typedef std::vector<future> vectorF;

void createPoints(vectorP&, vectorP&, int, int);
void getNumsMulti(vectorP, vectorP, int, int, promise&);
bool compareYs(pair, pair);
bool compareAngles(pair, pair);
bool colinear(pair, pair, pair);
int crossProduct(pair, pair, pair);
vectorI getNums(vectorP, vectorP, int, int);
vectorP findPairs(vectorP, vectorP);
stackP grahamsScan(vectorP);
pair bruteForceFindLine(vectorP, vectorP);
pair findLineSingle(vectorP, vectorP);
pair findLineMulti(vectorP, vectorP);

int main(int argc, char** argv) {
  unsigned int i;
  int example;
  vectorP busters, ghosts, pairs;
  pair res;
  
  if (argc < 2) {
    std::cerr << "Usage: <example_number>" << std::endl;
    return 1;
  }

  example = std::atoi(argv[1]);
  switch (example) {
    case 0:
      busters.push_back(pair(2,7));
      ghosts.push_back(pair(6,4));
      break;

    case 1:
      busters.push_back(pair(9,3));
      ghosts.push_back(pair(1,1));
      break;

    case 2:
      busters.push_back(pair(2,6));
      busters.push_back(pair(8,8));
      ghosts.push_back(pair(5,3));
      ghosts.push_back(pair(4,10));
      break;

    case 3:
      busters.push_back(pair(2, 6));
      busters.push_back(pair(5, 3));
      ghosts.push_back(pair(8, 8));
      ghosts.push_back(pair(4, 10));
      break;

    case 4:
      busters.push_back(pair(2, 6));
      busters.push_back(pair(8, 8));
      ghosts.push_back(pair(5, 3));
      ghosts.push_back(pair(6, 6));
      break;

    case 5:
      busters.push_back(pair(4,5));
      busters.push_back(pair(5,9));
      busters.push_back(pair(10,4));
      ghosts.push_back(pair(3,2));
      ghosts.push_back(pair(7,5));
      ghosts.push_back(pair(2,6));
      break;

    case 6:
      busters.push_back(pair(2, 2));
      busters.push_back(pair(10, 3));
      busters.push_back(pair(4, 10));
      ghosts.push_back(pair(5, 3));
      ghosts.push_back(pair(4, 5));
      ghosts.push_back(pair(6, 5));
      break;

    case 7:
      busters.push_back(pair(2,1));
      busters.push_back(pair(5,3));
      busters.push_back(pair(6,1));
      busters.push_back(pair(8,9));
      ghosts.push_back(pair(10,2));
      ghosts.push_back(pair(3,5));
      ghosts.push_back(pair(9,2));
      ghosts.push_back(pair(7,4));
      break;

    case 8:
      busters.push_back(pair(1,1));
      busters.push_back(pair(5,3));
      busters.push_back(pair(6,13));
      busters.push_back(pair(8,9));
      busters.push_back(pair(10,2));
      ghosts.push_back(pair(3,5));
      ghosts.push_back(pair(9,8));
      ghosts.push_back(pair(7,6));
      ghosts.push_back(pair(12,10));
      ghosts.push_back(pair(15,7));
      break;

    case 9:
      busters.push_back(pair(1,1));
      busters.push_back(pair(1,6));
      busters.push_back(pair(8,9));
      busters.push_back(pair(15,6));
      busters.push_back(pair(15,1));
      ghosts.push_back(pair(4,2));
      ghosts.push_back(pair(6,7));
      ghosts.push_back(pair(7,2));
      ghosts.push_back(pair(11,5));
      ghosts.push_back(pair(13,4));
      break;
  }

  //createPoints(busters, ghosts, 3, 500);
  
  std::cout << "**********BRUTE FORCE**********" << std::endl;
  res = bruteForceFindLine(busters, ghosts);
  busters[res.x].operator<<(std::cout);
  ghosts[res.y].operator<<(std::cout);
  std::cout << "**********BRUTE FORCE**********" << std::endl;

  std::cout << std::endl;

  std::cout << "************SINGLE***********" << std::endl;
  res = findLineSingle(busters, ghosts);
  busters[res.x].operator<<(std::cout);
  ghosts[res.y].operator<<(std::cout);
  std::cout << "************SINGLE***********" << std::endl;

  std::cout << std::endl;

  std::cout << "************MULTI************" << std::endl;
  res = findLineMulti(busters, ghosts);
  busters[res.x].operator<<(std::cout);
  ghosts[res.y].operator<<(std::cout);
  std::cout << "************MULTI************" << std::endl;

  std::cout << std::endl;

  std::cout << "*********FINDING PAIRS*********" << std::endl;
  pairs = findPairs(busters, ghosts);
  for (i = 0; i < pairs.size(); i++) {
    pairs[i].operator<<(std::cout);

    if (i % 2 != 0 && i != pairs.size() - 1) {
      std::cout << std::endl;
    }
  }
  std::cout << "*********FINDING PAIRS*********" << std::endl;

  return 0;
}

void createPoints(vectorP& busters, vectorP& ghosts, int n, int grid) {
  bool flag;
  int i, x, y;
  vectorP comb;
  vectorP::iterator it;

  srand((unsigned int) time(NULL));

  while (true) {
    comb.swap(vectorP(2*n));

    // generate random coordinates
    for (i = 0; i < 2*n; i++) {
      x = rand() % (grid + 1);
      y = rand() % (grid + 1);
      comb[i] = pair(x, y);
    }

    // ensure no duplicates
    flag = false;
    for (i = 0; i < 2 * n; i++) {
      it = std::find(comb.begin() + i + 1, comb.end(), comb[i]);
      if (it != comb.end()) {
        flag = true;
        break;
      }
    }
    if (flag) {
      continue;
    }

    // ensure no 3 points are colinear
    flag = false;
    for (i = 0; i < 2 * n; i++) {
      for (x = 0; x < 2 * n; x++) {
        if (i != x) {
          for (y = 0; y < 2 * n; y++) {
            if (i != y && x != y) {
              if (colinear(comb[i], comb[x], comb[y])) {
                flag = true;
                break;
              }
            }
          }
          if (flag) {
            break;
          }
        }
      }
      if (flag) {
        break;
      }
    }

    if (flag) {
      continue;
    }
    else {
      break;
    }
  }

  // split comb
  for (i = 0; i < n; i++) {
    busters.push_back(comb[i]);
  }
  for (i = n; i < 2 * n; i++) {
    ghosts.push_back(comb[i]);
  }
}

void getNumsMulti(vectorP busters, vectorP ghosts, int bidx, int gidx, promise& p) {
  unsigned int i;
  vectorI nums(4, 0);

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

  p.set_value(nums);
}

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
    line = findLineMulti(busters, ghosts);
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
    comb.swap(vectorP());
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

pair findLineMulti(vectorP busters, vectorP ghosts) {
  bool flag;
  unsigned int i, j, k, x, y;
  pair res;
  stackP s;
  vectorI nums;
  vectorP comb;
  vectorP::iterator it0, it1;
  vectorT threads;
  vectorR promises(busters.size());
  vectorF futures;

  if (busters.size() == 1) {
    res = pair(0, 0);
  }
  else {
    for (i = 0; i < busters.size(); i++) {
      comb.push_back(busters[i]);
    }
    for (i = 0; i < ghosts.size(); i++) {
      comb.push_back(ghosts[i]);
    }

    s = grahamsScan(comb);
    comb.swap(vectorP());
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

        res = pair(x, y);
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
          futures.push_back(promises[i].get_future());
          threads.push_back(thread(getNumsMulti, busters, ghosts, x, i, std::move(promises[i])));
        }

        flag = false;
        for (i = 0; i < ghosts.size(); i++) {
          threads[i].join();
          if (!flag) {
            nums = futures[i].get();
            if (nums[0] == nums[1] || nums[2] == nums[3]) {
              res = pair(x,i);
              flag = true;
            }
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
          futures.push_back(promises[i].get_future());
          threads.push_back(thread(getNumsMulti, busters, ghosts, i, y, std::move(promises[i])));
        }

        flag = false;
        for (i = 0; i < busters.size(); i++) {
          threads[i].join();
          if (!flag) {
            nums = futures[i].get();
            if (nums[0] == nums[1] || nums[2] == nums[3]) {
              res = pair(i, y);
              flag = true;
            }
          }
        }
      }
    }
  }

  return res;
}