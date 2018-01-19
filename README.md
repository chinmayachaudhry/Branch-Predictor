# Branch-Predictor (Binodal, GShare, Hybrid, Yeh-Patt)

## Bimodal Branch Predictor
### Command Line
./sim_bp bimodal <i<sub>B</sub>> <i<sub>BTB</sub>> <assoc<sub>BTB</sub>> <Trace_File>
- __i<sub>B</sub>__: No. of PC bits used to index the bimodal table
- __i<sub>BTB</sub>__: No. of PC used to index the BTB
- __assoc<sub>BTB</sub>__: BTB Associativity

### Description
- Each entry int the prediction table contains 2-bit counter.
- Table is indexed by using branch PC's i+1:2(inclusive), where i is the number of bits needed to index the predictor table.
- All entries in predictor table are initialized to '2' i.e. __*Weakly Taken*__
- Steps to complete:
  1. __INDEX__:         Determine the branch's *index* into the prediction table
  2. __PREDICT__:       Make a prediction. Predict *Taken* id counter >= 2, else predict *Not Taken*.
  3. __UPDATE__:        Update the branch predictor, based on actual branch outcome. Increment the counter if branch actually *Taken*, else decrement. Counter *saturates* at '3' and '0'


## GShare Branch Predictor
### Command Line
./sim_bp gshare <i<sub>B</sub>> <ghr<sub>B</sub>> <i<sub>BTB</sub>> <assoc<sub>BTB</sub>> <Trace_File>
- __ghr<sub>B</sub>__: No. of GHR bits.

### Description
- Index used to access predictor table is based on both the branch's PC as well as GHR
- GHR initialized to 0 and all entries in the table are initialized to 2 i.e. __*Weakly Taken*__
- Steps to complete:
  1. __INDEX__: Determine the branch's *index* into prediction table. The current *ghr*-bit GHR is XORed with the uppermost *ghr* bits of the *i* bits from PC for indexing.
  2. __PREDICT__: Make a prediction. Predict *Taken* id counter >= 2, else predict *Not Taken*.
  3. __UPDATE__: Update both predictor table and GHR
     - Increment counter if branch was actually taken, else decrement. Counter *saturates* at 0 and 3.
     - Update GHR. Shift register right by 1 bit and place branche's actual outcome into MSB. (1: Taken, 0: Not-Taken)

