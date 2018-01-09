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
		ll predictions, mispredictions, true_predictions;
		ll index_mask;
		int predicted_branch;
		ll  offset_bits, sets;
		vector<int> predictor_table;
		vector<int> :: iterator it;
		ll mask(ll N)
		{
			ll mask = 0;
			for(ll i = 0; i<N; i++)
			{
				mask <<= 1;
				mask |= 1;
			} 
			return (mask);
		}
	
	public:
		bimodal(ll index_bits)
		{
			predictions = mispredictions = true_predictions = 0;
			offset_bits = 2;
			index_mask = mask(index_bits);
//			cout << "Mask: " << index_mask << endl << endl;
			sets = (ll)pow(2.0, index_bits);
//			cout << "Sets: " << sets << endl;
			for(ll i = 0; i<sets; i++)
				predictor_table.push_back(2);			// Initialize 'Weakly Taken' i.e 2
		}
		
		ll calc_Index(ll address)
		{
			return (((address) >> offset_bits) & index_mask);
		}
	
		int predict(ll address, char actual_branch)
		{	
			ll index = calc_Index(address);
			if(predictor_table.at(index) >= 2)
				return 1;
			else
				return 0;
		}
		
		void update_prediction_table(ll address, int predicted_branch, char actual_branch)
		{
		//	cout << "MisPred: " << mispredictions << endl;
		//	cout << "Actual: " << actual_branch << endl;
		//	cout << "Predicted: " << predicted_branch << endl;

			ll index = calc_Index(address);
			if(actual_branch == 't')
			{
				if(predictor_table.at(index) < 3)
					predictor_table.at(index) = predictor_table.at(index) + 1;
				if(predicted_branch == 0)
					mispredictions++;
				else
					true_predictions++;
			}		
			else if(actual_branch == 'n')
			{	
				if(predictor_table.at(index) > 0)
					predictor_table.at(index) = predictor_table.at(index) - 1;
				if(predicted_branch == 1)
					mispredictions++;
				else
					true_predictions++;
			}

		}

		void access(ll address, char actual_branch)
		{
			predictions++;
			predicted_branch = predict(address, actual_branch);
			//cout << predict(address, actual_branch) << endl;
			update_prediction_table(address, predicted_branch, actual_branch);
		}

		void display_table()
		{
			cout << endl << "Final Bimodal Table Contents:" << endl;
			for(it = predictor_table.begin(); it != predictor_table.end(); it++)
			{
				int index = distance(predictor_table.begin(), it); 
				cout << "table[" << index << "]: " << predictor_table.at(index) << endl;	
			}
		}


		void print_stats()
		{
			cout << endl << "Final Branch Predictor Statistics:" << endl;
			cout << "a. Number of branches: " << predictions << endl;
			cout << "b. Number of predictions from the branch predictor: " << predictions << endl;
			cout << "c. Number of mispredictions from the branch predictor: " << mispredictions << endl;
			cout << "d. Number of mispredictions from the BTB: " << "0" << endl;
			//cout << "e.Misprediction Rate: " << fixed << setprecision(2) << (float)((mispredictions_BTB + mispredictions)/(float)(predictions))*100 << " percent" << endl;
//			cout << "True Predictions: " << true_predictions << endl;		
		}


};
#endif
