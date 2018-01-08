# Branch-Predictor
Branch Predictor(BiModal, GShare, Hybrid, Yeh-Patt)

Bimodal Branch Predictor
-> Each entry int the prediction table contains 2-bit counter.
-> Table is indexed by uing branch PC's i+1:2(inclusive), where i is the number of bits needed to index the predictor table.
-> All entries in predictor table are initialized to '2' i.e. 'Weakly Taken' state.
Steps to complete:
a. INDEX:         Determine the branch's index into the prediction table
b. PREDICT:       Make a prediction. Predict 'Taken' id counter >= 2, else predict 'Not Taken'.
c. UPDATE:        Update the branch predictor, based on actual branch outcome. Increment the counter if branch actually 'Taken', else decrement. Counter saturates at '3' and '0'.
