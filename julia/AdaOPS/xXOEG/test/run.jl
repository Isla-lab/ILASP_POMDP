using POMDPs, AdaOPS, RockSample, POMDPSimulators, ParticleFilters, POMDPModelTools, POMDPTools, POMDPModels, POMDPPolicies, BasicPOMCP, Statistics, CSV, POMCPOW, ARDESPOT, Random, LinearAlgebra, Dates



function lower_rules_des(p::RockSamplePOMDP, b::ScenarioBelief)
    # Define your policy here based on the state
    # println("USING LOWER DES")
    actions = []
    prob_actions = []
    # println("CALLED")

    if length(b.scenarios) > 0
        pos = b.scenarios[1][2].pos
    else
        return 0
    end

    rocks = zeros(length(p.rocks_positions))  
    for j in 1:length(b.scenarios)
        for i in 1:length(p.rocks_positions)
            rocks[i] += b.scenarios[j][2].rocks[i] * b.scenarios[j][1]
        end
    end
    normalize!(rocks)
    # println(rocks)

    targets = []
    dists = zeros(length(p.rocks_positions))
    for i in 1:length(rocks)
        dists[i] = abs(pos[1] - p.rocks_positions[i][1]) + abs(pos[2] - p.rocks_positions[i][2])
        if dists[i] == 0 && rocks[i] <= 0.8 && rocks[i] > 0
            push!(actions, 5+i)
            push!(prob_actions, 0.85)
        end
        if (dists[i] <= 1 && rocks[i] > 0) || (rocks[i] >= 0.7 && rocks[i] <= 0.8)
            push!(targets, (i, dists[i], rocks[i]))
        end
    end
    # println(targets)
    if length(targets) > 0
        sort!(targets, by = x -> +x[2] - 0.5*x[3])
        target = targets[1]
        i = target[1]
        if rocks[i] <= 0.5
            push!(actions, 5+i)
            push!(prob_actions, 0.85*0.35)
        end
        if (dists[i] == 0 && rocks[i] >= 0.9)
            push!(prob_actions, 0.65)
            push!(actions, 1)
        end
        if pos[1] - p.rocks_positions[i][1] > 0
            push!(prob_actions, 0.73)
            push!(actions, 5)
        end
        if pos[1] - p.rocks_positions[i][1] < 0
            push!(prob_actions, 0.57)
            push!(actions, 3)
        end
        if pos[2] - p.rocks_positions[i][2] > 0
            push!(prob_actions, 0.65)
            push!(actions, 4)
        end
        if pos[2] - p.rocks_positions[i][2] < 0
            push!(prob_actions, 0.65)
            push!(actions, 2)
        end
    end


    if length(actions) > 0
        prob_actions = prob_actions ./ sum(prob_actions)
        cumulative_probabilities = cumsum(prob_actions)
        random_number = rand()
        for i in 1:length(cumulative_probabilities)
            if random_number <= cumulative_probabilities[i]
                action = actions[i]
                break
            end
        end
    else
        return p.discount_factor^(p.map_size[1]-pos[1]) * p.exit_reward
    end

    if action == 1
        return p.good_rock_reward
    elseif action == 3
        return p.discount_factor^(p.map_size[1]-pos[1]) * p.exit_reward
    elseif length(targets) > 0
        target = targets[1]
        i = target[1]
        cum_dist = dists[i]
        ret = p.discount_factor^(cum_dist) * p.good_rock_reward
        return ret
    else
        return p.discount_factor^(p.map_size[1]-pos[1]) * p.exit_reward
    end
end






function lower_rules_ada(p::RockSamplePOMDP, b::WPFBelief)
    # Define your policy here based on the state
    # println("USING LOWER")
    actions = []
    prob_actions = []

    if length(b.particles) > 0
        pos = b.particles[1].pos
    else
        return 0
    end

    rocks = zeros(length(p.rocks_positions))  
    for j in 1:length(b.particles)
        for i in 1:length(p.rocks_positions)
            rocks[i] += b.particles[j].rocks[i] * b.weights[j]
        end
    end
    normalize!(rocks)
    # println(rocks)

    times = []
    dists = zeros(length(p.rocks_positions))
    for i in 1:length(rocks)
        dists[i] = abs(pos[1] - p.rocks_positions[i][1]) + abs(pos[2] - p.rocks_positions[i][2])
        if (dists[i] <= 0 && rocks[i] <= 0.8 && rocks[i] > 0) || (dists[i] <= 2 && rocks[i] <= 0.6 && rocks[i] > 0)
            push!(actions, 5+i)
            push!(prob_actions, 0.84)
            push!(times, 0)
        end
        if dists <= 0 && rocks[i] >= 0.9
            push!(actions, 1)
            push!(times, 0)
            push!(prob_actions, 0.66)
        end
        if rocks[i] > 0
            if rocks[i] > 0.7 && p.rocks_positions[i][2] - pos[2] < 1 && p.rocks_positions[i][1] - pos[1] > 0 #east possible
                push!(actions, 3)
                push!(times, 0)
                push!(prob_actions, 0.89)
            end
            if rocks[i] < 0.9 && dists[i] < 2 && p.rocks_positions[i][1] - pos[1] < 0 #west possible
                push!(actions, 5)
                push!(times, 0)
                push!(prob_actions, 0.99)
            end
            if rocks[i] > 0.5 && dists[i] > 1 && p.rocks_positions[i][2] - pos[2] > 0 #north possible
                push!(actions, 2)
                push!(times, 0)
                push!(prob_actions, 0.96)
            end
            if rocks[i] > 0.6 && p.rocks_positions[i][2] - pos[2] < 0 #south possible
                push!(actions, 4)
                push!(times, 0)
                push!(prob_actions, 0.83)
            end
        end
    end


    if length(actions) > 0
        prob_actions = prob_actions ./ sum(prob_actions)
        cumulative_probabilities = cumsum(prob_actions)
        random_number = rand()
        for i in 1:length(cumulative_probabilities)
            if random_number <= cumulative_probabilities[i]
                action = actions[i]
                break
            end
        end
    else
        return p.discount_factor^(p.map_size[1]-pos[1]) * p.exit_reward
    end

    if action == 1
        return p.good_rock_reward
    elseif action == 3
        return p.discount_factor^(p.map_size[1]-pos[1]) * p.exit_reward
    elseif length(targets) > 0
        target = targets[1]
        i = target[1]
        cum_dist = dists[i]
        ret = p.discount_factor^(cum_dist) * p.good_rock_reward
        return ret
    else
        return p.discount_factor^(p.map_size[1]-pos[1]) * p.exit_reward
    end
end























function default_rules(p::RockSamplePOMDP, b::ParticleCollection)
    # Define your policy here based on the state
    # println("USING DEFAULT")
    actions = []
    prob_actions = []

    if length(b.particles) > 0
        pos = b.particles[1].pos
    else
        return 3
    end

    rocks = zeros(length(p.rocks_positions))  
    for j in 1:length(b.particles)
        for i in 1:length(p.rocks_positions)
            rocks[i] += b.particles[j].rocks[i]
        end
    end
    normalize!(rocks)
    # println(rocks)

    targets = []
    dists = zeros(length(p.rocks_positions))
    for i in 1:length(rocks)
        dists[i] = abs(pos[1] - p.rocks_positions[i][1]) + abs(pos[2] - p.rocks_positions[i][2])
        if dists[i] == 0 && rocks[i] <= 0.8 && rocks[i] > 0
            push!(actions, 5+i)
            push!(prob_actions, 0.85)
        end
        if (dists[i] <= 1 && rocks[i] > 0) || (rocks[i] >= 0.7 && rocks[i] <= 0.8)
            push!(targets, (i, dists[i], rocks[i]))
        end
    end
    if length(targets) > 0
        sort!(targets, by = x -> +x[2] - 0.5*x[3])
        target = targets[1]
        i = target[1]
        if rocks[i] <= 0.5
            push!(actions, 5+i)
            push!(prob_actions, 0.85*0.35)
        end
        if (dists[i] == 0 && rocks[i] >= 0.9)
            push!(prob_actions, 0.65)
            push!(actions, 1)
        end
        if pos[1] - p.rocks_positions[i][1] > 0
            push!(prob_actions, 0.73)
            push!(actions, 5)
        end
        if pos[1] - p.rocks_positions[i][1] < 0
            push!(prob_actions, 0.57)
            push!(actions, 3)
        end
        if pos[2] - p.rocks_positions[i][2] > 0
            push!(prob_actions, 0.65)
            push!(actions, 4)
        end
        if pos[2] - p.rocks_positions[i][2] < 0
            push!(prob_actions, 0.65)
            push!(actions, 2)
        end
    end

    if length(actions) > 0
        prob_actions = prob_actions ./ sum(prob_actions)
        cumulative_probabilities = cumsum(prob_actions)
        random_number = rand()
        for i in 1:length(cumulative_probabilities)
            if random_number <= cumulative_probabilities[i]
                action = actions[i]
                return [action]
            end
        end
        # action =  rand(actions)
    else
        return [3]
    end
end







function default_trivial(p::RockSamplePOMDP, b::ParticleCollection)
    return [3] #default to east
end

gamma = 0.95

uninformed_ub = 10 / (1-gamma)

#AdaOPS

# bounds_ada = AdaOPS.IndependentBounds(FORollout(RSExitSolver()), uninformed_ub, check_terminal=true, consistency_fix_thresh=1e-5) #lower bound = exit; upper bound = trivial
bounds_ada = AdaOPS.IndependentBounds(FORollout(RSExitSolver()), FOValue(RSMDPSolver()), check_terminal=true, consistency_fix_thresh=1e-5) #lower bound = exit; upper bound = hindsight
# bounds_ada = AdaOPS.IndependentBounds(lower_rules_ada, uninformed_ub, check_terminal=true, consistency_fix_thresh=1e-5) #lower bound = rules; upper bound = trivial
# bounds_ada = AdaOPS.IndependentBounds(lower_rules_ada, FOValue(RSMDPSolver()), check_terminal=true, consistency_fix_thresh=1e-5) #lower bound = rules; upper bound = hindsight

#DESPOT

bounds_des = ARDESPOT.IndependentBounds(DefaultPolicyLB(RSExitSolver()), FullyObservableValueUB(RSMDPSolver()), check_terminal=true, consistency_fix_thresh=1e-5) #lower bound = exit; upper bound = hindsight
# bounds_des = ARDESPOT.IndependentBounds(lower_rules_des, FullyObservableValueUB(RSMDPSolver()), check_terminal=true, consistency_fix_thresh=1e-5) #lower bound = rules; upper bound = hindsight
# bounds_des = ARDESPOT.IndependentBounds(lower_rules_des, uninformed_ub, check_terminal=true, consistency_fix_thresh=1e-5) #lower bound = rules; upper bound = trivial
# bounds_des = ARDESPOT.IndependentBounds(DefaultPolicyLB(RSExitSolver()), uninformed_ub, check_terminal=true, consistency_fix_thresh=1e-5) #lower bound = exit; upper bound = trivial


#CREATE SOLVERS

solver_des = DESPOTSolver(bounds=bounds_des, default_action=default_rules) #default_actions: default_rules: rules; default_trivial: trivial (exit)

solver_ada = AdaOPSSolver(bounds=bounds_ada,
                        default_action=default_rules,
                        delta=0.3,
                        m_min=30,
                        m_max=200,
                        tree_in_info=true,
                        num_b=10_000
                        ) #default_actions: default_rules: rules; default_trivial: trivial (exit)


#CREATE PROBLEM AND SOLVE

m = RockSamplePOMDP(12, 4)
adaops = solve(solver_ada, m)
ret = 0
i = 0
for (r,s) in stepthrough(m, adaops, "r,s", max_steps=90)
    global ret, i, gamma
    ret += r*(gamma^i)
    i=i+1
end
println("RETURN adaops")
println(ret)
