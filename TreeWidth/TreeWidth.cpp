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
	vector<unordered_set<size_t> > subsets;
	const vector<vector<size_t> >* G;
	vector<size_t> subsetValues;

public:
	Node(const vector<vector<size_t> >* G) : G(G) {}

	Node() : G(NULL) {}

	void appendChild(size_t child) {
		children.push_back(child);
	}

	const vector<size_t>& getChildren() const {
		return children;
	}

	void addPiece(unordered_set<size_t> b) {
		piece = b;
		vector<vector<size_t> > queue;
		queue.push_back(vector<size_t> {*piece.begin()});
		generateAllIS(queue);
	}

	size_t nChildren() const {
		return children.size();
}	

	void generateAllIS(vector<vector<size_t> > queue) {
		auto it = piece.begin();
		while (++it != piece.end()) {
			vector<vector<size_t> > newQueue;
			size_t x = *it;
			while (!queue.empty()) {
				vector<size_t> v = queue.back();
				queue.pop_back();
				if (isIndependent(*G, v)) {
					unordered_set<size_t> s(v.begin(), v.end());
					subsets.push_back(s);
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
			if (isIndependent(*G, v)) {
				unordered_set<size_t> s(v.begin(), v.end());
				subsets.push_back(s);
			}
		}
	}

	vector<unordered_set<size_t> > getSubsets() {
		return subsets;
	}

	vector<size_t> generateValuesLeaf() {
		if (valuesGenerated()) {
			return subsetValues;
		}
		vector<size_t> subsetValues;
		for (unordered_set<size_t> v : subsets) {
			subsetValues.push_back(v.size());
		}
		return subsetValues;
	}

	void pushBackValue(size_t val, const unordered_set<size_t>& toAdd) {
		subsetValues.push_back(val);
		subsets[subsetValues.size() - 1].insert(toAdd.begin(), toAdd.end());
	}

	bool valuesGenerated() {
		return subsetValues.size() == subsets.size();
	}

	vector<size_t> getValues() {
		return subsetValues;
	}

	unordered_set<size_t> getPiece() {
		return piece;
	}
};

bool SubsetIsValid(const unordered_set<size_t>& U_i, const unordered_set<size_t>& U, const unordered_set<size_t>& V_t, const unordered_set<size_t>& V_i) {
	unordered_set<size_t> intersection1, intersection2;
	for (size_t s : U_i) {
		if (V_t.find(s) != V_t.end()) {
			intersection1.insert(s);
		}
	}
	for (size_t s : U) {
		if (V_i.find(s) != V_i.end()) {
			intersection2.insert(s);
		}
	}
	if (intersection1.size() != intersection2.size()) {
		return false;
	}
	for (size_t s : intersection1) {
		if (intersection2.find(s) == intersection2.end()) {
			return false;
		}
	}
	return true;
}

pair<vector<unordered_set<size_t> >, vector<size_t> > MaxIndependentSet(vector<Node>& TD, size_t currentNodeIx) {
	Node& currentNode = TD[currentNodeIx];
	if (currentNode.nChildren() == 0) {
		//Leaf 
		vector<size_t> subsetValues = currentNode.generateValuesLeaf();
		vector<unordered_set<size_t> > independentSets = currentNode.getSubsets();
		return make_pair(independentSets, subsetValues);
	}
	else {
		if (currentNode.valuesGenerated()) {
			return make_pair(currentNode.getSubsets(), currentNode.getValues());
		}
		vector<unordered_set<size_t> > independentSets = currentNode.getSubsets();
		pair<vector<unordered_set<size_t> >, vector<size_t> > result;
		// for all indep sets in Node
		for (unordered_set<size_t> U : independentSets) {
			size_t ft_u = U.size();
			unordered_set<size_t> bestestSet;
			//sum_{i=1}^d
			for (size_t child : currentNode.getChildren()) {
				pair<vector<unordered_set<size_t> >, vector<size_t> > subResult = MaxIndependentSet(TD, child);
				size_t max_f = 0;
				unordered_set<size_t> bestSet;
				//max{ft - w}
				for (int k = 0; k < subResult.first.size(); ++k) {
					size_t f_t = subResult.second[k];
					unordered_set<size_t> U_i = subResult.first[k];
					if (!SubsetIsValid(U_i, U, currentNode.getPiece(), TD[child].getPiece())) {
						continue;
					}
					//Intersection of U_i and U
					size_t intersectionSize = U_i.size();
					for (size_t s : U_i) {
						if (U.find(s) == U.end()) {
							--intersectionSize;
						}
					}
					size_t value = f_t - intersectionSize;
					if (value > max_f) {
						max_f = value;
						bestSet = U_i;
					}
				}
				ft_u += max_f;
				bestestSet.insert(bestSet.begin(), bestSet.end());
			}
			currentNode.pushBackValue(ft_u, bestestSet);
			bestestSet.insert(U.begin(), U.end());
			result.first.push_back(bestestSet);
			result.second.push_back(ft_u);
		}
		return result;
	}	
}

int main()
{
	string filepath = "C:\\Users\\biz\\Documents\\Visual Studio 2015\\Projects\\Treewidth\\data\\";
	string filename = "TruncatedTetrahedralGraph";
	ifstream ifsgr(filepath + filename + ".gr");
	ifstream ifstd(filepath + filename + ".td");

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
		Node n(&G);
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

	pair<vector<unordered_set<size_t> >, vector<size_t> > maxIS = MaxIndependentSet(TD, root);
	for (size_t k = 0; k < maxIS.first.size(); ++k) {
		cout << "IS size: " << maxIS.second[k] << endl;
		for (size_t s : maxIS.first[k]) {
			cout << s << " ";
		}
		cout << endl;
	}
	

    return 0;
}

