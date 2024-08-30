#pragma once

#include "../Math/Numerals.h"
#include "SystemBase.h"
#include "ResourceManagerSystem.h"

class b2World;
struct b2Vec2;

namespace LKT
{
	constexpr float TIME_STEP = 1 / 60.0f;
	constexpr int32 VELOCITY_ITERATIONS = 6;
	constexpr int32 POSITIONS_ITERATIONS = 2;

	class Physics2DSystem : public SystemBase
	{
		struct Physics2DComponentData
		{
			int32 instancesCount;
			int32 allocatedInstances;

			void* buffer;

			b2Vec2* position;
			b2Vec2* linearVelocity;

			float* angle;
			float* angularVelocity;
			float* linearDamping;
			float* angularDamping;
			float* gravityScale;

			float* friction;
			float* restitution;
			float* restitutionThreshold;
			float* density;

			char* allowSleed;
			char* awake;
			char* fixedRotation;
			char* bullet;
			char* bodyType;
			char* enabled;

			char* shape;
			char* isSensor;
		};

		Physics2DComponentData physicsComponentData;

	public:
		~Physics2DSystem();
		Physics2DSystem();

		virtual uint32 CreateComponent(const Entity& e, void* componentData) override;

		void Process(float deltaTime);

	private:
		void ProcessPhysics2D(float deltaTime);

		float accumulator = 0.0f;
		b2World* physicsWorld = nullptr;
	};
}