#include "experiment.h"

using namespace std;

EXPERIMENT::PARAMS::PARAMS()
:   NumRuns(1000),
    NumSteps(100000),
    SimSteps(1000),
    TimeOut(100),
    MinDoubles(0),
    MaxDoubles(20),
    TransformDoubles(-4),
    TransformAttempts(1000),
    Accuracy(0.01),
    UndiscountedHorizon(1000),
    AutoExploration(true),
    NormProbActions(false)
{
}

EXPERIMENT::EXPERIMENT(SIMULATOR& real,
    SIMULATOR& simulator, const string& outputFile,
    EXPERIMENT::PARAMS& expParams, MCTS::PARAMS& searchParams)
:   Real(real),
    Simulator(simulator),
    ExpParams(expParams),
    SearchParams(searchParams),
    OutputFile(outputFile.c_str())
{
    if (ExpParams.AutoExploration)
    {
        if (SearchParams.UseRave)
            SearchParams.ExplorationConstant = 0;
        else
            SearchParams.ExplorationConstant = simulator.GetRewardRange();
    }
    MCTS::InitFastUCB(SearchParams.ExplorationConstant);
}

void EXPERIMENT::Run()
{
    Timer timer;

    MCTS mcts(Simulator, SearchParams);

    double undiscountedReturn = 0.0;
    double discountedReturn = 0.0;
    double discount = 1.0;
    bool terminal = false;
    bool outOfParticles = false;
    int t;
    int num_steps = 0;

    if (use_fixed_seed) {
        Real.set_seed(real_seed);
        real_seed++;
    }
    STATE* state = Real.CreateStartState();

    if (SearchParams.Verbose >= 1)
        Real.DisplayState(*state, cout);

    double prev_time;
    for (t = 0; t < ExpParams.NumSteps; t++)
    {
        // std::cout << "ELAPSED " << timer.elapsed()*1e-9 << std::endl;
        // std::cout << "TIMEOUT " << ExpParams.TimeOut << std::endl;
        if (XES::enabled())
            XES::logger().start_event();

        SIMULATOR::observation_t observation;
        double reward;

        if (XES::enabled()) {
            Simulator.log_beliefs(mcts.BeliefState());
        }
        Timer step_timer;
        int action = mcts.SelectAction();
        Results.TimePerStep.Add(step_timer.elapsed());
        num_steps++;

        terminal = Real.Step(*state, action, observation, reward);
        //cout << "REAL action " <<  action << " observation " << observation << endl;

        Results.Reward.Add(reward);
        undiscountedReturn += reward;
        discountedReturn += reward * discount;
        discount *= Real.GetDiscount();

        if (SearchParams.Verbose >= 1)
        {
            Real.DisplayAction(action, cout);
            Real.DisplayState(*state, cout);
            Real.DisplayObservation(*state, observation, cout);
            Real.DisplayReward(reward, cout);
        }

        if (XES::enabled()) {
            XES::logger().add_attribute({"simulation", true});
            Real.log_action(action);
            Real.log_observation(*state, observation);
            Real.log_reward(reward);
        }

        if (XES::enabled())
            XES::logger().end_event();

        if (terminal)
        {

            cout << "Terminated" << endl;
            break;
        }
        outOfParticles = !mcts.Update(action, observation, reward);
        if (outOfParticles)
            break;

        if (timer.elapsed()*1e-9 > ExpParams.TimeOut)
        {
            cout << "Timed out after " << t << " steps in "
                << Results.Time.GetTotal() << "seconds" << endl;
            break;
        }
        // cout << timer.elapsed() - prev_time << endl;
        prev_time = timer.elapsed();
    }

    if (outOfParticles)
    {
        cout << "Out of particles, finishing episode with SelectRandom" << endl;
        HISTORY history = mcts.GetHistory();
        while (++t < ExpParams.NumSteps)
        {
            cout << t << endl;
            SIMULATOR::observation_t observation;
            double reward;

            // This passes real state into simulator!
            // SelectRandom must only use fully observable state
            // to avoid "cheating"
            Timer step_timer;
            int action = Simulator.SelectRandom(*state, history, mcts.BeliefState(), mcts.GetStatus());
            Results.TimePerStep.Add(step_timer.elapsed());
            num_steps++;
            terminal = Real.Step(*state, action, observation, reward);

            Results.Reward.Add(reward);
            undiscountedReturn += reward;
            discountedReturn += reward * discount;
            discount *= Real.GetDiscount();

            if (SearchParams.Verbose >= 1)
            {
                Real.DisplayAction(action, cout);
                Real.DisplayState(*state, cout);
                Real.DisplayObservation(*state, observation, cout);
                Real.DisplayReward(reward, cout);
            }

            if (terminal)
            {
                cout << "Terminated" << endl;
                break;
            }

            history.Add(action, observation);
        }
    }

    Results.Time.Add(timer.elapsed());
    Results.NumSteps.Add(num_steps);
    Results.UndiscountedReturn.Add(undiscountedReturn);
    Results.DiscountedReturn.Add(discountedReturn);
    cout << "Discounted return = " << discountedReturn
        << ", average = " << Results.DiscountedReturn.GetMean() << endl;
    cout << "Undiscounted return = " << undiscountedReturn
        << ", average = " << Results.UndiscountedReturn.GetMean() << endl;

    if (XES::enabled())
        XES::logger().add_attributes(
            {{"discounted return", discountedReturn},
             {"undiscounted return", undiscountedReturn}});
}

void EXPERIMENT::MultiRun()
{
    for (int n = 0; n < ExpParams.NumRuns; n++)
    {
        cout << "Starting run " << n + 1 << " with "
             << SearchParams.NumSimulations << " simulations... " << endl;

        Real.pre_run();

        if (XES::enabled())
            XES::logger().start_trace();

        if (XES::enabled()) {
            XES::logger().add_attributes(
                {{"run", n + 1}, {"simulations", SearchParams.NumSimulations}});
            Simulator.log_run_info();
        }

        // cout << "ITER NUMBER " << n << endl; 
        Run();

        if (XES::enabled())
            XES::logger().end_trace();

        Real.post_run();

        if (Results.Time.GetTotal()*1e-9 > ExpParams.TimeOut)
        {
            cout << "Timed out after " << n << " runs in "
                << Results.Time.GetTotal() << "seconds" << endl;
            break;
        }
    }
}

void EXPERIMENT::DiscountedReturn()
{
    cout << "Main runs" << endl;
    OutputFile << "Simulations\tRuns\tUndiscounted return\tUndiscounted error\tDiscounted return\tDiscounted error\tTime\n";

    SearchParams.MaxDepth = Simulator.GetHorizon(ExpParams.Accuracy, ExpParams.UndiscountedHorizon);
    ExpParams.SimSteps = Simulator.GetHorizon(ExpParams.Accuracy, ExpParams.UndiscountedHorizon);
    ExpParams.NumSteps = Real.GetHorizon(ExpParams.Accuracy, ExpParams.UndiscountedHorizon);

    if (XES::enabled()) {
        XES::logger().add_attributes({
            {"MaxDepth", SearchParams.MaxDepth},
            {"SimSteps", ExpParams.SimSteps},
            {"NumSteps", ExpParams.NumSteps},
            {"shield", SearchParams.use_shield},
        });
        Simulator.log_problem_info();
    }

    for (int i = ExpParams.MinDoubles; i <= ExpParams.MaxDoubles; i++)
    {
        if (use_fixed_seed) {
            UTILS::RandomSeed(fixed_seed);
            Simulator.set_seed(fixed_seed);
            real_seed = fixed_seed + 1;
        }

        SearchParams.NumSimulations = 1 << i;
        SearchParams.NumStartStates = 1 << i;
        if (i + ExpParams.TransformDoubles >= 0)
            SearchParams.NumTransforms = 1 << (i + ExpParams.TransformDoubles);
        else
            SearchParams.NumTransforms = 1;
        SearchParams.MaxAttempts = SearchParams.NumTransforms * ExpParams.TransformAttempts;

        Results.Clear();
        MultiRun();

        if (XES::enabled()) {
            XES::logger().add_attributes(
                    {{"average undiscounted return",
                    Results.UndiscountedReturn.GetMean()},
                    {"average undiscounted return std",
                    Results.UndiscountedReturn.GetStdErr()},
                    {"average discounted return", Results.DiscountedReturn.GetMean()},
                    {"average discounted return std",
                    Results.DiscountedReturn.GetStdErr()},
                    {"average time", Results.Time.GetMean()*1e-9},
                    {"average time std", Results.Time.GetStdDev()*1e-9},
                    {"total time", Results.Time.GetTotal()*1e-9},
                    {"average num steps", Results.NumSteps.GetMean()},
                    {"average num steps std", Results.NumSteps.GetStdDev()},
                    {"average time per step", Results.TimePerStep.GetMean()*1e-9},
                    {"average time per step std", Results.TimePerStep.GetStdDev()*1e-9}
                    });
        }

        cout << "Simulations = " << SearchParams.NumSimulations << endl
            << "Runs = " << Results.Time.GetCount() << endl
            << "Undiscounted return = " << Results.UndiscountedReturn.GetMean()
            << " +- " << Results.UndiscountedReturn.GetStdErr() << endl
            << "Discounted return = " << Results.DiscountedReturn.GetMean()
            << " +- " << Results.DiscountedReturn.GetStdErr() << endl
            << "Time = " << Results.Time.GetMean() << endl;
        OutputFile << SearchParams.NumSimulations << "\t"
            << Results.Time.GetCount() << "\t"
            << Results.UndiscountedReturn.GetMean() << "\t"
            << Results.UndiscountedReturn.GetStdErr() << "\t"
            << Results.DiscountedReturn.GetMean() << "\t"
            << Results.DiscountedReturn.GetStdErr() << "\t"
            << Results.Time.GetMean() << endl;
    }
}

void EXPERIMENT::AverageReward()
{
    cout << "Main runs" << endl;
    OutputFile << "Simulations\tSteps\tAverage reward\tAverage time\n";

    SearchParams.MaxDepth = Simulator.GetHorizon(ExpParams.Accuracy, ExpParams.UndiscountedHorizon);
    ExpParams.SimSteps = Simulator.GetHorizon(ExpParams.Accuracy, ExpParams.UndiscountedHorizon);

    for (int i = ExpParams.MinDoubles; i <= ExpParams.MaxDoubles; i++)
    {
        SearchParams.NumSimulations = 1 << i;
        SearchParams.NumStartStates = 1 << i;
        if (i + ExpParams.TransformDoubles >= 0)
            SearchParams.NumTransforms = 1 << (i + ExpParams.TransformDoubles);
        else
            SearchParams.NumTransforms = 1;
        SearchParams.MaxAttempts = SearchParams.NumTransforms * ExpParams.TransformAttempts;

        Results.Clear();
        Run();

        cout << "Simulations = " << SearchParams.NumSimulations << endl
            << "Steps = " << Results.Reward.GetCount() << endl
            << "Average reward = " << Results.Reward.GetMean()
            << " +- " << Results.Reward.GetStdErr() << endl
            << "Average time = " << Results.Time.GetMean() / Results.Reward.GetCount() << endl;
        OutputFile << SearchParams.NumSimulations << "\t"
            << Results.Reward.GetCount() << "\t"
            << Results.Reward.GetMean() << "\t"
            << Results.Reward.GetStdErr() << "\t"
            << Results.Time.GetMean() / Results.Reward.GetCount() << endl;
    }
}

//----------------------------------------------------------------------------
