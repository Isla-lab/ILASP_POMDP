ILASP="/usr/local/bin/ILASP" # path to ILASP

#ROCKSAMPLE
#READ TRACES
#Remember to comment either pocman or rocksample section in the end of the following script (see comments in it)
python read.py

#GENERATE ILASP FILES WITH MODE BIAS (it will take a while)
for A in "north" "south" "west" "east" "exit" "sample" "check" "target_sample"; do
    $ILASP --version=4 -s -nc --max-rule-length=6 -ml=5 $A".las" > "search_"$A".txt"
done

#POST-PROCESS THE SEARCH SPACE FOR BETTER PERFORMANCE, ACCORDING TO TASK NEEDS
python cut_ss_rocksample.py
python update_ss_rocksample.py

#RUN ILASP
for A in "north" "south" "west" "east" "exit" "sample" "check" "target_sample"; do
    $ILASP --version=4 --debug $A"_cut.las"
done

#GENERATE EXAMPLES FOR WEAK CONSTRAINTS
#currently only for target_sample. 
#If you want to generate for other actions, you have to:
# -modify target_atom name in the following script 
# -generate an ASP file (in our case, target_sample.lp) with the specific rules for your action learned before
# -modify the mode bias in mb_rocksample_opt.las to replace target_sample with the name of the action of interest
#Check comments in the python script, target_sample.lp and mb_rocksample_opt.las for more details and useful comments
python generate_opt_las.py

#RUN ILASP TO FIND WEAK CONSTRAINTS
for A in "target_sample"; do
    $ILASP --version=4 --debug $A"_opt.las"
done








#POCMAN================================================================================================================
# #READ TRACES (it will take a while, traces are huge!)
# # Remember to comment either pocman or rocksample section in the end of the following script (see comments in it)
# python read.py

# #GENERATE ILASP FILES WITH MODE BIAS (it will take a while)
# for A in "move"; do
#     $ILASP --version=4 -s -nc --max-rule-length=6 -ml=5 $A".las" > "search_"$A".txt"
# done

# #CUT MODE BIAS
# python cut_ss_pocman.py
# python update_ss_pocman.py

# #RUN ILASP
# for A in "move"; do # SIZE = distanza massima percorso
#     $ILASP --version=4 --debug $A"_cut.las"
# done
