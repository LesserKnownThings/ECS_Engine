#pragma once

#include "AssetViewerWindow.h"

namespace LKT
{
	class ParticleSystem;

	class ParticleSystemAssetViewer : public AssetViewerWindow
	{
	public:
		~ParticleSystemAssetViewer();
		ParticleSystemAssetViewer(LazyAssetPtr<Asset> &inAsset);

	protected:
		void RenderContent() override;
		void RenderAssetContent() override;

	private:
		void SimulateAsset(float deltaTime);

		ParticleSystem *ps;

		ImVec2 childSize = ImVec2(200, 200);
	};
}