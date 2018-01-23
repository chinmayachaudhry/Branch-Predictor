#ifndef __YEHPATT_H__
#define __YEHPATT_H__
#include "btb.h"
#include <vector>

typedef long long ll;

class yehpatt
{
	private:			
		ll predictions, mispredictions, true_predictions;
		ll index_mask, history_index_bits, predictor_index_bits;
		int index, predictor_table_index, history_table_index;
		int predicted_branch;
		ll  offset_bits, sets, history_sets;
		std::vector<int> predictor_table;
		std::vector<int> history_table;
		std::vector<int> :: iterator it;
		btb *branch_buffer;
		ll mask(ll);
	
	public:
		yehpatt(ll, ll, btb*);
		ll calc_Index(ll address) {return ((address & index_mask) >> 2);}	// 4-Byte instruction. Hence lowest 2 bits are always 0 for address. Hence ignored
		int predict(ll, char);
		void update(int, char);
		void access(ll, char);
		void display_table();
		void print_stats();
};
#endif
