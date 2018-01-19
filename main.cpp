#include "bimodal.h"
#include "btb.h"
#include "gshare.h"
#include "hybrid.h"
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
	ll chooser_table_index_bits, bimodal_index_bits, gshare_index_bits;

	ll address;
	int predictor_type;
	char actual_branch;		
	char *fname = (char *)malloc(100);

	if(strcmp(argv[1], "bimodal") == 0)
		predictor_type = 0;
	else if(strcmp(argv[1], "gshare") == 0)
		predictor_type = 1;
	else if(strcmp(argv[1], "hybrid") == 0)
		predictor_type = 2;
	ifstream fin;

	bimodal *b;
	gshare *g;
	hybrid *h;
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
		
		case 2:
			chooser_table_index_bits = atol(argv[2]);
			gshare_index_bits = atol(argv[3]);
			ghr_bits = atol(argv[4]);
			bimodal_index_bits = atol(argv[5]);
			btb_index_bits = atol(argv[6]);
			btb_assoc = atol(argv[7]);
			//cout << "index: " << btb_index_bits << " assoc: " << btb_assoc << endl;
			strcpy(fname, argv[8]);
			if(btb_index_bits == 0)
				branch_buffer = NULL;
			else
				branch_buffer = new btb(btb_index_bits, btb_assoc);
			h = new hybrid(bimodal_index_bits, gshare_index_bits, ghr_bits, chooser_table_index_bits, branch_buffer);
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
						b->access(address, actual_branch);	
				}	
				else
					b->access(address, actual_branch);	
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

		case 2:
			while(fin >> hex >> address >> actual_branch)
			{
				branches++;
				if(btb_index_bits != 0 && btb_assoc != 0)
				{
					branch_buffer->access(address, actual_branch);
					if(branch_buffer->get_btb_Hit() == true)		
						h->access(address, actual_branch);	
				}	
				else
					h->access(address, actual_branch);	
			}
			fin.close();
			break;
	}
	
	cout << "Command Line:" << endl;
	switch(predictor_type)
	{
		case 0:	
			cout << "./sim_bp bimodal " << argv[2] << " " << argv[3] << " " << argv[4] << " " << argv[5] << endl;
			b->display_table();
			b->print_stats();
			break;

		case 1:	
			cout << "./sim_bp gshare " << argv[2] << " " << argv[3] << " " << argv[4] << " " << argv[5] << " " << argv[6] << endl;
			g->display_table();
			g->print_stats();
			break;	
		
		case 2:	
			cout << "./sim_bp hybrid " << argv[2] << " " << argv[3] << " " << argv[4] << " " << argv[5] << " " << argv[6] << " " << argv[7] << " " << argv[8] << endl;
			h->display_table();
			h->print_stats();
			break;	
	}
	
	return 1;
}
