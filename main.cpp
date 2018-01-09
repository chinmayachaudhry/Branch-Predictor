#include "bimodal.h"


int main(int argc, char *argv[])
{
	string trace_file;
	ll predictor_index_bits, btb_index_bits, btb_assoc;

	ll address;
	int predictor_type;
	char actual_branch;		
	char *fname = (char *)malloc(100);
	strcpy(fname, argv[5]);

	if(strcmp(argv[1], "bimodal") == 0)
		predictor_type = 0;
	ifstream fin;

	bimodal *b;

	switch(predictor_type)
	{
		case 0:
			predictor_index_bits = atol(argv[2]);
			btb_index_bits = atol(argv[3]);
			btb_assoc = atol(argv[4]);
			b = new bimodal(predictor_index_bits);
//			b->display_table();

	}

	fin.open(fname);
	while(fin >> hex >> address >> actual_branch)
	{
		b->access(address, actual_branch);	
	}
	fin.close();

	
	cout << "Command Line:" << endl;
	cout << "./sim_bp bimodal " << argv[2] << " " << argv[3] << " " << argv[4] << " " << argv[5] << endl;
	b->display_table();
	b->print_stats();




	return 1;
}
