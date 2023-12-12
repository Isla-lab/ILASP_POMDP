RUNS=50  # number of runs per experiment
N=15; # number of particles and simulations (2^X)
POMCP="../build/pomcp" #executable
DIR="./rocksample_results" #to save results
mkdir -p $DIR

TK=3; #UCT SEARCH - 1: only legal; 2: preferred; 3: rules (OURS);
RK=4; #ROLLOUT - 1: random; 2: preferred; 3: all probs equal to the maximum; 4: weighted probs according to coverage factor (OURS)

### ROCKSAMPLE
#PARAMETERS:
#--size : grid size
#--number : num rocks
#--soft : use soft bias in rollout
#--val : percentage of examples from which rules are generated (EXP-4 in paper) 
###
# $POMCP --problem rocksample --size 12 --number 4 --soft=true --val 100 --timeout 100000000 --mindouble $((N)) --maxdouble $((N)) --runs $((RUNS)) --xes 1 --rolloutknowledge=$((RK)) --treeknowledge=$((TK)) --smarttreecount 10 --smarttreevalue 20;
# mv ./log.xes "$DIR/rocksample_$((N))part_$((SIZE))size_$((ROCKS))rocks_r$((RK))_t$((TK)).xes";





### POCMAN
#PARAMETERS:
#--problem : minipocman (10x10) or pocman (17x19)
#--size : food probability percentage
#--number : ghost probability percentage
#--ghosts : num ghosts
#--soft : use soft bias in rollout
###
$POMCP --problem "minipocman" --size=50 --number=75 --ghosts=2 --timeout 100000000 --mindouble $((N)) --maxdouble $((N)) --runs $((RUNS)) --xes 1 --rolloutknowledge=$((RK)) --treeknowledge=$((TK)) --smarttreecount 10 --smarttreevalue 20;
mv ./log.xes $DIR"/"$NAME"_$((N))part_r$((RK))_t$((TK))_ghosts$((GH))_fp$((FP))_gp$((GP)).xes";