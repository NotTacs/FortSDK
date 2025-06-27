#pragma once

namespace SDK
{
	class UEngine : public UObject {
      public:
         DEFINE_PTR( UObject, UEngine, GameViewport );
      public:
        static UEngine *GetEngine();
	  public:
        static UClass *StaticClass() { return StaticClassImpl( "Engine" );
		}
	};
}