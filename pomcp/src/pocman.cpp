#include "pocman.h"
#include "utils.h"

using namespace std;
using namespace UTILS;

POCMAN::POCMAN(int xsize, int ysize, double food_prob, double chase_prob)
:   Maze(xsize, ysize),
    PassageY(-1),
    SmellRange(1),
    HearRange(2),
    FoodProb(food_prob),
    ChaseProb(chase_prob),
    DefensiveSlip(0.25),
    PowerNumSteps(15),
    RewardClearLevel(+1000),
    RewardDefault(-1),
    RewardDie(-100),
    RewardEatFood(+10),
    RewardEatGhost(+25),
    RewardHitWall(-25)
{
    NumActions = 4;
    NumObservations = 1 << 10;
        // See ghost N
        // See ghost E
        // See ghost S
        // See ghost W
        // Can move N
        // Can move E
        // Can move S
        // Can move W
        // Smell food
        // Hear ghost
    RewardRange = 100;
    Discount = 0.95;

    SetProbActions(prob_pocman);
}

MICRO_POCMAN::MICRO_POCMAN(double food_prob, double chase_prob)
:   POCMAN(7, 7, food_prob, chase_prob)
{
    int maze[7][7] =
    {
        { 3, 3, 3, 3, 3, 3, 3 },
        { 3, 3, 0, 3, 0, 3, 3 },
        { 3, 0, 3, 3, 3, 0, 3 },
        { 3, 3, 3, 0, 3, 3, 3 },
        { 3, 0, 3, 3, 3, 0, 3 },
        { 3, 3, 0, 3, 0, 3, 3 },
        { 3, 3, 3, 3, 3, 3, 3 }
    };

    for (int x = 0; x < 7; x++)
        Maze.SetCol(x, maze[x]);
    NumGhosts = 1;
    GhostRange = 3;
    PocmanHome = COORD(3, 0);
    GhostHome = COORD(3, 4);
}

MINI_POCMAN::MINI_POCMAN(double food_prob, double chase_prob, int ghosts)
:   POCMAN(10, 10, food_prob, chase_prob)
{
    int maze[10][10] =
    {
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 },
        { 3, 0, 0, 3, 0, 0, 3, 0, 0, 3 },
        { 3, 0, 3, 3, 3, 3, 3, 3, 0, 3 },
        { 3, 3, 3, 0, 0, 0, 0, 3, 3, 3 },
        { 0, 0, 3, 0, 1, 1, 3, 3, 0, 0 },
        { 0, 0, 3, 0, 1, 1, 3, 3, 0, 0 },
        { 3, 3, 3, 0, 0, 0, 0, 3, 3, 3 },
        { 3, 0, 3, 3, 3, 3, 3, 3, 0, 3 },
        { 3, 0, 0, 3, 0, 0, 3, 0, 0, 3 },
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 }
    };

    for (int x = 0; x < 10; x++){
        Maze.SetCol(x, maze[x]);
    }
    counts_food = Maze;
    counts_food.SetAllValues(0);
    counts_ghost = Maze;
    counts_ghost.SetAllValues(0);
    counts_wall = Maze;
    counts_wall.SetAllValues(0);

    NumGhosts = ghosts;
    GhostRange = 4;
    PocmanHome = COORD(4, 2);
    GhostHome = COORD(4, 4);
    PassageY = 5;
}

FULL_POCMAN::FULL_POCMAN(double food_prob, double chase_prob, int ghosts)
:   POCMAN(17, 19, food_prob, chase_prob)
{
    // Transposed maze
    int maze[19][17] = {
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, },
        { 3, 0, 0, 3, 0, 0, 0, 3, 0, 3, 0, 0, 0, 3, 0, 0, 3, },
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, },
        { 3, 0, 0, 3, 0, 3, 0, 0, 0, 0, 0, 3, 0, 3, 0, 0, 3, },
        { 3, 3, 3, 3, 0, 3, 3, 3, 0, 3, 3, 3, 0, 3, 3, 3, 3, },
        { 0, 0, 0, 3, 0, 0, 0, 3, 0, 3, 0, 0, 0, 3, 0, 0, 0, },
        { 0, 0, 0, 3, 0, 1, 1, 1, 1, 1, 1, 1, 0, 3, 0, 0, 0, },
        { 0, 0, 0, 3, 0, 1, 0, 1, 1, 1, 0, 1, 0, 3, 0, 0, 0, },
        { 1, 1, 1, 3, 0, 1, 0, 1, 1, 1, 0, 1, 0, 3, 1, 1, 1, },
        { 0, 0, 0, 3, 0, 1, 0, 0, 0, 0, 0, 1, 0, 3, 0, 0, 0, },
        { 0, 0, 0, 3, 0, 1, 1, 1, 1, 1, 1, 1, 0, 3, 0, 0, 0, },
        { 0, 0, 0, 3, 0, 3, 0, 0, 0, 0, 0, 3, 0, 3, 0, 0, 0, },
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, },
        { 3, 0, 0, 3, 0, 0, 0, 3, 0, 3, 0, 0, 0, 3, 0, 0, 3, },
        { 3, 3, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 3, 3, },
        { 0, 3, 0, 3, 0, 3, 0, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, },
        { 3, 3, 3, 3, 0, 3, 3, 3, 0, 3, 3, 3, 0, 3, 3, 3, 3, },
        { 3, 0, 0, 0, 0, 0, 0, 3, 0, 3, 0, 0, 0, 0, 0, 0, 3, },
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3  }
    };

    // Transpose to rows
    for (int x = 0; x < 19; x++){
        Maze.SetRow(x, maze[18 - x]);
    }
    counts_food = Maze;
    counts_food.SetAllValues(0);
    counts_ghost = Maze;
    counts_ghost.SetAllValues(0);
    counts_wall = Maze;
    counts_wall.SetAllValues(0);

    NumGhosts = ghosts;
    GhostRange = 6;
    PocmanHome = COORD(8, 6);
    GhostHome = COORD(8, 10);
    PassageY = 10;
}

STATE* POCMAN::Copy(const STATE& state) const
{
    const POCMAN_STATE& pocstate = safe_cast<const POCMAN_STATE&>(state);
    POCMAN_STATE* newstate = MemoryPool.Allocate();
    *newstate = pocstate;
    return newstate;
}

void POCMAN::Validate(const STATE& state) const
{
    const POCMAN_STATE& pocstate = safe_cast<const POCMAN_STATE&>(state);
    assert(Maze.Inside(pocstate.PocmanPos));
    assert(Passable(pocstate.PocmanPos));
    for (int g = 0; g < NumGhosts; g++)
    {
        assert(Maze.Inside(pocstate.GhostPos[g]));
        assert(Passable(pocstate.GhostPos[g]));
    }
}

STATE* POCMAN::CreateStartState() const
{
    POCMAN_STATE* startState = MemoryPool.Allocate();
    startState->GhostPos.resize(NumGhosts);
    startState->GhostDir.resize(NumGhosts);
    startState->Food.resize(Maze.GetXSize() * Maze.GetYSize());
    NewLevel(*startState);
    return startState;
}

void POCMAN::FreeState(STATE* state) const
{
    POCMAN_STATE* pocstate = safe_cast<POCMAN_STATE*>(state);
    MemoryPool.Free(pocstate);
}

COORD POCMAN::NextPos(const COORD& from, int dir) const
{
    COORD nextPos;
    if (from.X == 0 && from.Y == PassageY && dir == COORD::E_WEST)
        nextPos = COORD(Maze.GetXSize() - 1, from.Y);
    else if (from.X == Maze.GetXSize() - 1 && from.Y == PassageY && dir == COORD::E_EAST)
        nextPos = COORD(0, from.Y);
    else
        nextPos = from + COORD::Compass[dir];

    if (Maze.Inside(nextPos) && Passable(nextPos))
        return nextPos;
    else
        return COORD::Null;
}

bool POCMAN::Step(STATE& state, int action,
    observation_t& observation, double& reward) const
{
    POCMAN_STATE& pocstate = safe_cast<POCMAN_STATE&>(state);
    reward = RewardDefault;
    observation = 0;

    // cout << COORD::CompassChar[action];
    COORD newpos = NextPos(pocstate.PocmanPos, action);
    if (newpos.Valid())
        pocstate.PocmanPos = newpos;
    else
        reward += RewardHitWall;

    if (pocstate.PowerSteps > 0)
        pocstate.PowerSteps--;

    int hitGhost = -1;
    for (int g = 0; g < NumGhosts; g++)
    {
        if (pocstate.GhostPos[g] == pocstate.PocmanPos)
            hitGhost = g;
        MoveGhost(pocstate, g);
        if (pocstate.GhostPos[g] == pocstate.PocmanPos)
            hitGhost = g;
    }

    if (hitGhost >= 0)
    {
        if (pocstate.PowerSteps > 0)
        {
            reward += RewardEatGhost;
            pocstate.GhostPos[hitGhost] = GhostHome;
            pocstate.GhostDir[hitGhost] = -1;
        }
        else
        {
            reward += RewardDie;
            return true;
        }
    }

    observation = MakeObservations(pocstate);

    int pocIndex = Maze.Index(pocstate.PocmanPos);
    if (pocstate.Food[pocIndex])
    {
        pocstate.Food[pocIndex] = false;
        pocstate.NumFood--;
        if (pocstate.NumFood == 0)
        {
            reward += RewardClearLevel;
            return true;
        }
        if (CheckFlag(Maze(pocstate.PocmanPos.X, pocstate.PocmanPos.Y), E_POWER))
            pocstate.PowerSteps = PowerNumSteps;
        reward += RewardEatFood;
    }

    return false;
}

int POCMAN::MakeObservations(const POCMAN_STATE& pocstate) const
{
    int observation = 0;
    for (int d = 0; d < 4; d++)
    {
        if (SeeGhost(pocstate, d) >= 0)
            SetFlag(observation, d);
        COORD wpos = NextPos(pocstate.PocmanPos, d);
        if (wpos.Valid() && Passable(wpos))
            SetFlag(observation, d + 4);
    }
    if (SmellFood(pocstate))
        SetFlag(observation, 8);
    if (HearGhost(pocstate))
        SetFlag(observation, 9);
    return observation;
}

bool POCMAN::LocalMove(STATE& state, const HISTORY& history,
    observation_t stepObs, const STATUS& status) const
{
    POCMAN_STATE& pocstate = safe_cast<POCMAN_STATE&>(state);
    
    int numGhosts = Random(1, 3); // Change 1 or 2 ghosts at a time
    for (int i = 0; i < numGhosts; ++i)
    {
        int g = Random(NumGhosts);
        pocstate.GhostPos[g] = COORD(
            Random(Maze.GetXSize()),
            Random(Maze.GetYSize()));
        if (!Passable(pocstate.GhostPos[g]) 
            || pocstate.GhostPos[g] == pocstate.PocmanPos)
            return false;
    }

    COORD smellPos;
    for (smellPos.X = -SmellRange; smellPos.X <= SmellRange; smellPos.X++)
    {
        for (smellPos.Y = -SmellRange; smellPos.Y <= SmellRange; smellPos.Y++)
        {
            COORD pos = pocstate.PocmanPos + smellPos;
            if (smellPos != COORD(0, 0) &&
                Maze.Inside(pos) && 
                CheckFlag(Maze(pos), E_SEED))
                pocstate.Food[Maze.Index(pos)] = Bernoulli(FoodProb * 0.5);
        }
    }

    // Just check the last time-step, don't check for full consistency
    if (history.Size() == 0)
        return true;
    int observation = MakeObservations(pocstate);
    return history.Back().Observation == observation;
}

void POCMAN::MoveGhost(POCMAN_STATE& pocstate, int g) const
{
    if (COORD::ManhattanDistance(
            pocstate.PocmanPos, pocstate.GhostPos[g]) < GhostRange)
    {
        if (pocstate.PowerSteps > 0)
            MoveGhostDefensive(pocstate, g);
        else
            MoveGhostAggressive(pocstate, g);
    }
    else
    {
        MoveGhostRandom(pocstate, g);
    }
}

void POCMAN::MoveGhostAggressive(POCMAN_STATE& pocstate, int g) const
{
    if (!Bernoulli(ChaseProb))
    {
        MoveGhostRandom(pocstate, g);
        return;
    }

    int bestDist = Maze.GetXSize() + Maze.GetYSize();
    COORD bestPos = pocstate.GhostPos[g];
    int bestDir = -1;
    for (int dir = 0; dir < 4; dir++)
    {
        int dist = COORD::DirectionalDistance(
            pocstate.PocmanPos, pocstate.GhostPos[g], dir);
        COORD newpos = NextPos(pocstate.GhostPos[g], dir);
        if (dist <= bestDist && newpos.Valid()
            && COORD::Opposite(dir) != pocstate.GhostDir[g])
        {
            bestDist = dist;
            bestPos = newpos;
        }
    }

    pocstate.GhostPos[g] = bestPos;
    pocstate.GhostDir[g] = bestDir;
}

void POCMAN::MoveGhostDefensive(POCMAN_STATE& pocstate, int g) const
{
    if (Bernoulli(DefensiveSlip) && pocstate.GhostDir[g] >= 0)
    {
        pocstate.GhostDir[g] = -1;
        return;
    }

    int bestDist = 0;
    COORD bestPos = pocstate.GhostPos[g];
    int bestDir = -1;
    for (int dir = 0; dir < 4; dir++)
    {
        int dist = COORD::DirectionalDistance(
            pocstate.PocmanPos, pocstate.GhostPos[g], dir);
        COORD newpos = NextPos(pocstate.GhostPos[g], dir);
        if (dist >= bestDist && newpos.Valid()
            && COORD::Opposite(dir) != pocstate.GhostDir[g])
        {
            bestDist = dist;
            bestPos = newpos;
        }
    }

    pocstate.GhostPos[g] = bestPos;
    pocstate.GhostDir[g] = bestDir;
}

void POCMAN::MoveGhostRandom(POCMAN_STATE& pocstate, int g) const
{
    // Never switch to opposite direction
    // Currently assumes there are no dead-ends.
    COORD newpos;
    int dir;
    int tempc = 0;
    do
    {
        dir = Random(4);
        newpos = NextPos(pocstate.GhostPos[g], dir);
    }
    while (COORD::Opposite(dir) == pocstate.GhostDir[g] || !newpos.Valid());
    pocstate.GhostPos[g] = newpos;
    pocstate.GhostDir[g] = dir;
}

void POCMAN::NewLevel(POCMAN_STATE& pocstate) const
{
    pocstate.PocmanPos = PocmanHome;
    for (int g = 0; g < NumGhosts; g++)
    {
        pocstate.GhostPos[g] = GhostHome;
        pocstate.GhostPos[g].X += g % 2;
        pocstate.GhostPos[g].Y += g / 2;
        pocstate.GhostDir[g] = -1;
    }

    pocstate.NumFood = 0;
    for (int x = 0; x < Maze.GetXSize(); x++)
    {
        for (int y = 0; y < Maze.GetYSize(); y++)
        {
            int pocIndex = Maze.Index(x, y);
            if (CheckFlag(Maze(x, y), E_SEED)
                && (CheckFlag(Maze(x, y), E_POWER)
                    || Bernoulli(FoodProb)))
            {
                pocstate.Food[pocIndex] = 1;
                pocstate.NumFood++;
            }
            else
            {
                pocstate.Food[pocIndex] = 0;
            }
        }
    }

    pocstate.PowerSteps = 0;
}

int POCMAN::SeeGhost(const POCMAN_STATE& pocstate, int action) const
{
    COORD eyepos = pocstate.PocmanPos + COORD::Compass[action];
    while (Maze.Inside(eyepos) && Passable(eyepos))
    {
        for (int g = 0; g < NumGhosts; g++)
            if (pocstate.GhostPos[g] == eyepos)
                return g;
        eyepos += COORD::Compass[action];
    }
    return -1;
}

bool POCMAN::HearGhost(const POCMAN_STATE& pocstate) const
{
    for (int g = 0; g < NumGhosts; g++)
        if (COORD::ManhattanDistance(
            pocstate.GhostPos[g], pocstate.PocmanPos) <= HearRange)
            return true;
    return false;
}

bool POCMAN::SmellFood(const POCMAN_STATE& pocstate) const
{
    COORD smellPos;
    for (smellPos.X = -SmellRange; smellPos.X <= SmellRange; smellPos.X++)
        for (smellPos.Y = -SmellRange; smellPos.Y <= SmellRange; smellPos.Y++)
            if (Maze.Inside(pocstate.PocmanPos + smellPos) 
                && pocstate.Food[Maze.Index(pocstate.PocmanPos + smellPos)])
                return true;
    return false;
}









//XES



void POCMAN::log_problem_info() const {
    XES::logger().add_attributes({
            {"problem", "pocman"},
            {"RewardRange", RewardRange},
            {"SizeX", Maze.GetXSize()},
            {"SizeY", Maze.GetYSize()},
            {"Ghosts", NumGhosts}
        });
    
    XES::logger().start_list("wall");
    for (int y = Maze.GetYSize() - 1; y >= 0; y--)
    {
        for (int x = 0; x < Maze.GetXSize(); x++)
        {
            COORD pos(x, y);
            if( Passable(pos))
                XES::logger().add_attribute({std::to_string(x) + "," + std::to_string(y), 1});
        }
    }
    XES::logger().end_list();
}

void POCMAN::log_beliefs(const BELIEF_STATE& beliefState) const {
    std::unordered_map<int, int> dist;
    int id;
    counts_ghost.SetAllValues(0);
    counts_food.SetAllValues(0);
    counts_wall.SetAllValues(0);
    for (int i = 0; i < beliefState.GetNumSamples(); i++)
    {
        const POCMAN_STATE* pocstate =
            safe_cast<const POCMAN_STATE*>(
                beliefState.GetSample(i));

        for (int g = 0; g < NumGhosts; g++)
            counts_ghost(pocstate->GhostPos[g])++;
        for (int y = Maze.GetYSize() - 1; y >= 0; y--){
            for (int x = 0; x < Maze.GetXSize(); x++){
                COORD pos(x, y);
                int index = Maze.Index(pos);
                if (pocstate->Food[index]) counts_food(x, y)++;
                if (Passable(pos)) counts_wall(x, y)++;
            }
        }
    }

    const STATE* state = beliefState.GetSample(0);
    const auto& ps = safe_cast<const POCMAN_STATE&>(*state);
    XES::logger().add_attribute({"coord x", ps.PocmanPos.X });
    XES::logger().add_attribute({"coord y", ps.PocmanPos.Y });
    XES::logger().start_list("belief");
    XES::logger().start_list("ghosts");
    for (int y = Maze.GetYSize() - 1; y >= 0; y--)
    {
        for (int x = 0; x < Maze.GetXSize(); x++)
        {
            if( (double) counts_ghost(x, y) > 0)
                XES::logger().add_attribute({std::to_string(x) + "," + std::to_string(y), (double) counts_ghost(x, y) / beliefState.GetNumSamples()});
        }
    }
    XES::logger().end_list();

    XES::logger().start_list("food");
    for (int y = Maze.GetYSize() - 1; y >= 0; y--)
    {
        for (int x = 0; x < Maze.GetXSize(); x++)
        {
            if( (double) counts_food(x, y) > 0)
                XES::logger().add_attribute({std::to_string(x) + "," + std::to_string(y), (double) counts_food(x, y) / beliefState.GetNumSamples()});
        }
    }
    XES::logger().end_list();

    XES::logger().end_list();
}

// void ROCKSAMPLE::log_state(const STATE& state) const {
//     const ROCKSAMPLE_STATE& rs = safe_cast<const ROCKSAMPLE_STATE&>(state);
//     XES::logger().start_list("state");
//     XES::logger().add_attribute({"coord x", rs.AgentPos.X });
//     XES::logger().add_attribute({"coord y", rs.AgentPos.Y });
//     XES::logger().start_list("rocks");
//     int i = 0;
//     for (const auto &r : rs.Rocks) {
//         XES::logger().start_list("rock");
//         XES::logger().add_attributes({
//                 {"number", i},
//                 {"valuable", r.Valuable},
//                 {"collected", r.Collected}
//                 });
//         XES::logger().end_list();
//         ++i;
//     }
//     XES::logger().end_list(); // end rocks
//     XES::logger().end_list(); // end state
// }

void POCMAN::log_action(int action) const {
    switch (action) {
        case COORD::E_EAST:
            XES::logger().add_attribute({"action", "east"});
            return;

        case COORD::E_NORTH:
            XES::logger().add_attribute({"action", "north"});
            return;

        case COORD::E_SOUTH:
            XES::logger().add_attribute({"action", "south"});
            return;

        case COORD::E_WEST:
            XES::logger().add_attribute({"action", "west"});
            return;

        default:
            return;
    }
}

void POCMAN::log_observation(const STATE& state, observation_t observation) const {
    XES::logger().add_attribute({"observation", observation});
    // switch (observation) {
    //     case E_NONE:
    //         XES::logger().add_attribute({"observation", "none"});
    //         return;
    //     case E_GOOD:
    //         XES::logger().add_attribute({"observation", "good"});
    //         return;
    //     case E_BAD:
    //         XES::logger().add_attribute({"observation", "bad"});
    //         return;
    // }
}

void POCMAN::log_reward(double reward) const {
    XES::logger().add_attribute({"reward", reward});
}

//XES END











void POCMAN::GenerateLegal(const STATE& state, const HISTORY& history, 
    vector<int>& legal, const STATUS& status) const
{
    const POCMAN_STATE& pocstate = safe_cast<const POCMAN_STATE&>(state);

    // Don't move into walls 
    for (int a = 0; a < 4; ++a)
    {
        COORD newpos = NextPos(pocstate.PocmanPos, a);
        if (newpos.Valid())
            legal.push_back(a);
    }
}

void POCMAN::GeneratePreferred(const STATE& state, const HISTORY& history, 
    vector<int>& actions, const STATUS& status) const
{
    const POCMAN_STATE& pocstate = safe_cast<const POCMAN_STATE&>(state);
    if (history.Size())
    {
        int action = history.Back().Action;
        int observation = history.Back().Observation;

        // If power pill and can see a ghost then chase it
        if (pocstate.PowerSteps > 0 && (observation & 15 != 0))
        {
            for (int a = 0; a < 4; ++a)
                if (CheckFlag(observation, a))
                    actions.push_back(a);
        }
        
        // Otherwise avoid observed ghosts and avoid changing directions
        else
        {
            for (int a = 0; a < 4; ++a)
            {
                COORD newpos = NextPos(pocstate.PocmanPos, a);        
                if (newpos.Valid() && !CheckFlag(observation, a)
                    && COORD::Opposite(a) != action)
                    actions.push_back(a);
            }
        }
    }
}











void POCMAN::GenerateFromRulesHardcoded15(const STATE& state, const BELIEF_STATE &belief,
    vector<int>& actions, const STATUS& status) const
{
    if (belief.GetNumSamples() == 0) return;

    // GRID<int> counts_ghost(Maze.GetXSize(), Maze.GetYSize());
    // GRID<int> counts_food(Maze.GetXSize(), Maze.GetYSize());
    // counts_ghost.SetAllValues(0);
    // counts_food.SetAllValues(0);
    const POCMAN_STATE& pocstate = safe_cast<const POCMAN_STATE&>(state);
    COORD pocpos = pocstate.PocmanPos;

    prob_ghost_short_list.clear();
    prob_ghost_list.clear();
    prob_ghost_short_list.clear();
    prob_food_list.clear();
    vector<int> tmp_actions = actions;
    tmp_actions.clear();


    // // chrono::steady_clock::time_point start = chrono::steady_clock::now();
    // for (int i = 0; i < belief.GetNumSamples(); i++)
    // {
    //     const POCMAN_STATE* ps =
    //         safe_cast<const POCMAN_STATE*>(
    //             belief.GetSample(i));

    //     for (int g = 0; g < NumGhosts; g++)
    //         counts_ghost(ps->GhostPos[g])++;
    //     for (int y = min(Maze.GetYSize()-1, pocpos.Y+food_range); y >= max(0, pocpos.Y-food_range); y--){
    //         COORD pos(pocpos.X, y);
    //         int index = Maze.Index(pos);
    //         if (ps->Food[index]) counts_food(pocpos.X, y)++;
    //     }
    //     for (int x = max(0, pocpos.X-food_range); x <min(Maze.GetXSize(), pocpos.X+food_range); x++){
    //         COORD pos(x, pocpos.Y);
    //         int index = Maze.Index(pos);
    //         if (ps->Food[index]) counts_food(x, pocpos.Y)++;
    //     }

    // }
    // chrono::steady_clock::time_point end = chrono::steady_clock::now();
    // cout << "DURATION BELIEF " << chrono::duration_cast<chrono::microseconds> (end-start).count() << endl;

    // double prob_ghost_n = 0;
    // double prob_ghost_s = 0;
    // double prob_ghost_w = 0;
    // double prob_ghost_e = 0;
    // double prob_ghost_short_n = 0;
    // double prob_ghost_short_s = 0;
    // double prob_ghost_short_w = 0;
    // double prob_ghost_short_e = 0;
    // double prob_food_n = 0;
    // double prob_food_e = 0;
    // double prob_food_w = 0;
    // double prob_food_s = 0;
    max_prob_food_w = 0;
    max_prob_food_n = 0;
    max_prob_food_s = 0;
    max_prob_food_e = 0;
    max_prob_ghost_w = 0;
    max_prob_ghost_e = 0;
    max_prob_ghost_n = 0;
    max_prob_ghost_s = 0;
    max_prob_ghost_short_w = 0;
    max_prob_ghost_short_e = 0;
    max_prob_ghost_short_n = 0;
    max_prob_ghost_short_s = 0;
    // cout << pocpos.X << ", " << pocpos.Y << endl;
    for(int x = 1; x <= ghost_range; x++){
        if (pocpos.X >= x){
            // prob_ghost_w = (double) counts_ghost(pocpos.X - x, pocpos.Y) / (belief.GetNumSamples()); 
            max_prob_ghost_w = max((double) counts_ghost(pocpos.X - x, pocpos.Y) / (belief.GetNumSamples()), max_prob_ghost_w);// (prob_ghost_w>max_prob_ghost_w) ? prob_ghost_w : max_prob_ghost_w;
            if (x <= food_range){
                // prob_ghost_short_w = (double) counts_ghost(pocpos.X - x, pocpos.Y) / (belief.GetNumSamples());
                max_prob_ghost_short_w = max((double) counts_ghost(pocpos.X - x, pocpos.Y) / (belief.GetNumSamples()), max_prob_ghost_short_w);// (prob_ghost_short_w>max_prob_ghost_short_w) ? prob_ghost_short_w : max_prob_ghost_short_w;
                // prob_food_w = (double) counts_food(pocpos.X - x, pocpos.Y) / (belief.GetNumSamples());
                max_prob_food_w = max((double) counts_food(pocpos.X - x, pocpos.Y) / (belief.GetNumSamples()), max_prob_food_w);// (prob_food_w>max_prob_food_w) ? prob_food_w : max_prob_food_w;}
            }
        }
        if (pocpos.X + x < Maze.GetXSize()){
            // prob_ghost_e = (double) counts_ghost(pocpos.X + x, pocpos.Y) / (belief.GetNumSamples()); 
            max_prob_ghost_e = max((double) counts_ghost(pocpos.X + x, pocpos.Y) / (belief.GetNumSamples()), max_prob_ghost_e);// (prob_ghost_e>max_prob_ghost_e) ? prob_ghost_e : max_prob_ghost_e;
            if (x <= food_range){
                // prob_ghost_short_e = (double) counts_ghost(pocpos.X + x, pocpos.Y) / (belief.GetNumSamples());
                max_prob_ghost_short_e = max((double) counts_ghost(pocpos.X + x, pocpos.Y) / (belief.GetNumSamples()), max_prob_ghost_short_e);// (prob_ghost_short_e>max_prob_ghost_short_e) ? prob_ghost_short_e : max_prob_ghost_short_e;
                // prob_food_e = (double) counts_food(pocpos.X + x, pocpos.Y) / (belief.GetNumSamples());
                max_prob_food_e = max((double) counts_food(pocpos.X + x, pocpos.Y) / (belief.GetNumSamples()), max_prob_food_e);// (prob_food_e>max_prob_food_e) ? prob_food_e : max_prob_food_e;}
            }

        }
        if (pocpos.Y >= x){
            // prob_ghost_s = (double) counts_ghost(pocpos.X, pocpos.Y - x) / (belief.GetNumSamples()); 
            max_prob_ghost_s = max((double) counts_ghost(pocpos.X, pocpos.Y - x) / (belief.GetNumSamples()), max_prob_ghost_s);// (prob_ghost_s>max_prob_ghost_s) ? prob_ghost_s : max_prob_ghost_s;
            if (x <= food_range){
                // prob_ghost_short_s = (double) counts_ghost(pocpos.X, pocpos.Y - x) / (belief.GetNumSamples());
                max_prob_ghost_short_s = max((double) counts_ghost(pocpos.X, pocpos.Y - x) / (belief.GetNumSamples()), max_prob_ghost_short_s);// (prob_ghost_short_s>max_prob_ghost_short_s) ? prob_ghost_short_s : max_prob_ghost_short_s;
                // prob_food_s = (double) counts_food(pocpos.X, pocpos.Y - x) / (belief.GetNumSamples());
                max_prob_food_s = max((double) counts_food(pocpos.X, pocpos.Y - x) / (belief.GetNumSamples()), max_prob_food_s);// (prob_food_s>max_prob_food_s) ? prob_food_s : max_prob_food_s;}
            }

        }
        if (pocpos.Y + x < Maze.GetYSize()){
            // prob_ghost_n = (double) counts_ghost(pocpos.X, pocpos.Y + x) / (belief.GetNumSamples()); 
            max_prob_ghost_n = max((double) counts_ghost(pocpos.X, pocpos.Y + x) / (belief.GetNumSamples()), max_prob_ghost_n);// (prob_ghost_n>max_prob_ghost_n) ? prob_ghost_n : max_prob_ghost_n;
            if (x <= food_range){
                // prob_ghost_short_n = (double) counts_ghost(pocpos.X, pocpos.Y + x) / (belief.GetNumSamples());
                max_prob_ghost_short_n = max((double) counts_ghost(pocpos.X, pocpos.Y + x) / (belief.GetNumSamples()), max_prob_ghost_short_n);// (prob_ghost_short_n>max_prob_ghost_short_n) ? prob_ghost_short_n : max_prob_ghost_short_n;
                // prob_food_n = (double) counts_food(pocpos.X, pocpos.Y + x) / (belief.GetNumSamples());
                max_prob_food_n = max((double) counts_food(pocpos.X, pocpos.Y + x) / (belief.GetNumSamples()), max_prob_food_n);// (prob_food_n>max_prob_food_n) ? prob_food_n : max_prob_food_n;}
            }

        }
    }
    // cout << "=================" << endl;
    COORD dirpos = pocpos+COORD::Compass[COORD::E_WEST];
    if (max_prob_ghost_w <= min_prob && Maze.Inside(dirpos) && Passable(dirpos)){
        tmp_actions.push_back(COORD::E_WEST);
        prob_food_list.push_back(max_prob_food_w);
        prob_ghost_short_list.push_back(max_prob_ghost_short_w);
        prob_ghost_list.push_back(max_prob_ghost_w);    
    }
    dirpos = pocpos+COORD::Compass[COORD::E_EAST];
    if (max_prob_ghost_e <= min_prob && Maze.Inside(dirpos) && Passable(dirpos)){
        tmp_actions.push_back(COORD::E_EAST);
        prob_food_list.push_back(max_prob_food_e);
        prob_ghost_short_list.push_back(max_prob_ghost_short_e);
        prob_ghost_list.push_back(max_prob_ghost_e);    
    }
    dirpos = pocpos+COORD::Compass[COORD::E_SOUTH];
    if (max_prob_ghost_s <= min_prob && Maze.Inside(dirpos) && Passable(dirpos)){
        tmp_actions.push_back(COORD::E_SOUTH);
        prob_food_list.push_back(max_prob_food_s);
        prob_ghost_short_list.push_back(max_prob_ghost_short_s);
        prob_ghost_list.push_back(max_prob_ghost_s);    
    }
    dirpos = pocpos+COORD::Compass[COORD::E_NORTH];
    if (max_prob_ghost_n <= min_prob && Maze.Inside(dirpos) && Passable(dirpos)){
        tmp_actions.push_back(COORD::E_NORTH);
        prob_food_list.push_back(max_prob_food_n);
        prob_ghost_short_list.push_back(max_prob_ghost_short_n);
        prob_ghost_list.push_back(max_prob_ghost_n);    
    }

    // cout << "TMP ACTIONS ARE: ";
    // for (auto a : tmp_actions) cout << a << " ";
    // cout << endl;

    if (tmp_actions.size() > 1){
        // cout << "PROB FOOD LIST ARE ";
        // for (auto el : prob_food_list) cout << el << " ";
        // cout << endl;
        // cout << "PROB GHOST SHORT LIST ARE ";
        // for (auto el : prob_ghost_short_list) cout << el << " ";
        // cout << endl;  
        int max_food = arg_max(prob_food_list);
        double max_food_value = prob_food_list[max_food];
        int tmp = max_food;
        vector<int> max_food_tmp = {max_food};
        int i = 0;
        while (max_food == tmp){
            // cout << "IN WHILE"  << endl;
            prob_food_list.erase(prob_food_list.begin() + max_food);
            if (prob_food_list.size() == 0) break;
            tmp = arg_max(prob_food_list);
            if(prob_food_list[tmp] == max_food_value) {
                if (tmp < max_food) max_food_tmp.push_back(tmp);
                else max_food_tmp.push_back(tmp + 1+i);
            }
            else break;
            i = i+1;
            max_food = tmp;
        }

        if (max_food_tmp.size() > 1){
            for (auto idx : max_food_tmp){
                if (prob_ghost_short_list[idx] == 0){
                    actions.push_back(tmp_actions[idx]);
                }
            }
        }
        else actions.push_back(tmp_actions[max_food]);
    }
    else actions = tmp_actions;

    // cout << "FINAL ACTIONS ARE: ";
    // for (auto a : actions) cout << a << " ";
    // cout << endl;
    // chrono::steady_clock::time_point end = chrono::steady_clock::now();
    // cout << chrono::duration_cast<chrono::microseconds> (end-start).count() << endl;
}







void POCMAN::DisplayBeliefs(const BELIEF_STATE& beliefState,
    ostream& ostr) const
{
    GRID<int> counts(Maze.GetXSize(), Maze.GetYSize());
    counts.SetAllValues(0);
    for (int i = 0; i < beliefState.GetNumSamples(); i++)
    {
        const POCMAN_STATE* pocstate =
            safe_cast<const POCMAN_STATE*>(
                beliefState.GetSample(i));

        for (int g = 0; g < NumGhosts; g++)
            counts(pocstate->GhostPos[g])++;
    }

    for (int y = Maze.GetYSize() - 1; y >= 0; y--)
    {
        for (int x = 0; x < Maze.GetXSize(); x++)
        {
            ostr.width(6);
            ostr.precision(2);
            ostr << fixed << (double) counts(x, y) / beliefState.GetNumSamples();
        }
        ostr << endl;
    }
}

void POCMAN::DisplayState(const STATE& state, ostream& ostr) const
{
    const POCMAN_STATE& pocstate = safe_cast<const POCMAN_STATE&>(state);
    ostr << endl;
    for (int x = 0; x < Maze.GetXSize() + 2; x++)
        ostr << "X ";
    ostr << endl;
    for (int y = Maze.GetYSize() - 1; y >= 0; y--)
    {
        if (y == PassageY)
            ostr << "< ";
        else
            ostr << "X ";
        for (int x = 0; x < Maze.GetXSize(); x++)
        {
            COORD pos(x, y);
            int index = Maze.Index(pos);
            char c = ' ';
            if (!Passable(pos))
                c = 'X';
            if (pocstate.Food[index])
                c = CheckFlag(Maze(x, y), E_POWER) ? '+' : '.';
            for (int g = 0; g < NumGhosts; g++)
                if (pos == pocstate.GhostPos[g])
                    c = (pos == pocstate.PocmanPos ? '@' :
                        (pocstate.PowerSteps == 0 ? 'A' + g : 'a' + g));
            if (pos == pocstate.PocmanPos)
                c = pocstate.PowerSteps > 0 ? '!' : '*';
            ostr << c << ' ';
        }
        if (y == PassageY)
            ostr << ">" << endl;
        else
            ostr << "X" << endl;
    }
    for (int x = 0; x < Maze.GetXSize() + 2; x++)
        ostr << "X ";
    ostr << endl;
}

void POCMAN::DisplayObservation(const STATE& state, observation_t observation, ostream& ostr) const
{
    const POCMAN_STATE& pocstate = safe_cast<const POCMAN_STATE&>(state);
    GRID<char> obs(Maze.GetXSize(), Maze.GetYSize());
    obs.SetAllValues(' ');

    // Pocman
    obs(pocstate.PocmanPos) = pocstate.PowerSteps > 0 ? '!' : '*';

    for (int d = 0; d < 4; d++)
    {
        // See ghost
        if (CheckFlag(observation, d))
        {
            COORD eyepos = pocstate.PocmanPos + COORD::Compass[d];
            while (Maze.Inside(eyepos) && Passable(eyepos))
            {
                obs(eyepos) = (pocstate.PowerSteps == 0 ? 'A': 'a');
                eyepos += COORD::Compass[d];
            }
        }

        // Feel wall
        if (!CheckFlag(observation, d + 4) 
            && Maze.Inside(pocstate.PocmanPos + COORD::Compass[d]))
            obs(pocstate.PocmanPos + COORD::Compass[d]) = 'X';
    }

    // Hear ghost
    if (CheckFlag(observation, 9))
    {
        COORD hearPos;
        for (hearPos.X = -HearRange; hearPos.X <= HearRange; hearPos.X++)
            for (hearPos.Y = -HearRange; hearPos.Y <= HearRange; hearPos.Y++)
                if (COORD::ManhattanDistance(hearPos, pocstate.PocmanPos) <= HearRange
                    && Maze.Inside(pocstate.PocmanPos + hearPos) 
                    && obs(pocstate.PocmanPos + hearPos) == ' ')
                    obs(pocstate.PocmanPos + hearPos) = (pocstate.PowerSteps == 0 ? 'A': 'a');
    }

    // Smell food
    if (CheckFlag(observation, 8))
    {
        COORD smellPos;
        for (smellPos.X = -SmellRange; smellPos.X <= SmellRange; smellPos.X++)
            for (smellPos.Y = -SmellRange; smellPos.Y <= SmellRange; smellPos.Y++)
                if (Maze.Inside(pocstate.PocmanPos + smellPos) 
                    && obs(pocstate.PocmanPos + smellPos) == ' ')
                    obs(pocstate.PocmanPos + smellPos) = '.';
    }

    ostr << endl;
    for (int x = 0; x < Maze.GetXSize() + 2; x++)
        ostr << "# ";
    ostr << endl;
    for (int y = Maze.GetYSize() - 1; y >= 0; y--)
    {
        ostr << "# ";
        for (int x = 0; x < Maze.GetXSize(); x++)
            ostr << obs(x, y) << ' ';
        ostr << "#" << endl;
    }
    for (int x = 0; x < Maze.GetXSize() + 2; x++)
        ostr << "# ";
    ostr << endl;
}

void POCMAN::DisplayAction(int action, ostream& ostr) const
{
    ostr << "Pocman moves " << COORD::CompassString[action] << endl;
}
