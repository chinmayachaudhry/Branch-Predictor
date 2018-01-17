#ifndef __BTB_CACHELINE__
#define __BTB_CACHELINE__
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

#endif
