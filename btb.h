#ifndef __BTB__
#define __BTB__
#include <string>

typedef long long ll;

class btb_cacheline
{
	private:
		ll address, tag, sequence_num;
		std::string validity;
	
	public:
		btb_cacheline()
		{
			address = -1;
			tag = sequence_num = -1;
			validity = "Invalid";
		}

		ll getTag(){return this->tag;}
		ll getAddress()	{return this->address;}
		ll getSequence_num() {return this->sequence_num;}
		std::string getValidity() {return this->validity;}

		void setAddress(ll address) {this->address = address;}
		void setTag(ll tag) {this->tag = tag;}
		void setValidity(std::string validity) {this->validity = validity;}
		void setSequence_num(ll sequence_num) {this->sequence_num = sequence_num;}
};

class btb
{
	private:
		btb_cacheline **cacheline;
		ll index_mask, tag_mask, sets;
		int index_bits, btb_assoc;
		ll mispredictions, branch_count;
		bool btb_Hit;
		ll mask(int);
	public:
		btb(ll, ll);
		ll calcIndex(ll address) {return ((address & index_mask) >> 2);}
		ll calcTag(ll address) {return ((address & tag_mask) >> (index_bits + 2));}
		void access(ll, char);
		void replace_cacheline(ll);
		btb_cacheline *find_cacheline(ll);
		btb_cacheline *cacheline_to_replace(ll);
		btb_cacheline *invalid_cacheline(ll);
		void display();
		
		bool get_btb_Hit() {return this->btb_Hit;}
		ll getMispredictions() {return this->mispredictions;}
		ll get_branch_count() {return this->branch_count;}
};

#endif
