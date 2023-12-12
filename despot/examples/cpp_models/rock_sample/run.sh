RUNS=50  # num. runs per experiment
DESPOT="./rock_sample" #executable
DIR="./rocksample_results" #to save results
mkdir -p $DIR

#PARAMETERS
# size = grid size
# number = num. rocks
# UB = upper bound - "MDP": hindsight; "TRIVIAL": R_max / (1-gamma) 
# LB = lower bound - "RULES": rules (OURS); "TRIVIAL": east; "RULES_BAD": rules from 11 particles (EXP-3 in paper); "RULES$((X))": rules from X (20,40,60) percentage of examples (EXP-4 in paper); "MMAP": preferred
UB="MDP";
LB="RULES";
$DESPOT --size=12 --number=4 --ubtype=$UB --lbtype=$LB --runs=$((RUNS)) --save="$DIR/rocksample_$((SIZE))size_$((ROCKS))rocks_ub_$UB""_lb_$LB"".txt";


