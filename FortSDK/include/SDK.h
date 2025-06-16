#pragma once
#include <Windows.h>
#include <iostream>
#include <thread>
#include <string>
#include <cstring>
#include <memory>
#include <vector>
#include <map>
#include <mutex>
#include <unordered_map>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <type_traits>
#include <algorithm>
#include <Psapi.h>
#include <time.h>
#include <codecvt>
#include <locale>
#include <chrono>
#include "sdk/memcury.h"
#include "sdk/Offsets.h"
#include "sdk/KismetMemoryLibrary.h"
#include "sdk/KismetLogLibrary.h"
#include "sdk/Memory.h"
#include "sdk/Math.h"
#include "sdk/Version.h"
#include "UnrealContainers.h"
#include "sdk/CoreObject_classes.h"
#include "sdk/KismetPropertyLibrary.h"

namespace SDK 
{

bool Init();

extern FUObjectArray GUObjectArray;
extern std::unique_ptr<FKismetPropertyLibrary> PropLibrary;
extern FEngineVersion Engine_Version;
extern FFortniteVersion Fortnite_Version;

}

template <class T>
T SDK::UObject::Get( const std::string &ClassName,
                     const std::string &PropName ) {
        SDK::FPropertyInfo PropInfo =
            PropLibrary->GetPropertyByName( ClassName,
                                                           PropName );

        if ( PropInfo.Offset != -1 ) {
                return *reinterpret_cast<T *>(
                    reinterpret_cast<uintptr_t>( this ) + PropInfo.Offset );
        } else {
                UE_LOG( LogGetterSetter, Log, "Failed to get %s from class %s",
                        PropName.c_str(), ClassName.c_str() );

                if constexpr ( std::is_pointer<T>::value )
                        return nullptr;
                else
                        return T{};
        }
}