#include "pch.h"
#include "../../include/SDK.h"

SDK::FString SDK::FEngineVersion::ToString() const {
        std::string TempString =
            std::format( "{}.{}.{}", Major, Minor, Patch );
        return FString( TempString );
}
SDK::FString SDK::FFortniteVersion::ToString() const {
        std::string TempString = std::format( "{}.{}.{}", Major, Minor, Patch );
        return FString( TempString );
}