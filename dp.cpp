#include <bits/stdc++.h>

using namespace std;

#define HEIGHT 3
#define SIZE 3

typedef array<int, HEIGHT> state;
typedef array<int, HEIGHT> spread;

set<state> vis;
map<state, vector<state>> dependencies;

void combinations(int k, int i, spread cur, vector<spread> &result) {
	if (!k) {
		result.push_back(cur);
		return;
	}
	if (i < HEIGHT - 1) combinations(k, i+1, cur, result);
	cur[i]++;
	combinations(k-1, i, cur, result);
}
void combinations(vector<spread> &result) { combinations(SIZE, 0, {0, 0, 0}, result); }

void reduce(state &s) {
	int mn = 1<<30;
	for (auto x : s) mn = min(mn, x);
	for (auto &x : s) x -= mn;
	for (int i = 0; i < HEIGHT / 2; i++) {
		if (s[i] < s[HEIGHT - i - 1]) return;
		break;
	}
	for (int i = 0; i < HEIGHT / 2; i++)
		swap(s[i], s[HEIGHT - i - 1]);
}

void f(state cur) {
	reduce(cur);
	if (vis.count(cur)) return;
	vis.insert(cur);
	int req = -1;
	for (int i = HEIGHT - 1; ~i; i--)
		if (!cur[i])
			req = i;
	assert (~req);
	vector<spread> combis;
	combinations(combis);
	for (spread combi : combis) if (combi[req]) {
		bool good = 1;
		int flag = 0;
		state next = cur;
		for (int i = 0; i < HEIGHT; i++) next[i] += combi[i];
		for (int i = 0; i < HEIGHT; i++) {
			if (flag == 0 && combi[i]) flag = 1;
			else if (flag == 1 && !combi[i]) flag = 2;
			else if (flag == 2 && combi[i]) good = 0;
			if (!i || !combi[i] || !combi[i-1]) continue;
			good &= next[i] > cur[i-1] && next[i-1] > cur[i];
		}
		if (!good) continue;
		reduce(next);
		dependencies[cur].push_back(next);
		f(next);
	}
}

int main() {
	f({0, 0, 0});
	for (auto [s, v] : dependencies) {
		for (auto x : s) cout << x << ' ';
		cout << ":\n";
		for (auto t : v) {
			cout << '\t';
			for (auto x : t) cout << x << ' ';
			cout << '\n';
		}
	}
}
