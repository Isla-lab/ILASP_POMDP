% #include <incmode>.



#program base.



% variables (task-specific)
#const num_rocks=4.
rock(0..num_rocks-1).
perc_rocks(0).
perc_rocks(25).
perc_rocks(50).
perc_rocks(75).
perc_rocks(100).
ranges_dist(-24..24).
range_guess(0).
range_guess(10).
range_guess(20).
range_guess(30).
range_guess(40).
range_guess(50).
range_guess(60).
range_guess(70).
range_guess(80).
range_guess(90).
range_guess(100).

% context (environmental features, task specific)
#external guess(R,D) : rock(R), range_guess(D).
#external dist(R,D) : rock(R), ranges_dist(D).
#external delta_x(R,D) : rock(R), ranges_dist(D).
#external delta_y(R,D) : rock(R), ranges_dist(D).
#external sampled(R) : rock(R).
#external num_sampled(R) : perc_rocks(R).



% MODIFY THESE RULES ACCORDING TO THE ACTION YOU ARE INTERESTED IN
target_sample(V1) :- V2 <= 1, dist(V1, V2), not sampled(V1). 
target_sample(V1) :- V2 <= 80, V2 >= 70, guess(V1, V2), not sampled(V1).


% MODIFY THE NAME OF THE ACTION AND THE ARITY, ACCORDING TO THE ACTION YOU ARE INTERESTED IN
#show target_sample/1.