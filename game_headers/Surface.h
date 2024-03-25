#ifndef SURFACE_CLASS_H
#define SURFACE_CLASS_H

#include"../engine_headers/Behavior.h"
#include"../engine_headers/GameObject.h"

class Surface : public Behavior
{
public:
	enum Surfaces
	{
		TeeBox,
		Green,
		Fairway,
		Rough,
		Cartpath
	};

	static Surface surfaces[5];

	Surface(float bounceAngle, float energyAbsorbtion, float rollFriction, float swingSpeedFactor, float stoppingSpeed, Surfaces type);

	Surfaces type;
	float bounceAngle; //1 = vertical bounce, 0 = symmetrical bounce
	float firstBounceEnergyAbsorbtion; //friction
	float rollFriction;
	float swingSpeedFactor;
	float stoppingSpeed;
};

#endif

