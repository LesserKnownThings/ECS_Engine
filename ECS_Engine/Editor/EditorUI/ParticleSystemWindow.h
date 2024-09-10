#pragma once

#include "EditorUI/AssetViewerWindow.h"

namespace LKT
{
	class ParticleSystem;

	class ParticleSystemWindow : public AssetViewerWindow
	{
	public:
		~ParticleSystemWindow();
		ParticleSystemWindow(LazyAssetPtr<Asset> &inAsset);

	protected:
		void RenderContent() override;
		void RenderAsset() override;

	private:
		void SimulateAsset(float deltaTime);

		ParticleSystem *ps;

		ImVec2 childSize = ImVec2(200, 200);
	};
}