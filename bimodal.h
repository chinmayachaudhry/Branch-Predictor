#ifndef __BIMODAL_H__
#define __BIMODAL_H__
#include "btb.h"
#include <vector>

typedef long long ll;

class bimodal
{
	private:			
		ll predictions, mispredictions, true_predictions;
		ll index_mask;
		int predicted_branch;
		ll  offset_bits, sets;
		std::vector<int> predictor_table;
		std::vector<int> :: iterator it;
		btb *branch_buffer;
		ll mask(ll);
	
	public:
		bimodal(ll, btb*);
		ll calc_Index(ll address) {return ((address & index_mask) >> 2);}	// 4-Byte instruction. Hence lowest 2 bits are always 0 for address. Hence ignored
		int b_predict(ll, char);
		void b_update_prediction_table(ll, int, char);
		void b_access(ll, char);
		void b_display_table();
		void b_print_stats();
};
#endif
