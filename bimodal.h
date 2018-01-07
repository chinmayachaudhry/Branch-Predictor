#ifndef __BIMODAL_H__
#define __BIMODAL_H__
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <stdio.h>
#include <vector>
#include <bitset>
#include <iomanip>

using namespace std;
typedef long long ll;

class bimodal
{
	private:			
		ll predictions, mispredictions;
		ll branch_pred_outcome, index_mask;
		int  offset_bits, sets;
		vector<int> predictor_table;
		vector<int> :: iterator it;
		ll mask(int N)
		{
			ll mask = 0
			for(int i = 0; i<N; i++)
			{
				mask << = 1;
				mask |= 1;
			} 
			return (mask);
		}
	public:
		void bimodal(int index_bits)
		{
			predictions = mispredictions = 0;
			offset_bits = 2;
			index_mask = mask(index_bits + offset_bits);
			sets = (int)pow(2.0, index_bits);
		
			for(int i = 0; i<sets; i++)
				predictor_table[i] = 1;
		}
		
		ll calc_Index(ll address)
		{
			return (((ll)(address) & (index_mask)) >> offset_bits);
		}

		
		void access(ll address, char actual_branch)
		{
			this->predictions++;
			char predicted_branch = predict(address, actual_branch);
			update_prediction_table(address, predicted_branch, actual_branch);
		}

		char predict(ll address, char actual_branch)
		{	
			ll index = calc_Index(address);
			if(predictor_table.at(index) >= 2)
				return('t');
			else
				return('n');
		}
		
		void update_prediction_table(ll address, char predicted_branch, char actual_prediction)
		{
			ll index = calc_Index(address);
			if(actual_branch == 't')
			{
				if(predictor_table.at(index) < 3)
					predictor_table.at(index)++;
				if(predicted_branch == 'n')
					mispredictions++;	
			}		
			else if(actual_branch == 'n')
			{	
				if(predictor_table.at(index) > 0)
					predictor_table.at(index)--;
				if(predicted_branch == 't')
					mispredictions++;	
			}
		}

		void display_table()
		{
			cout << "Final Bimodal Table Contents:" << endl;
			for(it = predictor_table.begin(); it != predictor_table.end(), it++)
			{
				int index = distance(predictor_table.begin(), it); 
				cout << "table[" << index << "]: " << predictor_table.at(index) << endl;	
			}
		}


		void print_stats()
		{
			cout << "Final Branch Predictor Statistics:" << endl;
			cout << "a. Number of branches: " << predictions << endl;
			cout << "b. Number of prediction from the branch predictor: " << predictions << endl;
			cout << "c. Number of mispredictins from the branch predictor: " << mispredictions << endl;
			cout << "d. Number of misprediction from the BTB: " << "0" << endl;
			cout << "e.Misprediction Rate: " << fixed << setprecision(2) << (float)((mispredictions_BTB + mispredictions)/(float)(predictions))*100 << " percent" << endl;
		
		}

#endif
