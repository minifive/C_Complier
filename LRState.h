#include "stdafx.h"
#include "LRProduction.h"
#include "TOKEN.h"

#ifndef _LRSTATE_H
#define _LRSTATE_H

class LRState {
public:    
	vector<LRProduction> productions;//存放LR状态中的所有产生式
    map<Token,int> action;           //存放LR状态的语义动作
	bool operator<(const LRState& r)const {
		for (size_t i = 0; i < min(productions.size(), r.productions.size()); i++) {
			if (!(productions[i] == r.productions[i])) {
				return productions[i] < r.productions[i];
			}
		}
		return productions.size() < r.productions.size();
	}

	bool addLALR(const LRState& r){

		for (size_t i = 0; i < min(productions.size(), r.productions.size()); i++)
		{
			if (productions[i].LALRequal(r.productions[i]))
				productions.push_back(r.productions[i]);
		}
	}
};

#endif //_LRSTATE_H