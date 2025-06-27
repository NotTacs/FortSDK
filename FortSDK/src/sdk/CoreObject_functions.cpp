#include "pch.h"
#include "../../include/SDK.h"

bool SDK::UObjectBase::IsValidLowLevelFast(bool bRecursive) const {
        // As DEFAULT_ALIGNMENT is defined to 0 now, I changed that
        // to the original numerical value here
        const int32 AlignmentCheck = MIN_ALIGNMENT - 1;

        // Check 'this' pointer before trying to access any of the
        // Object's members
        if ( ( this == nullptr ) || *(int*)this < 0x100 ) {
                UE_LOG_W( LogUObjectBase, Error,
                          TEXT( "\'this\' pointer is invalid." ) );
                return false;
        }
        if ( *(int*)this & AlignmentCheck ) {
                UE_LOG_W( LogUObjectBase, Error,
                          TEXT( "\'this\' pointer is misaligned." ) );
                return false;
        }
        if ( *(void **)this == nullptr ) {
                UE_LOG_W( LogUObjectBase, Error,
                          TEXT( "Virtual functions table is invalid." ) );
                return false;
        }

        
        // These should all be 0.
        const int CheckZero = ( ObjectFlags & ~RF_AllFlags ) |
                              ( *(int*)ClassPrivate & AlignmentCheck ) |
                              ( *(int*)OuterPrivate & AlignmentCheck );
        if ( !!CheckZero ) {
                UE_LOG_W( LogUObjectBase, Error,
                          TEXT( "Object flags are invalid or either "
                                "Class or Outer is misaligned" ) );
                return false;
        }
        // Avoid infinite recursion so call IsValidLowLevelFast on
        // the class object with bRecirsive = false.
        if ( bRecursive && !ClassPrivate->IsValidLowLevelFast( false ) ) {
                UE_LOG_W( LogUObjectBase, Error,
                          TEXT( "Class object failed "
                                "IsValidLowLevelFast test." ) );
                return false;
        }
        // Lightweight versions of index checks.
        if ( InternalIndex <= 0 ) {
                UE_LOG_W( LogUObjectBase, Error,
                          TEXT( "Object array index or name index is "
                                "invalid." ) );
                return false;
        }
        return true;
}

SDK::FString SDK::FName::ToString() const { 
    static FString& ( *InternalToString )( const FName *, FString & ) =
            decltype( InternalToString )( SDK::Offsets::FName__ToString );
    
    FString Ret = InternalToString( this, Ret );
    return Ret;
}

SDK::FNativeFuncPtr SDK::UFunction::Func() const {
        return *reinterpret_cast<SDK::FNativeFuncPtr *>(
            __int64( this ) + SDK::Offsets::Members::UFunction__Exec );
}

void SDK::UFunction::SetNativeFunc(FNativeFuncPtr InFunc) {
        *reinterpret_cast<SDK::FNativeFuncPtr *>(
            __int64( this ) + SDK::Offsets::Members::UFunction__Exec ) = InFunc;
}

MEMBER_PTR( SDK::UStruct *, SDK::UStruct::SuperStruct,
            SDK::Offsets::Members::UStruct__SuperStruct );

MEMBER_PTR( SDK::UField *, SDK::UStruct::Children,
            SDK::Offsets::Members::UStruct__Children );

MEMBER_PTR( SDK::FField *, SDK::UStruct::ChildrenProperties,
            SDK::Offsets::Members::UStruct__ChildProperties );

MEMBER_PTR( SDK::int32, SDK::UStruct::Size,
            SDK::Offsets::Members::UStruct__Size );
MEMBER_PTR( SDK::int32, SDK::UStruct::MinAlignment,
            SDK::Offsets::Members::UStruct__MinAlignment );
SDK::TArray<SDK::uint8> &SDK::UStruct::Script() const {
        return *reinterpret_cast<SDK::TArray<SDK::uint8> *>(
            reinterpret_cast<uintptr_t>( this ) +
            SDK::Offsets::Members::UStruct__Script );
};
MEMBER_PTR( void *, SDK::UStruct::PropertyLink,
            SDK::Offsets::Members::UStruct__PropertyLink );
MEMBER_PTR( void *, SDK::UStruct::RefLink,
            SDK::Offsets::Members::UStruct__RefLink );
MEMBER_PTR( void *, SDK::UStruct::DestructorLink,
            SDK::Offsets::Members::UStruct__DestructorLink );
MEMBER_PTR( void *, SDK::UStruct::PostConstructorLink,
            SDK::Offsets::Members::UStruct__PostConstructorLink );


SDK::UObject* SDK::UClass::GetClassDefaultObject() const {
        std::string Name = "Default__" + this->GetFName().ToString().ToString();
        return reinterpret_cast<SDK::UObject*>(
            GUObjectArray.FindObject( Name ));
}

bool SDK::UStruct::IsChildOf(const UStruct* SomeBase) const
{
        if ( SomeBase == nullptr ) {
                return false;
        }

        bool bOldResult = false;
        for ( const UStruct *TempStruct = this; TempStruct;
              TempStruct = TempStruct->GetSuperStruct() ) {
                if ( TempStruct == SomeBase ) {
                        bOldResult = true;
                        break;
                }
        }

        return bOldResult;
}

SDK::UClass* SDK::StaticClassImpl(const char* ClassName) {
        static std::unordered_map<std::string, SDK::UClass *> ClassCache;
        auto It = ClassCache.find( ClassName );
        if ( It != ClassCache.end() ) {
                return It->second;
        }
        auto Class = reinterpret_cast<SDK::UClass *>(
            SDK::GUObjectArray.FindObject( ClassName ) );
        if ( !Class ) {
                UE_LOG( LogFortSDK, Warning, "Failed to find class By Name: %s",
                        ClassName );
                return nullptr;
        }
        ClassCache[ClassName] = Class;
        return Class;
}

void SDK::UObject::ProcessEvent(UFunction* Function, void* Parms) const {
        static void ( *ProcessEvent )( const UObject *Object, UFunction *Func,
                                       void *Params ) = decltype(ProcessEvent)(Offsets::UObject__ProcessEvent);
        return ProcessEvent( this, Function, Parms );
}


SDK::UFunction* SDK::UObject::GetFunction(const std::string& FunctionName) {
        UFunction *Found = nullptr;
        for ( UStruct *Struct = this->GetClass(); Struct;
              Struct = Struct->GetSuperStruct() ) {
                SDK::FFunctionInfo FuncInfo =
                    SDK::PropLibrary->GetFunctionByName(
                        Struct->GetFName().ToString().ToString(),
                        FunctionName );
                if ( FuncInfo.Func ) {
                        Found = FuncInfo.Func;
                        break;
                }


        }
        
        return Found;
}