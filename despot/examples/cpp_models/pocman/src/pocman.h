#ifndef POCMAN_H
#define POCMAN_H

#include <despot/interface/pomdp.h>
#include <despot/solver/pomcp.h>
#include <despot/util/coord.h>
#include <despot/util/grid.h>
#include <despot/core/particle_belief.h>
#include <random>

namespace despot {

/* ==============================================================================
 * AdventurerState class
 * ==============================================================================*/

class PocmanState: public State {
public:
	Coord pocman_pos;
	std::vector<Coord> ghost_pos;
	std::vector<int> ghost_dir;
	std::vector<bool> food; // bit vector
	int num_food;
	int power_steps;
};

/* ==============================================================================
 * PocmanBelief class
 * ==============================================================================*/

class Pocman;
class PocmanBelief: public ParticleBelief {
protected:
	const Pocman* pocman_;
public:
	static int num_particles;

	PocmanBelief(std::vector<State*> particles, const DSPOMDP* model, Belief* prior =
		NULL);
	void Update(ACT_TYPE action, OBS_TYPE obs);
};

/* ==============================================================================
 * Pocman class
 * ==============================================================================*/
/**
 * The implementation is adapted from that included in the POMCP software.
 */

class Pocman: public DSPOMDP {
public:
	virtual bool Step(State& state, double rand_num, ACT_TYPE action, double& reward,
		OBS_TYPE& observation) const;
	virtual void Validate(const State& state) const;
	int NumActions() const;
	virtual double ObsProb(OBS_TYPE obs, const State& state, ACT_TYPE action) const;

	virtual State* CreateStartState(std::string type = "DEFAULT") const;
	virtual Belief* InitialBelief(const State* start,
		std::string type = "PARTICLE") const;

	inline double GetMaxReward() const {
		return reward_clear_level_;
	}
	ScenarioUpperBound* CreateScenarioUpperBound(std::string name = "DEFAULT",
		std::string particle_bound_name = "DEFAULT") const;

	inline ValuedAction GetBestAction() const {
		return ValuedAction(0, reward_hit_wall_);
	}
	ParticleLowerBound* CreateParticleLowerBound(std::string name = "DEFAULT") const;
	ScenarioLowerBound* CreateScenarioLowerBound(std::string name = "DEFAULT",
		std::string particle_bound_name = "DEFAULT") const;

	POMCPPrior* CreatePOMCPPrior(std::string name = "DEFAULT") const;

	virtual void PrintState(const State& state, std::ostream& out = std::cout) const;
	virtual void PrintObs(const State& state, OBS_TYPE observation,
		std::ostream& out = std::cout) const;
	void PrintBelief(const Belief& belief, std::ostream& out = std::cout) const;
	virtual void PrintAction(ACT_TYPE action, std::ostream& out = std::cout) const;

	State* Allocate(int state_id, double weight) const;
	virtual State* Copy(const State* particle) const;
	virtual void Free(State* particle) const;
	int NumActiveParticles() const;

	bool LocalMove(State& state, const History& history, int obs) const;

public:
	Pocman(int xsize, int ysize, double food_prob, double ghost_prob, int num_ghosts);

	enum {
		E_PASSABLE, E_SEED, E_POWER
	};

	Grid<int> maze_;
	int num_ghosts_, passage_y_, ghost_range_, smell_range_, hear_range_;
	Coord pocman_home_, ghost_home_;
	double food_prob_, chase_prob_, defensive_slip_;
	double reward_clear_level_, reward_default_, reward_die_;
	double reward_eat_food_, reward_eat_ghost_, reward_hit_wall_;

	struct ActionList{
		std::vector<int> actions;
		std::vector<int> times;
	};

	mutable ActionList actionList;
	mutable int last_time;
    mutable std::mt19937 random_state;
	int power_num_steps_;
	Coord NextPos(const Coord& from, int dir) const;
	bool Passable(const Coord& pos) const {
		return CheckFlag(maze_(pos), E_PASSABLE);
	}

    static int arg_max(std::vector<double> &vec) {
        return std::distance(vec.begin(), max_element(vec.begin(), vec.end()));
    }

private:
	void MoveGhost(PocmanState& pocstate, int g, Random &random) const;
	void MoveGhostAggressive(PocmanState& pocstate, int g, Random &random) const;
	void MoveGhostDefensive(PocmanState& pocstate, int g, Random &random) const;
	void MoveGhostRandom(PocmanState& pocstate, int g, Random &random) const;
	void NewLevel(PocmanState& pocstate) const;
	int SeeGhost(const PocmanState& pocstate, ACT_TYPE action) const;
	bool HearGhost(const PocmanState& pocstate) const;
	bool SmellFood(const PocmanState& pocstate) const;
	int MakeObservations(const PocmanState& pocstate) const;

	mutable MemoryPool<PocmanState> memory_pool_;
};

class MicroPocman: public Pocman {
public:
	MicroPocman(double food_prob, double ghost_prob, int num_ghosts);
};

class MiniPocman: public Pocman {
public:
	MiniPocman(double food_prob, double ghost_prob, int num_ghosts);
};

class FullPocman: public Pocman {
public:
	FullPocman(double food_prob, double ghost_prob, int num_ghosts);
};

} // namespace despot

#endif
