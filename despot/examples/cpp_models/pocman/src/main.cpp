#include <despot/planner.h>

#include "pocman.h"

using namespace despot;

class MyPlanner: public Planner {
public:
  MyPlanner() {
  }

  DSPOMDP* InitializeModel(option::Option* options) {
    int size = 0;
    int food_prob, num_ghosts, prob_ghost;
    DSPOMDP* model;
    if (options[E_SIZE])
      size = atoi(options[E_SIZE].arg);
    else {
      cerr << "Specify size of my map using --size option" << endl;
      exit(0);
    }
    if (options[E_NUMBER])
      num_ghosts = atoi(options[E_NUMBER].arg);
    else {
      num_ghosts = 2;
    }
    if (options[E_prob_food])
      food_prob = atoi(options[E_prob_food].arg);
    else {
      food_prob = 50;
    }
    if (options[E_prob_ghosts])
      prob_ghost = atoi(options[E_prob_ghosts].arg);
    else {
      prob_ghost = 75;
    }
    cout << prob_ghost << " " << food_prob << " " << num_ghosts << endl;
    if (size == 0)
      model = new MiniPocman(food_prob/100.0, prob_ghost/100.0, num_ghosts);
    else
      model = new FullPocman(food_prob/100.0, prob_ghost/100.0, num_ghosts);
    return model;
  }

  World* InitializeWorld(std::string&  world_type, DSPOMDP* model, option::Option* options)
  {
      return InitializePOMDPWorld(world_type, model, options);
  }

  void InitializeDefaultParameters() {
     Globals::config.num_scenarios = 100;
  }

  std::string ChooseSolver(){
	  return "DESPOT";
  }
};

int main(int argc, char* argv[]) {
  return MyPlanner().RunEvaluation(argc, argv);
}
