#ifndef POCMAN_H
#define POCMAN_H

#include "simulator.h"
#include "coord.h"
#include "grid.h"
#include "beliefstate.h"

class POCMAN_STATE : public STATE
{
public:

    COORD PocmanPos;
    std::vector<COORD> GhostPos;
    std::vector<int> GhostDir;
    std::vector<bool> Food; // bit vector
    int NumFood;
    int PowerSteps;
};

class POCMAN : public SIMULATOR
{
public:

    virtual STATE* Copy(const STATE& state) const;
    virtual void Validate(const STATE& state) const;
    virtual STATE* CreateStartState() const;
    virtual void FreeState(STATE* state) const;
    virtual bool Step(STATE& state, int action, 
        observation_t& observation, double& reward) const;

    virtual bool LocalMove(STATE& state, const HISTORY& history,
        observation_t stepObs, const STATUS& status) const;
    void GenerateLegal(const STATE& state, const HISTORY& history,
        std::vector<int>& legal, const STATUS& status) const;
    void GeneratePreferred(const STATE& state, const HISTORY& history,
        std::vector<int>& legal, const STATUS& status) const;
    virtual void GenerateFromRulesHardcoded15(const STATE& state, const BELIEF_STATE &belief,
        std::vector<int>& legal, const STATUS& status) const;
    virtual void DisplayBeliefs(const BELIEF_STATE& beliefState, 
        std::ostream& ostr) const;
    virtual void DisplayState(const STATE& state, std::ostream& ostr) const;
    virtual void DisplayObservation(const STATE& state, observation_t observation, std::ostream& ostr) const;
    virtual void DisplayAction(int action, std::ostream& ostr) const;

    // xes logging
    virtual void log_problem_info() const;
    virtual void log_beliefs(const BELIEF_STATE& beliefState) const;
    // virtual void log_state(const STATE& state) const;
    virtual void log_action(int action) const;
    virtual void log_observation(const STATE& state, observation_t observation) const;
    virtual void log_reward(double reward) const;

protected:

    POCMAN(int xsize, int ysize, double food_prob, double chase_prob);

    enum { 
        E_PASSABLE,
        E_SEED,
        E_POWER
    };

    GRID<int> Maze;
    mutable GRID<int> counts_ghost;
    mutable GRID<int> counts_food;
    mutable GRID<int> counts_wall;
    int NumGhosts, PassageY, GhostRange, SmellRange, HearRange;
    COORD PocmanHome, GhostHome;
    double FoodProb, ChaseProb, DefensiveSlip;
    double RewardClearLevel, RewardDefault, RewardDie; 
    double RewardEatFood, RewardEatGhost, RewardHitWall; 
    int PowerNumSteps;

    int ghost_range = 2; //6 for 15 part
    int food_range = 2;
    float min_prob = 0.6; //0.5 for 15 part
    mutable double max_prob_food_w = 0;
    mutable double max_prob_food_n = 0;
    mutable double max_prob_food_s = 0;
    mutable double max_prob_food_e = 0;
    mutable double max_prob_ghost_w = 0;
    mutable double max_prob_ghost_e = 0;
    mutable double max_prob_ghost_n = 0;
    mutable double max_prob_ghost_s = 0;
    mutable double max_prob_ghost_short_w;
    mutable double max_prob_ghost_short_e;
    mutable double max_prob_ghost_short_n;
    mutable double max_prob_ghost_short_s;
    mutable std::vector<double> max_prob_ghost_short_list_w;
    mutable std::vector<double> max_prob_ghost_short_list_e;
    mutable std::vector<double> max_prob_ghost_short_list_n;
    mutable std::vector<double> max_prob_ghost_short_list_s;
    mutable std::vector<double> prob_food_list = {};
    mutable std::vector<double> prob_ghost_list = {};
    mutable std::vector<double> prob_ghost_short_list = {};

    static int arg_max(std::vector<double> &vec) {
        return std::distance(vec.begin(), max_element(vec.begin(), vec.end()));
    }

    std::vector<double> prob_pocman = {0.98, 0.98, 0.98, 0.98}; //probability of actions derived from axioms

private:

    void MoveGhost(POCMAN_STATE& pocstate, int g) const;
    void MoveGhostAggressive(POCMAN_STATE& pocstate, int g) const;
    void MoveGhostDefensive(POCMAN_STATE& pocstate, int g) const;
    void MoveGhostRandom(POCMAN_STATE& pocstate, int g) const;
    void NewLevel(POCMAN_STATE& pocstate) const;
    int SeeGhost(const POCMAN_STATE& pocstate, int action) const;    
    bool HearGhost(const POCMAN_STATE& pocstate) const;
    bool SmellFood(const POCMAN_STATE& pocstate) const;
    COORD NextPos(const COORD& from, int dir) const;
    bool Passable(const COORD& pos) const { return UTILS::CheckFlag(Maze(pos), E_PASSABLE); }
    int MakeObservations(const POCMAN_STATE& pocstate) const;

    mutable MEMORY_POOL<POCMAN_STATE> MemoryPool;
};

class MICRO_POCMAN : public POCMAN
{
public:

    MICRO_POCMAN(double food_prob, double chase_prob);
};

class MINI_POCMAN : public POCMAN
{
public:

    MINI_POCMAN(double food_prob, double chase_prob, int ghosts);
};

class FULL_POCMAN : public POCMAN
{
public:

    FULL_POCMAN(double food_prob, double chase_prob, int ghosts);
};

#endif // POCMAN_H
