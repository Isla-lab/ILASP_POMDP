from pm4py.objects.log.importer.xes import importer as xes_importer
import numpy as np
import copy









def gen_ilp_pocman(actions_all, ghost_norm_all, food_norm_all, observations_all, disc_return_all, avg_return):

    ex_count = 1
    east_list = []
    north_list = []
    south_list = []
    west_list = []
    full_list = []

    action_list = ["north", "east", "south", "west"]

    for z in range(len(disc_return_all)):
        if disc_return_all[z] >= avg_return:
            for i in range(len(actions_all[z])): 
                context = ""
                line = ""
                included = ""
                excluded = []

                #build included and excluded
                included = actions_all[z][i]
                for ac in action_list:
                    if ac != actions_all[z][i]:
                        excluded.append(ac)




                #context    
                context += "pacman((" + str(current_all[z][i][0]) + "," + str(current_all[z][i][1]) +")). "
                pacman = (current_all[z][i][0], current_all[z][i][1])
                if food_norm_all[z][i] != [] and ghost_norm_all[z][i] != []:
                    food_probs = [[el[0], el[1], int(el[2]*100)] for el in food_norm_all[z][i]]
                    for x in range(1, 10):
                        food_probs_small = [f for f in food_probs if (0 > f[0]-pacman[0] >= -x) and f[1]==pacman[1]]
                        # if x > pacman[0]:
                            # break
                        food_probs_small.sort(key = lambda a : a[-1])
                        if food_probs_small != []:
                            context += "food(west," + str(food_probs_small[-1][-1]) + "," + str(x) + "). "
                        # else:
                            # context += "food(west,0," + str(x) + "). "
                    for x in range(1, 10):
                        food_probs_small = [f for f in food_probs if (0 < f[0]-pacman[0] <= x) and f[1]==pacman[1]]
                        # if x > 9-pacman[0]:
                            # break
                        food_probs_small.sort(key = lambda a : a[-1])
                        if food_probs_small != []:
                            context += "food(east," + str(food_probs_small[-1][-1]) + "," + str(x) + "). "
                        # else:
                            # context += "food(east,0," + str(x) + "). "
                    for y in range(1, 10):
                        food_probs_small = [f for f in food_probs if (0 > f[1]-pacman[1] >= -y) and f[0]==pacman[0]]
                        # if y > pacman[1]:
                            # break
                        food_probs_small.sort(key = lambda a : a[-1])
                        if food_probs_small != []:
                            context += "food(south," + str(food_probs_small[-1][-1]) + "," + str(y) + "). "
                        # else:
                            # context += "food(south,0," + str(y) + "). "
                    for y in range(1, 10):
                        food_probs_small = [f for f in food_probs if (0 < f[1]-pacman[1] <= y) and f[0]==pacman[0]]
                        # if y > 9-pacman[1]:
                            # break
                        food_probs_small.sort(key = lambda a : a[-1])
                        if food_probs_small != []:
                            context += "food(north," + str(food_probs_small[-1][-1]) + "," + str(y) + "). "



                    ghost_probs = [[el[0], el[1], int(el[2]*100)] for el in ghost_norm_all[z][i]]
                    for x in range(1, 10):
                        ghost_probs_small = [f for f in ghost_probs if (0 > f[0]-pacman[0] >= -x) and f[1]==pacman[1]]
                        ghost_probs_small.sort(key = lambda a : a[-1])
                        if ghost_probs_small != []:
                            context += "ghost(west," + str(ghost_probs_small[-1][-1]) + "," + str(x) + "). "
                    for x in range(1, 10):
                        ghost_probs_small = [f for f in ghost_probs if (0 < f[0]-pacman[0] <= x) and f[1]==pacman[1]]
                        ghost_probs_small.sort(key = lambda a : a[-1])
                        if ghost_probs_small != []:
                            context += "ghost(east," + str(ghost_probs_small[-1][-1]) + "," + str(x) + "). "
                    for y in range(1, 10):
                        ghost_probs_small = [f for f in ghost_probs if (0 > f[1]-pacman[1] >= -y) and f[0]==pacman[0]]
                        ghost_probs_small.sort(key = lambda a : a[-1])
                        if ghost_probs_small != []:
                            context += "ghost(south," + str(ghost_probs_small[-1][-1]) + "," + str(y) + "). "
                    for y in range(1, 10):
                        ghost_probs_small = [f for f in ghost_probs if (0 < f[1]-pacman[1] <= y) and f[0]==pacman[0]]
                        ghost_probs_small.sort(key = lambda a : a[-1])
                        if ghost_probs_small != []:
                            context += "ghost(north," + str(ghost_probs_small[-1][-1]) + "," + str(y) + "). "

                    if i > 0:
                        observation = format(observations_all[z][i-1], "010b")
                        if observation[5] == "0":
                            context += "feel_wall(north). "
                        if observation[4] == "0":
                            context += "feel_wall(east). "
                        if observation[3] == "0":
                            context += "feel_wall(south). "
                        if observation[2] == "0":
                            context += "feel_wall(west). "








                #build example lines separately for each action
                line = "#pos(ex" + str(ex_count) + ", {" + included + "}, {}, {" + context + "})."
                if included == "east":
                    east_list.append(line)
                if included == "north":
                    north_list.append(line)
                if included == "south":
                    south_list.append(line)
                if included == "west":
                    west_list.append(line)
                ex_count += 1
                for exc in excluded:
                    line = "#pos(ex" + str(ex_count) + ", {}, {" + exc + "}, {" + context + "})."
                    if exc == "east":
                        east_list.append(line)
                    if exc == "north":
                        north_list.append(line)
                    if exc == "south":
                        south_list.append(line)
                    if exc == "west":
                        west_list.append(line)
                    ex_count += 1
                
                line = "#pos(ex" + str(ex_count) + ", {move(" + included + ")}, {"
                line = line + "}, {" + context + "})."
                full_list.append(line)
                ex_count += 1



    with open("mb_pocman.las", "r") as f:
        mb_lines = f.readlines()

    with open("move.las", "w") as ilp_file:
        for l in full_list:
            ilp_file.write(l)
            ilp_file.write("\n")
        ilp_file.write("\n")
        for mb_l in mb_lines:
            ilp_file.write(mb_l)







def gen_ilp_rs(actions_all, delta_all, dist_all, ext_norm_all, quality_all, targets_all, avg_return):

    #ROCKSAMPLE
    NUM_ROCKS = 4

    ex_count = 1
    east_list = []
    west_list = []
    north_list = []
    south_list = []
    check_list = []
    exit_list = []
    target_sample_list = []
    sample_list = []

    sampled = [] #sampled rocks

    for z in range(len(actions_all)):
        for i in range(len(actions_all[z])): 
            if dist_all[z][i][1] < avg_return:
            # if True:
                if (actions_all[z][i] == "sample" and quality_all[z][i] != "bad") or i == len(actions_all[z])-1:
                    j = i
                    target_sample = -1
                    target = -1
                    while j >= 0:
                        if (actions_all[z][j] == "sample" and j!=i):
                            break

                        context = ""
                        line = ""
                        line_ts = ""
                        included = ""
                        excluded = ""
                        included_ts = ""
                        excluded_ts = ""





                        if actions_all[z][j] == "sample" and quality_all[z][j] != "bad":
                            sampled.append(targets_all[z][j])
                            target_sample = targets_all[z][j]
                        
                        target = copy.deepcopy(target_sample)
                        
                        if actions_all[z][j] == "check":
                            target = targets_all[z][j]

                        #build included and excluded
                        if target != -1:
                            included = actions_all[z][j] +"(" + str(target) + ")"
                            for id_rock in range(np.shape(dist_all[z][j][0])[0]):
                                if id_rock != target:
                                    excluded += actions_all[z][j] + "(" + str(id_rock) + "), "
                            excluded = excluded[:-2]

                        else:
                            included = "exit"

                        #also for target sample
                        if target_sample != -1:
                            included_ts = "target_sample(" + str(target_sample) + ")"
                            for id_rock in range(np.shape(dist_all[z][j][0])[0]):
                                if id_rock != target_sample:
                                    excluded_ts += "target_sample(" + str(id_rock) + "), "
                            excluded_ts = excluded_ts[:-2]










                        #context
                        for k in range(np.shape(dist_all[z][j][0])[0]):
                            if k not in sampled[:-1]: #exclude previously sampled rocks, except the current one
                                context += "dist(" + str(k) + "," + str(dist_all[z][j][0][k]) + "). "
                                context += "delta_x(" + str(k) + "," + str([d[0] for d in delta_all[z][j][0]][k]) + "). "
                                context += "delta_y(" + str(k) + "," + str([d[1] for d in delta_all[z][j][0]][k]) + "). "

                        for k in sampled[:-1]:
                            dist_all[z][j][0][k] = 100
                        best_distances = np.where(np.array(dist_all[z][j][0])==np.min(dist_all[z][j][0]))[0]
                        for k in sampled[:-1]:
                            dist_all[z][j][0][k] = -100
                        worst_distances = np.where(np.array(dist_all[z][j][0])==np.max(dist_all[z][j][0]))[0]

                        step = 10
                        guesses = [int(np.floor(el*100/step)*step) for el in ext_norm_all[z][j][0]]
                        for k, guess in enumerate(guesses):
                            if k not in sampled[:-1]:
                                context += "guess(" + str(k) + "," + str(guess) + "). "
                        for k in sampled[:-1]:
                            guesses[k] = -100
                        best_guesses = np.where(np.array(guesses)==np.max(guesses))[0]
                        for k in sampled[:-1]:
                            guesses[k] = 100
                        worst_guesses = np.where(np.array(guesses)==np.min(guesses))[0]

                        if target_sample == -1: 
                            context += "num_sampled(" + str(int(100. * len(sampled) / NUM_ROCKS)) + "). "
                        else: #last sample is yet to be performed
                            context += "num_sampled(" + str(int(100. * len(sampled[:-1]) / NUM_ROCKS)) + "). "








                        #build example line
                        if target_sample != -1:
                            context += "target_sample(" + str(target_sample) + "). "
                            line_ts += "#pos(ex" + str(ex_count) + ", {" + included_ts + "}, {" + excluded_ts + "}, {" + context.replace("target_sample(" + str(target_sample) + "). ", "") + "})."
                            ex_count += 1

                        line += "#pos(ex" + str(ex_count) + ", {" + included + "}, {" + excluded + "}, {" + context + "})."
                        ex_count += 1

                        #append to list for file writing
                        if target_sample != -1: #motion is relevant only when a target exists
                            if "east" in included:
                                east_list.append(line)
                            elif "west" in included:
                                west_list.append(line)
                            elif "north" in included:
                                north_list.append(line)
                            elif "south" in included:
                                south_list.append(line)
                        if "check" in included:
                            check_list.append(line)
                        elif "sample" in included:
                            sample_list.append(line)
                        elif "exit" in included:
                            exit_list.append(line)
                            
                        if line_ts != "":
                            target_sample_list.append(line_ts)





                        #build "negative" example line
                        if "sample" not in included:
                            line = "#pos(ex" + str(ex_count) + ", {}, {sample(_)}, {" + context + "})."
                            ex_count += 1
                            sample_list.append(line)
                        if "exit" not in included:
                            line = "#pos(ex" + str(ex_count) + ", {}, {exit}, {" + context + "})."
                            ex_count += 1
                            exit_list.append(line)
                        if "check" not in included:
                            line = "#pos(ex" + str(ex_count) + ", {}, {check(_)}, {" + context + "})."
                            ex_count += 1
                            check_list.append(line)
                        if target_sample != -1: #motion is relevant only when a target exists
                            if "east" not in included:
                                line = "#pos(ex" + str(ex_count) + ", {}, {east(_)}, {" + context + "})."
                                ex_count += 1
                                east_list.append(line)
                            if "north" not in included:
                                line = "#pos(ex" + str(ex_count) + ", {}, {north(_)}, {" + context + "})."
                                ex_count += 1
                                north_list.append(line)
                            if "south" not in included:
                                line = "#pos(ex" + str(ex_count) + ", {}, {south(_)}, {" + context + "})."
                                ex_count += 1
                                south_list.append(line)
                            if "west" not in included:
                                line = "#pos(ex" + str(ex_count) + ", {}, {west(_)}, {" + context + "})."
                                ex_count += 1
                                west_list.append(line)
                        else:
                            line_ts = "#pos(ex" + str(ex_count) + ", {}, {target_sample(_)}, {" + context + "})."
                            ex_count += 1
                            target_sample_list.append(line_ts)



                        if (actions_all[z][j] == "check" and any([ac for ac in actions_all[z][j:i] if "sample" not in ac and "check" not in ac])):
                            break

                        #to next iteration
                        j -= 1



        sampled = []


    with open("mb_rocksample.las", "r") as f:
        mb_lines = f.readlines()

    #write files
    with open("east.las", "w") as ilp_file:
        for l in east_list:
            ilp_file.write(l)
            ilp_file.write("\n")            
        ilp_file.write("\n")
        ilp_file.write("#modeb(1, target_sample(var(rocks)), (positive)).\n")
        ilp_file.write("#modeh(east).\n")
        for mb_l in mb_lines:
            ilp_file.write(mb_l)

    with open("exit.las", "w") as ilp_file:
        for l in exit_list:
            ilp_file.write(l)
            ilp_file.write("\n")            
        ilp_file.write("\n")
        ilp_file.write("#modeh(exit).\n")
        for mb_l in mb_lines:
            ilp_file.write(mb_l)

    with open("north.las", "w") as ilp_file:
        for l in north_list:
            ilp_file.write(l)
            ilp_file.write("\n")            
        ilp_file.write("\n")
        ilp_file.write("#modeb(1, target_sample(var(rocks)), (positive)).\n")
        ilp_file.write("#modeh(north).\n")
        for mb_l in mb_lines:
            ilp_file.write(mb_l)

    with open("south.las", "w") as ilp_file:
        for l in south_list:
            ilp_file.write(l)
            ilp_file.write("\n")            
        ilp_file.write("\n")
        ilp_file.write("#modeb(1, target_sample(var(rocks)), (positive)).\n")
        ilp_file.write("#modeh(south).\n")
        for mb_l in mb_lines:
            ilp_file.write(mb_l)

    with open("west.las", "w") as ilp_file:
        for l in west_list:
            ilp_file.write(l)
            ilp_file.write("\n")            
        ilp_file.write("\n")
        ilp_file.write("#modeb(1, target_sample(var(rocks)), (positive)).\n")
        ilp_file.write("#modeh(west).\n")
        for mb_l in mb_lines:
            ilp_file.write(mb_l)

    with open("target_sample.las", "w") as ilp_file:
        for l in target_sample_list:
            ilp_file.write(l)
            ilp_file.write("\n")            
        ilp_file.write("\n")
        ilp_file.write("#modeh(target_sample(var(rocks))).\n")
        for mb_l in mb_lines:
            ilp_file.write(mb_l)

    with open("check.las", "w") as ilp_file:
        for l in check_list:
            ilp_file.write(l)
            ilp_file.write("\n")            
        ilp_file.write("\n")
        ilp_file.write("#modeh(check(var(rocks))).\n")
        for mb_l in mb_lines:
            ilp_file.write(mb_l)

    with open("sample.las", "w") as ilp_file:
        for l in sample_list:
            ilp_file.write(l)
            ilp_file.write("\n")            
        ilp_file.write("\n")
        ilp_file.write("#modeh(sample(var(rocks))).\n")
        for mb_l in mb_lines:
            ilp_file.write(mb_l)







#READ XES (UNCOMMENT EITHER POCMAN OR ROCKSAMPLE)=============================================================================================================================

#POCMAN
# filenames = ["pocman.xes"]
# for filename in filenames:
#     #GENERAL DATA ABOUT SCENARIO
#     log = xes_importer.apply(filename)
#     ghosts = log.attributes["Ghosts"]
#     size = log.attributes["SizeX"]
#     avg_return = log.attributes["average discounted return"]
#     std_return = log.attributes["average discounted return std"]

#     current_all = [] #current position of pacman
#     dist_to_food_all = [] #manhattan from current pos to food
#     delta_to_food_all = [] # (delta_x, delta_y) from current pos to food
#     dist_to_ghost_all = [] #manhattan from current pos to ghost
#     delta_to_ghost_all = [] # (delta_x, delta_y) from current pos to ghost
#     food_norm_all = [] #sum of believes with target / sum of believes
#     ghost_norm_all = [] #sum of believes with target / sum of believes
#     observations_all = [] #pacman observations as 10-digits binary [see NESW; wall NESW; smell food; hear ghost]
#     actions_all = []
#     rewards_all = []
#     disc_return_all = []

#     i = 1
#     while i < len(log):

#         #INIT DATA ARRAYS
#         dist_to_food = [] #manhattan from current pos to food
#         delta_to_food = [] # (delta_x, delta_y) from current pos to food
#         dist_to_ghost = [] #manhattan from current pos to ghost
#         delta_to_ghost = [] # (delta_x, delta_y) from current pos to ghost
#         food_norm = [] #sum of believes with target / sum of believes
#         ghost_norm = [] #sum of believes with target / sum of believes
#         observations = []
#         rewards = []
#         actions = []
#         current = []
#         disc_return = log[i].attributes["discounted return"]
#         for j in range(len(log[i])):
#             if log[i][j]["belief"] != None and log[i][j]["belief"]["children"]["ghosts"]!= None and log[i][j]["belief"]["children"]["food"] != None:
#                 belief = [(k, v) for k, v in log[i][j]["belief"]["children"]["food"]["children"].items()]
#                 norm_belief_food = belief#[(k, v/sum_b) for k, v in belief]
#                 belief = [(k, v) for k, v in log[i][j]["belief"]["children"]["ghosts"]["children"].items()]
#                 norm_belief_ghost = belief#[(k, v/sum_b) for k, v in belief]

#                 rewards.append(log[i][j]["reward"])
#                 observations.append(log[i][j]["observation"])
#                 actions.append(log[i][j]["action"])
#                 current.append((log[i][j]["coord x"], log[i][j]["coord y"]))
#                 food = []
#                 ghost = []
#                 for x in range(size):
#                     for y in range(size):
#                         if any([ el for el in norm_belief_food if el[0] == (str(x) + "," + str(y))]):
#                             tup = [ el for el in norm_belief_food if el[0] == (str(x) + "," + str(y))][0]
#                             food.append([x,y,tup[1]])
#                         else:
#                             food.append([x,y,0.])

#                         if any([ el for el in norm_belief_ghost if el[0] == (str(x) + "," + str(y))]):
#                             tup = [ el for el in norm_belief_ghost if el[0] == (str(x) + "," + str(y))][0]
#                             ghost.append([x,y,tup[1]])
#                         else:
#                             ghost.append([x,y,0.])
                
#                 food_norm.append(food)
#                 ghost_norm.append(ghost)


#         actions_all.append(actions)
#         ghost_norm_all.append(ghost_norm)
#         food_norm_all.append(food_norm)
#         observations_all.append(observations)
#         rewards_all.append(rewards)
#         disc_return_all.append(disc_return)
#         current_all.append(current)

#         i += 1


#     actions_all = np.array(actions_all, dtype=object)
#     ghost_norm_all = np.array(ghost_norm_all, dtype=object)
#     food_norm_all = np.array(food_norm_all, dtype=object)
#     observations_all = np.array(observations_all, dtype=object)
#     rewards_all = np.array(rewards_all, dtype=object)
#     disc_return_all = np.array(disc_return_all, dtype=object)
#     current_all = np.array(current_all, dtype=object)

# gen_ilp_pocman(actions_all, ghost_norm_all, food_norm_all, observations_all, disc_return_all, avg_return)











#ROCKSAMPLE
filenames = ["rocksample.xes"]
inner_rocks = False
# filenames = ["rs_run100_part11.xes", "rs_run100_part13.xes", "rs_run100_part15.xes", "rs_run1000_part11.xes", "rs_run1000_part13.xes", "rs_run1000_part15.xes"]
for filename in filenames:
    #GENERAL DATA ABOUT SCENARIO
    log = xes_importer.apply(filename)
    try:
        rocks = [(log.attributes["rocks"]["children"][k]["children"]["coord x"], log.attributes["rocks"]["children"][k]["children"]["coord y"]) for k in log.attributes["rocks"]["children"].keys()]
    except:
        inner_rocks = True
    avg_return = log.attributes["average discounted return"]
    std_return = log.attributes["average discounted return std"]

    dist_to_rocks_all = [] #manhattan from current pos to rocks
    delta_to_rocks_all = [] # (delta_x, delta_y) from current pos to rocks
    correct_belief_ext_all = [] #sum of believes with target
    correct_belief_ext_norm_all = [] #sum of believes with target / sum of believes
    actions_all = []
    targets_all = [] #for sample and check
    quality_all = [] #for sample, distinguish between good or bad sample

    #ALL actions in rocksample
    i = 1
    while i < len(log):
        if inner_rocks:
            try:
                rocks = [(log[i].attributes["rocks"]["children"][k]["children"]["coord x"], log[i].attributes["rocks"]["children"][k]["children"]["coord y"]) for k in log[i].attributes["rocks"]["children"].keys()]
            except:
                i += 1
                continue
        #INIT DATA ARRAYS
        dist_to_rocks = [] #manhattan from current pos to rocks
        delta_to_rocks = [] # (delta_x, delta_y) from current pos to rocks
        correct_belief_ext = [] #sum of believes with target
        correct_belief_ext_norm = [] #sum of believes with target / sum of believes
        actions = []
        targets = [] #for sample and check
        quality = [] #for sample, distinguish between good or bad sample
        for j in range(len(log[i])):
            if log[i][j]["belief"] != None:
                belief = [(k, v) for k, v in log[i][j]["belief"]["children"].items()]
                sum_b = np.sum([v[1] for v in belief])
                norm_belief = [(k, v/sum_b) for k, v in belief]

                current = (log[i][j]["coord x"], log[i][j]["coord y"])
                dist = []
                delta = []
                correct_belief = []
                correct_belief_norm = []
                for target in range(len(rocks)):
                    rock = rocks[target]
                    dist.append(abs(rock[0] - current[0]) + abs(rock[1] - current[1]))
                    delta.append((rock[0] - current[0], rock[1] - current[1]))

                    sum_target_norm = np.sum([v[1] for v in norm_belief if len(bin(int(v[0]))[2:])>target and bin(int(v[0]))[2:][-target-1]=="1"])
                    sum_target = np.sum([v[1] for v in belief if len(bin(int(v[0]))[2:])>target and bin(int(v[0]))[2:][-target-1]=="1"])
                    correct_belief.append(sum_target)
                    correct_belief_norm.append(sum_target_norm)
                dist_to_rocks.append((dist, log[i].attributes["discounted return"], log[i].attributes["run"]))
                delta_to_rocks.append((delta, log[i].attributes["discounted return"], log[i].attributes["run"]))
                correct_belief_ext.append((correct_belief, log[i].attributes["discounted return"], log[i].attributes["run"]))
                correct_belief_ext_norm.append((correct_belief_norm, log[i].attributes["discounted return"], log[i].attributes["run"]))

                actions += [log[i][j]["action"].split(" ")[0]]
                if "sample" in log[i][j]["action"]:
                    target = 100
                    for q in range(len(rocks)):
                        if current == rocks[q]:
                            target = q
                            break
                    targets.append(target)
                elif "check" in log[i][j]["action"]:
                    targets.append(int(log[i][j]["action"].split(" ")[1]))
                else:
                    targets.append(-1)
                if log[i][j]["reward"] > 0.:
                    quality.append("good")
                else:
                    quality.append("bad")

        targets_all.append(targets)
        quality_all.append(quality)
        actions_all.append(actions)
        dist_to_rocks_all.append(dist_to_rocks)
        delta_to_rocks_all.append(delta_to_rocks)
        correct_belief_ext_all.append(correct_belief_ext)
        correct_belief_ext_norm_all.append(correct_belief_ext_norm)

        i += 1

gen_ilp_rs(actions_all=actions_all, delta_all=delta_to_rocks_all, dist_all=dist_to_rocks_all, ext_norm_all=correct_belief_ext_norm_all, quality_all=quality_all, targets_all=targets_all, avg_return=avg_return)