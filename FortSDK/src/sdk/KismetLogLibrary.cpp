#include "pch.h"
#include "../../include/SDK.h"

void SDK::FKismetLogLibrary::Log_Internal( const char *Str, ... ) {
        std::time_t time = std::time( nullptr );
        std::tm now = {};
        localtime_s( &now, &time );

        char timeAsStr[20];
        std::strftime( timeAsStr, sizeof( timeAsStr ), "%Y-%m-%d %H:%M:%S",
                       &now );

        char argbuf[2048];
        va_list args;
        va_start( args, Str );
        vsnprintf( argbuf, sizeof( argbuf ), Str, args );
        va_end( args );

        if ( this->m_stream ) {
                ( *this->m_stream )
                    << "[" << timeAsStr << "]" << argbuf << "\n";
        }
        else {
                UE_LOG( LogFortSDK, Log, "%s", argbuf );
        }
}

void SDK::FKismetLogLibrary::SwitchStream(std::ofstream& NewStream) {
        m_stream = &NewStream;
}

void SDK::FKismetLogLibrary::SetNullStream()
{ m_stream = nullptr; }