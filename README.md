# PAPER REFERENCE
This repo contains code for the paper:
@article{meli2024learning,
  title={Learning logic specifications for policy guidance in POMDPs: an inductive logic programming approach},
  author={Meli, Daniele and Castellini, Alberto and Farinelli, Alessandro},
  journal={Journal of Artificial Intelligence Research},
  volume={79},
  pages={765--816},
  year={2024}
}

which extends the paper:
@inproceedings{mazzi2023learning,
  title={Learning Logic Specifications for Soft Policy Guidance in POMCP},
  author={Mazzi, Giulio and Meli, Daniele and Castellini, Alberto and Farinelli, Alessandro},
  booktitle={Proceedings of the 2023 International Conference on Autonomous Agents and Multiagent Systems},
  pages={373--381},
  year={2023}
}

Please cite both if using or extending this code.

For inductive logic programming, please refer to:
https://www.ilasp.com/
For answer set programming, please refer to:
http://wp.doc.ic.ac.uk/arusso/wp-content/uploads/sites/47/2015/01/clingo_guide.pdf

### IMPORTANT: It is strongly suggested that you know something about the above, before continuing with this code!

# STRUCTURE
The repo contains code for learning policy heuristics in logical formalism (answer set programming), and then solving the rocksample and the pocman (partially observable pacman) domains in DESPOT, POMCP and AdaOPS.
- despot and pomcp folders contain C++ code for the two solvers.
- julia folder contains julia code for running AdaOPS and ARDespot (equivalent to C++ DESPOT).
- ilasp folder contains traces and files from rocksample and pocman, to generate the heuristics for the two tasks using Inductive Learning of Answer Set Programs (ILASP)
- supplementary.pdf contains further experiments with Julia implementation of DESPOT in the rocksample domain.

# RUN
- POMCP: launch run.sh in pomcp/scripts to replicate experiments in the paper. Follow comments in the code for parameter description. The output is a xes file with statistics
- DESPOT: launch run.sh in despot/examples/cpp_models/{pocman, rock_sample} to replicate experiments in the paper. Follow comments in the code for parameter description. The output is a txt file with statistics
- Julia: launch run.jl in julia/AdaOPS/xXOEG/test to replicate experiments in the paper (only rocksample). Follow comments in the code for parameter description.
- ilasp: unzip traces.zip, then launch learn.sh to generate ASP heuristics from POMDP traces (follow comments in the bash file for more details). 
