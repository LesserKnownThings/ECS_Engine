#pragma once

#include "Systems/AssetManager/AssetPath.h"
#include "ISerializable.h"
#include "Object.h"

#include <string>

// TODO

/// <summary>
/// The serialization is not exactly working right now.
/// This is how it's supposed to work:
/// 1. Clang ASTMatchers will find all the annotations in the headers and will add them to the metadata
/// 2. The engine reads all the metadata and deserializes the data into the engine
///
/// For now, the functions are virtual so custom systems will have their own serialization.
/// </summary>

namespace LKT
{
	class Asset : public Object, public ISerializable
	{
	public:
		Asset() = default;
		virtual ~Asset() = default;

	protected:
		virtual void LoadAsset(const AssetPath& inPath);

		virtual bool Serialize(std::ostream &outStream) const override;
		virtual bool Deserialize(std::ifstream &inStream) override;

		AssetPath path;
		friend class AssetFactory;
	};
}