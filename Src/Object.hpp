#pragma once
#include "Vec2.hpp"
struct Object
{
	Vec2d pos;
	Vec2d size;
	Vec2d delta;
	//double dx, dy;
	Object() {}
	Object(double x, double y, double w, double h) {
		this->pos.x = x;
		this->pos.y = y;
		this->size.x = w;
		this->size.y = h;
	}
	Object(double x, double y, double w, double h, double dx, double dy) {
		this->pos.x = x;
		this->pos.y = y;
		this->size.x = w;
		this->size.y = h;
		this->delta.x = dx;
		this->delta.y = dy;
	}
};

struct Laser : Object
{
	double arc;
	Laser() {}
	Laser(double x, double y, double w, double h, double arc) : Object(x, y, w, h)
	{
		this->arc = arc;
	}
};

struct Player : Object
{
	Player() {}
	Player(double x, double y, double w, double h) : Object(x, y, w, h)
	{
		this->size.x = 2;
		this->size.y = 2;
	}
};