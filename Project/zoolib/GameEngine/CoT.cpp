#include "zoolib/GameEngine/CoT.h"

#include "zoolib/GameEngine/Cog_Toon.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
// MARK: - sCoT

// CoT == Cog or Toon.

// Attempts to instantiate a Cog from the Val, and if that fails it attempts to
// instantiate a Toon wrapped in a sCog_Toon.

Cog sCoT(const ZQ<Val>& iVal)
	{
	if (Cog theCog = sCog(iVal))
		return theCog;

	return sCog_Toon(sToon(iVal));
	}

} // namespace GameEngine
} // namespace ZooLib
