#include "btb_cacheline.h"
#include "btb.h"
#include <math.h>
#include <iostream>

using namespace std;
typedef long long ll;

ll btb :: mask(int N)
{
	ll mask = 0;
	for(int i = 0; i<(int)N; i++)
	{	
		mask <<= 1;
		mask |= 1;
	}
	return (mask);
}

btb :: btb(ll index_bits, ll btb_assoc)
{
	this->index_bits = (int)index_bits;
	this->btb_assoc = (int)btb_assoc;
//	std::cout << "I: " << index_bits << " A: " << btb_assoc << std::endl;
	sets = (ll)pow(2.0, index_bits);
	mispredictions = branch_count = 0;
	index_mask = mask(index_bits + 2);
	tag_mask = mask(64);			// Assuming the pc address can be upto 64 bits long

	cacheline = new btb_cacheline*[sets];
	for(ll i = 0; i<sets; i++)
		cacheline[i] = new btb_cacheline[btb_assoc];
}

void btb :: access(ll address, char actual_branch)
{
	branch_count++;
	btb_Hit = false;
	
	/*int index = calcIndex(address);
	cout << " Before Access:" << endl << "Index: " << calcIndex(address) << endl;
	for(int i = 0; i<(int)btb_assoc; i++)
		cout << cacheline[index][i].getAddress() << "\t";
	cout << endl;*/
	
	btb_cacheline *block = find_cacheline(address);
	if(block == NULL)
	{
		btb_Hit = false;
		if(actual_branch == 't')
			mispredictions++;
		replace_cacheline(address);
	}
	else
	{
		btb_Hit = true;
		block->setSequence_num(branch_count);
	}

	/*cout << "After Access:" << endl << "Index: " << calcIndex(address) << endl;
	for(int i = 0; i<(int)btb_assoc; i++)
		cout << cacheline[index][i].getAddress() << "\t";
	cout << endl << endl;*/
}

btb_cacheline * btb :: find_cacheline(ll address)
{
	ll tag = calcTag(address);
	ll index = calcIndex(address);

	for(int i = 0; i<(int)btb_assoc; i++)
	{
		if((cacheline[index][i].getTag() == tag) && (cacheline[index][i].getValidity() == "Valid"))
			return (&cacheline[index][i]);
	}	
	return NULL;
}

void btb :: replace_cacheline(ll address)
{
	ll tag = calcTag(address);
	ll index = calcIndex(address);
	btb_cacheline *evicted_cacheline = cacheline_to_replace(index);

	evicted_cacheline->setAddress(address);
	evicted_cacheline->setTag(tag);
	evicted_cacheline->setSequence_num(branch_count);
	evicted_cacheline->setValidity("Valid");
} 

btb_cacheline * btb :: cacheline_to_replace(ll index)
{
	btb_cacheline * cacheline_invalid = invalid_cacheline(index);
	ll minimum = branch_count;
	int assoc_to_replace = 0;
	if(cacheline_invalid == NULL)
	{
		for(int i = 0; i<(int)btb_assoc; i++)
		{
			if(cacheline[index][i].getSequence_num() < minimum)
			{
				assoc_to_replace = i;
				minimum = cacheline[index][i].getSequence_num();
//				cacheline_invalid = &cacheline[index][i];
			}
		}
		return (&cacheline[index][assoc_to_replace]);
	}
	return cacheline_invalid;
}

btb_cacheline * btb :: invalid_cacheline(ll index)
{
	for(int i = 0; i<(int)btb_assoc; i++)
	{
		if(cacheline[index][i].getValidity() == "Invalid")
			return (&cacheline[index][i]);
	}
	return NULL;
}

void btb :: display()
{
	cout << endl << "Final BTB Tag  Array Contents {valid, pc}:" << endl;
	for(int i = 0; i<sets; i++)
	{
		cout << "Set" << setfill(' ') << setw(6) << i << ":   ";
		for(int j = 0; j<(int)btb_assoc; j++)
		{
			if(cacheline[i][j].getValidity() == "Valid")
				cout << "{1, " << showbase << internal << setfill(' ') << hex << setw(10) << cacheline[i][j].getAddress() << "} " << dec;
			else
				cout << "{0, " << showbase << internal << setfill(' ') << hex << setw(10) << cacheline[i][j].getAddress() << "} " << dec;
		}
		cout << endl;	
	}
	cout << endl;
}
