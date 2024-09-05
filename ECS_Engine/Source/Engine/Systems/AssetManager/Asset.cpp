#include "Asset.h"

namespace LKT
{
	void Asset::LoadAsset(const AssetPath& inPath)
	{
		path = inPath;
	}

	bool Asset::Serialize(std::ostream &outStream) const
	{
		outStream.write(reinterpret_cast<const char *>(&path), sizeof(path));

		return true;
	}

	bool Asset::Deserialize(std::ifstream &inStream)
	{
		inStream.read(reinterpret_cast<char *>(&path), sizeof(path));

		return true;
	}
}