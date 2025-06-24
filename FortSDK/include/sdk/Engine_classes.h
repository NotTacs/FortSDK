#pragma once

namespace SDK
{
	class UEngine : public UObject {
      public:
        static UEngine *GetEngine();
	  public:
        static UClass *StaticClass() { return StaticClassImpl( "Engine" );
		}
	};
}