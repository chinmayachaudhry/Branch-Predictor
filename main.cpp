#include "bimodal.h"
#include "btb.h"
#include "gshare.h"
//#include "btb_cacheline.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <stdio.h>
#include <vector>
#include <bitset>

using namespace std;

int main(int argc, char *argv[])
{
	string trace_file;
	ll predictor_index_bits, ghr_bits, btb_index_bits, btb_assoc;

	ll address;
	int predictor_type;
	char actual_branch;		
	char *fname = (char *)malloc(100);

	if(strcmp(argv[1], "bimodal") == 0)
		predictor_type = 0;
	else if(strcmp(argv[1], "gshare") == 0)
		predictor_type = 1;
	ifstream fin;

	bimodal *b;
	gshare *g;
	btb *branch_buffer;
	switch(predictor_type)
	{
		case 0:
			predictor_index_bits = atol(argv[2]);
			btb_index_bits = atol(argv[3]);
			btb_assoc = atol(argv[4]);
			//cout << "index: " << btb_index_bits << " assoc: " << btb_assoc << endl;
			strcpy(fname, argv[5]);
			if(btb_index_bits == 0)
				branch_buffer = NULL;
			else
				branch_buffer = new btb(btb_index_bits, btb_assoc);
			b = new bimodal(predictor_index_bits, branch_buffer);
			break;
//			b->display_table();
//			cout << "BIMODAL";
	
		case 1:
			predictor_index_bits = atol(argv[2]);
			ghr_bits = atol(argv[3]);
			btb_index_bits = atol(argv[4]);
			btb_assoc = atol(argv[5]);
			//cout << "index: " << btb_index_bits << " assoc: " << btb_assoc << endl;
			strcpy(fname, argv[6]);
			if(btb_index_bits == 0)
				branch_buffer = NULL;
			else
				branch_buffer = new btb(btb_index_bits, btb_assoc);
			g = new gshare(predictor_index_bits, ghr_bits, branch_buffer);
			break;
//			b->display_table();
//			cout << "BIMODAL";

	}

	fin.open(fname);
	static int branches = 0;
	switch(predictor_type)
	{
		case 0:
			while(fin >> hex >> address >> actual_branch)
			{
				branches++;
				if(btb_index_bits != 0 && btb_assoc != 0)
				{
					branch_buffer->access(address, actual_branch);
					if(branch_buffer->get_btb_Hit() == true)		
						b->b_access(address, actual_branch);	
				}	
				else
					b->b_access(address, actual_branch);	
			}
			fin.close();
			break;

		case 1:
			while(fin >> hex >> address >> actual_branch)
			{
				branches++;
				if(btb_index_bits != 0 && btb_assoc != 0)
				{
					branch_buffer->access(address, actual_branch);
					if(branch_buffer->get_btb_Hit() == true)		
						g->access(address, actual_branch);	
				}	
				else
					g->access(address, actual_branch);	
			}
			fin.close();
			break;
	}
	
	cout << "Command Line:" << endl;
	switch(predictor_type)
	{
		case 0:	
			cout << "./sim_bp bimodal " << argv[2] << " " << argv[3] << " " << argv[4] << " " << argv[5] << endl;
			b->b_display_table();
			b->b_print_stats();
			break;

		case 1:	
			cout << "./sim_bp gshare " << argv[2] << " " << argv[3] << " " << argv[4] << " " << argv[5] << " " << argv[6] << endl;
			g->display_table();
			g->print_stats();
			break;	
	}
/*	cout << " Total branches: " << branches <<endl;
	cout << "BTB Count: " << branch_buffer->get_branch_count() << endl;
	cout << "BTB Misprediction: " << branch_buffer->getMispredictions() << endl;
*/
	return 1;
}
