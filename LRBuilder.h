#pragma once
#include "TokenManager.h"
#include "ProductionManager.h"
#include "LRProduction.h"
#include "LRState.h"
#include "TOKEN.h"

enum LRAction {
	SHIFT=0,
	REDUCE=1,
	GOTO=2,
	ACCEPT=3,
	ERROR=4
};

struct LRTableItem{
	LRAction action;
	int target;
};

class LRBuilder {
	friend class CompilerGenerater;
public:
	LRBuilder(TokenManager&, ProductionManager&);
private:
	TokenManager& tokenManager;
	ProductionManager& productionManager;
	map<LRState, int> lrstatus;
	map<int, LRState > lrstatus_id;
	map<int, vector<int>> first; 
	map<int, bool> nullable;
	map<NonterminalToken, vector<Production> > productionbuffer;
	map<vector<int>, vector<int> > firstbuffer;
public:
	int build(const string& start);
	
private:
	int buildState(vector<LRProduction> initProduction);
	void buildTable(const string& start);
public:
	int startState;
	int initFirst();
	vector<int> getFirst(const vector<int>& tokens);
	int findState(const LRState& state);
	vector<vector<LRTableItem>> lrTable;
	
};

