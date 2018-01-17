#include "bimodal.h"
#include "btb.h"
#include "btb_cacheline.h"

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
//			b->display_table();
//			cout << "BIMODAL";
	}

	fin.open(fname);
	static int branches = 0;
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

	
	cout << "Command Line:" << endl;
	switch(predictor_type)
	{
		case 0:	
			cout << "./sim_bp bimodal " << argv[2] << " " << argv[3] << " " << argv[4] << " " << argv[5] << endl;
			b->display_table();
			b->print_stats();
	}
/*	cout << " Total branches: " << branches <<endl;
	cout << "BTB Count: " << branch_buffer->get_branch_count() << endl;
	cout << "BTB Misprediction: " << branch_buffer->getMispredictions() << endl;
*/
	return 1;
}
