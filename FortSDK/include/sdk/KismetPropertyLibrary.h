#pragma once
namespace SDK
{
	struct FPropertyInfo
	{
        void *Prop;
        int32_t Offset;
        std::string ClassName;
        std::string PropName;

        FPropertyInfo() = default;

        FPropertyInfo( const FPropertyInfo & ) = default;
        FPropertyInfo &operator=( const FPropertyInfo & ) = default;
	};

    struct FFunctionInfo
    {
            UFunction *Func;
            std::string ClassName;
    };

    class FKismetPropertyManager
    {
          public:
            void *Prop;

          public:
            int GetOffset();
    };

	class FKismetPropertyLibrary
	{
          public:
            std::vector<FPropertyInfo> AllPropertyInfo;
            std::vector<FFunctionInfo> AllFunctionInfo;
          public:
            FPropertyInfo GetPropertyByName( const std::string& ClassName, const std::string& PropName );
            FFunctionInfo GetFunctionByName( const std::string &ClassName,
                                             const std::string &FunctionName );
	};
}