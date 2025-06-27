#include "pch.h"
#include "../../include/SDK.h"

std::string SDK::FEngineVersion::ToString() const {
        std::string TempString =
            std::format( "{}.{}.{}", Major, Minor, Patch );
        return TempString;
}
std::string SDK::FFortniteVersion::ToString() const {
        std::string TempString = std::format( "{}.{}.{}", Major, Minor, Patch );
        return TempString;
}