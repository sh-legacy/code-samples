#include <algorithm>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

class Solution {
private:
    bool* used;
    int * first, index, *d;
    vector <int> *input;
    vector <pair<int, int>> euler, tree; // first = vertex, second = height.
    //pair<int, int>* tree;
    map<pair<int, int>, int> w;

    void build(int v, int l, int r) {
        if (l == r) {
            tree[v] = euler[l];
        }
        else {
            int middle = (l + r) / 2;
            build(v * 2, l, middle);
            build(v * 2 + 1, middle + 1, r);

            tree[v] = (tree[v * 2].second < tree[v * 2 + 1].second) ? tree[v * 2] : tree[v * 2 + 1];
        }
    }

    pair<int, int> getMin(int l, int r, int v, int cl, int cr) {
        if (l > r) {
            return { -1, 1000000000 };
        }
        if (cl == l && cr == r) {
            return tree[v];
        }
        int cm = (cl + cr) / 2;
        auto a = getMin(l, min(cm, r), v * 2, cl, cm);
        auto b = getMin(max(cm + 1, l), r, v * 2 + 1, cm + 1, cr);
        return (a.second < b.second) ? a : b;
    }

    void dfsMod(int v, int preLength = 0, int currentHeight = 0) {
        used[v] = true;
        euler.push_back({ v, currentHeight });
        index++;
        for (int curr : input[v]) {
            if (!used[curr]) {
                first[curr] = index;
                d[curr] = preLength + w[{v, curr}];
                dfsMod(curr, d[curr], currentHeight + 1);
                euler.push_back({ v, currentHeight });
                index++;
            }
        }
    }

    int getLCA(int a, int b) {
        return this->getMin(min(first[a], first[b]), max(first[a], first[b]), 1, 0, index - 1).first;
    }

public:
    Solution(vector<int>* t, int n, const map<pair<int, int>, int> &weights) {
        used = new bool[n];
        fill(used, used + n, false);
        first = new int[n];
        d = new int[n];
        first[0] = d[0] = index = 0;
        input = t;
        w = weights;
        dfsMod(0);

        tree.resize(euler.size() * 4);
        build(1, 0, euler.size() - 1);
    }

    int getDistance(int a, int b) {
        int lca = this->getLCA(a, b);
        return d[a] + d[b] - 2 * d[lca];
    }

};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);

    vector<int> t[50010];
    map<pair<int, int>, int> w;
    int n, a, b, cw, q;

    cin >> n;

    for (int i = 1; i < n; i++) {
        cin >> a >> b >> cw;

        t[a].push_back(b);
        t[b].push_back(a);

        w[{a, b}] = cw;
        w[{b, a}] = cw;
    }

    Solution* s = new Solution(t, n, w);

    cin >> q;
    while (q--) {
        cin >> a >> b;
        cout << s->getDistance(a, b) << endl;
    }
}