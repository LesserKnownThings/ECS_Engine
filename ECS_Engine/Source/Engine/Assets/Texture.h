#pragma once

#include "Systems/AssetManager/Asset.h"

#include <cstdint>
#include <string>

namespace LKT
{
	/**
	 * Currently only supporting raw texture form, this is not very good as it takes too much space.
	 * I'll look into adding compressed texture support with: DXT/BC1-BC7, ASTC, or ETC.
	 */

	class Texture : public Asset
	{
	public:
		uint32_t GetTextureID() const { return textureID; }

		int32_t GetWidth() const { return width; }
		int32_t GetHeight() const { return height; }

		void UnloadAsset() override;

	protected:
		void LoadAsset(void *buffer) override;

	private:
		int32_t width = 0;
		int32_t height = 0;
		int32_t components = 0;

		uint32_t textureID = 0;
	};
}