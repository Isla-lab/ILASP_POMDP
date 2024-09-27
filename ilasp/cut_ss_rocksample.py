NUM_VARS = 3










atoms = ["east", "north", "south", "west", "exit", "target_sample", "check", "sample"]

for atom in atoms:

    f_read = "search_" + atom + ".txt" #+ "_num_sampled.txt"
    f_write = "search_" + atom + "_cut.txt" #+ "_num_sampled_mod.txt"

    save_lines = []
    with open(f_read, "r") as f:
        for l in f:
            good = True

            #exclude constraints
            if atom not in l:
                good = False
                continue
            
            for i in range(NUM_VARS): #one arithmetic comparison per variable
                if l.count("V"+str(i+1)+" <") + l.count("V"+str(i+1)+" >") > 1:
                    good = False
                    break

            #ONLY ONE VARIABLE ROCK PER RULE
            if "sample" in atom or atom == 'check': #only head atoms with a variable (rock id)
                for body in [" sampled", "target_sample", "guess", "dist", "delta_x", "delta_y"]:
                    if body in l and body+"(V1" not in l: 
                        good = False
                        break
                
            if not good:
                continue

            try:
                where = l.index("dist")
                id_br = l[where:].index(")")
                #upper and lower bound on dist
                if (l[where + id_br-2: where + id_br] + " <") not in l and (l[where + id_br-2: where + id_br] + " >") not in l:
                    good = False
            except:
                pass

            try:
                where = l.index("delta_x")
                id_br = l[where:].index(")")
                #upper and lower bound on delta_x
                if (l[where + id_br-2: where + id_br] + " <") not in l and (l[where + id_br-2: where + id_br] + " >") not in l:
                    good = False
            except:
                pass

            try:
                where = l.index("delta_y")
                id_br = l[where:].index(")")
                #upper and lower bound on delta_y
                if (l[where + id_br-2: where + id_br] + " <") not in l and (l[where + id_br-2: where + id_br] + " >") not in l:
                    good = False
            except:
                pass

            try:
                where = l.index(" guess")
                id_br = l[where:].index(")")
                #upper and lower bound on guess
                if (l[where + id_br-2: where + id_br] + " <") not in l and (l[where + id_br-2: where + id_br] + " >") not in l:
                    good = False
            except:
                pass

            try:
                where = l.index("num_sampled")
                id_br = l[where:].index(")")
                #lower bound on num_sampled
                if (l[where + id_br-2: where + id_br] + " >") not in l :
                    good = False
                if l.count(l[where + id_br-2: where + id_br]) > 2: #no other atom may contain the rock variable
                    good = False
            except:
                pass

            #exit must depend on the number of sampled actions
            if atom=="exit" and ("delta" in l or "num_sampled" not in l):# or "delta" in l):
                good = False

            #motion actions depend on delta and target, but not on num_sampled
            elif (atom=="north" or atom=="south" or atom=="west" or atom=="east") and ("target_sample" not in l or "delta" not in l or "num_sampled" in l):
                good = False
            
            #sample actions depend on target but not on delta or num_sampled
            elif (atom=="sample" or atom=="target_sample") and ("target_sample" not in l or "delta" in l or "num_sampled" in l):
                good = False
            
            #check actions do not depend on delta or num_sampled
            elif atom == "check" and ("delta" in l or "num_sampled" in l):
                good = False

            if good:
                save_lines.append(l)


    with open(f_write, "w") as f:
        for l in save_lines:
            f.write(l)