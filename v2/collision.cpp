#include "collision.h"


struct BoundDescription {
	// Used in sweep_and_prune
	double location;
	bool is_start;
	int particle_index;
};


bool operator< (const BoundDescription& b1, const BoundDescription& b2) {
	if (b1.location != b2.location) {
		return b1.location < b2.location;
	}

	// Handle corner case that an interval has 0 width
	// We don't want the end of an interval to occur before the beginning
	else {
		if (b1.is_start) {
			return true;
		}
		else {
			return false;
		}
	}
}


struct VectorHash {
	// For creating unordered_set<vector<int>>
	size_t operator()(const std::vector<int>& v) const {
		std::hash<int> hasher;
		size_t seed = 0;
		for (int i : v) {
			seed ^= hasher(i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
		return seed;
	}
};


unordered_set<vector<int>, VectorHash> sweep_and_prune(vector<vector<double>> bounds) {
	// Detect collisions between objects in 1-dimension using a sweep and prune algorithm
	// bounds is given by ((min_bound_1, max_bound_1), (min_bound_2, max_bound_2), ...)

	int n = bounds.size();
	vector<BoundDescription> bound_descriptions(n*2, BoundDescription{ 0, true, 0 });

	for (int i = 0; i < n; i++) {
		bound_descriptions[i*2] = BoundDescription{ bounds[i][0], true, i };
		bound_descriptions[i * 2 + 1] = BoundDescription{ bounds[i][1], false, i };
	}
	sort(bound_descriptions.begin(), bound_descriptions.end());

	unordered_set<vector<int>, VectorHash> potential_collisions{};
	unordered_set<int> active_intervals{};
	for (int i = 0; i < n; i++) {
		BoundDescription bd = bound_descriptions[i];
		if (bd.is_start) {
			for (unordered_set<int>::iterator itr = active_intervals.begin(); itr != active_intervals.end(); itr++) {
				potential_collisions.insert(vector<int>{*itr, bd.particle_index});
			}			
			active_intervals.insert(bd.particle_index);

		}
		else {
			active_intervals.erase(bd.particle_index);
		}
	}

	return potential_collisions;
}


bool check_collision(Particle* p1, Particle* p2) {
	// Check if two particles actually collide
	double d = dist(p1->get_r(), p2->get_r());
	return d < p1->get_radius() + p2->get_radius();
}


vector<int> find_colliding_pair(vector<Particle*> particles) {
	// Searches for collisions between particles. Stops when the first colliding pair is found.
	// Returns vector<int>{0,0} if there are no collisions.
	// The algorithm first checks if particles are *potentially intersecting*.
	// Two particles potentially intersect iff their bounding cubes intersect.
	// Then, the positions and radii are inspected to check if they actaully collide.

	vector<vector<double>> x_bounds(particles.size(), vector<double>{0, 0});
	vector<vector<double>> y_bounds(particles.size(), vector<double>{0, 0});
	vector<vector<double>> z_bounds(particles.size(), vector<double>{0, 0});

	for (int i = 0; i < particles.size(); i++) {
		Particle* p = particles[i];
		x_bounds[i] = vector<double>{ p->get_r()[0] - p->get_radius(), p->get_r()[0] + p->get_radius() };
		y_bounds[i] = vector<double>{ p->get_r()[1] - p->get_radius(), p->get_r()[1] + p->get_radius() };
		z_bounds[i] = vector<double>{ p->get_r()[2] - p->get_radius(), p->get_r()[2] + p->get_radius() };
	}

	unordered_set<vector<int>, VectorHash> x_collisions = sweep_and_prune(x_bounds);
	unordered_set<vector<int>, VectorHash> y_collisions = sweep_and_prune(y_bounds);
	unordered_set<vector<int>, VectorHash> z_collisions = sweep_and_prune(z_bounds);

	for (unordered_set<vector<int>>::iterator itr = x_collisions.begin(); itr != x_collisions.end(); itr++) {
		vector<int> pair = *itr;
		if (y_collisions.find(pair) != y_collisions.end() || y_collisions.find(vector<int> {pair[1], pair[0]}) != y_collisions.end()) {
			if (z_collisions.find(pair) != z_collisions.end() || z_collisions.find(vector<int> {pair[1], pair[0]}) != z_collisions.end()) {
				if (check_collision(particles[pair[0]], particles[pair[1]])) {
					return pair;
				}
			}
		}
	}
	return vector<int>{0, 0};  // If no collisions
}


void do_collisions(vector<Particle*>& particles) {
	// While there are still colliding particles
	for (int i = 0; i < p::max_collision_checks; i++) {
		vector<int> pair = find_colliding_pair(particles);
		if (pair == vector<int>{0, 0}) {
			return;
		}

		sort(pair.begin(), pair.end());
		Particle* p1 = particles[pair[0]];
		Particle* p2 = particles[pair[1]];

		Particle* p = new Particle{ p1,p2 };

		delete p1;
		delete p2;

		particles.erase(particles.begin() + pair[1]);
		particles.erase(particles.begin() + pair[0]);

		particles.push_back(p);

		cout << "A collision occured. " << particles.size() << " particles remain.\n";
	}
}
