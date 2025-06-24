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
#include <unordered_set>
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

#include "sdk/Engine_classes.h"

template <class T>
T& SDK::UObject::Get( const std::string &ClassName,
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
                else {
                        T Variable = T{};
                        return Variable;
                }

        }
}

static std::vector<std::string> GetParamNames( SDK::UFunction *fn ) {
        std::vector<std::string> ParamNames;
        std::unordered_set<std::string> Seen;

        auto TryAdd = [&]( const std::string &name ) {
                if ( Seen.insert( name ).second )
                        ParamNames.push_back( name );
        };

        if ( SDK::UEChanges::bUsesFPropertySystem ) {
                if ( SDK::UEChanges::bUsesUE5FProp ) {
                        SDK::FProperty500 *Prop = nullptr;
                        fn->GetPropertyLink( &Prop );
                        for ( ; Prop; Prop = Prop->PropertyLinkNext ) {
                                TryAdd(
                                    Prop->NamePrivate.ToString().ToString() );
                        }
                } else {
                        SDK::FProperty *Prop = nullptr;
                        fn->GetPropertyLink( &Prop );
                        for ( ; Prop; Prop = Prop->PropertyLinkNext ) {
                                TryAdd(
                                    Prop->NamePrivate.ToString().ToString() );
                        }
                }
        } else {
                SDK::UProperty *Prop = nullptr;
                fn->GetPropertyLink( &Prop );
                for ( ; Prop; Prop = Prop->PropertyLinkNext ) {
                        TryAdd( Prop->GetFName().ToString().ToString() );
                }
        }

        const static bool bUseChildProperties =
            SDK::Offsets::Members::UStruct__ChildProperties != -1;

        if ( bUseChildProperties ) {
                for ( SDK::FField *Next = fn->GetChildrenProperties(); Next;
                      Next = Next->Next ) {
                        SDK::FProperty *NextProp =
                            reinterpret_cast<SDK::FProperty *>( Next );
                        TryAdd( NextProp->NamePrivate.ToString().ToString() );
                }
        } else {
                for ( SDK::UField *Next = fn->GetChildren(); Next;
                      Next = Next->Next ) {
                        if ( !Next->GetClass() || Next->IsA<SDK::UFunction>() )
                                continue;

                        SDK::UProperty *NextProp =
                            reinterpret_cast<SDK::UProperty *>( Next );
                        TryAdd( NextProp->GetFName().ToString().ToString() );
                }
        }

        return ParamNames;
}

template <typename Ret, typename T, typename... Args, size_t... Is>
Ret CallFunc_Impl( T *Obj, const std::string &className,
                   const std::string &functionName,
                   std::tuple<Args...> &argsTuple,
                   std::index_sequence<Is...> ) {

        SDK::UFunction *fn =
            SDK::PropLibrary->GetFunctionByName( className, functionName ).Func;
        if ( !fn ) {
                UE_LOG( LogFortSDK, VeryVerbose, "fn not found" );
                if constexpr ( !std::is_void_v<Ret> )
                        return Ret{};
                else
                        return;
        }

        const size_t paramSize = fn->GetSize();
        uint8_t *buffer = (uint8_t *)malloc( paramSize );
        std::memset( buffer, 0, paramSize );

        auto Params = GetParamNames( fn );

        for ( auto &Param : Params ) {
                UE_LOG( LogFortSDK, VeryVerbose, "Param: %s", Param.c_str() );
        }
        size_t i = 0;
        (
            [&] {
                    if ( i >= Params.size() ) {
                            UE_LOG( LogFortSDK, Warning,
                                    "Too many arguments for function %s::%s",
                                    className.c_str(), functionName.c_str() );
                            return;
                    }

                    const std::string &argName = Params[i];
                    const auto &propInfo = SDK::PropLibrary->GetPropertyByName(
                        functionName, argName );
                    if ( propInfo.Offset != -1 ) {
                            const auto &arg = std::get<Is>( argsTuple );
                            std::memcpy( buffer + propInfo.Offset, &arg,
                                         sizeof( arg ) );
                            UE_LOG( LogFortSDK, VeryVerbose,
                                    "Arg[%zu] %s -> offset %d, size %zu", i,
                                    argName.c_str(), propInfo.Offset,
                                    sizeof( arg ) );
                    } else {
                            UE_LOG(
                                LogFortSDK, Warning,
                                "Failed to find property for arg %s in %s::%s",
                                argName.c_str(), className.c_str(),
                                functionName.c_str() );
                    }
                    ++i;
            }(),
            ... );

        auto Flgs = fn->FunctionFlags();
        fn->FunctionFlags() |= 0x400;
        if ( Obj )
                Obj->ProcessEvent( fn, buffer );
        fn->FunctionFlags() = Flgs;

        if constexpr ( !std::is_void_v<Ret> ) {
                const auto &retProp = SDK::PropLibrary->GetPropertyByName(
                    functionName, "ReturnValue" );
                Ret result{};
                if ( retProp.Offset != -1 ) {
                        std::memcpy( &result, buffer + retProp.Offset,
                                     sizeof( Ret ) );
                        UE_LOG( LogFortSDK, VeryVerbose,
                                "ReturnValue copied from offset %d",
                                retProp.Offset );
                } else {
                        UE_LOG(
                            LogFortSDK, Warning,
                            "Failed to find ReturnValue property for %s::%s",
                            className.c_str(), functionName.c_str() );
                }
                return result;
        }
}



template <typename Ret, typename... Args>
Ret SDK::UObject::CallFunc(const std::string& ClassName,
    const std::string& FunctionName, Args &&...args) {
        auto argsTuple = std::make_tuple( std::forward<Args>( args )... );
        return CallFunc_Impl<Ret>( this, ClassName, FunctionName, argsTuple,
                                   std::index_sequence_for<Args...>{} );
}

template <typename T>
void SDK::UObject::Set(const std::string& ClassName,
    const std::string& PropName, const T& Value) {
        SDK::FPropertyInfo PropInfo =
            PropLibrary->GetPropertyByName( ClassName, PropName );

        if ( !this ) {
                return;
        }

        if ( PropInfo.Offset != -1 ) {
                UE_LOG( LogGetterSetter, VeryVerbose, "Found: %s, %d",
                        PropName.c_str(), PropInfo.Offset );
                *reinterpret_cast<T *>(
                    reinterpret_cast<uintptr_t>( this ) + PropInfo.Offset ) = Value;
        } else {
                UE_LOG( LogGetterSetter, Warning, "Failed to get %s from class %s",
                        PropName.c_str(), ClassName.c_str() );

                return;
        }
}