#include "pch.h"

SDK::FUObjectArray SDK::GUObjectArray = SDK::FUObjectArray();
SDK::FEngineVersion SDK::Engine_Version = SDK::FEngineVersion();
SDK::FFortniteVersion SDK::Fortnite_Version = SDK::FFortniteVersion();
std::unique_ptr<SDK::FKismetPropertyLibrary> SDK::PropLibrary = std::make_unique<SDK::FKismetPropertyLibrary>();

SDK::uint32 SDK::Offsets::Members::UStruct__SuperStruct = -1;
SDK::uint32 SDK::Offsets::Members::UStruct__Children = -1;
SDK::uint32 SDK::Offsets::Members::UStruct__ChildProperties = -1;
SDK::uint32 SDK::Offsets::Members::UStruct__Size = -1;
SDK::uint32 SDK::Offsets::Members::UStruct__MinAlignment = -1;
SDK::uint32 SDK::Offsets::Members::UStruct__Script = -1;
SDK::uint32 SDK::Offsets::Members::UStruct__PropertyLink = -1;
SDK::uint32 SDK::Offsets::Members::UStruct__RefLink = -1;
SDK::uint32 SDK::Offsets::Members::UStruct__DestructorLink = -1;
SDK::uint32 SDK::Offsets::Members::UStruct__PostConstructorLink = -1;
SDK::uint32 SDK::Offsets::Members::UStruct__ScriptAndPropertyObjectReferences =
    -1;
bool SDK::UEChanges::bUsesFPropertySystem = false;
bool SDK::UEChanges::bUsesUE5FProp = false;

bool SDK::Init() {

    /*
    * -----------------
    * SORT INITALIZERS INTO SEPERATE FUNCTIONS
    * -----------------
    */
        std::unique_ptr<SDK::FKismetLogLibrary> logLibrary =
            std::make_unique<SDK::FKismetLogLibrary>( );

        logLibrary->Log_Internal( "ImageBase: %p", GetModuleHandle( 0 ) );

        bool bIsChunked = true;

        std::unique_ptr<SDK::FKismetMemoryLibrary> memLibrary =
            std::make_unique<SDK::FKismetMemoryLibrary>( "48 8B 05 ? ? ? ? 48 8B 0C C8 48 8B 04 D1", uint64_t(GetModuleHandle(0)) );
        if ( memLibrary->GetStatus() == 0 ) {
                logLibrary->Log_Internal(
                    "Failed to use default method, resorting to method 1" );
                memLibrary->SwitchPattern(
                    "48 8B 05 ? ? ? ? 48 8B 0C C8 48 8B 04 D1",
                    uint64_t(GetModuleHandle( 0 ) ));
                if ( memLibrary->GetStatus() == 0 ) {
                        logLibrary->Log_Internal(
                            "Failed to use method 1, resorting to method "
                            "2" );


                        //BEGIN UNCHUNKED

                        bIsChunked = false;

                        memLibrary->SwitchPattern(
                            "48 8B 05 ? ? ? ? 48 8D 14 C8 EB 03 49 8B D6 8B 42 "
                            "08 C1 E8 1D A8 01 0F 85 ? ? ? ? F7 86 ? ? ? ? ? ? "
                            "? ?",
                            uint64_t(GetModuleHandle( 0 ) ));

                        if ( memLibrary->GetStatus() == 0 ) {
                                logLibrary->Log_Internal(
                                    "Failed to use method 2, resorting to "
                                    "method 3" );

                                memLibrary->SwitchPattern(
                                    "48 8B 05 ? ? ? ? 48 8D 1C C8 81 4B ? ? ? "
                                    "? ? 49 63 76 30",
                                    uint64_t(GetModuleHandle( 0 ) ));

                                if ( memLibrary->GetStatus() == 0 ) {
                                        logLibrary->Log_Internal(
                                            "Failed to find GObjectsAddress." );
                                        return false;
                                } else {
                                        logLibrary->Log_Internal(
                                            "Found GObjectsAddress: %p with "
                                            "method 3" );
                                }
                        } else {
                                logLibrary->Log_Internal(
                                    "Found GObjectsAddress: %p with method 2" );
                        }

                } else {
                        logLibrary->Log_Internal(
                            "Found GObjectsAddress: %p with method 1", memLibrary->GetAddress() );
                }
        } else {
                logLibrary->Log_Internal( "Found GObjectsAddress: %p",
                                          memLibrary->GetAddress() );
        }
        memLibrary->RelativeOffset( 3 );
        logLibrary->Log_Internal( "GObjects: %p", memLibrary->GetAddress() );

        Offsets::GUObjectArray__ObjObjects = memLibrary->GetAddress();

        GUObjectArray = FUObjectArray(
            reinterpret_cast<void *>( Offsets::GUObjectArray__ObjObjects ),
            bIsChunked );

        logLibrary->Log_Internal( "GUObjectArray Num: %d",
                                  GUObjectArray.GetObjectArrayNum() );

        /*
        * -----------------------
        * BEGIN FName::ToString
        * -----------------------
        */

        memLibrary->SwitchStringRef( L"\tSessionName: %s", 0 );
        if ( memLibrary->GetStatus() != 0 ) {
                memLibrary->ScanFor( { 0xE8 }, false );
                memLibrary->RelativeOffset( 1 );
                Offsets::FName__ToString = memLibrary->GetAddress();
                logLibrary->Log_Internal( "Found FName::ToString: %p",
                                          Offsets::FName__ToString );
        } else {
                logLibrary->Log_Internal(
                    "Failed to find FName::ToString with `SessionName:` String Ref" );
                return false;
        }

        /*
        * ---------------------
        * BEGIN FMemory::Realloc
        * ---------------------
        */

        //ToDo: Optimize this with fallbacks
        memLibrary->SwitchPattern(
            "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 54 41 55 41 56 41 "
            "57 48 83 EC ? 4C 8B 2D" );
        if (memLibrary->GetStatus() != 0)
        {
                Offsets::FMemory__Realloc = memLibrary->GetAddress();
                logLibrary->Log_Internal( "Found FMemory::Realloc: %p",
                                          Offsets::FMemory__Realloc );
        } else {
                memLibrary->SwitchStringRef( L"LogCountedInstances", 0 );
                if ( memLibrary->GetStatus() != 0 ) {
                        memLibrary->ScanFor( { 0xE8 } );
                        memLibrary->RelativeOffset( 1 );
                        Offsets::FMemory__Realloc = memLibrary->GetAddress();
                        logLibrary->Log_Internal( "Found FMemory::Realloc: %p",
                                                  Offsets::FMemory__Realloc );
                } else {
                        logLibrary->Log_Internal(
                            "Failed to find FMemory::Realloc with "
                            "'LogCountedInstances' String Ref" );
                        return false;
                }
        }


        /*
        * --------------------
        * BEGIN UFunction::Exec
        * --------------------
        */
        memLibrary->SwitchPattern( "FF 95 ? ? ? ? 48 8B 6C 24" );
        if ( memLibrary->GetStatus() == 0 ) {
                memLibrary->SwitchPattern(
                    "FF 95 ? ? ? ? 4C 8D 5C 24 ? 48 89 9F" );
                if ( memLibrary->GetStatus() == 0 ) {
                        memLibrary->SwitchPattern(
                            "FF 97 ? ? ? ? 48 8B 6C 24" );

                        if ( memLibrary->GetStatus() == 0 ) {
                                logLibrary->Log_Internal(
                                    "Failed to find UFunction::Exec with "
                                    "either "
                                    "pattern. REPORT TO cumware" );
                                return false;
                        }
                }
        }
        memLibrary->AbsoluteOffset( 2 );
        Offsets::Members::UFunction__Exec = memLibrary->GetAddress<uint32_t>();

        logLibrary->Log_Internal( "Found UFunction::Exec: %d",
                                  Offsets::Members::UFunction__Exec );


        /*
        * ------------------
        * BEGIN SetupEngineVersion
        * ------------------
        */
        UFunction* GetEngineVersionFN = Cast<UFunction>(GUObjectArray.FindObject( "GetEngineVersion" ));
        uint64_t NativeFuncAddress = (uint64_t)GetEngineVersionFN->GetNativeFunc();
        memLibrary->SwitchAddress( NativeFuncAddress );
        memLibrary->ScanFor( { 0xE8 } );
        memLibrary->RelativeOffset( 1 );

        static FString &( *GetEngineVersion )( FString &retstr ) =
            decltype( GetEngineVersion )( memLibrary->GetAddress() );

        FString TempString = GetEngineVersion( TempString );

        if (TempString.ToString().contains("Live") ||
            TempString.ToString().contains("Cert"))
        {
                logLibrary->Log_Internal(
                    "There is not active support for this current build. "
                    "please wait for new production." );
                return false;
        }

        logLibrary->Log_Internal( "TS: %s", TempString.ToString().c_str() );

        std::vector<std::string> Parts;
        FString Test = L"-";
        TempString.ParseIntoArray( Parts, Test );

        logLibrary->Log_Internal( "Parts: %d", Parts.size() );

        for ( auto &Part : Parts ) {
                logLibrary->Log_Internal( "Part: %s", Part.c_str() );
        }

        SDK::Engine_Version = 
            FEngineVersion( Parts[0] );
        SDK::Fortnite_Version =
            FFortniteVersion( Parts[2] );
        
        logLibrary->Log_Internal(
            "EngineVersion: %s",
            SDK::Engine_Version.ToString().c_str() );
        logLibrary->Log_Internal(
            "FortniteVersion: %s",
            SDK::Fortnite_Version.ToString().c_str() );

        /*
        * ------------------------
        * BEGIN MemberOffsets
        * ------------------------
        */
        Offsets::Members::UStruct__SuperStruct =
            Engine_Version >= FEngineVersion(4,22,0) ? 0x40 : 0x30; /*adds support for the class FStructBaseChain introduced in 4.22*/
        Offsets::Members::UStruct__Children =
            Offsets::Members::UStruct__SuperStruct + sizeof( void * );
        Offsets::Members::UStruct__ChildProperties = Fortnite_Version >= FFortniteVersion(12, 10, 0)
            ? Offsets::Members::UStruct__Children + sizeof( void * )
                : -1; /*adds support for ChildrenProperties which is introduced at some point idk, could be introduced in 4.25 i believe*/
        Offsets::Members::UStruct__Size =
            Offsets::Members::UStruct__ChildProperties != -1
                ? Offsets::Members::UStruct__ChildProperties + sizeof( void * )
                : Offsets::Members::UStruct__Children + sizeof( void * );
        Offsets::Members::UStruct__MinAlignment =
            Offsets::Members::UStruct__Size + sizeof( int32_t );
        Offsets::Members::UStruct__Script =
            Offsets::Members::UStruct__MinAlignment + sizeof( int32_t );
        Offsets::Members::UStruct__PropertyLink =
            Offsets::Members::UStruct__Script + sizeof(TArray<uint8_t>);
        Offsets::Members::UStruct__RefLink =
            Offsets::Members::UStruct__PropertyLink + sizeof( void * );
        Offsets::Members::UStruct__DestructorLink =
            Offsets::Members::UStruct__RefLink + sizeof( void * );
        Offsets::Members::UStruct__PostConstructorLink =
            Offsets::Members::UStruct__DestructorLink + sizeof( void * );
        Offsets::Members::UStruct__ScriptAndPropertyObjectReferences =
            Offsets::Members::UStruct__PostConstructorLink + sizeof( void * );
        UEChanges::bUsesFPropertySystem = Engine_Version >= FEngineVersion(4,25,0);
        UEChanges::bUsesUE5FProp = Fortnite_Version >= FFortniteVersion(20,00,0);
        
        logLibrary->Log_Internal( "UStruct::PropertyLink: %p",
                                  Offsets::Members::UStruct__PropertyLink );
        logLibrary->Log_Internal( "bUsesFPropertySystem: %d",
                                  UEChanges::bUsesFPropertySystem );
        logLibrary->Log_Internal( "bUsesUE5PropDih: %d",
                                  UEChanges::bUsesUE5FProp );
        logLibrary->Log_Internal( "UStruct::SuperStruct: %p",
                                  Offsets::Members::UStruct__SuperStruct );

        /*
        * ---------------------
        * BEGIN ProcessEvent
        * ---------------------
        */
        memLibrary->SwitchPattern( "41 FF 92 ? ? ? ? F6 C3" );

        if ( Fortnite_Version >= FFortniteVersion(19,00,0) ) {
                memLibrary->SwitchPattern(
                    "41 FF 92 ? ? ? ? E9 ? ? ? ? 49 8B C8" );
                logLibrary->Log_Internal( "Using Expiremntal sig" );
        }

        if ( memLibrary->GetStatus() != 0 ) {
            memLibrary->ScanFor( { 0x40, 0x55 }, false );
            Offsets::UObject__ProcessEvent = memLibrary->GetAddress();
        } else {
                logLibrary->Log_Internal(
                    "Failed to find UObject::ProcessEvent. Please alert "
                    "NotTacs on discord." );
                return false;
        }
        logLibrary->Log_Internal( "Found UObject::ProcessEvent: %p",
                                  Offsets::UObject__ProcessEvent );


        return true;
}