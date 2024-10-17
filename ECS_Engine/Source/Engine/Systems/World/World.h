#pragma once

#include "BuildMacros.h"
#include "Object.h"
#include "Systems/AssetManager/LazyAssetPtr.h"
#include "Systems/Physics/Rect2D.h"

#include <vector>

namespace LKT
{
	class Asset;
	class Camera;

	class World : public Object
	{
	public:
		virtual ~World();

		virtual void Initialize();
		virtual void Uninitialize();

	protected:
		Camera *camera = nullptr;
	};
}