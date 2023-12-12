actions = ["move"]
relevant_atoms = ["food(V", "ghost(V"]

for a in actions:
    lines_search = []
    lines_opt = []
    with open("search_"+a+".txt", "r") as f:
        for l in f:
            good = True
            seen_vars = []
            for atom in relevant_atoms:
                if atom in l:
                    where_atom = l.index(atom)
                    where_start_atom = l[where_atom:].index("(")
                    where_end_atom = l[where_atom:].index(")")
                    substring = [l[where_atom + where_start_atom + 1 : where_atom + where_end_atom].split(",")[-2]]
                    substring = [s for s in substring if "V" in s][-1]
                    for relevant_var in substring:
                        count_more = l.replace(" ", "").count(relevant_var + ">=")
                        count_less = l.replace(" ", "").count(relevant_var + "<=")
                        if count_less+count_more != 1 or relevant_var in seen_vars: #count_more > 1 or count_less > 1 or 
                            good = False
                            break
                        seen_vars.append(relevant_var)

                if not good:
                    break
            
            if good and "feel_wall(V1)" in l and ("food(V1" in l or "ghost(V1" in l):# and not((l.count(">=") + l.count("<=")) == 1 and l.count(">= 0") == 1):
                lines_search.append(l)

    



    with open("search_"+a+"_cut.txt", "w") as f:
        f.writelines(lines_search)