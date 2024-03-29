#ifndef BASEROCKSAMPLE_H
#define BASEROCKSAMPLE_H

#include <despot/interface/pomdp.h>
#include <despot/solver/pomcp.h>
#include <despot/core/mdp.h>
#include <math.h>
#include "numeric"
#include <random>
#include "algorithm"
#include <despot/util/coord.h>
#include <despot/util/grid.h>
#include <despot/core/builtin_lower_bounds.h>
#include <despot/core/builtin_policy.h>
#include <despot/core/builtin_upper_bounds.h>
#include <despot/core/particle_belief.h>

namespace despot {

/* =============================================================================
 * RockSampleState class
 * =============================================================================*/

class RockSampleState: public State {
public:
	RockSampleState();
	RockSampleState(int _state_id);

	std::string text() const;
};

/* =============================================================================
 * BaseRockSample class
 * =============================================================================*/

class BaseRockSample: public MDP,
	public BeliefMDP,
	public StateIndexer,
	public StatePolicy {
	friend class RockSampleENTScenarioLowerBound;
	friend class RockSampleMMAPStateScenarioLowerBound;
	friend class RockSampleEastScenarioLowerBound;
	friend class RockSampleRulesDefaultPolicy;
	friend class RockSampleRules20DefaultPolicy;
	friend class RockSampleRules40DefaultPolicy;
	friend class RockSampleRules60DefaultPolicy;
	friend class RockSampleRulesDefaultPolicy11;
	friend class RockSampleParticleUpperBound1;
	friend class RockSampleParticleUpperBound2;
	friend class RockSampleMDPParticleUpperBound;
	friend class RockSampleApproxParticleUpperBound;
	friend class RockSampleEastBeliefPolicy;
	friend class RockSampleMDPBeliefUpperBound;
	friend class RockSamplePOMCPPrior;

protected:
	Grid<int> grid_;
	std::vector<Coord> rock_pos_;
	int size_, num_rocks_;
	Coord start_pos_;
	double half_efficiency_distance_;
	mutable int last_time;
    std::vector<double> prob_actions = {0.649, 0.570, 0.649, 0.733, 0.2, 0.849, 0.657}; //Probabilities of actions depending of counterexamples of axioms
    // std::vector<double> prob_actions = {680/5832.0 * 3538/9902.0, 1083/5832.0 * 3538/9902.0, 911/5832.0 * 3538/9902.0, 148/5832.0 * 3538/9902.0, 2307/9902.0, 677/9902.0 * 3538/9902.0, 1087/9902.0}; // 11 PART!!!
	// mutable std::mt19937 random_state;
	// std::mt19937 gen(std::random_device{}());

    struct Target{
        int val;
        int dist;
        int rock;
        int X;
        int Y;
    };

	struct ActionList{
		std::vector<int> actions;
		std::vector<int> times;
	};

	mutable ActionList actionList;
    mutable std::uniform_real_distribution<> unif_dist;
    mutable std::mt19937 random_state;
	RockSampleState* rock_state_;
	mutable MemoryPool<RockSampleState> memory_pool_;

	std::vector<RockSampleState*> states_;
protected:
	void InitGeneral();
	void Init_4_4();
	void Init_5_5();
	void Init_5_7();
	void Init_7_8();
	void Init_11_11();
	void Init_12_4();
	void InitStates();
	bool GetObservation(double rand_num, const RockSampleState& rockstate,
		int rock) const;

	std::vector<std::vector<std::vector<State> > > transition_probabilities_;
	std::vector<std::vector<double> > alpha_vectors_; // For blind policy
	mutable std::vector<ValuedAction> mdp_policy_;

public:
	enum { // FRAGILE: Don't change!
		E_BAD = 0,
		E_GOOD = 1,
		E_NONE = 2
	};

	enum { // FRAGILE: Don't change!
		E_EXIT = 4,
		E_SAMPLE = 5
	};

public:
	BaseRockSample(std::string map);
	BaseRockSample(int size, int rocks);

	virtual bool Step(State& state, double rand_num, ACT_TYPE action,
		double& reward, OBS_TYPE& obs) const = 0;
	virtual int NumActions() const = 0;
	virtual double ObsProb(OBS_TYPE obs, const State& state, ACT_TYPE action) const = 0;

	const std::vector<State>& TransitionProbability(int s, ACT_TYPE a) const;
	int NextState(int s, ACT_TYPE a) const;
	double Reward(int s, ACT_TYPE a) const;

	State* CreateStartState(std::string type = "DEFAULT") const;
	std::vector<State*> InitialParticleSet() const;
	std::vector<State*> NoisyInitialParticleSet() const;
	Belief* InitialBelief(const State* start, std::string type = "PARTICLE") const;

	inline double GetMaxReward() const {
		return 10;
	}
	ScenarioUpperBound* CreateScenarioUpperBound(std::string name = "DEFAULT",
		std::string particle_bound_name = "DEFAULT") const;
	BeliefUpperBound* CreateBeliefUpperBound(std::string name = "DEFAULT") const;

	inline ValuedAction GetBestAction() const {
		return ValuedAction(E_EXIT, 0);
	}
	ScenarioLowerBound* CreateScenarioLowerBound(std::string name = "DEFAULT",
		std::string particle_bound_name = "DEFAULT") const;
	BeliefLowerBound* CreateBeliefLowerBound(std::string name = "DEFAULT") const;

	POMCPPrior* CreatePOMCPPrior(std::string name = "DEFAULT") const;

	void PrintState(const State& state, std::ostream& out = std::cout) const;
	void PrintBelief(const Belief& belief, std::ostream& out = std::cout) const;
	virtual void PrintObs(const State& state, OBS_TYPE observation, std::ostream& out = std::cout) const = 0;
	void PrintAction(ACT_TYPE action, std::ostream& out = std::cout) const;

	State* Allocate(int state_id, double weight) const;
	State* Copy(const State* particle) const;
	void Free(State* particle) const;
	int NumActiveParticles() const;

	Belief* Tau(const Belief* belief, ACT_TYPE action, OBS_TYPE obs) const;
	void Observe(const Belief* belief, ACT_TYPE action, std::map<OBS_TYPE, double>& obss) const;
	double StepReward(const Belief* belief, ACT_TYPE action) const;

	int NumStates() const;

    static bool compare_actions(ValuedAction a, ValuedAction b){
		return a.value >= b.value;
	}

    static bool compare_targets(Target a, Target b){
        if (a.dist != b.dist) return a.dist < b.dist;
        return a.val > b.val;
	}

	// void NormProbActions(){
	// 	double sum = std::accumulate(prob_actions.begin(), prob_actions.end(), 0.0);
	// 	for_each(prob_actions.begin(), prob_actions.end(), [sum](double &c){ c /= sum; });
	// }

	const State* GetState(int index) const;
	int GetIndex(const State* state) const;

	inline int GetAction(const State& tagstate) const {
		return 0;
	}

	int GetRobPosIndex(const State* state) const;
	Coord GetRobPos(const State* state) const;
	bool GetRock(const State* state, int rock) const;
	void SampleRock(State* state, int rock) const;
	int GetX(const State* state) const;
	void IncX(State* state) const;
	void DecX(State* state) const;
	int GetY(const State* state) const;
	void IncY(State* state) const;
	void DecY(State* state) const;

protected:
	void InitializeTransitions();
	Coord IndexToCoord(int pos) const;
	int CoordToIndex(Coord c) const;
	std::vector<ValuedAction>& ComputeOptimalSamplingPolicy() const;
	RockSampleState* MajorityRockSampleState(const std::vector<State*>& particles) const;
};

} // namespace despot

#endif
