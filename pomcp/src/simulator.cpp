#include "simulator.h"

using namespace std;
std::mt19937 gen(std::random_device{}());
using namespace UTILS;

SIMULATOR::KNOWLEDGE::KNOWLEDGE()
:   TreeLevel(LEGAL),
    RolloutLevel(LEGAL),
    SmartTreeCount(10),
    dynamic_tree_count(false),
    SmartTreeValue(1.0)
{
}

SIMULATOR::STATUS::STATUS()
:   Phase(TREE),
    Particles(CONSISTENT)
{
}

SIMULATOR::SIMULATOR() 
:   Discount(1.0),
    NumActions(0),
    NumObservations(0),
    RewardRange(1.0),
    clingo_logger([](Clingo::WarningCode, char const *message) { cout << message << endl;}),
    clingo_control({{}, clingo_logger, 20})
{
    // cout << NormProbActions << endl;
    // if (NormProbActions){
    //     double sum = accumulate(prob_actions.begin(), prob_actions.end(), 0.0);
    //     for_each(prob_actions.begin(), prob_actions.end(), [sum](double &c){ c /= sum; });
    // }
}

SIMULATOR::SIMULATOR(int numActions, observation_t numObservations, double discount)
:   NumActions(numActions),
    NumObservations(numObservations),
    Discount(discount),
    clingo_logger([](Clingo::WarningCode, char const *message) { cout << message << endl;}),
    clingo_control({{}, clingo_logger, 20})
{ 
    assert(discount > 0 && discount <= 1);
    // cout << NormProbActions << endl;
    // if (NormProbActions){
    //     double sum = accumulate(prob_actions.begin(), prob_actions.end(), 0.0);
    //     for_each(prob_actions.begin(), prob_actions.end(), [sum](double &c){ c /= sum; });
    // }
}

SIMULATOR::~SIMULATOR() 
{ 
}

void SIMULATOR::Validate(const STATE& state) const 
{ 
}

bool SIMULATOR::LocalMove(STATE& state, const HISTORY& history,
    observation_t stepObs, const STATUS& status) const
{
    return true;
}

void SIMULATOR::GenerateLegal(const STATE& state, const HISTORY& history, 
    std::vector<int>& actions, const STATUS& status) const
{
    for (int a = 0; a < NumActions; ++a)
        actions.push_back(a);
}

void SIMULATOR::GeneratePreferred(const STATE& state, const HISTORY& history, 
    std::vector<int>& actions, const STATUS& status) const
{
}

void SIMULATOR::GenerateFromRulesHardcoded11(const STATE& state, const BELIEF_STATE &belief, 
        std::vector<int>& actions, const STATUS& status) const
{
}

void SIMULATOR::GenerateFromRulesHardcoded15(const STATE& state, const BELIEF_STATE &belief, 
        std::vector<int>& actions, const STATUS& status) const
{
}

void SIMULATOR::GenerateFromRulesHardcoded15_60(const STATE& state, const BELIEF_STATE &belief, 
        std::vector<int>& actions, const STATUS& status) const
{
}

void SIMULATOR::GenerateFromRulesHardcoded15_40(const STATE& state, const BELIEF_STATE &belief, 
        std::vector<int>& actions, const STATUS& status) const
{
}

void SIMULATOR::GenerateFromRulesHardcoded15_20(const STATE& state, const BELIEF_STATE &belief, 
        std::vector<int>& actions, const STATUS& status) const
{
}

int SIMULATOR::SelectRandom(const STATE &state, const HISTORY &history,
                            const BELIEF_STATE &belief,
                            const STATUS &status) const {
    static vector<int> actions;

    if (Knowledge.RolloutLevel >= KNOWLEDGE::RULES_WEIGHT)
    {
        // cout << "SELECT RANDOM WITH WEIGHT" << endl;
        chrono::steady_clock::time_point start = chrono::steady_clock::now();
        
        actions.clear();
        static vector<int> actions_r;
        actions_r.clear();
        GenerateLegal(state, history, actions, status);
        if (Knowledge.val == 20) GenerateFromRulesHardcoded15_20(state, belief, actions_r, status);
        else if (Knowledge.val == 40) GenerateFromRulesHardcoded15_40(state, belief, actions_r, status);
        else if (Knowledge.val == 60) GenerateFromRulesHardcoded15_60(state, belief, actions_r, status);
        else GenerateFromRulesHardcoded15(state, belief, actions_r, status);

        if (!Knowledge.soft) actions = actions_r;
        std::vector<double> prob_actual; //to match with actual actions generated
        prob_actual.clear();
        double max_prob = *max_element(prob_actions.begin(), prob_actions.end());
        for (auto a:actions){
            auto found = find(actions_r.begin(), actions_r.end(), a);
            if (found != actions_r.end()){
                if (a<6){
                    prob_actual.push_back(prob_actions[a]);
                }
                else{
                    prob_actual.push_back(prob_actions[6]);
                }
            }
            else {prob_actual.push_back(1-max_prob); /* cout << "ACTION " << a << " NOT SUGGESTED BY RULES!" << endl; */}
        }
        double sum = accumulate(prob_actual.begin(), prob_actual.end(), 0.0);
        for_each(prob_actual.begin(), prob_actual.end(), [sum](double &c){ c /= sum; });


        if (!actions.empty()){
            std::discrete_distribution<std::size_t> d{prob_actual.begin(), prob_actual.end()};
            int index = d(gen);
            return actions[index];
        }
    }

    if (Knowledge.RolloutLevel >= KNOWLEDGE::RULES)
    {
        // cout << "SELECT RANDOM ONLY" << endl;
        chrono::steady_clock::time_point start = chrono::steady_clock::now();

        actions.clear();
        // auto actions_r = actions;
        static vector<int> actions_r;
        actions_r.clear();
        GenerateLegal(state, history, actions, status);
        if (Knowledge.val == 20) GenerateFromRulesHardcoded15_20(state, belief, actions_r, status);
        else if (Knowledge.val == 40) GenerateFromRulesHardcoded15_40(state, belief, actions_r, status);
        else if (Knowledge.val == 60) GenerateFromRulesHardcoded15_60(state, belief, actions_r, status);
        else GenerateFromRulesHardcoded15(state, belief, actions_r, status);

        prob_actual.clear();
        double max_prob = *max_element(prob_actions.begin(), prob_actions.end());
        for (auto a:actions){
            auto found = find(actions_r.begin(), actions_r.end(), a);
            if (found != actions_r.end()){
                prob_actual.push_back(max_prob);
            }
            else prob_actual.push_back(1-max_prob);
        }
        double sum = accumulate(prob_actual.begin(), prob_actual.end(), 0.0);
        for_each(prob_actual.begin(), prob_actual.end(), [sum](double &c){ c /= sum; });
        if (!actions.empty())
        {
            std::discrete_distribution<std::size_t> d{prob_actual.begin(), prob_actual.end()};
            int index = d(gen);
            return actions[index];
        }

    }

    if (Knowledge.RolloutLevel >= KNOWLEDGE::SMART)
    {
        actions.clear();
        GeneratePreferred(state, history, actions, status);
        if (!actions.empty())
            return actions[Random(actions.size())];
    }
        
    if (Knowledge.RolloutLevel >= KNOWLEDGE::LEGAL)
    {
        actions.clear();
        GenerateLegal(state, history, actions, status);
        if (!actions.empty())
            return actions[Random(actions.size())];
    }

    return Random(NumActions);
}

void SIMULATOR::Prior(const STATE* state, const HISTORY& history,
    VNODE* vnode, const STATUS& status) const
{
    static vector<int> actions;
    
    if (Knowledge.TreeLevel == KNOWLEDGE::PURE || state == nullptr)
    {
        vnode->SetChildren(0, 0);
        return;
    }
    else
    {
        if (Knowledge.TreeLevel <= KNOWLEDGE::SMART) {
            vnode->SetChildren(+LargeInteger, -Infinity);

            if (Knowledge.TreeLevel >= KNOWLEDGE::LEGAL) {
                actions.clear();
                GenerateLegal(*state, history, actions, status);

                for (vector<int>::const_iterator i_action = actions.begin();
                     i_action != actions.end(); ++i_action) {
                    int a = *i_action;
                    QNODE &qnode = vnode->Child(a);
                    qnode.Value.Set(0, 0);
                    qnode.AMAF.Set(0, 0);
                }
            }

            if (Knowledge.TreeLevel >= KNOWLEDGE::SMART) {
                actions.clear();
                GeneratePreferred(*state, history, actions, status);

                for (vector<int>::const_iterator i_action = actions.begin();
                     i_action != actions.end(); ++i_action) {
                    int a = *i_action;
                    QNODE &qnode = vnode->Child(a);
                    qnode.Value.Set(Knowledge.SmartTreeCount,
                                    Knowledge.SmartTreeValue);
                    qnode.AMAF.Set(Knowledge.SmartTreeCount,
                                   Knowledge.SmartTreeValue);
                }
            }

            /*
            std::cout << "prior" << std::endl;
            for (int action = 0; action < NumActions; action++) {
                QNODE& qnode = vnode->Child(action);
                std::cout << qnode.Value.GetValue() << " " << qnode.Value.GetCount() << std::endl;
            }
            std::cout << std::endl;
            */
        }

        else if (Knowledge.TreeLevel == KNOWLEDGE::RULES)
        {
            // cout << "TREE KNOWLEDGE NORMAL" << endl;
            chrono::steady_clock::time_point start = chrono::steady_clock::now();
            vnode->SetChildren(0, 0);

            actions.clear();
            if (Knowledge.val == 20) GenerateFromRulesHardcoded15_20(*state,  vnode->Beliefs(), actions, status);
            else if (Knowledge.val == 40) GenerateFromRulesHardcoded15_40(*state, vnode->Beliefs(), actions, status);
            else if (Knowledge.val == 60) GenerateFromRulesHardcoded15_60(*state, vnode->Beliefs(), actions, status);
            else GenerateFromRulesHardcoded15(*state, vnode->Beliefs(), actions, status);

            for (auto a : actions) {
                QNODE& qnode = vnode->Child(a);
                if (!Knowledge.dynamic_tree_count) {
                    qnode.Value.Set(Knowledge.SmartTreeCount, Knowledge.SmartTreeValue);
                    qnode.AMAF.Set(Knowledge.SmartTreeCount, Knowledge.SmartTreeValue);
                }
                else {
                    qnode.Value.Set(Knowledge.SmartTreeCount, get_tree_count(a));
                    qnode.AMAF.Set(Knowledge.SmartTreeCount, get_tree_count(a));
                }
            }

            actions.clear();
            GenerateLegal(*state, history, actions, status);

            for (int a = 0; a < NumActions; a++) {
                if (find(actions.begin(), actions.end(), a) == actions.end()) {
                    QNODE &qnode = vnode->Child(a);
                    qnode.Value.Set(+LargeInteger, -Infinity);
                    qnode.AMAF.Set(+LargeInteger, -Infinity);
                }
            }

            chrono::steady_clock::time_point end = chrono::steady_clock::now();
            // cout << chrono::duration_cast<chrono::nanoseconds> (end-start).count() << endl;
        }
    }
}

bool SIMULATOR::HasAlpha() const
{
    return false;
}

void SIMULATOR::AlphaValue(const QNODE& qnode, double& q, int& n) const
{
}

void SIMULATOR::UpdateAlpha(QNODE& qnode, const STATE& state) const
{
}

void SIMULATOR::DisplayBeliefs(const BELIEF_STATE& beliefState, 
    ostream& ostr) const
{
}

void SIMULATOR::DisplayState(const STATE& state, ostream& ostr) const 
{
}

void SIMULATOR::DisplayAction(int action, ostream& ostr) const 
{
    ostr << "Action " << action << endl;
}

void SIMULATOR::DisplayObservation(const STATE& state, observation_t observation, ostream& ostr) const
{
    ostr << "Observation " << observation << endl;
}

void SIMULATOR::DisplayReward(double reward, std::ostream& ostr) const
{
    ostr << "Reward " << reward << endl;
}

double SIMULATOR::GetHorizon(double accuracy, int undiscountedHorizon) const 
{ 
    if (Discount == 1)
        return undiscountedHorizon;
    return log(accuracy) / log(Discount);
}
