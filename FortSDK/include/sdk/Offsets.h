#pragma once

namespace SDK
{
namespace Offsets 
{
extern uint64_t FMemory__Realloc;
extern uint64_t FMemory__Free;
extern uint64_t GUObjectArray__ObjObjects;
extern uint64_t FName__ToString;
extern uint64_t UObject__ProcessEvent;
namespace Members
{
extern uint32_t UFunction__Exec;
extern uint32_t UStruct__SuperStruct;
extern uint32_t UStruct__Children;
extern uint32_t UStruct__ChildProperties;
extern uint32_t UStruct__Size;
extern uint32_t UStruct__MinAlignment;
extern uint32_t UStruct__Script;
extern uint32_t UStruct__PropertyLink;
extern uint32_t UStruct__RefLink;
extern uint32_t UStruct__DestructorLink;
extern uint32_t UStruct__PostConstructorLink;
extern uint32_t UStruct__ScriptAndPropertyObjectReferences;
extern uint32_t UProperty__Offset_Internal;
}
}
namespace UEChanges
{
extern bool bUsesFPropertySystem;
extern bool bUsesUE5FProp;
}
}

