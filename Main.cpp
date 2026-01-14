#include <iostream>
#include <string>
#include <chrono>
#include <fstream>

#include "GraphColoring.h"


using namespace std;
using namespace std::chrono;
using namespace szx;


void loadInput(istream& is, GraphColoring& gc) {
	is >> gc.nodeNum >> gc.edgeNum >> gc.colorNum;
	gc.edges.resize(gc.edgeNum);
	for (auto edge = gc.edges.begin(); edge != gc.edges.end(); ++edge) { is >> (*edge)[0] >> (*edge)[1]; }
}

void saveOutput(ostream& os, NodeColors& nodeColors) {
	for (auto color = nodeColors.begin(); color != nodeColors.end(); ++color) { os << *color << endl; }
}

void test(istream& inputStream, ostream& outputStream, long long secTimeout, int randSeed) {
	GraphColoring gc;
	loadInput(inputStream, gc);

	steady_clock::time_point endTime = steady_clock::now() + seconds(secTimeout);
	NodeColors nodeColors(gc.nodeNum);

	//auto st = clock();

	solveGraphColoring(nodeColors, gc, [&]() { return duration_cast<milliseconds>(endTime - steady_clock::now()).count(); }, randSeed);

	//cout << endl << clock() - st << endl;

	saveOutput(outputStream, nodeColors);
}
void test(istream& inputStream, ostream& outputStream, long long secTimeout) {
	return test(inputStream, outputStream, secTimeout, static_cast<int>(time(nullptr) + clock()));
}


int main(int argc, char* argv[]) {
	//cerr << "load environment." << endl;
	if (argc > 2) {
		long long secTimeout = atoll(argv[1]);
		int randSeed = atoi(argv[2]);
		test(cin, cout, secTimeout, randSeed);
	} else {
		ifstream ifs("to/instances/DSJC0250.5.txt");
		//ofstream ofs("to/DSJC1000.5.txt");
		//test(ifs, ofs, 10); // for self-test.
		test(ifs, cout, 100000);
	}
	return 0;
}
