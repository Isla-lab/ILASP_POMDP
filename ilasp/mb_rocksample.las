#modeb(1, guess(var(rocks), var(guess_value)), (positive)).
#modeb(1, dist(var(rocks), var(ranges_dist)), (positive)).
#modeb(1, delta_x(var(rocks), var(ranges_dist)), (positive)).
#modeb(1, delta_y(var(rocks), var(ranges_dist)), (positive)).
#modeb(1, num_sampled(var(perc_rocks)), (positive)).
#modeb(1,var(ranges_dist)>=const(dist_thr), (positive)).
#modeb(1,var(ranges_dist)<=const(dist_thr), (positive)).
#modeb(1,var(guess_value)>=const(guess_value), (positive)).
#modeb(1,var(guess_value)<=const(guess_value), (positive)).
#modeb(1,var(perc_rocks)>=const(perc_rocks), (positive)).
#modeb(1, sampled(var(rocks))).
#constant(dist_thr, 1..4).
#constant(dist_thr, -1).
#constant(perc_rocks, 25).
#constant(perc_rocks, 50).
#constant(perc_rocks, 75).
#constant(guess_value, 30).
#constant(guess_value, 40).
#constant(guess_value, 50).
#constant(guess_value, 60).
#constant(guess_value, 70).
#constant(guess_value, 80).
#constant(guess_value, 90).
#maxv(3).
#max_penalty(100).