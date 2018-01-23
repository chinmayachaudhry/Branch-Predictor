# Branch-Predictor (Binodal, GShare, Hybrid, Yeh-Patt)

## Bimodal Branch Predictor
### Command Line
./sim_bp bimodal <i<sub>B</sub>> <i<sub>BTB</sub>> <assoc<sub>BTB</sub>> <Trace_File>
- __i<sub>B</sub>__: No. of PC bits used to index the bimodal table
- __i<sub>BTB</sub>__: No. of PC used to index the BTB
- __assoc<sub>BTB</sub>__: BTB Associativity

### Description
- Each entry in the prediction table contains 2-bit counter.
- Table is indexed by using branch PC's i+1:2(inclusive), where i is the number of bits needed to index the predictor table.
- All entries in predictor table are initialized to '2' i.e. __*Weakly Taken*__
- Steps to complete:
  1. __INDEX__:         Determine the branch's *index* into the prediction table
  2. __PREDICT__:       Make a prediction. Predict *Taken* id counter >= 2, else predict *Not Taken*.
  3. __UPDATE__:        Update the branch predictor, based on actual branch outcome. Increment the counter if branch actually *Taken*, else decrement. Counter *saturates* at '3' and '0'


## GShare Branch Predictor
### Command Line
./sim_bp gshare <i<sub>G</sub>> <ghr<sub>B</sub>> <i<sub>BTB</sub>> <assoc<sub>BTB</sub>> <Trace_File>
- __ghr<sub>B</sub>__: No. of GHR bits.
- __<i<sub>G</sub>__: No. of PC bits used to index GShare Table
- Rest same as above

### Description
- Index used to access predictor table is based on both the branch's PC as well as GHR
- GHR initialized to 0 and all entries in the table are initialized to 2 i.e. __*Weakly Taken*__
- Steps to complete:
  1. __INDEX__: Determine the branch's *index* into prediction table. The current *ghr*-bit GHR is XORed with the uppermost *ghr* bits of the *i* bits from PC for indexing.
  2. __PREDICT__: Make a prediction. Predict *Taken* id counter >= 2, else predict *Not Taken*.
  3. __UPDATE__: Update both predictor table and GHR
     - Increment counter if branch was actually taken, else decrement. Counter *saturates* at 0 and 3.
     - Update GHR. Shift register right by 1 bit and place branche's actual outcome into MSB. (1: Taken, 0: Not-Taken)

## Hybrid Branch Predictor
### Command Line
./sim_bp hybrid <i<sub>C</sub>> <i<sub>G</sub>> <ghr<sub>B</sub>> <i<sub>BTB</sub>> <assoc<sub>BTB</sub>> <Trace_File>
- __ghr<sub>B</sub>__: No. of GHR bits.
- __i<sub>C</sub>__ : No. of C bits used to index Chooser Table
- Rest same as above

### Description
- *hybrid predictor* contains both the __*Bimodal*__ and __*GShare*__ predictors.
- Uses *Chooser Table* of 2<sup>i<sub>C</sub></sup> 2-bit counters to select which predictor's prediction to use.
- All counters in chooser table initialized to 1 i.e. initally use *Bimodal's* prediction
- Use i<sub>C</sub>+1:2 bits of PC as index into *chooser table*
- Steps to complete:
  1. Get one prediction from *Bimodal* and *Gshare* each.
  2. Get indexed couter from _chooser table_. If counter >= 2, use gshare predictio, else use bimodal prediction.
  3. Update only the selected predictor. Regardless of the predictor, always update *GHR_Register*
  4. Update the chooser table counters. If *gshare prediction* was correct, increment counter(saturating at 3). If *bimodal prediction* is correct, decrement counter(saturating at 0).
  5. If both predictors give the same prediction, __DON'T__ update chooser table counters. Rationale being that there is no reason to prefer 1 predictor over other

## Yeh-Patt Branch Predictor
### Command Line
./sim_bp yehpatt <i<sub>H</sub>> <i<sub>P</sub>> <i<sub>BTB</sub>> <assoc<sub>BTB</sub>> <Trace_File>
- __i<sub>P</sub>__: No. of bits used to index Pattern Table
- __i<sub>H</sub>__: No. of bits used to index History Table
- Rest same as above

### Description
- History Table ia a set of __i<sub>H</sub>__ local history registers
- Pattern Table is a set of 2-bit saturating counters for each history entry
- All shifted registers are initialized to __zero__ and entries in prediction table are initialized to _2_
- Steps to complete:
  1. __INDEX__: Determine the branch's __index__ into history table using PC.
  2. __INDEX__: Determine branch's __index__ into pattern table.
  3. __PREDICT__: Make a prediction
  4. __UPDATE__: Update the branch predictor and GHR register based on branch's actual outcome
     - Increment predictor table's counter if branch was actually taken, else decrement. Counter *saturates* at 0 and 3.
     - Update local history register. Shift register right by 1 bit and place branche's actual outcome into MSB. (1: Taken, 0: Not-Taken)

