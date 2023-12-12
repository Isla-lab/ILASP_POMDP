import re
from clingo import Function as Fun, Control, Number
from gringoParser import string2fun as parse








NUM_VAR = 4









class Solver(object):    
    def __init__(self, filename):
        self.filename = filename
        self.control = Control(["0"])
        self.control.load(filename)
        self.control.ground([("base", [])])        
        self.atoms = []
        self.hidden_atoms = []
        
    def on_model(self, model):
        self.atoms[:] = model.symbols(shown=True)
        self.hidden_atoms[:] = model.symbols(atoms=True)

    def solve(self, context, head):
        '''head = str, the atom we are interested to find'''
        answer_set = []

        #DELETE EXTERNALS FROM PREVIOUS CALL
        for atom in self.hidden_atoms:
            self.control.assign_external(atom, False)
        #GROUND NEW EXTERNALS
        for atom in context:
            self.control.assign_external(parse(atom), True)

        result = self.control.solve(on_model = self.on_model)

        if result.satisfiable:
            for atom in self.atoms:
                if atom.name == head:
                    answer_set.append(str(atom))
        
        return answer_set










#MODIFY THIS TO FIND WEAK CONSTRAINTS ABOUT OTHER ACTIONS
task_names = ["target_sample"]

for task_name in task_names:
    target_atom = task_name
    asp_solver = Solver(task_name + ".lp")
    opt_lines = []
    bad_lines = []
    orig_lines = []
    bad = 1

    with open(task_name + ".las", "r") as f:
        for l in f.readlines():
            orig_lines.append(l)
            if "(ex" in l:
                if "#pos" in l:
                    curly_brac_l = [m.start() for m in re.finditer('{', l)]
                    curly_brac_r = [m.start() for m in re.finditer('}', l)]
                    #original ex
                    head = l[curly_brac_l[0]+1:curly_brac_r[0]]
                    context = l[curly_brac_l[2]+1:curly_brac_r[2]]
                    context = context.replace(" ", "")
                    context = context.split(".")[:-1]
                    #get other heads from asp program with learned normal rules
                    possible_heads = asp_solver.solve(context, target_atom)
                    #head not in possible heads: REAL COUNTEREXAMPLE!
                    if (head != "" and head not in possible_heads) or (head == "" and len(possible_heads) !=0): 
                        bad_lines.append(l) #save just for statistics         
                    #head in possible_heads: generate ex for weak constraints    
                    elif head != "" and head in possible_heads: 
                        ch = "a"
                        if len(possible_heads) > 1:
                            new_line = l[: curly_brac_l[0]] + "{}, {}, {" + head + ". " + l[curly_brac_l[2]+1 :]
                            opt_lines.append(new_line)
                            for h in possible_heads:
                                # add brave ordering for each different head than the one in the original example
                                if h != head:
                                    where_id = l.find("(ex") + 1
                                    where_comma = l.find(",")
                                    ex_id = l[where_id : where_comma]
                                    opt_lines.append(new_line.replace(head, h).replace(ex_id, ex_id + "_" + ch))
                                    opt_lines.append("#brave_ordering(" + ex_id + ", " + ex_id + "_" + ch + ", <).\n")
                                    ch = chr(ord(ch) + 1)



    with open("mb_rocksample_opt.las", "r") as f:
        mb_lines = f.readlines()


    with open(task_name + "_opt.las", "w") as f:
        for l in opt_lines:
            f.write(l)
        f.write("\n")
        for l in mb_lines:
            f.write(l)

