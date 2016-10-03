// TreeWidth.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_set>

using namespace std;
typedef unordered_set<size_t> bag;

class Node {
private:
	vector<size_t> children;
	bag piece;
	vector<pair<size_t, vector<size_t> > > subsets;

public:
	Node() {}

	void appendChild(size_t child) {
		children.push_back(child);
	}

	const vector<size_t>& getChildren() const {
		return children;
	}
};

int main()
{
	string filepath = "C:\\Users\\biz\\Documents\\Visual Studio 2015\\Projects\\Treewidth\\data\\";
	ifstream ifsgr(filepath + "web4.gr");
	ifstream ifstd(filepath + "web4.td");

	string line;
	size_t nNodes, nEdges;
	while (getline(ifsgr, line)) {
		if (line[0] == 'c') {
			continue;
		}
		if (line[0] == 'p') {
			istringstream iss(line);
			string temp;
			iss >> temp >> temp >> nNodes >> nEdges;
			break;
		}
	}
	vector<vector<size_t> > G(nNodes + 1);
	while (getline(ifsgr, line)) {
		if (line[0] == 'c') {
			continue;
		}
		istringstream iss(line);
		size_t v1, v2;
		iss >> v1 >> v2;
		G[v1].push_back(v2);
		G[v2].push_back(v1);
	}
	ifsgr.close();

	size_t nBags;
	while (getline(ifstd, line)) {
		if (line[0] == 'c') {
			continue;
		}
		if (line[0] == 's') {
			istringstream iss(line);
			string temp;
			iss >> temp >> temp >> nBags;
			break;
		}
	}

	vector<bag> bags(nBags + 1);
	vector<vector<size_t> > T(nBags + 1);
	while (getline(ifstd, line)) {
		if (line[0] == 'c') {
			continue;
		}
		if (line[0] == 'b') {
			istringstream iss(line);
			string temp;
			size_t i, k;
			iss >> temp >> i;
			while (iss >> k) {
				bags[i].insert(k);
			}
		}
		else {
			istringstream iss(line);
			size_t b1, b2;
			iss >> b1 >> b2;
			T[b1].push_back(b2);
			T[b2].push_back(b1);
		}
	}

	size_t i = 1;
	while (T[i].size() > 1) {
		++i;
	}

	size_t nVisited = 0;
	vector<bool>hasVisited(vector<bool>(nBags, false));
	vector<size_t> curr = T[i];
	vector<size_t> queue;
	while (nVisited < nBags)
	{
		for (size_t q : curr)
		{
			if (!hasVisited[q]) {
				hasVisited[q] = true;
				++nVisited;
				queue.push_back(q);
			}
		}
		curr = T[queue.back()];
		queue.pop_back();
	}



    return 0;
}

