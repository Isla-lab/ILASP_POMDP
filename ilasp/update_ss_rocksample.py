atoms = ["east", "north", "south", "west", "exit", "target_sample", "check", "sample"]

for atom in atoms:

    f_ss = "search_" + atom + "_cut.txt" #+ "_num_sampled.txt"
    f_read = atom + ".las" #+ "_num_sampled_mod.txt"
    f_write = atom + "_cut.las" #+ "_num_sampled_mod.txt"

    with open(f_ss, "r") as f:
        ss = f.readlines()

    examples = []
    with open(f_read, "r") as f:
        for l in f.readlines():
            if l[0:4] != "#pos":
                break
            examples.append(l)
    
    with open(f_write, "w") as f:
        for l in examples:
            f.write(l)
        f.write("\n")
        for l in ss:
            f.write(l)
