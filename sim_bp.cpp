#include <stdlib.h>
#include <assert.h>
#include<string.h>
#include <iostream>
#include <fstream>
#include<math.h>
#include<stdio.h>
#include <vector>
#include <bitset>
#include <iomanip>

using namespace std;
typedef long long ll;
typedef unsigned long ul;

class BTB_Cell
{
	private:
		ll Tag;
		ll Flags;
		ll Ref_Num;
		ll Address;
		
	public:
		BTB_Cell();
		ll getTag();
		ll getFlags();
		ll getRef_Num();
		ll getAddress();
		void setRef_Num(ul);
		void setFlags(ll);
		void setTag(ll);
		void setAddress(ll);
		void Set_Invalid();
		bool Valid_Check();
};

BTB_Cell :: BTB_Cell()
{
	Tag = 0;
	Flags = 0;
	Address = -1;
}

ll BTB_Cell :: getTag()
{
	return Tag;
}

ll BTB_Cell :: getFlags()
{
	return Flags;
}

ll BTB_Cell :: getRef_Num()
{
	return Ref_Num;
}

ll BTB_Cell :: getAddress()
{
	return Address;
}

void BTB_Cell :: setRef_Num(ul Ref_Num)
{
	this->Ref_Num = Ref_Num;
}

void BTB_Cell :: setFlags(ll Flags)
{
	this->Flags = Flags;
}

void BTB_Cell :: setTag(ll Tag)
{
	this->Tag = Tag;
}

void BTB_Cell :: setAddress(ll Address)
{
	this->Address = Address;
}

void BTB_Cell :: Set_Invalid()
{
	Tag = 0; 
	Flags = 0;
}

bool BTB_Cell :: Valid_Check()
{
	if(Flags!=0)
		return true;
	else
		return false;
}


class BTB { 
	private:
		BTB_Cell **Cell;
		int **PseudoLRU;
		ll Blocksize, BTB_Size, Assoc, Sets, Tag, Offset, Index, Repl_Policy, Inclusion, BTB_Index_Bits; 
		int Index_Width, Offset_Width, Tag_Width;
		ll offset_mask, index_mask, tag_mask;
		ll Reads, Writes, RdMiss, BTB_Misprediction, RdHits, WtHits, WriteBacks, Mem_Traffic, Mem_Traffic_Back;
		ll mask(int);
		
	public: 
		unsigned long Trace_No;
		BTB();
		void Set_Values(ll, ll, ll, ll);
		ll calcIndex(ll);
		ll calcOffset(ll);
		ll Access(char, ll);
		void Update_Ref_Num(BTB_Cell *, int, ll, ll, ll);
		BTB_Cell *Replace_Cell(ll, char);
		BTB_Cell *findCell(ll);
		int getCell_Way(ll);
		BTB_Cell *Block;
		BTB_Cell *New_Block;
		ll Evicted_Block_Address;
		ll L1_Evicted_Block_Flag;
		ll L1_Evict;
		ll BTB_Accessing;
		ll L2_Evicted_Block_Flag;
		ll getReads();
		ll getWrites();
		ll getRdMiss();
		ll getBTB_Misprediction();
		ll getRdHit();
		ll getWtHit();
		ll getWtBacks();
		ll getMem_Traffic();
		ll getMem_Traffic_Back();
		void Display();
};

ll BTB::mask(int N)
{
	ll mask = 0;
	for(int i = 0; i<N; i++)
	{
	   mask <<=1;
	   mask |=1;
	}
	return (mask);
}

void BTB :: Display()
{
	BTB_Cell *Block;
	for (int i=0; i< Sets; i++)
	{
		cout << "Set" << setfill(' ') << setw(6) << i << ":   ";
		for(int j=0; j<Assoc; j++)
		{
			Block = &Cell[i][j];
			cout << "{" << Block->getFlags() << ", " << showbase << internal << setfill(' ') << hex << setw(10) << Block->getAddress() << "}  " << dec;
		}
		cout << endl;
	}
	cout << endl;
}

BTB :: BTB()
{
	Reads = 0;
	Writes = 0;
	RdMiss = 0;
	BTB_Misprediction = 0;
	RdHits = 0;
	WtHits = 0;
	Trace_No = 0;
	WriteBacks = 0;
	Evicted_Block_Address = -1;
	L1_Evict = 0;		//0: No Eviction 1: Eviction
	L1_Evicted_Block_Flag = -1;
	L2_Evicted_Block_Flag = -1;
	Mem_Traffic = 0;
	Mem_Traffic_Back = 0;
}

void BTB :: Set_Values(ll BTB_Index_Bits, ll Assoc, ll Repl_Policy, ll Inclusion)
{
	this->BTB_Index_Bits = BTB_Index_Bits;
	this->Assoc = Assoc;
	this->Repl_Policy = Repl_Policy;
	this->Inclusion = Inclusion;
	
	//Sets = (BTB_Size)/(Assoc*Blocksize);
	
	Offset_Width = 2;
	Index_Width = BTB_Index_Bits;
	//Tag_Width = 64 - (Offset_Width) - (Index_Width);
	
	offset_mask = mask(Offset_Width);
	index_mask = mask((Index_Width+Offset_Width));
	//tag_mask = mask((Tag_Width+Index_Width+Offset_Width));
	
	Sets = pow(2.0, BTB_Index_Bits);
	
	Cell = new BTB_Cell*[Sets];
	for (long long i=0; i<Sets; i++)
		Cell[i] = new BTB_Cell[Assoc];
	
}


ll BTB :: calcIndex(ll x)
{
	return (((ll)(x) & (index_mask))>>Offset_Width);
}

ll BTB :: calcOffset(ll x)
{
	return ((ll)(x) & (offset_mask));
}

ll BTB :: Access(char Op, ll x)
{
	int Flag = 0;

	Trace_No++;

	
	Block = findCell(x);
	if (Block == NULL)															// Tag NOT found. MISS
	{
		if (Op =='t')
			BTB_Misprediction++;

		BTB_Cell *New_Block = Replace_Cell(x, Op);
		
		Flag = 1;	
		
		/*else																	// Replace the Invalid Block with new Address
		{
			New_Block = Replace_Cell(x, Op);
			if (Op == 'w')
				New_Block->setFlags(1);
			Flag = 1;
		}*/
		Flag = 1;
	}
	else																		// Tag Found. HIT
	{
		Update_Ref_Num(Block,(int)0, Repl_Policy, x, (ll)0);
			Block->setFlags(1);	
		Flag = 0;
	}
	
	
	return (Flag);
}

void BTB :: Update_Ref_Num(BTB_Cell *Block, int A, ll Repl_Policy, ll x, ll Way)
{
	if (Repl_Policy == 0) 								//LRU Replacement
	{
		Block->setRef_Num(Trace_No);
	}	
}

int BTB :: getCell_Way(ll x)
{
	ll Index, j, tag, pos;
	pos = Assoc;
	tag = (x);
	Index = calcIndex(x);
	
	for (j=0; j<Assoc; j++)
	{
		if (Cell[Index][j].Valid_Check())								//Not 0. i.e. Valid Tag.
		{
			if (Cell[Index][j].getTag() == tag)							// Compare with current Tag
			{
				pos = j;
				break;
			}
		}
	}
	return ((int)pos);
}

BTB_Cell * BTB :: findCell(ll x)
{
	Index = calcIndex(x);
	int pos = getCell_Way(x);
	if (pos == Assoc)
		return NULL;													// No Cell found
	else
		return (&(Cell[Index][pos]));									// Return address of found Cell
		
}

BTB_Cell * BTB :: Replace_Cell(ll x, char Op)
{
	ll tag, Index, j, Way, min;
	Index = calcIndex(x);
	min = Trace_No;
	int Flag = 0;
	for (j=0; j<Assoc; j++)
	{
		if((Cell[Index][j].Valid_Check()) == 0)									// Not Valid i.e. Empty Block. i.e. Replace this one.
		{
			Way = j;
			L1_Evict = 0;
			goto replace;
		}
	}
	
	if(Repl_Policy == 0  || Repl_Policy == 1)
	{
		for(j=0; j<Assoc; j++)													// FIFO Replacement and LRU Replacement	Miss
		{
			if((Cell[Index][j].getRef_Num()) <= min)
			{
				Way = j;
				min = Cell[Index][j].getRef_Num();
			}
		}
	}
	
	replace:
	BTB_Cell *evicted_block = &Cell[Index][Way];
		
	evicted_block->setTag(x);
	evicted_block->setFlags(1);
	evicted_block->setAddress(x);
	
	// if LRU used, send 0 to Update_Ref_Num, if FIFO used, send 1
	if (Flag == 0)
		Update_Ref_Num(evicted_block, (int)0, Repl_Policy, x, Way);
	
	return evicted_block;
}


ll BTB :: getReads()
{
	return Reads;
}

ll BTB :: getWrites()
{
	return Writes;
}

ll BTB :: getRdMiss()
{
	return RdMiss;
}

ll BTB :: getBTB_Misprediction()
{
	return BTB_Misprediction;
}

ll BTB :: getRdHit()
{
	return RdHits;
}

ll BTB :: getWtHit()
{
	return WtHits;
}

ll BTB :: getWtBacks()
{
	return WriteBacks;
}

ll BTB :: getMem_Traffic()
{
	return Mem_Traffic;
}

ll BTB :: getMem_Traffic_Back()
{
	return Mem_Traffic_Back;
}

enum{
	N = 0,
	T
};

class Predictor
{
	private:
		ll Size_Predictor_Table, Size_GHR, Size_History_Register;
		ll mask(ll);
		ll Index_Bits, Offset_Width, Index_Width, offset_mask, index_mask, GHR_mask, GHR_Reg;
		ll Branches, Predictions, Mispredictions, Mispredictions_BTB, True_prediction;
		ll *Predictor_Table;  
		ll *GHR_Table;
		ll *History_Table;
		ll PC_Index, History_Table_Index;
        
	public:
		ll GHR_Bits;
		void Set_Values(ll, ll, ll);
		Predictor();
		ll Access(char, ll, ll);
		ll Index(ll);
		ll Predict(ll);
		void Update(ll, char, ll, ll);
		ll getBranches();
		ll getPredictions();
		ll getMispredictions();
		ll getMispredictions_BTB();
		ll getGHR_Reg();
		ll getPC_Index();
		void Display_table();
		void setGHR_Reg(ll);
		void Display_History_table();
};

void Predictor :: Display_table()
{
	for (ll i=0; i <Size_Predictor_Table; i++)
	{
		cout<<"table[" << i << "]: " << Predictor_Table[i] << endl;
	}
}

void Predictor :: Display_History_table()
{
	for (ll i=0; i <Size_History_Register; i++)
	{
		cout<<"table[" << i << "]: " ;
		if (History_Table[i] != 0)
			cout << showbase << internal << setfill(' ') << hex << setw(6) << History_Table[i] << dec << endl;
		else
			cout << "0x " << History_Table[i] << dec << endl;
	}
}

ll Predictor::mask(ll N)
{
	ll mask = 0;
	for(int i = 0; i<N; i++)
	{
	   mask <<=1;
	   mask |=1;
	}
	return (mask);
}

Predictor :: Predictor()
{
	Branches = 0;
	Predictions = 0;
	Mispredictions = 0;
	Mispredictions_BTB = 0;
	
}

void Predictor :: Set_Values(ll Index_Bits, ll GHR_Bits, ll Predictor_Used)
{
	
	this->Index_Bits = Index_Bits;
	
	if (Predictor_Used == 0 || Predictor_Used == 1 || Predictor_Used == 2)
	{	
		Size_Predictor_Table = pow(2.0, Index_Bits);	
		
		//Predictor_Table  = new ll[table_size_b];
		Predictor_Table  = new ll[Size_Predictor_Table];
		for(int i=0; i<Size_Predictor_Table; i++)
			Predictor_Table[i] = 2;    					//initializing the branch predictor table with 2 i.e., weakly taken
		
		this->GHR_Bits = GHR_Bits;
		Index_Width = (ll)(Index_Bits);
		index_mask = mask(Index_Width);
		if (GHR_Bits != 0)
		{
			GHR_mask = mask((ll)GHR_Bits);
			Size_GHR = pow(2.0, GHR_Bits);

			GHR_Reg = 0;
		}
	}
	
	else if (Predictor_Used == 3)
	{
		Size_Predictor_Table = pow(2.0, Index_Bits);	
	
		//Predictor_Table  = new ll[table_size_b];
		Predictor_Table  = new ll[Size_Predictor_Table];
		for(int i=0; i<Size_Predictor_Table; i++)
			Predictor_Table[i] = 2;    					//initializing the branch predictor table with 2 i.e., weakly taken
		
		this->GHR_Bits = GHR_Bits;
		Index_Width = (ll)(GHR_Bits);
		index_mask = mask(Index_Width);
	
		Size_History_Register = pow(2.0, GHR_Bits);
		History_Table  = new ll[Size_History_Register];
		for(int i=0; i<Size_History_Register; i++)
			History_Table[i] = 0; 
	}
}

ll Predictor :: Access(char op, ll addr, ll Predictor_Used)
{
	Branches++;
	
	
	if (Predictor_Used == 0 || Predictor_Used == 1 || Predictor_Used == 2)
	{
		PC_Index = Index(addr);
		if (GHR_Bits != 0)
		{
			ll temp = GHR_Reg << (Index_Bits - GHR_Bits);
			PC_Index = PC_Index ^ temp;
		}
		if ((Predictor_Used == 0) || (Predictor_Used == 1))
		{
			ll branch_Pred = Predict(PC_Index);
			Update(branch_Pred, op, PC_Index, Predictor_Used);
		}
	}
	
	else if(Predictor_Used == 3)
	{
		History_Table_Index = Index(addr);
		PC_Index = History_Table[History_Table_Index];
	}
}

ll Predictor :: Index(ll addr)
{
	
	return (((ll)(addr >> 2) & (index_mask)));										// >> Offset_Width);
}


ll Predictor :: Predict(ll PC_Index)
{
	Predictions++;
	if (Predictor_Table[PC_Index] >= 2)
		return 1;
	else
		return 0;
}

void Predictor :: Update(ll branch_Pred, char op, ll PC_Index, ll Predictor_Used)
{
	
	if (Predictor_Used == 0 || Predictor_Used == 1 || Predictor_Used == 2)
	{

		if (op == 't')
		{
			if(Predictor_Table[PC_Index] < 3)
				Predictor_Table[PC_Index] = Predictor_Table[PC_Index] + 1;
		}
		else if(op == 'n')
		{
			if(Predictor_Table[PC_Index] > 0)
				Predictor_Table[PC_Index] = Predictor_Table[PC_Index] - 1;
		}
		
		if((Predictor_Used == 0) || (Predictor_Used == 1))
		{
			if ((branch_Pred == 1 && op == 't') || (branch_Pred == 0 && op == 'n'))
				True_prediction++;
			else
				Mispredictions++;
			

		}
		if (GHR_Bits != 0)
		{
			GHR_Reg = GHR_Reg >> 1;
			if (op == 't')
			{
				GHR_Reg |= (1U << (GHR_Bits - 1));
			}
			else if(op == 'n')
			{
				GHR_Reg &= ~(1U << (GHR_Bits - 1));
			}
		}
	}
	
	else if (Predictor_Used == 3)
	{
		if (op == 't')
		{
			if(Predictor_Table[PC_Index] < 3)
				Predictor_Table[PC_Index] = Predictor_Table[PC_Index] + 1;
		}	
		else if(op == 'n')
		{
			if(Predictor_Table[PC_Index] > 0)
				Predictor_Table[PC_Index] = Predictor_Table[PC_Index] - 1;
		}
		
		if ((branch_Pred == 1 && op == 't') || (branch_Pred == 0 && op == 'n'))
			True_prediction++;
		else
			Mispredictions++;
		
		History_Table[History_Table_Index] = History_Table[History_Table_Index] >> 1;
		if (op == 't')
		{
			History_Table[History_Table_Index] |= (1U << (Index_Bits - 1));
		}
		else if(op == 'n')
		{
			History_Table[History_Table_Index] &= ~(1U << (Index_Bits - 1));
		}
			
	}
	
}

ll Predictor :: getBranches()
{
	return Branches;
}

ll Predictor :: getPredictions()
{
	return Predictions;
}

ll Predictor :: getMispredictions()
{
	return Mispredictions;
}

ll Predictor :: getMispredictions_BTB()
{
	return Mispredictions_BTB;
}

ll Predictor :: getGHR_Reg()
{
	return GHR_Reg;
}

ll Predictor :: getPC_Index()
{
	return PC_Index;
}

void Predictor :: setGHR_Reg(ll GHR_Reg)
{
	this->GHR_Reg = GHR_Reg;
}

int main(int argc, char *argv[]) {
   
	ll BiModal_Index_Bits, Index_BTB, Assoc_BTB, GHR_Index_Bits, GHR_Bits, Chooser_Table_Index, Size_Chooser, True_prediction, Mispredictions, Yeh_Patt_Index_Bits, Local_History_Register;
	string Trace_File;
	
	ifstream fin;		
	char op;
	long long addr, Predictor_Used, Trace_Count = 0, Flag = 0;
	
	char *Predictor_Type =  (char *)malloc(100);
	strcpy(Predictor_Type,argv[1]);
	
	char *fname =  (char *)malloc(100);
	strcpy(fname,argv[5]);
	
	ll *Chooser_Table;
	

	Predictor P1;
	Predictor P2;
	BTB B1;
	if(strcmp(Predictor_Type,"bimodal") == 0)
	{
		Predictor_Used = 0;
		BiModal_Index_Bits = atol(argv[2]);
		Index_BTB = atol(argv[3]);
		Assoc_BTB = atol(argv[4]);

		P1.Set_Values(BiModal_Index_Bits, 0, Predictor_Used);
		if (Index_BTB != 0 && Assoc_BTB != 0)
				B1.Set_Values(Index_BTB, Assoc_BTB, 0, 0);
		strcpy(fname,argv[5]);
	}
	
	else if(strcmp(Predictor_Type,"gshare") == 0)
	{
		Predictor_Used = 1;
		GHR_Index_Bits = atol(argv[2]);
		GHR_Bits = atol(argv[3]);
		Index_BTB = atol(argv[4]);
		Assoc_BTB = atol(argv[5]);

		P1.Set_Values(GHR_Index_Bits, GHR_Bits, Predictor_Used);
		if (Index_BTB != 0 && Assoc_BTB != 0)
				B1.Set_Values(Index_BTB, Assoc_BTB, 0, 0);
		strcpy(fname,argv[6]);
	}
	
	else if(strcmp(Predictor_Type,"hybrid") == 0)
	{

		Predictor_Used = 2;
		Chooser_Table_Index = atol(argv[2]);
		GHR_Index_Bits = atol(argv[3]);
		GHR_Bits = atol(argv[4]);
		BiModal_Index_Bits = atol(argv[5]);
		Index_BTB = atol(argv[6]);
		Assoc_BTB = atol(argv[7]);


		strcpy(fname,argv[8]);
		

		
		Size_Chooser = pow(2.0, Chooser_Table_Index);

		Chooser_Table = new ll[Size_Chooser];
		for(ll i=0; i<Size_Chooser; i++)
			Chooser_Table[i] = 1;
		

		
		P1.Set_Values(BiModal_Index_Bits, 0, Predictor_Used);
		P2.Set_Values(GHR_Index_Bits, GHR_Bits, Predictor_Used);
		if (Index_BTB != 0 && Assoc_BTB != 0)
				B1.Set_Values(Index_BTB, Assoc_BTB, 0, 0);
		Predictor_Used = 2;
		Mispredictions = 0;

		
	}	
	
	else if(strcmp(Predictor_Type,"yehpatt") == 0)
	{
		Predictor_Used = 3;	
		Local_History_Register = atol(argv[2]);
		BiModal_Index_Bits = atol(argv[3]);
		Index_BTB = atol(argv[4]);
		Assoc_BTB = atol(argv[5]);

		P1.Set_Values(BiModal_Index_Bits, Local_History_Register, Predictor_Used);
		if (Index_BTB != 0 && Assoc_BTB != 0)
				B1.Set_Values(Index_BTB, Assoc_BTB, 0, 0);
		strcpy(fname,argv[6]);
	}
	

	fin.open(fname);
	while(fin >> hex >> addr >> op)
	{

		if(Predictor_Used == 0 || Predictor_Used == 1)
		{
			Trace_Count++;
			if (Index_BTB != 0 && Assoc_BTB != 0)
			{
				ll Flag = B1.Access(op, addr);
				if (Flag == 0) 
					P1.Access(op, addr, 0);	
			}
			else
				P1.Access(op, addr, 0);	
		}
		else if(strcmp(Predictor_Type,"hybrid") == 0)
		{
			Trace_Count++;
			if (Index_BTB != 0 && Assoc_BTB != 0)
			{
				Flag = B1.Access(op, addr);

			}
			if (Flag == 0) 
			{		
				P1.Access(op, addr, Predictor_Used);	
				ll BiModal_branch_Pred = P1.Predict(P1.getPC_Index());

				P2.Access(op, addr, Predictor_Used);
				ll GShare_branch_Pred = P2.Predict(P2.getPC_Index());

				ll mask = 0;
				for(int i = 0; i<Chooser_Table_Index; i++)
				{
				   mask <<=1;
				   mask |=1;
				}
				
				ll Chooser_PC_Index = (((ll)(addr >> 2) & (mask)));
				ll Chooser_table_Prediction;
				if (Chooser_Table[Chooser_PC_Index] >= 2)
				{
					Chooser_table_Prediction = GShare_branch_Pred;

					P2.Update(GShare_branch_Pred, op, P2.getPC_Index(), Predictor_Used);
				}
				else
				{
					Chooser_table_Prediction = BiModal_branch_Pred;
					P1.Update(BiModal_branch_Pred, op, P1.getPC_Index(), Predictor_Used);
					
					ll Z = P2.getGHR_Reg() >> 1;
					if (op == 't')
					{
						Z |= (1U << (P2.GHR_Bits - 1));
					}
					else if(op == 'n')
					{
						Z &= ~(1U << (P2.GHR_Bits - 1));
					}
					P2.setGHR_Reg(Z);
				}
				
				if(BiModal_branch_Pred == GShare_branch_Pred)
				{
					Chooser_Table[Chooser_PC_Index] = Chooser_Table[Chooser_PC_Index];
				}
				else if((GShare_branch_Pred == 1 && op == 't' && BiModal_branch_Pred == 0) || (GShare_branch_Pred == 0 && op == 'n' && BiModal_branch_Pred == 1))
				{
					if(Chooser_Table[Chooser_PC_Index] < 3)
						Chooser_Table[Chooser_PC_Index] = Chooser_Table[Chooser_PC_Index] + 1;
				}
				else if((BiModal_branch_Pred == 1 && op == 't' && GShare_branch_Pred == 0) || (BiModal_branch_Pred == 0 && op == 'n' && GShare_branch_Pred == 1))
				{
					if(Chooser_Table[Chooser_PC_Index] > 0)
						Chooser_Table[Chooser_PC_Index] = Chooser_Table[Chooser_PC_Index] - 1;
				}
				
				if ((Chooser_table_Prediction == 1 && op == 't') || (Chooser_table_Prediction == 0 && op == 'n'))
					True_prediction++;
				else
					Mispredictions++;
			}
		}
		
		else if(Predictor_Used == 3)
		{
			Trace_Count++;

			if (Index_BTB != 0 && Assoc_BTB != 0)
			{
				ll Flag = B1.Access(op, addr);
				if (Flag == 0) 
				{
					P1.Access(op, addr, Predictor_Used);
					ll Yeh_Patt_branch_Pred = P1.Predict(P1.getPC_Index());
					P1.Update(Yeh_Patt_branch_Pred, op, P1.getPC_Index(), Predictor_Used);
				}
						
			}
			else
			{
				P1.Access(op, addr, Predictor_Used);	
				ll Yeh_Patt_branch_Pred = P1.Predict(P1.getPC_Index());
				P1.Update(Yeh_Patt_branch_Pred, op, P1.getPC_Index(), Predictor_Used);
			}	
		}
	}
	fin.close();

	cout << "Command Line:" << endl << "./sim_bp ";
	
	if (Predictor_Used == 0)
		cout << Predictor_Type << " " << BiModal_Index_Bits << " " << Index_BTB << " " << Assoc_BTB << " " << fname << endl;
	else if (Predictor_Used == 1)
		cout << Predictor_Type << " " << GHR_Index_Bits << " " << GHR_Bits << " " << Index_BTB << " " << Assoc_BTB << " " << fname << endl;
	else if (Predictor_Used == 2)
		cout << Predictor_Type << " " << Chooser_Table_Index << " " << GHR_Index_Bits << " " << GHR_Bits << " " << BiModal_Index_Bits << " " << Index_BTB << " " << Assoc_BTB << " " << fname << endl;
	else if (Predictor_Used == 3)
		cout << Predictor_Type << " " << Local_History_Register << " " << BiModal_Index_Bits << " " << Index_BTB << " " << Assoc_BTB << " " << fname << endl;
	
	if (Index_BTB != 0 && Assoc_BTB != 0)
	{
		cout << endl << "Final BTB Tag Array Contents {valid, pc}:" << endl;
		B1.Display();
	}
	if (Predictor_Used == 0)
	{
		cout << endl << "Final Bimodal Table Contents:" << endl;
		P1.Display_table();
	}	
	else if (Predictor_Used == 1)
	{
		cout << endl << "Final GShare Table Contents:" << endl;
		P1.Display_table();
	}	
	else if (Predictor_Used == 2)
	{
		cout << endl << "Final Bimodal Table Contents:" << endl;
		P1.Display_table();
		cout << endl << "Final GShare Table Contents:" << endl;
		P2.Display_table();
	}
	if (Predictor_Used == 3)
	{
		cout << endl << "Final History Table Contents:" << endl;
		P1.Display_History_table();
		cout << endl << "Final Prediction Table Contents: " << endl;
		P1.Display_table();
	}	
	

	if(Predictor_Used == 1)
	{
		cout << showbase << internal << setfill(' '); // fill with 0s
		cout << endl << "Final GHR Contents:" << hex << setw(10) << P1.getGHR_Reg() << dec << endl; 
	}
	else if(Predictor_Used == 2)
	{
		cout << showbase << internal << setfill(' '); // fill with 0s
		cout << endl << "Final GHR Contents: " << hex << setw(10) << P2.getGHR_Reg() << dec << endl;
		cout << endl << "Final Chooser Table Contents:" << endl;
		for(int i=0; i<Size_Chooser; i++)
			cout << "Choice table[" << i << "]: " << Chooser_Table[i] << endl;
	}

	cout << endl << "Final Branch Predictor Statistics:" << endl;
	
	if (Index_BTB == 0 && Assoc_BTB == 0)
		cout << "a. Number of branches: " << P1.getBranches() << endl;
	else
		cout << "a. Number of branches: " << Trace_Count << endl;
	
	cout << "b. Number of predictions from the branch predictor: " << P1.getPredictions() << endl;
	if(Predictor_Used == 0 || Predictor_Used == 1 || Predictor_Used == 3)
		cout << "c. Number of mispredictions from the branch predictor: " << P1.getMispredictions() << endl;
	else
		cout << "c. Number of mispredictions from the branch predictor: " << Mispredictions << endl;
	
	if(Predictor_Used == 0 || Predictor_Used == 1)
	{		
		if (Index_BTB == 0 && Assoc_BTB == 0)
			cout << "d. Number of mispredictions from the BTB: " << P1.getMispredictions_BTB() << endl;
		else
			cout << "d. Number of mispredictions from the BTB: " << B1.getBTB_Misprediction() << endl;
	}
	else
	{
		if (Index_BTB == 0 && Assoc_BTB == 0)
			cout << "d. Number of mispredictions from the BTB: " << P1.getMispredictions_BTB() << endl;
		else
			cout << "d. Number of mispredictions from the BTB: " << B1.getBTB_Misprediction() << endl;
	}
	
	if(Predictor_Used == 0 || Predictor_Used == 1 || Predictor_Used == 3)
	{
		if (Index_BTB == 0 && Assoc_BTB == 0)
			cout << "e. Misprediction Rate: " << fixed << setprecision(2) << (float)(((float)P1.getMispredictions_BTB() + (float)P1.getMispredictions())/(float)(P1.getBranches()))*100 << " percent" << endl;
		else
			cout << "e. Misprediction Rate: " << fixed << setprecision(2) << (float)(((float)B1.getBTB_Misprediction() + (float)P1.getMispredictions())/(float)(Trace_Count))*100 << " percent" << endl;
	}
		
	else
	{
		if (Index_BTB == 0 && Assoc_BTB == 0)
			cout << "e. Misprediction Rate: " << fixed << setprecision(2) << (float)(((float)P1.getMispredictions_BTB() + (float)Mispredictions)/(float)(P1.getBranches()))*100 << " percent" << endl;
		else
			cout << "e. Misprediction Rate: " << fixed << setprecision(2) << (float)(((float)B1.getBTB_Misprediction() + (float)Mispredictions)/(float)(Trace_Count))*100 << " percent" << endl;
	}
		
	return 0;
}

