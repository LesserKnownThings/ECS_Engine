#pragma once

#include "Systems/AssetManager/Asset.h"

#include <cstdint>
#include <string>

namespace LKT
{
	class Texture : public Asset
	{
	public:
		uint32_t GetTextureID() const { return textureID; }

		int32_t GetWidth() const { return width; }
		int32_t GetHeight() const { return height; }

	protected:
		void LoadAsset(const AssetPath &inPath) override;

	private:
		int32_t width = 0;
		int32_t height = 0;

		uint32_t textureID = 0;
	};
}