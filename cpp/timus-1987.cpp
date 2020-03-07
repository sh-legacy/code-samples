#include <algorithm>
#include <iostream>
#include <stack>
#include <vector>

#define TYPE_SEG_START 0
#define TYPE_SEG_END 2
#define TYPE_POINT 1

struct Point {
    int x, type, segment;
};

using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);

    int n, a, b, index = 1, m, c;
    vector <Point> points;
    cin >> n;
    while (n--) {
        cin >> a >> b;
        points.push_back({a, TYPE_SEG_START, index});
        points.push_back({ b, TYPE_SEG_END, index });
        index++;
    }
    cin >> m;
    while (m--) {
        cin >> c;
        points.push_back({ c, TYPE_POINT, -1 });
    }

    sort(points.begin(), points.end(), [](Point a, Point b) { return a.x < b.x || (a.x == b.x && a.type < b.type) || (a.x == b.x && a.type == b.type && a.segment < b.segment); });
    stack<int> s;
    for (Point point : points) {
        switch (point.type) {
        case TYPE_SEG_START:
            s.push(point.segment);
            break;
        case TYPE_SEG_END:
            s.pop();
            break;
        case TYPE_POINT:
            if (s.empty()) {
                cout << -1 << endl;
            } else {
                cout << s.top() << endl;
            }
            break;
        }
    }
}