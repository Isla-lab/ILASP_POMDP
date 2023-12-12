#modeh(move(var(dir))).
#modeb(1, food(var(dir), var(prob), const(dist)), (positive)).
#modeb(1, ghost(var(dir), var(prob), const(dist)), (positive)).
#modeb(2, var(prob)>=const(prob), (positive)).
#modeb(2, var(prob)<=const(prob), (positive)).
#modeb(1, feel_wall(var(dir))).
#constant(dist, 2).
#constant(dist, 4).
#constant(dist, 6).
#constant(prob, 40).
#constant(prob, 50).
#constant(prob, 60).
#constant(prob, 70).
#constant(prob, 80).
#constant(prob, 90).

#maxv(3).