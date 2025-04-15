#include <bits/stdc++.h>

using namespace std;

#define HEIGHT 3
#define SIZE 4

typedef long long ll;
typedef array<array<bool, SIZE>, HEIGHT> state;

struct matrix{
	int n, m;
	vector<vector<int>> vals;
	matrix(int _n, int _m) : n(_n), m(_m), vals(_n, vector<int>(_m, 0)) {}
};

constexpr const int MOD = 1e9 + 7;

matrix mul(matrix A, matrix B) {
	assert (A.m == B.n);
	matrix C(A.n, B.m);
	for (int i = 0; i < A.n; i++)
		for (int j = 0; j < B.m; j++)
			for (int k = 0; k < A.m; k++)
				C.vals[i][j] = (C.vals[i][j] + (1LL * A.vals[i][k] * B.vals[k][j] % MOD)) % MOD;
	return C;
}

matrix exp(matrix A, ll n) {
	assert (A.n == A.m);
	assert (n > 0);
	if (n == 1) return A;
	matrix B = exp(A, n / 2);
	B = mul(B, B);
	if (n & 1) B = mul(A, B);
	return B;
}

int matrix_rank(matrix& mat) {
	// Thanks GPT!
	int n = mat.n;
	int m = mat.m;
	auto vals = mat.vals; // Work on a copy to avoid modifying the original

	int rank = 0;
	for (int col = 0; col < m && rank < n; ++col) {
		// Find pivot row
		int pivot = rank;
		while (pivot < n && vals[pivot][col] == 0)
			++pivot;

		if (pivot == n) continue; // No pivot in this column

		// Swap current row with pivot row
		std::swap(vals[rank], vals[pivot]);

		// Eliminate below
		for (int i = 0; i < n; ++i) {
			if (i != rank && vals[i][col] != 0) {
				int factor = vals[i][col];
				int pivot_factor = vals[rank][col];
				for (int j = col; j < m; ++j) {
					vals[i][j] = vals[i][j] * pivot_factor - vals[rank][j] * factor;
				}
			}
		}

		++rank;
	}

	return rank;
}

const state nil = {};

set<state> vis;
map<state, vector<state>> dependencies;
array<vector<state>, HEIGHT> combinations;

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

void generate_combinations(int k, int i, int j, state cur) {
	if (!k) {
		if (connected(cur)) { for (int l = 0; l < HEIGHT; l++)
				if (cur[l][0])
					combinations[l].push_back(cur);
		}
		return;
	}
	if (j == SIZE) {
		generate_combinations(k, i+1, 0, cur);
		return;
	}
	if (i == HEIGHT) return;
	generate_combinations(k, i, j + 1, cur);
	cur[i][j] = 1;
	generate_combinations(k-1, i, j + 1, cur);
}
void generate_combinations() { generate_combinations(SIZE, 0, 0, nil); }

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
	for (int row = 0; row < HEIGHT / 2; row++)
		for (int cell = 0; cell < SIZE; cell++)
			if (s[row][cell] < s[HEIGHT - row - 1][cell]) return;
	for (int row = 0; row < HEIGHT / 2; row++)
		swap(s[row], s[HEIGHT - row - 1]);
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
	for (int i = 0; i < HEIGHT; i++)
		if (!cur[i][0])
			req = i;
	assert (~req);
	for (state combi : combinations[req]) {
		if (!connected(combi)) continue;
		state next = cur;
		bool good = 1;
		for (int i = 0; i < HEIGHT; i++)
			for (int j = 0; j < SIZE; j++)
				if (combi[i][j]) {
					if (cur[i][j]) good = 0;
					next[i][j] = 1;
				}
		reduce(next);
		if (!good || !connected(invert(next))) continue;
		dependencies[cur].push_back(next);
		f(next);
	}
}

void print_dependencies() {
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

int count (state s) {
	int cnt = 0;
	for (int i = 0; i < HEIGHT; i++)
		for (int j = 0; j < SIZE; j++)
			if (s[i][j])
				cnt++;
	return cnt;
}

void print_matrix(matrix M) {
	for (auto row : M.vals) {
		for (auto x : row) cout << x << ' ';
		cout << '\n';
	}
}

int main() {
	generate_combinations();
	f(nil);
	//print_dependencies();
	array<map<state, int>, HEIGHT> groups;
	array<int, HEIGHT> group_size = {};
	for (auto s : vis) {
		int gr = count(s) % HEIGHT;
		groups[gr][s] = group_size[gr]++;
	}
	int gr = 0;
	matrix acc(0, 0);
	do {
		int nxt = (gr + SIZE) % HEIGHT;
		matrix M(group_size[gr], group_size[nxt]);
		for (auto [s, i] : groups[gr])
			for (auto t : dependencies[s])
				M.vals[i][groups[nxt][t]]++;
		//cout << M.n << ' ' << matrix_rank(M) << '\n';
		//print_matrix(M); cout << '\n';
		if (!gr) acc = M;
		else acc = mul(acc, M);
		gr = nxt;
	} while (gr);
	print_matrix(acc); cout << '\n';
}
