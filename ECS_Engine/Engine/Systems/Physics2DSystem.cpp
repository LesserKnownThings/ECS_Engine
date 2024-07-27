#include "Physics2DSystem.h"
#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"

Physics2DSystem::~Physics2DSystem()
{
}

Physics2DSystem::Physics2DSystem()
{
	physicsWorld = new b2World(b2Vec2(0, -10));

}

uint32 Physics2DSystem::CreateComponent(const Entity& e, void* componentData)
{
	return uint32();
}

void Physics2DSystem::Process(float deltaTime)
{
	ProcessPhysics2D(deltaTime);
}

void Physics2DSystem::ProcessPhysics2D(float deltaTime)
{
	accumulator += deltaTime;

	while (accumulator >= TIME_STEP)
	{
		physicsWorld->Step(TIME_STEP, VELOCITY_ITERATIONS, POSITIONS_ITERATIONS);
		accumulator -= TIME_STEP;
	}
}
