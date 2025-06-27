# FortSDK

# Important Info Please Read:


TArray::GetData is what you should use for getting access into the array, never use iterators (for (const auto& Element : TArray)) unless your certain that your element type is a pointer.
when using TArray::GetData if your elementtype is not a pointer you need to override the elementsize in the GetData, by simply ElementType::StaticClass()->GetSize() into the param.

# FEATURES

- Supports FPropertys
- Supports UE5(Somewhat)
- Supports ScriptStructs
- Supports CallingFunctions without making a function yourself
