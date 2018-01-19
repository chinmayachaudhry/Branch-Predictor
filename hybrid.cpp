#include <iostream>
#include <math.h>
#include <vector>
#include "btb.h"
#include "hybrid.h"
#include <iomanip>

using namespace std;

ll hybrid :: mask(ll N)
{
	ll mask = 0;
	for(ll i = 0; i<N; i++)
	{
		mask <<= 1;
		mask |= 1;
	} 
	return (mask);
}

hybrid :: hybrid(ll bimodal_index_bits, ll ghr_index_bits, ll ghr_bits, ll chooser_table_index_bits, btb * branch_buffer)
{
	predictions = mispredictions = true_predictions = 0;
	index_mask = mask(chooser_table_index_bits + 2);				// 4-Byte instruction. Hence lowest 2 bits are always 0 for address. Hence ignored
	sets = (ll)pow(2.0, chooser_table_index_bits);
	for(ll i = 0; i<sets; i++)
		chooser_table.push_back(1);						// Initialize to choose GShare Prediction

	this->branch_buffer = branch_buffer;
	
	b = new bimodal(bimodal_index_bits, NULL);
	g = new gshare(ghr_index_bits, ghr_bits, NULL); 
}

int hybrid :: predict(ll address, char actual_branch)
{	
	this->chooser_table_index = calc_Index(address);
	if(chooser_table.at(chooser_table_index) >= 2)
		return 1;
	else
		return 0;
}

void hybrid :: update_chooser_table(ll address, int chooser_table_prediction, char actual_branch)
{
	//ll index = calc_Index(address);

	if(actual_branch == 't')
	{
		if(gshare_prediction == 1 && bimodal_prediction == 0)
		{
			if(chooser_table.at(chooser_table_index) < 3)
				chooser_table.at(chooser_table_index) = chooser_table.at(chooser_table_index) + 1;
		}
		else if(bimodal_prediction == 1 && gshare_prediction == 0)
		{
			if(chooser_table.at(chooser_table_index) > 0)
				chooser_table.at(chooser_table_index) = chooser_table.at(chooser_table_index) - 1;

		}
		if(chooser_table_prediction == 0)
			mispredictions++;
		else
			true_predictions++;
	}		
	else if(actual_branch == 'n')
	{	
		if(gshare_prediction == 0 && bimodal_prediction == 1)
		{
			if(chooser_table.at(chooser_table_index) < 3)
				chooser_table.at(chooser_table_index) = chooser_table.at(chooser_table_index) + 1;
		}
		else if(bimodal_prediction == 0 && gshare_prediction == 1)
		{
			if(chooser_table.at(chooser_table_index) > 0)
				chooser_table.at(chooser_table_index) = chooser_table.at(chooser_table_index) - 1;

		}	

		if(chooser_table_prediction == 1)
			mispredictions++;
		else
			true_predictions++;
	}

}

void hybrid :: access(ll address, char actual_branch)
{
	predictions++;
	this->choosen_predictor = predict(address, actual_branch);
	this->bimodal_prediction = b->predict(address, actual_branch);
	this->gshare_prediction = g->predict(address, actual_branch);
	if(choosen_predictor == 1)
	{
		g->update_prediction_table(address, gshare_prediction, actual_branch);	
		update_chooser_table(address, gshare_prediction, actual_branch);
	}
	else
	{
		update_chooser_table(address, bimodal_prediction, actual_branch);
		b->update_prediction_table(address, bimodal_prediction, actual_branch);
		g->update_ghr_register(actual_branch);
	}
}

void hybrid :: display_table()
{
	if(branch_buffer != NULL)
		branch_buffer->display();
	
	b->display_table();
	g->display_table();

	cout << endl << "Final Chooser Table Contents:" << endl;
	for(it = chooser_table.begin(); it != chooser_table.end(); it++)
	{
		int index = distance(chooser_table.begin(), it); 
		cout << "Choice table[" << index << "]: " << chooser_table.at(index) << endl;	
	}
}


void hybrid :: print_stats()
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


