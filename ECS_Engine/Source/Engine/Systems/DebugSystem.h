#pragma once

#include <string>

namespace LKT
{
	enum class DebugLevel
	{
		Log,
		Warning,
		Error
	};

	//TODO
	/// <summary>
	/// I really like how unreal separates its logs. It has a verbosity level and a log name. I'm gonna leave this reminder here so that I make the same system in the future where we can have a macro to have a custom log name so I can better point out issues.
	/// </summary>
	class DebugSystem
	{
	public:
		DebugSystem() = default;

		static void Log(DebugLevel level, const std::string& message);
	};
}