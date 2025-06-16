#include "pch.h"
#include "../../include/SDK.h"

SDK::FKismetMemoryLibrary::FKismetMemoryLibrary( const char* pattern,
                                                 uint64_t ModuleBaseAddress ) {
        uint64_t FoundAddress = this->FindPattern( pattern, ModuleBaseAddress );

        if ( FoundAddress == 0 ) {
                this->m_status = 0;
                printf( "Pattern not found: %s in module: %I64X\n", pattern,
                        ModuleBaseAddress );
                return;
        }

        this->m_address = FoundAddress;
        
}

uint64_t SDK::FKismetMemoryLibrary::FindPattern( const std::string &pattern, 
    uint64_t m_ModuleBase ) {
        MODULEINFO modInfo = { 0 };
        size_t sizeOfImage = 0;
        if ( GetModuleInformation( GetCurrentProcess(), (HMODULE)m_ModuleBase, &modInfo,
                                   sizeof( modInfo ) ) ) {
                sizeOfImage = static_cast<size_t>( modInfo.SizeOfImage );
        }
        std::vector<int> bytes;
        std::istringstream iss( pattern );
        std::string byteBuf;
        while ( iss >> byteBuf ) {
                if ( byteBuf == "?" || byteBuf == "??" ) {
                        bytes.push_back( -1 );
                } else {
                        bytes.push_back( static_cast<int>(
                            strtoul( byteBuf.c_str(), nullptr, 16 ) ) );
                }
        }

        for ( size_t i = 0; i <= sizeOfImage - bytes.size(); i++ ) {
                bool found = true;
                for ( size_t j = 0; j < bytes.size(); j++ ) {
                        if ( bytes[j] != -1 &&
                             bytes[j] !=
                                 *(unsigned char *)( m_ModuleBase + i +
                                                     j ) ) {
                                found = false;
                                break;
                        }
                }
                if (found) {
                        return m_ModuleBase + i;
                }
        }

        return 0;
}

SDK::FKismetMemoryLibrary::FKismetMemoryLibrary( const wchar_t* stringRef, 
    const char* moduleName ) {
        HMODULE hModule = GetModuleHandleA( moduleName );
        uint64_t StringRef =
            this->FindStringRef( stringRef, hModule );

        if (StringRef == 0) {
                this->m_status = 0;
                wprintf( L"String was not found: %s in module:\n",
                         stringRef );
                return;
        }
        this->m_address = StringRef;
}

SDK::FKismetMemoryLibrary::FKismetMemoryLibrary(uint64_t Address) {
        m_address = Address;
}

void SDK::FKismetMemoryLibrary::ResizeVirtualMemory(int32_t OldSize, int32_t NewSize) {
    void* NewData = VirtualAlloc( nullptr, NewSize, MEM_COMMIT | MEM_RESERVE, 
        PAGE_EXECUTE_READWRITE );
        if ( NewData == nullptr ) {
            this->m_status = 0;
            printf( "Failed to allocate memory via VirtualAlloc" );
            return;
        }
        memcpy( NewData, this->m_data, OldSize );
        this->m_data = NewData;
}

SDK::FKismetMemoryLibrary::FKismetMemoryLibrary( void *Data, int NewSize, int32_t OldSize )
{
        this->m_data = Data;
        this->ResizeVirtualMemory( OldSize, NewSize );
}


uint64_t SDK::FKismetMemoryLibrary::FindStringRef( const wchar_t *target,
                                                   HMODULE m_ModuleBase ) {
        return Memcury::Scanner::FindStringRef( target ).Get();
}

uint64_t SDK::FKismetMemoryLibrary::GetAddress() const
{
        return m_address;
};

uint32_t SDK::FKismetMemoryLibrary::GetStatus() const
{
    return m_status; 
}

void SDK::FKismetMemoryLibrary::SwitchPattern(const std::string& pattern, uint64_t m_ModuleBase)
{
        m_address = this->FindPattern( pattern, m_ModuleBase );

        m_status = m_address != 0;
}

void SDK::FKismetMemoryLibrary::AbsoluteOffset(int Offset) 
{
        m_address = m_address + Offset;
}
void SDK::FKismetMemoryLibrary::RelativeOffset(int Offset) 
{
        m_address =
            ( ( m_address + Offset + 4 ) + *(int32_t *)( m_address + Offset ) );

}

void SDK::FKismetMemoryLibrary::SwitchStringRef(const wchar_t* stringRef,
    const char* moduleName) {
        m_address = FindStringRef( stringRef, GetModuleHandleA(0) );
        
        m_status = m_address != 0;
}

void SDK::FKismetMemoryLibrary::SwitchAddress(uint64_t m_NewAddress) {
        m_address = m_NewAddress;
}

void SDK::FKismetMemoryLibrary::ScanFor(const std::vector<BYTE>& BytesToFind, bool forward,
    size_t skip) {
        if ( !m_address || BytesToFind.empty() )
                return;

        const size_t patternSize = BytesToFind.size();
        const SIZE_T scanLength = 0x800;
        size_t skipped = 0;

        for ( size_t i = 0; i < scanLength; ++i ) {
                uint8_t *ptr = reinterpret_cast<uint8_t *>(
                    forward ? m_address + i : m_address - i );

                if ( memcmp( ptr, BytesToFind.data(), patternSize ) == 0 ) {
                        if ( skipped < skip ) {
                                ++skipped;
                                continue;
                        }

                        m_address = reinterpret_cast<uint64_t>( ptr );
                        return;
                }
        }

        m_address = 0;
}