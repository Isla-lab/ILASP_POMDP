#modeo(1, target_sample(var(rocks)), (positive)). %REPLACE target_sample WITH THE ACTION OF INTEREST
#modeo(1, guess(var(rocks), var(guess_value)), (positive)).
#modeo(1, dist(var(rocks), var(ranges_dist)), (positive)).
#weight(1).
#weight(ranges_dist).
#weight(guess_value).
#maxp(2).
#maxv(2).
#max_penalty(100).