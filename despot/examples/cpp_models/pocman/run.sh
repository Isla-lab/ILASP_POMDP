RUNS=50  # num. runs per experiment
DESPOT="./pocman" #executable
DIR="pocman_results" #to save results
mkdir -p $DIR


#PARAMETERS
# size = 0 (10x10); 1 (17x19)
# number = num. ghosts
# --prob_food = probability of food
# --prob_ghosts = probability of ghosts
# --ubtype = upper bound - "DEFAULT": hindsight; "TRIVIAL": R_max / (1-gamma) 
# --lbtype = lower bound - "RULES": rules (OURS); "TRIVIAL": east; "SMART": preferred
$DESPOT --size=0 --number=2 --prob_food=50 --prob_ghosts=75 --ubtype="DEFAULT" --lbtype="RULES" --runs=$((RUNS)) --save="$DIR/pocman_$((SIZE))size_ub_$UB""_lb_$LB""_ghosts2_fp_$((FP))_gp_$((GP)).txt";