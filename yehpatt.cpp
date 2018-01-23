#include <iostream>
#include <math.h>
#include <vector>
#include "btb.h"
#include "yehpatt.h"
#include <iomanip>

using namespace std;

ll yehpatt :: mask(ll N)
{
	ll mask = 0;
	for(ll i = 0; i<N; i++)
	{
		mask <<= 1;
		mask |= 1;
	} 
	return (mask);
}

yehpatt :: yehpatt(ll history_index_bits, ll predictor_index_bits, btb * branch_buffer)
{
	predictions = mispredictions = true_predictions = 0;
	this->history_index_bits = history_index_bits;
	this->predictor_index_bits = predictor_index_bits;

	index_mask = mask(history_index_bits + 2);				// 4-Byte instruction. Hence lowest 2 bits are always 0 for address. Hence ignored
	sets = (ll)pow(2.0, predictor_index_bits);
	for(ll i = 0; i<sets; i++)
		predictor_table.push_back(2);				// Initialize 'Weakly Taken' i.e 2

	history_sets = pow(2.0, history_index_bits);
	for(ll i = 0; i<history_sets; i++)
		history_table.push_back(0);
	
	this->branch_buffer = branch_buffer;
}

int yehpatt :: predict(ll address, char actual_branch)
{	
	this->history_table_index = calc_Index(address);
	this->predictor_table_index = history_table.at(history_table_index);

	if(predictor_table.at(predictor_table_index) >= 2)
		return 1;
	else
		return 0;
}

void yehpatt :: update(int predicted_branch, char actual_branch)
{
//	ll index = calc_Index(address);
	history_table.at(history_table_index) = history_table.at(history_table_index) >> 1;
	if(actual_branch == 't')
	{
		if(predictor_table.at(predictor_table_index) < 3)
			predictor_table.at(predictor_table_index) = predictor_table.at(predictor_table_index) + 1;
		if(predicted_branch == 0)
			mispredictions++;
		else
			true_predictions++;
		history_table.at(history_table_index) |= (1U << (predictor_index_bits -1));
	}		
	else if(actual_branch == 'n')
	{	
		if(predictor_table.at(predictor_table_index) > 0)
			predictor_table.at(predictor_table_index) = predictor_table.at(predictor_table_index) - 1;
		if(predicted_branch == 1)
			mispredictions++;
		else
			true_predictions++;
		history_table.at(history_table_index) &= ~(1U << (predictor_index_bits -1));
	}


}

void yehpatt :: access(ll address, char actual_branch)
{
	predictions++;
	predicted_branch = predict(address, actual_branch);
	update(predicted_branch, actual_branch);
}

void yehpatt :: display_table()
{
	if(branch_buffer != NULL)
		branch_buffer->display();
	
	cout << endl << "Final History Table Contents:" << endl;
	for(it = history_table.begin(); it != history_table.end(); it++)
	{
		int index = distance(history_table.begin(), it); 
		cout << "table[" << index << "]: "; 
		if(history_table.at(index) != 0)
			cout << showbase << internal << setfill(' ') << hex << setw(6) << history_table.at(index) << dec << endl;	
		else
			cout << "0x " << history_table.at(index) << dec << endl;
		
	}
	
	cout << endl << "Final Prediction Table Contents:" << endl;
	for(it = predictor_table.begin(); it != predictor_table.end(); it++)
	{
		int index = distance(predictor_table.begin(), it); 
		cout << "table[" << index << "]: " << predictor_table.at(index) << endl;	
	}
}


void yehpatt :: print_stats()
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


