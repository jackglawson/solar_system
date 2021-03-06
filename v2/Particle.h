#pragma once

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <vector>
#include <iostream>
#include "params.h"
using namespace std;

#ifdef _DEBUG
#define new new( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#else
#define new new
#endif


class Particle
{
private:
	vector<double> r;
	vector<double> v;
	double m;
	int radius_type;			// 0 ... fixed radius

public:
	Particle();
	Particle(vector<double> r_in, vector<double> v_in, double m_in, int radius_type_in);
	Particle(Particle* p1, Particle* p2);			// collide two particles
	~Particle();

	vector<double> get_r();
	vector<double> get_v();
	double get_m();
	int get_radius_type();
	double get_radius();

	void set_r(vector<double> r);
	void set_v(vector<double> v);

	void print();
};

