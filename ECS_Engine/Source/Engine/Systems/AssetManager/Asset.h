#pragma once

#include "Object.h"

#include <string>

//TODO

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
	class Asset : public Object
	{
	public:
		Asset() = default;
		virtual ~Asset() = default;

		const std::string& GetName() const { return name; }

	protected:
		virtual void LoadAsset(const std::string& inPath);

		virtual void Serialize();
		virtual void Deserialize();

		std::string name;
		std::string path;

		friend class AssetFactory;
	};
}