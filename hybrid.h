#ifndef __HYBRID_H__
#define __HYBRID_H__
#include "btb.h"
#include "bimodal.h"
#include "gshare.h"
#include <vector>

typedef long long ll;

class hybrid
{
	private:			
		ll predictions, mispredictions, true_predictions;
		ll index_mask;
		int chooser_table_index;
		int choosen_predictor, bimodal_prediction, gshare_prediction;
		ll  offset_bits, sets;
		std::vector<int> chooser_table;
		std::vector<int> :: iterator it;
		btb *branch_buffer;
		ll mask(ll);
		bimodal *b;
		gshare *g;		

	public:
		hybrid(ll, ll, ll, ll, btb*);
		ll calc_Index(ll address) {return ((address & index_mask) >> 2);}	// 4-Byte instruction. Hence lowest 2 bits are always 0 for address. Hence ignored
		int predict(ll, char);
		void update_chooser_table(ll, int, char);
		void access(ll, char);
		void display_table();
		void print_stats();
};
#endif
