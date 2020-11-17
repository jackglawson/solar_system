#pragma once

#include <vector>
#include <iostream>
#include "params.h"
#include "rk4_step.h"
using namespace std;

vector<Particle> walk_to(double t_from, double t_to, vector<Particle> y0);
