#include"../game_headers/Surface.h"

Surface Surface::surfaces[5]
{
	Surface(0.3f, 0.3f, 0.95f, 1.0f, 3.5f, Surfaces::TeeBox),
	Surface(0.0f, 0.2f, 0.95f, 1.0f, 3.0f, Surfaces::Green),
	Surface(0.3f, 0.2f, 0.95f, 1.0f, 3.5f, Surfaces::Fairway),
	Surface(0.5f, 0.2f, 0.9f, 0.9f, 4.0f, Surfaces::Rough),
	Surface(0.0f, 0.7f, 0.99f, 1.0f, 3.0f, Surfaces::Cartpath),
};

Surface::Surface(float bounceAngle, float energyAbsorbtion, float rollFriction, float swingSpeedFactor, float stoppingSpeed, Surfaces type)
{
	Surface::type = type;
	Surface::bounceAngle = bounceAngle;
	Surface::firstBounceEnergyAbsorbtion = energyAbsorbtion;
	Surface::rollFriction = rollFriction;
	Surface::swingSpeedFactor = swingSpeedFactor;
	Surface::stoppingSpeed = stoppingSpeed;
}