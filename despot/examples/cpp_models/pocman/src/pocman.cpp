#include "pocman.h"

#include <despot/core/builtin_lower_bounds.h>
#include <despot/core/builtin_policy.h>
#include <despot/core/builtin_upper_bounds.h>
using namespace std;

std::mt19937 gen(std::random_device{}());

namespace despot {

/* ==============================================================================
 * PocmanBelief class
 * ==============================================================================*/
int PocmanBelief::num_particles = 50000;

PocmanBelief::PocmanBelief(vector<State*> particles, const DSPOMDP* model,
	Belief* prior) :
	ParticleBelief(particles, model, prior),
	pocman_(static_cast<const Pocman*>(model)) {
}

void PocmanBelief::Update(ACT_TYPE action, OBS_TYPE obs) {
	history_.Add(action, obs);

	vector<State*> updated;
	double reward;
	OBS_TYPE o;
	int cur = 0, N = particles_.size(), trials = 0;
	while (updated.size() < num_particles && trials < 10 * num_particles) {
		State* particle = pocman_->Copy(particles_[cur]);
		bool terminal = pocman_->Step(*particle, Random::RANDOM.NextDouble(),
			action, reward, o);

		if ((!terminal && o == obs)
			|| pocman_->LocalMove(*particle, history_, obs)) {
			updated.push_back(particle);
		} else {
			pocman_->Free(particle);
		}

		cur = (cur + 1) % N;

		trials++;
	}

	for (int i = 0; i < particles_.size(); i++)
		pocman_->Free(particles_[i]);

	particles_ = updated;

	for (int i = 0; i < particles_.size(); i++)
		particles_[i]->weight = 1.0 / particles_.size();
}

/* ==============================================================================
 * PocmanSmartParticleUpperBound class
 * ==============================================================================*/

class PocmanSmartParticleUpperBound: public ParticleUpperBound {
protected:
	const Pocman* pocman_;
public:
	PocmanSmartParticleUpperBound(const Pocman* model) :
		pocman_(model) {
	}

	double Value(const State& state) const {
		const PocmanState& pocstate = static_cast<const PocmanState&>(state);
		return (pocman_->reward_eat_food_ + pocman_->reward_eat_ghost_)
			* (1 - Globals::Discount(pocstate.num_food)) / (1 - Globals::Discount())
			+ pocman_->reward_clear_level_ * Globals::Discount(pocstate.num_food);
	}
};

/* ==============================================================================
 * PocmanApproxScenarioUpperBound class
 * ==============================================================================*/
class PocmanApproxScenarioUpperBound: public ScenarioUpperBound {
protected:
	const Pocman* pocman_;
public:
	PocmanApproxScenarioUpperBound(const Pocman* model) :
		pocman_(model) {
	}

	double Value(const vector<State*>& particles,
		RandomStreams& streams, History& history) const {
		double total_value = 0;
		for (int i = 0; i < particles.size(); i++) {
			PocmanState& state = static_cast<PocmanState&>(*(particles[i]));
			double value = 0;

			int max_dist = 0;

			for (int i = 0; i < state.food.size(); i++) {
				if (state.food[i] != 1)
					continue;
				Coord food_pos = pocman_->maze_.GetCoord(i);
				int dist = Coord::ManhattanDistance(state.pocman_pos, food_pos);
				value += pocman_->reward_eat_food_ * Globals::Discount(dist);
				max_dist = max(max_dist, dist);
			}

			// Clear level
			value += pocman_->reward_clear_level_ * pow(Globals::config.discount, max_dist);

			// Default move-reward
			value += pocman_->reward_default_ * (Globals::Discount() < 1
					? (1 - Globals::Discount(max_dist)) / (1 - Globals::Discount())
					: max_dist);

			// If pocman is chasing a ghost, encourage it
			if (state.power_steps > 0 && history.Size() &&
					(history.LastObservation() & 15) != 0) {
				int act = history.LastAction();
				int obs = history.LastObservation();
				if (CheckFlag(obs, act)) {
					bool seen_ghost = false;
					for (int dist = 1; !seen_ghost; dist++) {
						Coord ghost_pos = state.pocman_pos + Compass::DIRECTIONS[act] * dist;
						for (int g = 0; g < pocman_->num_ghosts_; g++)
							if (state.ghost_pos[g] == ghost_pos) {
								value += pocman_->reward_eat_ghost_ * Globals::Discount(dist);
								seen_ghost = true;
								break;
							}
					}
				}
			}

			// Ghost penalties
			double dist = 0;
			for (int g = 0; g < pocman_->num_ghosts_; g++)
				dist += Coord::ManhattanDistance(state.pocman_pos, state.ghost_pos[g]);
			value += pocman_->reward_die_ * pow(Globals::Discount(), dist / pocman_->num_ghosts_);

			// Penalize for doubling back, but not so much as to prefer hitting a wall
			if (history.Size() >= 2 &&
					Compass::Opposite(history.Action(history.Size() - 1))
					== history.Action(history.Size() - 2))
				value += pocman_->reward_hit_wall_ / 2;

			total_value += state.weight * value;
		}

		return total_value;
	}
};

/* ==============================================================================
 * PocmanLegalParticleLowerBound class
 * ==============================================================================*/

class PocmanLegalParticleLowerBound: public ParticleLowerBound {
protected:
	const Pocman* pocman_;
public:
	PocmanLegalParticleLowerBound(const DSPOMDP* model) :
		ParticleLowerBound(model),
		pocman_(static_cast<const Pocman*>(model)) {
	}

	ValuedAction Value(const vector<State*>& particles) const {
		const PocmanState& pocstate =
			static_cast<const PocmanState&>(*particles[0]);
		vector<int> legal;
		for (int a = 0; a < 4; ++a) {
			Coord newpos = pocman_->NextPos(pocstate.pocman_pos, a);
			if (newpos.x >= 0 && newpos.y >= 0)
				legal.push_back(a);
		}
		return ValuedAction(legal[Random::RANDOM.NextInt(legal.size())],
			State::Weight(particles)
				* (pocman_->reward_die_
					+ pocman_->reward_default_ / (1 - Globals::Discount())));
	}
};

/* ==============================================================================
 * PocmanSmartPolicy class
 * ==============================================================================*/










class PocmanRulesDefaultPolicy : public DefaultPolicy {
protected:
	const Pocman* poc_model_;
	mutable vector<ACT_TYPE> preferred_;
	mutable vector<ACT_TYPE> legal_;
	mutable vector<double> prob_ghost_short_list;
	mutable vector<double> prob_food_list;
	mutable vector<double> prob_ghost_list;
	mutable Grid<int> counts_ghost;
	mutable Grid<int> counts_food;
	int food_range=2;
	int ghost_range=6;
	double min_prob = 0.5;
public:
	PocmanRulesDefaultPolicy(const Pocman* model, ParticleLowerBound* bound) :
		DefaultPolicy(model, bound),
		poc_model_(model) {
	}

	ACT_TYPE Action(const vector<State*>& particles, RandomStreams& streams,
		History& history) const {

	preferred_.clear();
	legal_.clear();


    if (particles.size() == 0){
		// if (history.size()>0){
			// Coord dirpos;
			// Coord pocpos = history.Last
			// for (ACT_TYPE a=0; a < 4; a++){
			// 	dirpos = pocpos+Compass::DIRECTIONS[a];
			// 	if(poc_model_->maze_.Inside(dirpos) && poc_model_->Passable(dirpos))
			// 		legal_.push_back(a);
			// }

			return legal_[Random::RANDOM.NextInt(4)];
		// }
	}

	const PocmanState& pocstate =
		static_cast<const PocmanState&>(*particles[0]);

	Coord pocpos = pocstate.pocman_pos;

    prob_ghost_short_list.clear();
    prob_ghost_list.clear();
    prob_food_list.clear();
	counts_ghost.Resize(poc_model_->maze_.xsize(), poc_model_->maze_.ysize());
	counts_food.Resize(poc_model_->maze_.xsize(), poc_model_->maze_.ysize());
	counts_ghost.SetAllValues(0);
	counts_food.SetAllValues(0);
    vector<ACT_TYPE> tmp_actions = legal_;

    for (int i = 0; i < particles.size(); i++)
    {
        const PocmanState& ps =
            static_cast<const PocmanState&>(
                *particles[i]);

        for (int g = 0; g < poc_model_->num_ghosts_; g++)
            counts_ghost(ps.ghost_pos[g])++;
        for (int y = min(poc_model_->maze_.ysize()-1, pocpos.y+food_range); y >= max(0, pocpos.y-food_range); y--){
            Coord pos(pocpos.x, y);
            int index = poc_model_->maze_.Index(pos);
            if (ps.food[index]) counts_food(pocpos.x, y)++;
        }
        for (int x = max(0, pocpos.x-food_range); x < min(poc_model_->maze_.xsize(), pocpos.x+food_range); x++){
            Coord pos(x, pocpos.y);
            int index = poc_model_->maze_.Index(pos);
            if (ps.food[index]) counts_food(x, pocpos.y)++;
        }

    }


    double max_prob_food_w = 0;
    double max_prob_food_n = 0;
    double max_prob_food_s = 0;
    double max_prob_food_e = 0;
    double max_prob_ghost_w = 0;
    double max_prob_ghost_e = 0;
    double max_prob_ghost_n = 0;
    double max_prob_ghost_s = 0;
    double max_prob_ghost_short_w = 0;
    double max_prob_ghost_short_e = 0;
    double max_prob_ghost_short_n = 0;
    double max_prob_ghost_short_s = 0;
    // logi << pocpos.x << ", " << pocpos.y << endl;
    for(int x = 1; x <= ghost_range; x++){
        if (pocpos.x >= x){
            // prob_ghost_w = (double) counts_ghost(pocpos.x - x, pocpos.y) / (particles.size()); 
            max_prob_ghost_w = max((double) counts_ghost(pocpos.x - x, pocpos.y) / (particles.size()), max_prob_ghost_w);// (prob_ghost_w>max_prob_ghost_w) ? prob_ghost_w : max_prob_ghost_w;
            if (x <= food_range){
                // prob_ghost_short_w = (double) counts_ghost(pocpos.x - x, pocpos.y) / (particles.size());
                max_prob_ghost_short_w = max((double) counts_ghost(pocpos.x - x, pocpos.y) / (particles.size()), max_prob_ghost_short_w);// (prob_ghost_short_w>max_prob_ghost_short_w) ? prob_ghost_short_w : max_prob_ghost_short_w;
                // prob_food_w = (double) counts_food(pocpos.x - x, pocpos.y) / (particles.size());
                max_prob_food_w = max((double) counts_food(pocpos.x - x, pocpos.y) / (particles.size()), max_prob_food_w);// (prob_food_w>max_prob_food_w) ? prob_food_w : max_prob_food_w;}
            }
        }
        if (pocpos.x + x < poc_model_->maze_.xsize()){
            // prob_ghost_e = (double) counts_ghost(pocpos.x + x, pocpos.y) / (particles.size()); 
            max_prob_ghost_e = max((double) counts_ghost(pocpos.x + x, pocpos.y) / (particles.size()), max_prob_ghost_e);// (prob_ghost_e>max_prob_ghost_e) ? prob_ghost_e : max_prob_ghost_e;
            if (x <= food_range){
                // prob_ghost_short_e = (double) counts_ghost(pocpos.x + x, pocpos.y) / (particles.size());
                max_prob_ghost_short_e = max((double) counts_ghost(pocpos.x + x, pocpos.y) / (particles.size()), max_prob_ghost_short_e);// (prob_ghost_short_e>max_prob_ghost_short_e) ? prob_ghost_short_e : max_prob_ghost_short_e;
                // prob_food_e = (double) counts_food(pocpos.x + x, pocpos.y) / (particles.size());
                max_prob_food_e = max((double) counts_food(pocpos.x + x, pocpos.y) / (particles.size()), max_prob_food_e);// (prob_food_e>max_prob_food_e) ? prob_food_e : max_prob_food_e;}
            }

        }
        if (pocpos.y >= x){
            // prob_ghost_s = (double) counts_ghost(pocpos.x, pocpos.y - x) / (particles.size()); 
            max_prob_ghost_s = max((double) counts_ghost(pocpos.x, pocpos.y - x) / (particles.size()), max_prob_ghost_s);// (prob_ghost_s>max_prob_ghost_s) ? prob_ghost_s : max_prob_ghost_s;
            if (x <= food_range){
                // prob_ghost_short_s = (double) counts_ghost(pocpos.x, pocpos.y - x) / (particles.size());
                max_prob_ghost_short_s = max((double) counts_ghost(pocpos.x, pocpos.y - x) / (particles.size()), max_prob_ghost_short_s);// (prob_ghost_short_s>max_prob_ghost_short_s) ? prob_ghost_short_s : max_prob_ghost_short_s;
                // prob_food_s = (double) counts_food(pocpos.x, pocpos.y - x) / (particles.size());
                max_prob_food_s = max((double) counts_food(pocpos.x, pocpos.y - x) / (particles.size()), max_prob_food_s);// (prob_food_s>max_prob_food_s) ? prob_food_s : max_prob_food_s;}
            }

        }
        if (pocpos.y + x < poc_model_->maze_.ysize()){
            // prob_ghost_n = (double) counts_ghost(pocpos.x, pocpos.y + x) / (particles.size()); 
            max_prob_ghost_n = max((double) counts_ghost(pocpos.x, pocpos.y + x) / (particles.size()), max_prob_ghost_n);// (prob_ghost_n>max_prob_ghost_n) ? prob_ghost_n : max_prob_ghost_n;
            if (x <= food_range){
                // prob_ghost_short_n = (double) counts_ghost(pocpos.x, pocpos.y + x) / (particles.size());
                max_prob_ghost_short_n = max((double) counts_ghost(pocpos.x, pocpos.y + x) / (particles.size()), max_prob_ghost_short_n);// (prob_ghost_short_n>max_prob_ghost_short_n) ? prob_ghost_short_n : max_prob_ghost_short_n;
                // prob_food_n = (double) counts_food(pocpos.x, pocpos.y + x) / (particles.size());
                max_prob_food_n = max((double) counts_food(pocpos.x, pocpos.y + x) / (particles.size()), max_prob_food_n);// (prob_food_n>max_prob_food_n) ? prob_food_n : max_prob_food_n;}
            }

        }
    }
    Coord dirpos = pocpos+Compass::DIRECTIONS[Compass::WEST];
    if (max_prob_ghost_w <= min_prob && poc_model_->maze_.Inside(dirpos) && poc_model_->Passable(dirpos)){
        legal_.push_back(Compass::WEST);
		tmp_actions.push_back(Compass::WEST);
        prob_food_list.push_back(max_prob_food_w);
        prob_ghost_short_list.push_back(max_prob_ghost_short_w);
        prob_ghost_list.push_back(max_prob_ghost_w);    
    }
	else if (poc_model_->maze_.Inside(dirpos) && poc_model_->Passable(dirpos)) legal_.push_back(Compass::WEST);
    dirpos = pocpos+Compass::DIRECTIONS[Compass::EAST];
    if (max_prob_ghost_e <= min_prob && poc_model_->maze_.Inside(dirpos) && poc_model_->Passable(dirpos)){
        legal_.push_back(Compass::EAST);
		tmp_actions.push_back(Compass::EAST);
        prob_food_list.push_back(max_prob_food_e);
        prob_ghost_short_list.push_back(max_prob_ghost_short_e);
        prob_ghost_list.push_back(max_prob_ghost_e);    
    }
	else if (poc_model_->maze_.Inside(dirpos) && poc_model_->Passable(dirpos)) legal_.push_back(Compass::EAST);
    dirpos = pocpos+Compass::DIRECTIONS[Compass::SOUTH];
    if (max_prob_ghost_s <= min_prob && poc_model_->maze_.Inside(dirpos) && poc_model_->Passable(dirpos)){
        legal_.push_back(Compass::SOUTH);
		tmp_actions.push_back(Compass::SOUTH);
        prob_food_list.push_back(max_prob_food_s);
        prob_ghost_short_list.push_back(max_prob_ghost_short_s);
        prob_ghost_list.push_back(max_prob_ghost_s);    
    }
	else if (poc_model_->maze_.Inside(dirpos) && poc_model_->Passable(dirpos)) legal_.push_back(Compass::SOUTH);
    dirpos = pocpos+Compass::DIRECTIONS[Compass::NORTH];
    if (max_prob_ghost_n <= min_prob && poc_model_->maze_.Inside(dirpos) && poc_model_->Passable(dirpos)){
        legal_.push_back(Compass::NORTH);
		tmp_actions.push_back(Compass::NORTH);
        prob_food_list.push_back(max_prob_food_n);
        prob_ghost_short_list.push_back(max_prob_ghost_short_n);
        prob_ghost_list.push_back(max_prob_ghost_n);    
    }
	else if (poc_model_->maze_.Inside(dirpos) && poc_model_->Passable(dirpos)) legal_.push_back(Compass::NORTH);

    // logi << "TMP ACTIONS ARE: ";
    // for (auto a : tmp_actions) logi << a << " ";
    // logi << endl;

    if (tmp_actions.size() > 1){
        // logi << "PROB FOOD LIST ARE ";
        // for (auto el : prob_food_list) logi << el << " ";
        // logi << endl;
        // logi << "PROB GHOST SHORT LIST ARE ";
        // for (auto el : prob_ghost_short_list) logi << el << " ";
        // logi << endl;  
        int max_food = poc_model_->arg_max(prob_food_list);
        double max_food_value = prob_food_list[max_food];
        int tmp = max_food;
        vector<int> max_food_tmp = {max_food};
        int i = 0;
        while (max_food == tmp){
            // logi << "IN WHILE"  << endl;
            prob_food_list.erase(prob_food_list.begin() + max_food);
            if (prob_food_list.size() == 0) break;
            tmp = poc_model_->arg_max(prob_food_list);
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
                    preferred_.push_back(tmp_actions[idx]);
                }
            }
        }
        else preferred_.push_back(tmp_actions[max_food]);
    }
    else preferred_ = tmp_actions;

	if (preferred_.size() != 0){
		std::vector<double> prob_actual;
		for (auto a : preferred_){
			prob_actual.push_back(0.98);
		}

		double sum = accumulate(prob_actual.begin(), prob_actual.end(), 0.0);
		for_each(prob_actual.begin(), prob_actual.end(), [sum](double &c){ c /= sum; });

		std::discrete_distribution<std::size_t> d{prob_actual.begin(), prob_actual.end()};
		int index = d(gen);
		return preferred_[index];
	}
	else if (legal_.size() > 0) return legal_[Random::RANDOM.NextInt(legal_.size())];
	else{ logi << "OUT OF LEGAL ACTIONS" << endl; return 0;};
	
	}
};


















class PocmanSmartPolicy : public DefaultPolicy {
protected:
	const Pocman* pocman_;
	mutable vector<ACT_TYPE> preferred_;
	mutable vector<ACT_TYPE> legal_;
public:
	PocmanSmartPolicy(const Pocman* model, ParticleLowerBound* bound) :
		DefaultPolicy(model, bound),
		pocman_(model) {
	}

	ACT_TYPE Action(const vector<State*>& particles, RandomStreams& streams,
		History& history) const {
		const PocmanState& pocstate =
			static_cast<const PocmanState&>(*particles[0]);
		preferred_.clear();
		legal_.clear();

		if (history.Size()) {
			ACT_TYPE action = history.LastAction();
			OBS_TYPE observation = history.LastObservation();

			// If power pill and can see a ghost then chase it
			if (pocstate.power_steps > 0 && ((observation & 15) != 0)) {
				for (int a = 0; a < 4; a++)
					if (CheckFlag(observation, a))
						preferred_.push_back(a);
			} else { // Otherwise avoid observed ghosts and avoid changing directions
				for (int a = 0; a < 4; a++) {
					Coord newpos = pocman_->NextPos(pocstate.pocman_pos, a);
					if (newpos.x >= 0 && newpos.y >= 0
						&& !CheckFlag(observation, a)
						&& Compass::Opposite(a) != action)
						preferred_.push_back(a);
				}
			}

			if (preferred_.size() > 0)
				return preferred_[Random::RANDOM.NextInt(preferred_.size())];
		}

		for (int a = 0; a < 4; ++a) {
			Coord newpos = pocman_->NextPos(pocstate.pocman_pos, a);
			if (newpos.x >= 0 && newpos.y >= 0)
				legal_.push_back(a);
		}

		return legal_[Random::RANDOM.NextInt(legal_.size())];
	}
};

/* ==============================================================================
 * PocmanPOMCPPrior class
 * ==============================================================================*/

class PocmanPOMCPPrior: public POMCPPrior {
private:
	const Pocman* pocman_;

public:
	PocmanPOMCPPrior(const Pocman* model) :
		POMCPPrior(model),
		pocman_(model) {
	}

	void ComputePreference(const State& state) {
		const PocmanState& pocstate = static_cast<const PocmanState&>(state);
		legal_actions_.clear();
		preferred_actions_.clear();

		for (int a = 0; a < 4; a++) {
			Coord newpos = pocman_->NextPos(pocstate.pocman_pos, a);
			if (newpos.x >= 0 && newpos.y >= 0)
				legal_actions_.push_back(a);
		}

		if (history_.Size()) {
			ACT_TYPE action = history_.LastAction();
			OBS_TYPE observation = history_.LastObservation();

			// If power pill and can see a ghost then chase it
			if (pocstate.power_steps > 0 && ((observation & 15) != 0)) {
				for (int a = 0; a < 4; a++)
					if (CheckFlag(observation, a))
						preferred_actions_.push_back(a);
			} else { // Otherwise avoid observed ghosts and avoid changing directions
				for (int a = 0; a < 4; a++) {
					Coord newpos = pocman_->NextPos(pocstate.pocman_pos, a);
					if (newpos.x >= 0 && newpos.y >= 0
						&& !CheckFlag(observation, a)
						&& Compass::Opposite(a) != action)
						preferred_actions_.push_back(a);
				}
			}
		}
	}
};

/* ==============================================================================
 * Pocman class
 * ==============================================================================*/

Pocman::Pocman(int xsize, int ysize, double food_prob, double prob_ghost, int num_ghosts) :
	maze_(xsize, ysize),
	passage_y_(-1),
	smell_range_(1),
	hear_range_(2),
	food_prob_(food_prob),
	chase_prob_(prob_ghost),
	defensive_slip_(0.25),
	reward_clear_level_(+1000),
	reward_default_(-1),
	reward_die_(-100),
	reward_eat_food_(+10),
	reward_eat_ghost_(+25),
	reward_hit_wall_(-25),
	power_num_steps_(15) {
	// NumObservations = 1 << 10;
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
}

MicroPocman::MicroPocman(double food_prob, double prob_ghost, int num_ghosts) :
	Pocman(7, 7, food_prob, prob_ghost, num_ghosts) {
	int maze[7][7] = { { 3, 3, 3, 3, 3, 3, 3 },
		{ 3, 3, 0, 3, 0, 3, 3 },
		{ 3, 0, 3, 3, 3, 0, 3 },
		{ 3, 3, 3, 0, 3, 3, 3 },
		{ 3, 0, 3, 3, 3, 0, 3 },
		{ 3, 3, 0, 3, 0, 3, 3 },
		{ 3, 3, 3, 3, 3, 3, 3 } };

	for (int x = 0; x < 7; x++)
		maze_.SetCol(x, maze[x]);
	num_ghosts_ = num_ghosts;
	ghost_range_ = 3;
	pocman_home_ = Coord(3, 0);
	ghost_home_ = Coord(3, 4);
}

MiniPocman::MiniPocman(double food_prob, double prob_ghost, int num_ghosts) :
	Pocman(10, 10, food_prob, prob_ghost, num_ghosts) {
	int maze[10][10] = { { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 },
		{ 3, 0, 0, 3, 0, 0, 3, 0, 0, 3 },
		{ 3, 0, 3, 3, 3, 3, 3, 3, 0, 3 },
		{ 3, 3, 3, 0, 0, 0, 0, 3, 3, 3 },
		{ 0, 0, 3, 0, 1, 1, 3, 3, 0, 0 },
		{ 0, 0, 3, 0, 1, 1, 3, 3, 0, 0 },
		{ 3, 3, 3, 0, 0, 0, 0, 3, 3, 3 },
		{ 3, 0, 3, 3, 3, 3, 3, 3, 0, 3 },
		{ 3, 0, 0, 3, 0, 0, 3, 0, 0, 3 },
		{ 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 } };

	for (int x = 0; x < 10; x++)
		maze_.SetCol(x, maze[x]);

	logi << "FOOD PROB IS " << food_prob << endl;
	num_ghosts_ = num_ghosts;
	ghost_range_ = 4;
	pocman_home_ = Coord(4, 2);
	ghost_home_ = Coord(4, 4);
	passage_y_ = 5;
}

FullPocman::FullPocman(double food_prob, double prob_ghost, int num_ghosts) :
	Pocman(17, 19, food_prob, prob_ghost, num_ghosts) {
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
		{ 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 } };

	// Transpose to rows
	for (int x = 0; x < 19; x++)
		maze_.SetRow(x, maze[18 - x]);

	num_ghosts_ = num_ghosts;
	ghost_range_ = 6;
	pocman_home_ = Coord(8, 6);
	ghost_home_ = Coord(8, 10);
	passage_y_ = 10;
}

Coord Pocman::NextPos(const Coord& from, int dir) const {
	Coord nextPos;
	if (from.x == 0 && from.y == passage_y_ && dir == Compass::WEST)
		nextPos = Coord(maze_.xsize() - 1, from.y);
	else if (from.x == maze_.xsize() - 1 && from.y == passage_y_
		&& dir == Compass::EAST)
		nextPos = Coord(0, from.y);
	else
		nextPos = from + Compass::DIRECTIONS[dir];

	if (maze_.Inside(nextPos) && Passable(nextPos))
		return nextPos;
	else
		return Coord(-1, -1);
}

bool Pocman::Step(State& state, double rand_num, ACT_TYPE action, double& reward,
	OBS_TYPE& observation) const {
	Random random(rand_num);

	PocmanState& pocstate = static_cast<PocmanState&>(state);
	reward = reward_default_;
	observation = 0;

	Coord newpos = NextPos(pocstate.pocman_pos, action);
	if (newpos.x >= 0 && newpos.y >= 0)
		pocstate.pocman_pos = newpos;
	else
		reward += reward_hit_wall_;

	if (pocstate.power_steps > 0)
		pocstate.power_steps--;

	int hitGhost = -1;
	for (int g = 0; g < num_ghosts_; g++) {
		if (pocstate.ghost_pos[g] == pocstate.pocman_pos)
			hitGhost = g;
		MoveGhost(pocstate, g, random);
		if (pocstate.ghost_pos[g] == pocstate.pocman_pos)
			hitGhost = g;
	}

	if (hitGhost >= 0) {
		if (pocstate.power_steps > 0) {
			reward += reward_eat_ghost_;
			pocstate.ghost_pos[hitGhost] = ghost_home_;
			pocstate.ghost_dir[hitGhost] = -1;
		} else {
			reward += reward_die_;
			return true;
		}
	}

	observation = MakeObservations(pocstate);

	int pocIndex = maze_.Index(pocstate.pocman_pos);
	if (pocstate.food[pocIndex]) {
		pocstate.food[pocIndex] = false;
		pocstate.num_food--;
		if (pocstate.num_food == 0) {
			reward += reward_clear_level_;
			return true;
		}
		if (CheckFlag(maze_(pocstate.pocman_pos.x, pocstate.pocman_pos.y),
			E_POWER))
			pocstate.power_steps = power_num_steps_;
		reward += reward_eat_food_;
	}

	return false;
}

void Pocman::Validate(const State& state) const {
	const PocmanState& pocstate = static_cast<const PocmanState&>(state);
	assert(maze_.Inside(pocstate.pocman_pos));
	assert(Passable(pocstate.pocman_pos));
	for (int g = 0; g < num_ghosts_; g++) {
		assert(maze_.Inside(pocstate.ghost_pos[g]));
		assert(Passable(pocstate.ghost_pos[g]));
	}
}

int Pocman::NumActions() const {
	return 4;
}

double Pocman::ObsProb(OBS_TYPE obs, const State& state, ACT_TYPE action) const {
	return obs == MakeObservations(static_cast<const PocmanState&>(state));
}

State* Pocman::CreateStartState(string tyep) const {
	PocmanState* startState = memory_pool_.Allocate();
	startState->ghost_pos.resize(num_ghosts_);
	startState->ghost_dir.resize(num_ghosts_);
	startState->food.resize(maze_.xsize() * maze_.ysize());
	NewLevel(*startState);
	return startState;
}

Belief* Pocman::InitialBelief(const State* start, string type) const {
	int N = PocmanBelief::num_particles;
	vector<State*> particles(N);
	for (int i = 0; i < N; i++) {
		particles[i] = CreateStartState();
		particles[i]->weight = 1.0 / N;
	}

	return new PocmanBelief(particles, this);
}

int Pocman::MakeObservations(const PocmanState& pocstate) const {
	int observation = 0;
	for (int d = 0; d < 4; d++) {
		if (SeeGhost(pocstate, d) >= 0)
			SetFlag(observation, d);
		Coord wpos = NextPos(pocstate.pocman_pos, d);
		if (wpos.x >= 0 && wpos.y >= 0 && Passable(wpos))
			SetFlag(observation, d + 4);
	}
	if (SmellFood(pocstate))
		SetFlag(observation, 8);
	if (HearGhost(pocstate))
		SetFlag(observation, 9);
	return observation;
}

bool Pocman::LocalMove(State& state, const History& history, int obs) const {
	PocmanState& pocstate = static_cast<PocmanState&>(state);

	int numGhosts = Random::RANDOM.NextInt(1, 3); // Change 1 or 2 ghosts at a time
	for (int i = 0; i < numGhosts; ++i) {
		int g = Random::RANDOM.NextInt(num_ghosts_);
		pocstate.ghost_pos[g] = Coord(Random::RANDOM.NextInt(maze_.xsize()),
			Random::RANDOM.NextInt(maze_.ysize()));
		if (!Passable(pocstate.ghost_pos[g])
			|| pocstate.ghost_pos[g] == pocstate.pocman_pos)
			return false;
	}

	Coord smellPos;
	for (smellPos.x = -smell_range_; smellPos.x <= smell_range_; smellPos.x++) {
		for (smellPos.y = -smell_range_; smellPos.y <= smell_range_; smellPos.y++) {
			Coord pos = pocstate.pocman_pos + smellPos;
			if (smellPos != Coord(0, 0) && maze_.Inside(pos)
				&& CheckFlag(maze_(pos), E_SEED)) {
				double v = Random::RANDOM.NextDouble();
				pocstate.food[maze_.Index(pos)] = v < (food_prob_ * 0.5);
			}
		}
	}

	// Just check the last time-step, don't check for full consistency
	if (history.Size() == 0)
		return true;
	int observation = MakeObservations(pocstate);
	return history.LastObservation() == observation;
}

void Pocman::MoveGhost(PocmanState& pocstate, int g, Random &random) const {
	if (Coord::ManhattanDistance(pocstate.pocman_pos, pocstate.ghost_pos[g])
		< ghost_range_) {
		if (pocstate.power_steps > 0)
			MoveGhostDefensive(pocstate, g, random);
		else
			MoveGhostAggressive(pocstate, g, random);
	} else {
		MoveGhostRandom(pocstate, g, random);
	}
}

void Pocman::MoveGhostAggressive(PocmanState& pocstate, int g,
	Random &random) const {
	if (random.NextDouble() > chase_prob_) {
		MoveGhostRandom(pocstate, g, random);
		return;
	}

	int bestDist = maze_.xsize() + maze_.ysize();
	Coord bestPos = pocstate.ghost_pos[g];
	int bestDir = -1;
	for (int dir = 0; dir < 4; dir++) {
		int dist = Coord::DirectionalDistance(pocstate.pocman_pos,
			pocstate.ghost_pos[g], dir);
		Coord newpos = NextPos(pocstate.ghost_pos[g], dir);
		if (dist <= bestDist && newpos.x >= 0 && newpos.y >= 0
			&& Compass::Opposite(dir) != pocstate.ghost_dir[g]) {
			bestDist = dist;
			bestPos = newpos;
		}
	}

	pocstate.ghost_pos[g] = bestPos;
	pocstate.ghost_dir[g] = bestDir;
}

void Pocman::MoveGhostDefensive(PocmanState& pocstate, int g,
	Random &random) const {
	if (random.NextDouble() < defensive_slip_ && pocstate.ghost_dir[g] >= 0) {
		pocstate.ghost_dir[g] = -1;
		return;
	}

	int bestDist = 0;
	Coord bestPos = pocstate.ghost_pos[g];
	int bestDir = -1;
	for (int dir = 0; dir < 4; dir++) {
		int dist = Coord::DirectionalDistance(pocstate.pocman_pos,
			pocstate.ghost_pos[g], dir);
		Coord newpos = NextPos(pocstate.ghost_pos[g], dir);
		if (dist >= bestDist && newpos.x >= 0 && newpos.y >= 0
			&& Compass::Opposite(dir) != pocstate.ghost_dir[g]) {
			bestDist = dist;
			bestPos = newpos;
		}
	}

	pocstate.ghost_pos[g] = bestPos;
	pocstate.ghost_dir[g] = bestDir;
}

void Pocman::MoveGhostRandom(PocmanState& pocstate, int g,
	Random &random) const {
	// Never switch to opposite direction
	// Currently assumes there are no dead-ends.
	Coord newpos;
	int dir;
	do {
		dir = random.NextInt(4);
		newpos = NextPos(pocstate.ghost_pos[g], dir);
	} while (Compass::Opposite(dir) == pocstate.ghost_dir[g]
		|| !(newpos.x >= 0 && newpos.y >= 0));
	pocstate.ghost_pos[g] = newpos;
	pocstate.ghost_dir[g] = dir;
}

void Pocman::NewLevel(PocmanState& pocstate) const {
	pocstate.pocman_pos = pocman_home_;
	for (int g = 0; g < num_ghosts_; g++) {
		pocstate.ghost_pos[g] = ghost_home_;
		pocstate.ghost_pos[g].x += g % 2;
		pocstate.ghost_pos[g].y += g / 2;
		pocstate.ghost_dir[g] = -1;
	}

	pocstate.num_food = 0;
	for (int x = 0; x < maze_.xsize(); x++) {
		for (int y = 0; y < maze_.ysize(); y++) {
			int pocIndex = maze_.Index(x, y);
			// logi << maze_(x, y) << " " << CheckFlag(maze_(x, y), E_SEED) << " " << CheckFlag(maze_(x, y), E_POWER) << endl;
			if (CheckFlag(maze_(x, y), E_SEED)
				&& (CheckFlag(maze_(x, y), E_POWER)
					|| Random::RANDOM.NextDouble() < food_prob_)) {
				pocstate.food[pocIndex] = 1;
				pocstate.num_food++;
			} else {
				pocstate.food[pocIndex] = 0;
			}
		}
	}

	pocstate.power_steps = 0;
}

int Pocman::SeeGhost(const PocmanState& pocstate, ACT_TYPE action) const {
	Coord eyepos = pocstate.pocman_pos + Compass::DIRECTIONS[action];
	while (maze_.Inside(eyepos) && Passable(eyepos)) {
		for (int g = 0; g < num_ghosts_; g++)
			if (pocstate.ghost_pos[g] == eyepos)
				return g;
		eyepos += Compass::DIRECTIONS[action];
	}
	return -1;
}

bool Pocman::HearGhost(const PocmanState& pocstate) const {
	for (int g = 0; g < num_ghosts_; g++)
		if (Coord::ManhattanDistance(pocstate.ghost_pos[g], pocstate.pocman_pos)
			<= hear_range_)
			return true;
	return false;
}

bool Pocman::SmellFood(const PocmanState& pocstate) const {
	Coord smellPos;
	for (smellPos.x = -smell_range_; smellPos.x <= smell_range_; smellPos.x++)
		for (smellPos.y = -smell_range_; smellPos.y <= smell_range_; smellPos.y++)
			if (maze_.Inside(pocstate.pocman_pos + smellPos)
				&& pocstate.food[maze_.Index(pocstate.pocman_pos + smellPos)])
				return true;
	return false;
}

ScenarioUpperBound* Pocman::CreateScenarioUpperBound(string name,
	string particle_bound_name) const {
	if (name == "TRIVIAL") {
		return new TrivialParticleUpperBound(this);
	} else if (name == "APPROX") {
		return new PocmanApproxScenarioUpperBound(this);
	} else if (name == "SMART" || name == "DEFAULT") {
		return new PocmanSmartParticleUpperBound(this);
	} else {
		cerr << "Unsupported base upper bound: " << name << endl;
		exit(1);
		return NULL;
	}
}

ParticleLowerBound* Pocman::CreateParticleLowerBound(string name) const {
	if (name == "TRIVIAL") {
		return new TrivialParticleLowerBound(this);
	} else if (name == "LEGAL" || name == "DEFAULT") {
		return new PocmanLegalParticleLowerBound(this);
	} else {
		cerr << "Unsupported base lower bound: " << name << endl;
		exit(1);
		return NULL;
	}
}

ScenarioLowerBound* Pocman::CreateScenarioLowerBound(string name,
	string particle_bound_name) const {
	if (name == "TRIVIAL") {
		return new TrivialParticleLowerBound(this);
	} else if (name == "LEGAL") {
		return new PocmanLegalParticleLowerBound(this);
	} else if (name == "SMART" || name == "DEFAULT") {
		return new PocmanSmartPolicy(this,
			CreateParticleLowerBound(particle_bound_name));
	} else if (name == "RANDOM") {
		return new RandomPolicy(this,
			CreateParticleLowerBound(particle_bound_name));
	} else if (name == "RULES") {
		return new PocmanRulesDefaultPolicy(this,
			CreateParticleLowerBound(particle_bound_name));
	} else {
		cerr << "Unsupported lower bound algorithm: " << name << endl;
		exit(0);
		return NULL;
	}
}

POMCPPrior* Pocman::CreatePOMCPPrior(string name) const {
	if (name == "UNIFORM") {
		return new UniformPOMCPPrior(this);
	} else if (name == "DEFAULT" || name == "SMART") {
		return new PocmanPOMCPPrior(this);
	} else {
		cerr << "Unsupported POMCP prior: " << name << endl;
		exit(1);
		return NULL;
	}
}

void Pocman::PrintState(const State& state, ostream& ostr) const {
	const PocmanState& pocstate = static_cast<const PocmanState&>(state);
	ostr << endl;
	for (int x = 0; x < maze_.xsize() + 2; x++)
		ostr << "X ";
	ostr << endl;
	for (int y = maze_.ysize() - 1; y >= 0; y--) {
		if (y == passage_y_)
			ostr << "< ";
		else
			ostr << "X ";
		for (int x = 0; x < maze_.xsize(); x++) {
			Coord pos(x, y);
			int index = maze_.Index(pos);
			char c = ' ';
			if (!Passable(pos))
				c = 'X';
			if (pocstate.food[index])
				c = CheckFlag(maze_(x, y), E_POWER) ? '+' : '.';
			for (int g = 0; g < num_ghosts_; g++)
				if (pos == pocstate.ghost_pos[g])
					c = (
						pos == pocstate.pocman_pos ?
							'@' :
							(pocstate.power_steps == 0 ? 'A' + g : 'a' + g));
			if (pos == pocstate.pocman_pos)
				c = pocstate.power_steps > 0 ? '!' : '*';
			ostr << c << ' ';
		}
		if (y == passage_y_)
			ostr << ">" << endl;
		else
			ostr << "X" << endl;
	}
	for (int x = 0; x < maze_.xsize() + 2; x++)
		ostr << "X ";
	ostr << endl;
}

void Pocman::PrintObs(const State& state, OBS_TYPE observation,
	ostream& ostr) const {
	const PocmanState& pocstate = static_cast<const PocmanState&>(state);
	Grid<char> obs(maze_.xsize(), maze_.ysize());
	obs.SetAllValues(' ');

	// Pocman
	obs(pocstate.pocman_pos) = pocstate.power_steps > 0 ? '!' : '*';

	for (int d = 0; d < 4; d++) {
		// See ghost
		if (CheckFlag(observation, d)) {
			Coord eyepos = pocstate.pocman_pos + Compass::DIRECTIONS[d];
			while (maze_.Inside(eyepos) && Passable(eyepos)) {
				obs(eyepos) = (pocstate.power_steps == 0 ? 'A' : 'a');
				eyepos += Compass::DIRECTIONS[d];
			}
		}

		// Feel wall
		if (!CheckFlag(observation, d + 4)
			&& maze_.Inside(pocstate.pocman_pos + Compass::DIRECTIONS[d]))
			obs(pocstate.pocman_pos + Compass::DIRECTIONS[d]) = 'X';
	}

	// Hear ghost
	if (CheckFlag(observation, 9)) {
		Coord hearPos;
		for (hearPos.x = -hear_range_; hearPos.x <= hear_range_; hearPos.x++)
			for (hearPos.y = -hear_range_; hearPos.y <= hear_range_; hearPos.y++)
				if (Coord::ManhattanDistance(hearPos, Coord(0, 0)) <= hear_range_
					&& maze_.Inside(pocstate.pocman_pos + hearPos)
					&& obs(pocstate.pocman_pos + hearPos) == ' ')
					obs(pocstate.pocman_pos + hearPos) = (
						pocstate.power_steps == 0 ? 'A' : 'a');
	}

	// Smell food
	if (CheckFlag(observation, 8)) {
		Coord smellPos;
		for (smellPos.x = -smell_range_; smellPos.x <= smell_range_; smellPos.x++)
			for (smellPos.y = -smell_range_; smellPos.y <= smell_range_;
				smellPos.y++)
				if (maze_.Inside(pocstate.pocman_pos + smellPos)
					&& obs(pocstate.pocman_pos + smellPos) == ' ')
					obs(pocstate.pocman_pos + smellPos) = '.';
	}

	ostr << endl;
	for (int x = 0; x < maze_.xsize() + 2; x++)
		ostr << "# ";
	ostr << endl;
	for (int y = maze_.ysize() - 1; y >= 0; y--) {
		ostr << "# ";
		for (int x = 0; x < maze_.xsize(); x++)
			ostr << obs(x, y) << ' ';
		ostr << "#" << endl;
	}
	for (int x = 0; x < maze_.xsize() + 2; x++)
		ostr << "# ";
	ostr << endl;
}

void Pocman::PrintBelief(const Belief& belief, ostream& out) const {
	logi << "PRINTING BELIEF " << endl;
	Grid<int> counts(maze_.xsize(), maze_.ysize());
	counts.SetAllValues(0);
	vector<State*> particles = static_cast<const ParticleBelief&>(belief).particles();
	for (int i = 0; i < particles.size(); i++) {
		const PocmanState* pocstate = static_cast<const PocmanState*>(particles[i]);

		for (int g = 0; g < num_ghosts_; g++)
			counts(pocstate->ghost_pos[g])++;
	}

	for (int y = maze_.ysize() - 1; y >= 0; y --) {
		for (int x = 0; x < maze_.xsize(); x++) {
			logi.width(6);
			logi.precision(2);
			logi << fixed << (double) counts(x, y) / particles.size();
		}
		logi << endl;
	}
}

void Pocman::PrintAction(ACT_TYPE action, ostream& out) const {
	out << Compass::CompassString[action] << endl;
}

State* Pocman::Allocate(int state_id, double weight) const {
	PocmanState* state = memory_pool_.Allocate();
	state->state_id = state_id;
	state->weight = weight;
	return state;
}

State* Pocman::Copy(const State* particle) const {
	PocmanState* state = memory_pool_.Allocate();
	*state = *static_cast<const PocmanState*>(particle);
	state->SetAllocated();
	return state;
}

void Pocman::Free(State* particle) const {
	memory_pool_.Free(static_cast<PocmanState*>(particle));
}

int Pocman::NumActiveParticles() const {
	return memory_pool_.num_allocated();
}

} // namespace despot
