#include <bits/stdc++.h>

using namespace std;

#define HEIGHT 3
#define SIZE 4

typedef array<array<bool, SIZE>, HEIGHT> state;

const state nil = {};

set<state> vis;
map<state, vector<state>> dependencies;

void combinations(int k, int i, int j, state cur, vector<state> &result) {
	if (!k) {
		result.push_back(cur);
		return;
	}
	if (j == SIZE) {
		combinations(k, i+1, 0, cur, result);
		return;
	}
	if (i == HEIGHT) return;
	combinations(k, i, j + 1, cur, result);
	cur[i][j] = 1;
	combinations(k-1, i, j + 1, cur, result);
}
void combinations(vector<state> &result) { combinations(SIZE, 0, 0, nil, result); }

void reduce(state &s) {
	int mn = SIZE;
	for (int row = 0; row < HEIGHT; row++)
		for (int cell = 0; cell < SIZE; cell++)
			if (!s[row][cell])
				mn = min(mn, cell);
	for (int row = 0; row < HEIGHT; row++)
		for (int cell = 0; cell < SIZE; cell++)
			s[row][cell] = (cell + mn < SIZE ? s[row][cell + mn] : 0);
	// Reduce symmetric states
}

int dx[] = {-1, 1, 0, 0};
int dy[] = {0, 0, -1, 1};

bool connected(state s) {
	auto dfs = [&](auto &&self, int i, int j) -> int {
		if (i < 0 || i >= HEIGHT || j < 0 || j >= SIZE) return 1;
		if (!s[i][j]) return 1;
		s[i][j] = 0;
		for (int k = 0; k < 4; k++)
			self(self, i + dx[k], j + dy[k]);
		return 1;
	};
	int cnt = 0;
	for (int i = 0; i < HEIGHT; i++)
		for (int j = 0; j < SIZE; j++)
			if (s[i][j])
				cnt += dfs(dfs, i, j);
	return cnt == 1;
}

state invert(state s) {
	for (int i = 0; i < HEIGHT; i++)
		for (int j = 0; j < SIZE; j++)
			s[i][j] = !s[i][j];
	return s;
}

void f(state cur) {
	reduce(cur);
	if (vis.count(cur)) return;
	vis.insert(cur);
	int req = -1;
	for (int i = HEIGHT - 1; ~i; i--)
		if (!cur[i][0])
			req = i;
	assert (~req);
	vector<state> combis;
	combinations(combis);
	for (state combi : combis) if (combi[req][0]) {
		if (!connected(combi)) continue;
		state next = cur;
		bool good = 1;
		for (int i = 0; i < HEIGHT; i++)
			for (int j = 0; j < SIZE; j++)
				if (combi[i][j]) {
					if (cur[i][j]) good = 0;
					next[i][j] = 1;
				}
		if (!good || !connected(invert(next))) continue;
		reduce(next);
		dependencies[cur].push_back(next);
		f(next);
	}
}

int main() {
	f({0, 0, 0});
	for (auto [s, v] : dependencies) {
		for (auto row : s) {
			for (auto x : row) cout << (x ? 'X' : '-');
			cout << '\n';
		}
		for (auto t : v) {
			for (auto row : t) {
				cout << '\t';
				for (auto x : row) cout << (x ? 'X' : '-');
				cout << '\n';
			}
			cout << '\n';
		}
	}
}
