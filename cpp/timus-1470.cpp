#include <iostream>
#include <vector>

#define MAXN 130

using namespace std;

class FenwickTree {
    int t[MAXN][MAXN][MAXN];
public:
    FenwickTree() {
        for (int i = 0; i < MAXN; i++) {
            for (int j = 0; j < MAXN; j++) {
                for (int k = 0; k < MAXN; k++) {
                    t[i][j][k] = 0;
                }
            }
        }
    }

    void add(int x, int y, int z, int value) {
        int xc = x + 1, yc, zc;
        while (xc < MAXN) {
            yc = y + 1;
            while (yc < MAXN) {
                zc = z + 1;
                while (zc < MAXN) {
                    t[xc][yc][zc] += value;
                    zc += zc & -zc;
                }
                yc += yc & -yc;
            }
            xc += xc & -xc;
        }
    }

    int sum(int x, int y, int z) {
        int xc = x + 1, yc, zc, sum = 0;
        while (xc) {
            yc = y + 1;
            while (yc) {
                zc = z + 1;
                while (zc) {
                    sum += t[xc][yc][zc];
                    zc -= zc & -zc;
                }
                yc -= yc & -yc;
            }
            xc -= xc & -xc;
        }
        return sum;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);

    int N, type, xf, xt, yf, yt, zf, zt, k, res;

    FenwickTree* tree = new FenwickTree;
    cin >> N;
    while (true) {
        cin >> type;
        if (type == 1) {
            cin >> xf >> yf >> zf >> k;
            tree->add(xf, yf, zf, k);
        } else if (type == 2) {
            cin >> xf >> yf >> zf >> xt >> yt >> zt;
            res = tree->sum(xt, yt, zt);
            res -= tree->sum(xf - 1, yt, zt) + tree->sum(xt, yf - 1, zt) + tree->sum(xt, yt, zf - 1) + tree->sum(xf - 1, yf - 1, zf - 1);
            res += tree->sum(xf - 1, yf - 1, zt) + tree->sum(xf - 1, yt, zf - 1) + tree->sum(xt, yf - 1, zf - 1);

            cout << res << endl;
        } else {
            return 0;
        }
    }
}