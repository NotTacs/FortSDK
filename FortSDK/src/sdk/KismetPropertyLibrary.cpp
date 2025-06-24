#include "pch.h"
#include "../../include/SDK.h"

int SDK::FKismetPropertyManager::GetOffset() {
        if ( !Prop )
                return 0;
        if ( UEChanges::bUsesFPropertySystem ) {
                if ( UEChanges::bUsesUE5FProp ) {
                        SDK::FProperty500 *Prop = static_cast<SDK::FProperty500*>(this->Prop);
                        return Prop->Offset_Internal();
                }
                SDK::FProperty *Prop =
                    static_cast<SDK::FProperty *>( this->Prop );
                return Prop->Offset_Internal;
        } else {
                SDK::UProperty *Prop =
                    static_cast<SDK::UProperty *>( this->Prop );
                return Prop->Offset_Internal;
        }

        return 0;
}

SDK::FPropertyInfo SDK::FKismetPropertyLibrary::GetPropertyByName(const std::string& ClassName,
    const std::string& PropName)
{
        auto Found = std::find_if(AllPropertyInfo.begin(), AllPropertyInfo.end(), [&](const FPropertyInfo& PropInfo ) {
                    return PropInfo.ClassName == ClassName &&
                           PropInfo.PropName == PropName;
        } );


        if (Found != AllPropertyInfo.end()) {
                return *Found;
        } else {
                UE_LOG( LogFortSDK, VeryVerbose,
                        "Failed to get Prop by Pred" );
        }

        FPropertyInfo Info{};
        Info.ClassName = ClassName;
        Info.PropName = PropName;
        Info.Offset = -1;

        void *result = nullptr;
        UClass *Class = StaticClassImpl( ClassName.c_str() );


        if ( UEChanges::bUsesFPropertySystem ) {
                if ( UEChanges::bUsesUE5FProp ) {
                        SDK::FProperty500 *Prop = nullptr;
                        Class->GetPropertyLink( &Prop );
                        if ( !Prop ) {
                                UE_LOG( LogFortSDK, Warning,
                                        "Failed to get valid PropertyLink from "
                                        "class: %s",
                                        ClassName.c_str() );
                                return Info;
                        }
                        for ( Prop; Prop = Prop->PropertyLinkNext; ) {
                                if ( Prop->NamePrivate.ToString().ToString() ==
                                     PropName ) {
                                        result = Prop;
                                        break;
                                }
                        }
                } else {
                        FProperty *Prop = nullptr;
                        Class->GetPropertyLink( &Prop );
                        if ( !Prop ) {
                                UE_LOG( LogFortSDK, Warning,
                                        "Failed to get valid PropertyLink from "
                                        "class: %s",
                                        ClassName.c_str() );
                                return Info;
                        }
                        for ( Prop; Prop = Prop->PropertyLinkNext; ) {
                                if ( Prop->NamePrivate.ToString().ToString() ==
                                     PropName ) {
                                        result = Prop;
                                        break;
                                }
                        }
                }
        } else {
                UProperty *Prop = nullptr;
                Class->GetPropertyLink( &Prop );
                if ( !Prop ) {
                        UE_LOG(
                            LogFortSDK, Warning,
                            "Failed to get valid PropertyLink from class: %s",
                            ClassName.c_str() );
                        return Info;
                }
                for ( Prop; Prop = Prop->PropertyLinkNext; ) {
                        if ( Prop->GetFName().ToString().ToString() ==
                             PropName ) {
                                result = Prop;
                                break;
                        }
                }
        }

        if ( !result ) {
                UE_LOG( LogFortSDK, VeryVerbose,
                        "Failed to find %s from class %s with PropertyLink",
                        PropName.c_str(), ClassName.c_str() );
        }

        const static bool bUseChildProperties =
            SDK::Offsets::Members::UStruct__ChildProperties != -1;
        if ( bUseChildProperties ) /*ChildrenProperties are nice because all of
                                      these are properties*/
        {
                for ( FField *Next = Class->GetChildrenProperties(); Next;
                      Next = Next->Next ) {
                        FProperty *NextProp = reinterpret_cast<FProperty *>(
                            Next ); /*Should be FProperty on the same build that
                                       has ChildrenProperties im pretty sure*/

                        if ( NextProp->NamePrivate.ToString().ToString() ==
                             PropName ) {
                                result = NextProp;
                                break;
                        }
                }
        } else {
                /*I think its safe to only use UProperty here, but we should
                 * check this at some point*/
                for ( UField *Next = Class->GetChildren(); Next;
                      Next = Next->Next ) {
                        if ( !Next->GetClass() )
                                continue;
                        if ( Next->IsA<UFunction>() )
                                continue;
                        UProperty *NextProp =
                            reinterpret_cast<UProperty *>( Next );
                        if ( NextProp->GetFName().ToString().ToString() ==
                             PropName ) {
                                result = NextProp;
                                break;
                        }
                }
        }

        if ( !result ) {
                UE_LOG( LogFortSDK, Fatal,
                        "Failed to find %s from class %s entirely.",
                        PropName.c_str(), ClassName.c_str() );
                return Info;
        }

        Info.Prop = result;
        std::unique_ptr<FKismetPropertyManager> Manager =
            std::make_unique<FKismetPropertyManager>( Info.Prop );

        Info.Offset = Manager->GetOffset();

        AllPropertyInfo.push_back( Info );
        return Info;
}

SDK::FFunctionInfo SDK::FKismetPropertyLibrary::GetFunctionByName(const std::string& ClassName,
    const std::string& FunctionName)
{
        auto Found = std::find_if(AllFunctionInfo.begin(), AllFunctionInfo.end(), [&]( const FFunctionInfo& FunctionInfo ) {
                    return FunctionInfo.ClassName ==
                               ClassName && FunctionInfo.Func->GetFName()
                                                    .ToString()
                                                    .ToString() == FunctionName;
        });

        if ( Found != AllFunctionInfo.end() ) {
                return *Found;
        }

        FFunctionInfo FunctionInfo{};
        FunctionInfo.ClassName = ClassName;

        UFunction *result = nullptr;
        UClass *Class = StaticClassImpl( ClassName.c_str() );
        for (UField* Next = Class->GetChildren(); Next; Next = Next->Next)
        {
                if ( !Next->IsA<UFunction>() )
                        continue;
                if ( Next->GetFName().ToString().ToString() == FunctionName ) {
                        result = reinterpret_cast<UFunction *>( Next );
                        break;
                }
        }
        if ( !result ) {
                UE_LOG( LogFortSDK, VeryVerbose,
                        "Failed to find function %s from class %s.",
                        FunctionName.c_str(), ClassName.c_str() );
                return FunctionInfo;
        }

        FunctionInfo.Func = result;

        AllFunctionInfo.push_back( FunctionInfo );
        return FunctionInfo;
}