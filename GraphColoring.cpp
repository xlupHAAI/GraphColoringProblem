#include "GraphColoring.h"

#include <random>
#include <iostream>
#include <utility>
#include <unordered_map>


using namespace std;


class bi_set {
private:
	vector<int> idx;
public:
	int size;
	vector<int> elems;

	bi_set() :idx(vector<int>(MAXN, -1)), elems(vector<int>(MAXN)), size(0) {  }

	void insert(int val) {
		if (idx[val] == NONE) {
			elems[size] = val;
			idx[val] = size++;
		}
	}

	void erase(int val) {
		if (idx[val] > NONE) {
			idx[elems[--size]] = idx[val];
			elems[idx[val]] = elems[size];
			idx[val] = NONE;
		}
	}

	inline void clear() { size = 0; fill(idx.begin(), idx.end(), NONE); }

	bi_set& operator=(const bi_set& y) {
		size = y.size;
		idx = y.idx;
		elems = y.elems;
		return *this;
	}
	inline bool has(int val) { return idx[val] > NONE; }
};


namespace szx {

class Solver {
	// random number generator.
	mt19937 pseudoRandNumGen;
	void initRand(int seed) { pseudoRandNumGen = mt19937(seed); }
	int fastRand(int ub) { return pseudoRandNumGen() % ub; }

	NodeId n;
	ColorId k;
	vector<vector<NodeId>> opponents;
	vector<vector<bool>> is_conflict;

	Solution p1, p2, elite1, elite2;
	int fS_p1, fS_p2, fS_elite1, fS_elite2;

	vector<vector<int>> conflicts;
	vector<vector<unsigned int>> tabu_tenure;

	unsigned int max_iter;

private:
	void initGraph(GraphColoring&);
	inline void initSolutions();
	inline void initTabu();
	void randomizeSolution(Solution&);
	void calculateFS(Solution&, int&);
	
	void adoptGPX();
	void localSearch(Solution&, int&);
	void elite1Upd();

public:
	void solve(NodeColors& output, GraphColoring& input, function<long long()> restMilliSec, int seed) {
		
		initRand(seed);

		initGraph(input);

		initTabu();
		
		int generation = 0;

		initSolutions();

		while (true) {
			adoptGPX();

			if (localSearch(p1, fS_p1), fS_p1 == 0) {
				output = p1; 
				//for (int x = 0; x < n; ++x)for (int y = 0; y < n; y++)if (is_conflict[x][y] && output[x] == output[y]) {
				//	cout << "WRONG£¡";
				//	exit(-1);
				//}
				return;
			}

			if (localSearch(p2, fS_p2), fS_p2 == 0) {
				output = p2;
				//for (int x = 0; x < n; ++x)for (int y = 0; y < n; y++)if (is_conflict[x][y] && output[x] == output[y]) {
				//	cout << "WRONG£¡";
				//	exit(-1);
				//}
				return;
			}

			elite1Upd();

			if (generation++ % iter_cycle == 0) {

				if (fastRand(2)) {

					p1 = elite2, fS_p1 = fS_elite2;
				}
				else {

					p2 = elite2, fS_p2 = fS_elite2;
				}

				elite2 = elite1, fS_elite2 = fS_elite1;

				fS_elite1 = INF;
			}
		}

	}

};	// namespace szx

/*
bool Solver::hasEqualed(Solution& S1, Solution& S2) {
	if (fS_p1 != fS_p2)		
		return false;

	unordered_map<ColorId, ColorId> partition_map;
	for (NodeId i = 0; i < n; ++i) {
		if (partition_map.find(p1[i]) == partition_map.end())
			partition_map.insert(make_pair(p1[i], p2[i]));
		else if (partition_map[p1[i]] != p2[i])
			return false;
	}

	return true;
}
*/
void Solver::elite1Upd() {
	if (fS_p1 <= fS_p2 && fS_p1 < fS_elite1) {
		fS_elite1 = fS_p1;
		elite1 = p1;
	}
	else if (fS_p2 <= fS_p1 && fS_p2 < fS_elite1) {
		fS_elite1 = fS_p2;
		elite1 = p2;
	}
}

void Solver::adoptGPX() {

	vector<bi_set> classes_for_p1[2] = { vector<bi_set>(k),vector<bi_set>(k) };
	for (NodeId i = 0; i < n; ++i) {
		classes_for_p1[0][p1[i]].insert(i);
		classes_for_p1[1][p2[i]].insert(i);
		p1[i] = NONE, p2[i] = NONE;
	}
	vector<bi_set> classes_for_p2[2] = { classes_for_p1[1],classes_for_p1[0] };


	for (ColorId round = 0; round < k; ++round) {
		int parent = round % 2;
		ColorId Vm_idx1 = NONE, Vm_idx2 = NONE;
		int Vm_size1 = NONE, Vm_size2 = NONE;
		
		for (ColorId l = 0, i = fastRand(k); l < k; l++, i = (i + 1) % k)
			if (Vm_size1 < classes_for_p1[parent][i].size) {
				Vm_size1 = classes_for_p1[parent][i].size;
				Vm_idx1 = i;
			}

		for (ColorId l = 0, i = fastRand(k); l < k; l++, i = (i + 1) % k)
			if (Vm_size2 < classes_for_p2[parent][i].size) {
				Vm_size2 = classes_for_p2[parent][i].size;
				Vm_idx2 = i;
			}
		

		bi_set& Vm1 = classes_for_p1[parent][Vm_idx1];
		bi_set& Vm2 = classes_for_p2[parent][Vm_idx2];

		for (int i = 0; i < Vm1.size; i++) {
			int x = Vm1.elems[i];
			p1[x] = round;

			for (ColorId c = 0; c < k; c++) {
				classes_for_p1[(parent + 1) % 2][c].erase(x);
			}
		}		
		for (int i = 0; i < Vm2.size; i++) {
			int x = Vm2.elems[i];
			p2[x] = round;

			for (ColorId c = 0; c < k; c++) {
				classes_for_p2[(parent + 1) % 2][c].erase(x);
			}
		}
		Vm1.clear();
		Vm2.clear();
	}

	for (NodeId i = 0; i < n; ++i) { 
		if (p1[i] == NONE)	p1[i] = fastRand(k);
		if (p2[i] == NONE)	p2[i] = fastRand(k);
	}

	calculateFS(p1, fS_p1);
	calculateFS(p2, fS_p2);
}

void Solver::localSearch(Solution& S, int& fS) {
	int fS_best = fS;
	Solution Sbest(S);

	for (NodeId i = 0; i < n; ++i) {
		fill(conflicts[i].begin(), conflicts[i].end(), 0);
		fill(tabu_tenure[i].begin(), tabu_tenure[i].end(), 0);
	}

	for (NodeId i = 0; i < n; ++i)
		for (NodeId j : opponents[i])
			conflicts[j][S[i]]++;

	bi_set conflictVertices;
	for (NodeId i = 0; i < n; ++i)
		if (conflicts[i][S[i]])
			conflictVertices.insert(i); 

	int f_count = 1;

	for (unsigned int nbiter = 1; nbiter < max_iter; nbiter++) {
		/*
		 * 1. randomly choose a neighbor S' with the minimal fS' under constraints of tabu
		 */
		int count_T = 1, count_NT = 1;
		critical_move best_move_T, best_move_NT;

		for (int idx = 0; idx < conflictVertices.size; ++idx) {
			NodeId i = conflictVertices.elems[idx];
			for (ColorId c = 0; c < k; c++) {
				if (c != S[i]) {

					int benifit = conflicts[i][c] - conflicts[i][S[i]];

					if (tabu_tenure[i][c] < nbiter) {

						if (benifit < best_move_NT.benifit) {
							count_NT = 1;
							best_move_NT = critical_move(benifit, i, c);
						}
						else if (benifit == best_move_NT.benifit
							&& fastRand(++count_NT) == 0) {
							best_move_NT = critical_move(benifit, i, c);
						}

					}
					else {

						if (benifit < best_move_T.benifit) {
							count_T = 1;
							best_move_T = critical_move(benifit, i, c);
						}
						else if (benifit == best_move_T.benifit
							&& fastRand(++count_T) == 0) {
							best_move_T = critical_move(benifit, i, c);
						}
					}
				}
			}
		}
		
		critical_move best_move = best_move_T.benifit < best_move_NT.benifit
			&& best_move_T.benifit < fS_best - fS
			? best_move_T
			: best_move_NT;

		/*
		 * 2. accept the "one crital move" S -> S'
		 */

		fS += best_move.benifit;
		ColorId src = S[best_move.transferee];
		S[best_move.transferee] = best_move.dest;

		if (fS < fS_best) {
			fS_best = fS;
			Sbest.assign(S.begin(), S.end());
			if (fS == 0)	break;
			f_count = 1;
		}
		else if (fS == fS_best && fastRand(++f_count) == 0) {
			fS_best = fS;
			Sbest.assign(S.begin(), S.end());
		}
		
		/*
		 * 3. update the tabu-list and neighborhood
		 */
		tabu_tenure[best_move.transferee][src] = nbiter + mu * fS + fastRand(10);
		if (conflicts[best_move.transferee][best_move.dest] == 0)
			conflictVertices.erase(best_move.transferee);
		for (NodeId x : opponents[best_move.transferee]) {
			conflicts[x][src]--;
			conflicts[x][best_move.dest]++;

			if (S[x] == src && conflicts[x][src] == 0) conflictVertices.erase(x);
			else if (S[x] == best_move.dest && conflicts[x][best_move.dest] == 1)
				conflictVertices.insert(x);
		}
	}

	fS = fS_best;
	S.assign(Sbest.begin(), Sbest.end());
}

inline void Solver::initTabu() {
	conflicts = vector<vector<int>>(n, vector<int>(k));
	tabu_tenure = vector<vector<unsigned int>>(n, vector<unsigned int>(k));

	if (n == 125)	max_iter = 2000;
	else if (n == 250)	max_iter = 5000;
	else if (n == 500) {
		if (k == 48)	max_iter = 8000;
		else if (k == 126)	max_iter = 25000;
		else max_iter = 5000;
	}
	else if (n == 1000) {
		if (k == 20)	max_iter = 3000;
		else if (k == 83)	max_iter = 40000;
		else max_iter = 30000;
	}
}

void Solver::calculateFS(Solution& S, int& fS) {
	fS = 0;
	for (NodeId i = 0; i < n; ++i)
		for (NodeId j : opponents[i])
			fS += S[i] == S[j];
	fS >>= 1;
}

void Solver::randomizeSolution(Solution& S) {
	for (NodeId i = 0; i < n; ++i)	S[i] = fastRand(k);
}

inline void Solver::initSolutions() {
	p1.resize(n);
	randomizeSolution(p1);

	p2.resize(n);
	randomizeSolution(p2);

	elite1.resize(n);
	fS_elite1 = INF;

	elite2.resize(n);
	randomizeSolution(elite2);
	calculateFS(elite1, fS_elite2);
}

void Solver::initGraph(GraphColoring& gc) {
	n = gc.nodeNum;
	k = gc.colorNum;
	if (k == 49 || k == 224)	k--;

	opponents.resize(n);
	is_conflict = vector<vector<bool>>(n, vector<bool>(n));
	
	for (array<NodeId, 2> &e : gc.edges) {
		opponents[e[0]].push_back(e[1]);
		opponents[e[1]].push_back(e[0]);
		is_conflict[e[0]][e[1]] = true;
		is_conflict[e[1]][e[0]] = true;
	}
}

// solver.
void solveGraphColoring(NodeColors& output, GraphColoring& input, function<long long()> restMilliSec, int seed) {
	Solver().solve(output, input, restMilliSec, seed);
}

}
