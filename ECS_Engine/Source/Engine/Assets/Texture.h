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

	protected:
		void LoadAsset(const std::string& inPath) override;

	private:
		int32_t width = 0;
		int32_t height = 0;

		uint32_t textureID = 0;
	};
}