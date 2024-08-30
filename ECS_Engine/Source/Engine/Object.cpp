#include "Object.h"

LKT::Object::~Object()
{
	id.Reset();
}

LKT::Object::Object()
	: id(IDManager::GenerateGUID())
{
}

void LKT::Object::OverrideID(const UniqueID& inID)
{
	id = inID;
}
