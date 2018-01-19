#include <iostream>
#include <math.h>
#include <vector>
#include "btb.h"
#include "bimodal.h"
#include <iomanip>

using namespace std;

ll bimodal :: mask(ll N)
{
	ll mask = 0;
	for(ll i = 0; i<N; i++)
	{
		mask <<= 1;
		mask |= 1;
	} 
	return (mask);
}

bimodal :: bimodal(ll index_bits, btb * branch_buffer)
{
	predictions = mispredictions = true_predictions = 0;
	index_mask = mask(index_bits + 2);				// 4-Byte instruction. Hence lowest 2 bits are always 0 for address. Hence ignored
	sets = (ll)pow(2.0, index_bits);
	for(ll i = 0; i<sets; i++)
		predictor_table.push_back(2);				// Initialize 'Weakly Taken' i.e 2

	this->branch_buffer = branch_buffer;
}

int bimodal :: predict(ll address, char actual_branch)
{	
	ll index = calc_Index(address);
	if(predictor_table.at(index) >= 2)
		return 1;
	else
		return 0;
}

void bimodal :: update_prediction_table(ll address, int predicted_branch, char actual_branch)
{
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

void bimodal :: access(ll address, char actual_branch)
{
	predictions++;
	predicted_branch = predict(address, actual_branch);
	update_prediction_table(address, predicted_branch, actual_branch);
}

void bimodal :: display_table()
{
	if(branch_buffer != NULL)
		branch_buffer->display();
	
	cout << endl << "Final Bimodal Table Contents:" << endl;
	for(it = predictor_table.begin(); it != predictor_table.end(); it++)
	{
		int index = distance(predictor_table.begin(), it); 
		cout << "table[" << index << "]: " << predictor_table.at(index) << endl;	
	}
}


void bimodal :: print_stats()
{
	if(branch_buffer == NULL)
	{
		cout << endl << "Final Branch Predictor Statistics:" << endl;
		cout << "a. Number of branches: " << predictions << endl;
		cout << "b. Number of predictions from the branch predictor: " << predictions << endl;
		cout << "c. Number of mispredictions from the branch predictor: " << mispredictions << endl;
		cout << "d. Number of mispredictions from the BTB: " << "0" << endl;
		cout << "e. Misprediction Rate: " << fixed << setprecision(2) << (float)((mispredictions)/(float)(predictions))*100 << " percent" << endl;
	}
	else
	{
		cout << endl << "Final Branch Predictor Statistics:" << endl;
		cout << "a. Number of branches: " << branch_buffer->get_branch_count() << endl;
		cout << "b. Number of predictions from the branch predictor: " << predictions << endl;
		cout << "c. Number of mispredictions from the branch predictor: " << mispredictions << endl;
		cout << "d. Number of mispredictions from the BTB: " << branch_buffer->getMispredictions() << endl;
		cout << "e. Misprediction Rate: " << fixed << setprecision(2) << (float)((branch_buffer->getMispredictions() + mispredictions)/(float)(branch_buffer->get_branch_count()))*100 << " percent" << endl;
	}
}


