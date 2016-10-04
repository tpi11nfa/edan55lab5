// TreeWidth.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_set>
#include <deque>

using namespace std;

bool isIndependent(const vector<vector<size_t> >& G, vector<size_t> set) {
	for (size_t t : set) {
		for (size_t s : G[t]) {
			if (find(set.begin(), set.end(), s) != set.end()) {
				return false;
			}
		}
	}
	return true;
}

class Node {
private:
	vector<size_t> children;
	unordered_set<size_t> piece;
	vector<vector<size_t> > subsets;

public:
	Node() {}

	void appendChild(size_t child) {
		children.push_back(child);
	}

	const vector<size_t>& getChildren() const {
		return children;
	}

	void addPiece(unordered_set<size_t> b) {
		piece = b;
	}

	size_t nChildren() const {
		return children.size();
	}

	void generateISHelp(const vector<vector<size_t> >& G, vector<vector<size_t> > queue) {
		auto it = piece.begin();
		while (++it != piece.end()) {
			vector<vector<size_t> > newQueue;
			size_t x = *it;
			while (!queue.empty()) {
				vector<size_t> v = queue.back();
				queue.pop_back();
				if (isIndependent(G, v)) {
					subsets.push_back(v);
				}

				if (v.back() == *--piece.end()) {
					continue;
				}
				vector<size_t> v2 = v;
				
				v2.pop_back();
				v2.push_back(x);
				v.push_back(x);
				newQueue.push_back(v2);
				newQueue.push_back(v);
			}
			queue = newQueue;
		}
		while (!queue.empty()) {
			vector<size_t> v = queue.back();
			queue.pop_back();
			if (isIndependent(G, v)) {
				subsets.push_back(v);
			}
		}
	}

	void generateIS(const vector<vector<size_t> >& G) {
		vector<vector<size_t> > queue;
		queue.push_back(vector<size_t> {*piece.begin()});
		return generateISHelp(G, queue);
	}
};

vector<size_t> MaxIndependentSet(const vector<vector<size_t> >& G, vector<Node>& TD, size_t currentNode) {
	vector<size_t> result;
	if (TD[currentNode].nChildren() == 0) {
		//Leaf 
		TD[currentNode].generateIS(G);
	}
	else {
		//Recursion 10.18

		return result;
	}	
}

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

	vector<unordered_set<size_t> > bags(nBags + 1);
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

	size_t root = 1;
	while (T[root].size() > 1) {
		++root;
	}

	vector<bool> hasVisited(nBags + 1, false);
	size_t curr = root;
	deque<size_t> queue;
	vector<Node> TD(nBags + 1);
	queue.push_back(curr);

	while (!queue.empty())
	{
		curr = queue.front();
		queue.pop_front();
		hasVisited[curr] = true;
		Node n;
		n.addPiece(bags[curr]);
		for (size_t q : T[curr])
		{
			if (!hasVisited[q]) {
				queue.push_back(q);
				n.appendChild(q);
			}
		}
		TD[curr] = n;
	}

	MaxIndependentSet(G, TD, root);

    return 0;
}

