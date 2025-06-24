#pragma once

namespace SDK
{
class UClass *StaticClassImpl( const char *ClassName );
/**
 * Flags describing an object instance
 */
enum EObjectFlags {
        // Do not add new flags unless they truly belong here. There are
        // alternatives. if you change any the bit of any of the RF_Load flags,
        // then you will need legacy serialization
        RF_NoFlags = 0x00000000, ///< No flags, used to avoid a cast

        // This first group of flags mostly has to do with what kind of object
        // it is. Other than transient, these are the persistent object flags.
        // The garbage collector also tends to look at these.
        RF_Public = 0x00000001,     ///< Object is visible outside its package.
        RF_Standalone = 0x00000002, ///< Keep object around for editing even if
                                    ///< unreferenced.
        RF_MarkAsNative = 0x00000004,  ///< Object (UField) will be marked as
                                       ///< native on construction (DO NOT USE
                                       ///< THIS FLAG in HasAnyFlags() etc)
        RF_Transactional = 0x00000008, ///< Object is transactional.
        RF_ClassDefaultObject =
            0x00000010, ///< This object is its class's default object
        RF_ArchetypeObject =
            0x00000020, ///< This object is a template for another object -
                        ///< treat like a class default object
        RF_Transient = 0x00000040, ///< Don't save object.

        // This group of flags is primarily concerned with garbage collection.
        RF_MarkAsRootSet =
            0x00000080, ///< Object will be marked as root set on construction
                        ///< and not be garbage collected, even if unreferenced
                        ///< (DO NOT USE THIS FLAG in HasAnyFlags() etc)
        RF_TagGarbageTemp =
            0x00000100, ///< This is a temp user flag for various utilities that
                        ///< need to use the garbage collector. The garbage
                        ///< collector itself does not interpret it.

        // The group of flags tracks the stages of the lifetime of a uobject
        RF_NeedInitialization =
            0x00000200, ///< This object has not completed its initialization
                        ///< process. Cleared when ~FObjectInitializer completes
        RF_NeedLoad =
            0x00000400, ///< During load, indicates object needs loading.
        RF_KeepForCooker =
            0x00000800, ///< Keep this object during garbage collection because
                        ///< it's still being used by the cooker
        RF_NeedPostLoad = 0x00001000, ///< Object needs to be postloaded.
        RF_NeedPostLoadSubobjects =
            0x00002000, ///< During load, indicates that the object still needs
                        ///< to instance subobjects and fixup serialized
                        ///< component references
        RF_NewerVersionExists =
            0x00004000, ///< Object has been consigned to oblivion due to its
                        ///< owner package being reloaded, and a newer version
                        ///< currently exists
        RF_BeginDestroyed =
            0x00008000, ///< BeginDestroy has been called on the object.
        RF_FinishDestroyed =
            0x00010000, ///< FinishDestroy has been called on the object.

        // Misc. Flags
        RF_BeingRegenerated =
            0x00020000, ///< Flagged on UObjects that are used to create
                        ///< UClasses (e.g. Blueprints) while they are
                        ///< regenerating their UClass on load (See
                        ///< FLinkerLoad::CreateExport()), as well as UClass
                        ///< objects in the midst of being created
        RF_DefaultSubObject =
            0x00040000,            ///< Flagged on subobjects that are defaults
        RF_WasLoaded = 0x00080000, ///< Flagged on UObjects that were loaded
        RF_TextExportTransient =
            0x00100000, ///< Do not export object to text form (e.g.
                        ///< copy/paste). Generally used for sub-objects that
                        ///< can be regenerated from data in their parent
                        ///< object.
        RF_LoadCompleted =
            0x00200000, ///< Object has been completely serialized by linkerload
                        ///< at least once. DO NOT USE THIS FLAG, It should be
                        ///< replaced with RF_WasLoaded.
        RF_InheritableComponentTemplate =
            0x00400000, ///< Archetype of the object can be in its super class
        RF_DuplicateTransient =
            0x00800000, ///< Object should not be included in any type of
                        ///< duplication (copy/paste, binary duplication, etc.)
        RF_StrongRefOnFrame =
            0x01000000, ///< References to this object from persistent function
                        ///< frame are handled as strong ones.
        RF_NonPIEDuplicateTransient =
            0x02000000, ///< Object should not be included for duplication
                        ///< unless it's being duplicated for a PIE session
        RF_Dynamic =
            0x04000000, ///< Field Only. Dynamic field - doesn't get constructed
                        ///< during static initialization, can be constructed
                        ///< multiple times  // @todo: BP2CPP_remove
        RF_WillBeLoaded = 0x08000000, ///< This object was constructed during
                                      ///< load and will be loaded shortly
        RF_HasExternalPackage =
            0x10000000, ///< This object has an external package assigned and
                        ///< should look it up when getting the outermost
                        ///< package

        // RF_Garbage and RF_PendingKill are mirrored in EInternalObjectFlags
        // because checking the internal flags is much faster for the Garbage
        // Collector while checking the object flags is much faster outside of
        // it where the Object pointer is already available and most likely
        // cached. RF_PendingKill is mirrored in EInternalObjectFlags because
        // checking the internal flags is much faster for the Garbage Collector
        // while checking the object flags is much faster outside of it where
        // the Object pointer is already available and most likely cached.

        RF_PendingKill =
            0x20000000, ///< Objects that are pending destruction (invalid for
                        ///< gameplay but valid objects). This flag is mirrored
                        ///< in EInternalObjectFlags as PendingKill for
                        ///< performance
        RF_Garbage  =
            0x40000000, ///< Garbage from logical point of view and should not
                        ///< be referenced. This flag is mirrored in
                        ///< EInternalObjectFlags as Garbage for performance
        RF_AllocatedInSharedPage =
            0x80000000, ///< Allocated from a ref-counted page shared with other
                        ///< UObjects
};

class FName
{
      private:
        int32 ComparisonIndex;
        uint32 Number;

      public:

        FORCEINLINE int32 GetNumber() const { return Number; }

        FORCEINLINE void SetNumber( const int32 NewNumber ) {
                Number = NewNumber;
        }

        FORCEINLINE int32 GetComparisonIndex() const
        { return ComparisonIndex;
        }

        FString ToString() const;
};

/** Mask for all object flags */
#define RF_AllFlags                                                            \
        (EObjectFlags)0xffffffff ///< All flags, used mainly for error checking

class FUObjectArray;
class UObjectBase
{
      public:
        friend class UObjectBaseUtility;
        friend class FUObjectArray;

        public:

            bool IsValidLowLevel() const
            {
                if (this == nullptr)
                {
                            UE_LOG( LogUObjectBase, Warning,
                                    "NULL Object" );
                        return false;
                }
                if (!ClassPrivate)
                {
                        UE_LOG( LogUObjectBase, Warning,
                                "Object is not registered" );
                        return false;
                }
                return this->InternalIndex > 0;
            }
            bool IsValidLowLevelFast( bool bRecursive = true ) const;
            


            /**
             * Returns the unique ID of the object...these are reused so it is
             *only unique while the object is alive. Useful as a tag.
             **/
            FORCEINLINE uint32 GetUniqueID() const {
                    return (uint32)InternalIndex;
            }

            /** Returns the UClass that defines the fields of this object */
            FORCEINLINE class UClass *GetClass() const { return ClassPrivate; }

            /** Returns the UObject this object resides in */
            FORCEINLINE class UObject *GetOuter() const { return OuterPrivate; }

            /** Returns the logical name of this object */
            FORCEINLINE FName GetFName() const { return NamePrivate; }

        private:

          void **VTable;

          /** Flags used to track and report various object states. This needs to be 8 byte aligned on 32-bit 
            platforms to reduce memory waste */
          EObjectFlags ObjectFlags;

          /** Index into GObjectArray...very private. */
          int32 InternalIndex;

          /** Class the object belongs to. */
          class UClass *ClassPrivate;

          /** Name of this object */
          FName NamePrivate;

          /** Object this object resides in. */
          UObject *OuterPrivate;
};

/**
 * Flags describing a class.
 */
enum EClassFlags {
        /** No Flags */
        CLASS_None = 0x00000000u,
        /** Class is abstract and can't be instantiated directly. */
        CLASS_Abstract = 0x00000001u,
        /** Save object configuration only to Default INIs, never to local INIs.
           Must be combined with CLASS_Config */
        CLASS_DefaultConfig = 0x00000002u,
        /** Load object configuration at construction time. */
        CLASS_Config = 0x00000004u,
        /** This object type can't be saved; null it out at save time. */
        CLASS_Transient = 0x00000008u,
        /** This object type may not be available in certain context. (i.e. game
           runtime or in certain configuration). Optional class data is saved
           separately to other object types. (i.e. might use sidecar files) */
        CLASS_Optional = 0x00000010u,
        /** */
        CLASS_MatchedSerializers = 0x00000020u,
        /** Indicates that the config settings for this class will be saved to
           Project/User*.ini (similar to CLASS_GlobalUserConfig) */
        CLASS_ProjectUserConfig = 0x00000040u,
        /** Class is a native class - native interfaces will have CLASS_Native
           set, but not RF_MarkAsNative */
        CLASS_Native = 0x00000080u,
        /** Don't export to C++ header. */
        CLASS_NoExport = 0x00000100u,
        /** Do not allow users to create in the editor. */
        CLASS_NotPlaceable = 0x00000200u,
        /** Handle object configuration on a per-object basis, rather than
           per-class. */
        CLASS_PerObjectConfig = 0x00000400u,

        /** Whether SetUpRuntimeReplicationData still needs to be called for
           this class */
        CLASS_ReplicationDataIsSetUp = 0x00000800u,

        /** Class can be constructed from editinline New button. */
        CLASS_EditInlineNew = 0x00001000u,
        /** Display properties in the editor without using categories. */
        CLASS_CollapseCategories = 0x00002000u,
        /** Class is an interface **/
        CLASS_Interface = 0x00004000u,
        /**  Do not export a constructor for this class, assuming it is in the
           cpptext **/
        CLASS_CustomConstructor = 0x00008000u,
        /** all properties and functions in this class are const and should be
           exported as const */
        CLASS_Const = 0x00010000u,

        /** Class flag indicating objects of this class need deferred dependency
           loading */
        CLASS_NeedsDeferredDependencyLoading = 0x00020000u,

        /** Indicates that the class was created from blueprint source material
         */
        CLASS_CompiledFromBlueprint = 0x00040000u,

        /** Indicates that only the bare minimum bits of this class should be
           DLL exported/imported */
        CLASS_MinimalAPI = 0x00080000u,

        /** Indicates this class must be DLL exported/imported (along with all
           of it's members) */
        CLASS_RequiredAPI = 0x00100000u,

        /** Indicates that references to this class default to instanced. Used
           to be subclasses of UComponent, but now can be any UObject */
        CLASS_DefaultToInstanced = 0x00200000u,

        /** Indicates that the parent token stream has been merged with ours. */
        CLASS_TokenStreamAssembled = 0x00400000u,
        /** Class has component properties. */
        CLASS_HasInstancedReference = 0x00800000u,
        /** Don't show this class in the editor class browser or edit inline new
           menus. */
        CLASS_Hidden = 0x01000000u,
        /** Don't save objects of this class when serializing */
        CLASS_Deprecated = 0x02000000u,
        /** Class not shown in editor drop down for class selection */
        CLASS_HideDropDown = 0x04000000u,
        /** Class settings are saved to <AppData>/..../Blah.ini (as opposed to
           CLASS_DefaultConfig) */
        CLASS_GlobalUserConfig = 0x08000000u,
        /** Class was declared directly in C++ and has no boilerplate generated
           by UnrealHeaderTool */
        CLASS_Intrinsic = 0x10000000u,
        /** Class has already been constructed (maybe in a previous DLL version
           before hot-reload). */
        CLASS_Constructed = 0x20000000u,
        /** Indicates that object configuration will not check against ini
           base/defaults when serialized */
        CLASS_ConfigDoNotCheckDefaults = 0x40000000u,
        /** Class has been consigned to oblivion as part of a blueprint
           recompile, and a newer version currently exists. */
        CLASS_NewerVersionExists = 0x80000000u,
};

/**
 * Flags associated with each property in a class, overriding the
 * property's default behavior.
 * @warning When adding one here, please update ParsePropertyFlags()
 */
enum EPropertyFlags : uint64 {
        CPF_None = 0,

        CPF_Edit =
            0x0000000000000001, ///< Property is user-settable in the editor.
        CPF_ConstParm =
            0x0000000000000002, ///< This is a constant function parameter
        CPF_BlueprintVisible =
            0x0000000000000004, ///< This property can be read by blueprint code
        CPF_ExportObject =
            0x0000000000000008, ///< Object can be exported with actor.
        CPF_BlueprintReadOnly =
            0x0000000000000010,       ///< This property cannot be modified by
                                      ///< blueprint code
        CPF_Net = 0x0000000000000020, ///< Property is relevant to network
                                      ///< replication.
        CPF_EditFixedSize =
            0x0000000000000040, ///< Indicates that elements of an array can be
                                ///< modified, but its size cannot be changed.
        CPF_Parm = 0x0000000000000080, ///< Function/When call parameter.
        CPF_OutParm =
            0x0000000000000100, ///< Value is copied out after function call.
        CPF_ZeroConstructor =
            0x0000000000000200, ///< memset is fine for construction
        CPF_ReturnParm = 0x0000000000000400, ///< Return value.
        CPF_DisableEditOnTemplate =
            0x0000000000000800, ///< Disable editing of this property on an
                                ///< archetype/sub-blueprint
        // CPF_      						= 0x0000000000001000,	///<
        CPF_Transient =
            0x0000000000002000, ///< Property is transient: shouldn't be saved
                                ///< or loaded, except for Blueprint CDOs.
        CPF_Config = 0x0000000000004000, ///< Property should be loaded/saved as
                                         ///< permanent profile.
        // CPF_								= 0x0000000000008000,	///<
        CPF_DisableEditOnInstance =
            0x0000000000010000, ///< Disable editing on an instance of this
                                ///< class
        CPF_EditConst =
            0x0000000000020000, ///< Property is uneditable in the editor.
        CPF_GlobalConfig =
            0x0000000000040000, ///< Load config from base class, not subclass.
        CPF_InstancedReference =
            0x0000000000080000, ///< Property is a component references.
        // CPF_								= 0x0000000000100000,	///<
        CPF_DuplicateTransient =
            0x0000000000200000, ///< Property should always be reset to the
                                ///< default value during any type of
                                ///< duplication (copy/paste, binary
                                ///< duplication, etc.)
        // CPF_								= 0x0000000000400000,	///<
        // CPF_    							= 0x0000000000800000,	///<
        CPF_SaveGame =
            0x0000000001000000, ///< Property should be serialized for save
                                ///< games, this is only checked for
                                ///< game-specific archives with ArIsSaveGame
        CPF_NoClear = 0x0000000002000000, ///< Hide clear (and browse) button.
        // CPF_  							= 0x0000000004000000,	///<
        CPF_ReferenceParm =
            0x0000000008000000, ///< Value is passed by reference; CPF_OutParam
                                ///< and CPF_Param should also be set.
        CPF_BlueprintAssignable =
            0x0000000010000000, ///< MC Delegates only.  Property should be
                                ///< exposed for assigning in blueprint code
        CPF_Deprecated =
            0x0000000020000000, ///< Property is deprecated.  Read it from an
                                ///< archive, but don't save it.
        CPF_IsPlainOldData =
            0x0000000040000000, ///< If this is set, then the property can be
                                ///< memcopied instead of CopyCompleteValue /
                                ///< CopySingleValue
        CPF_RepSkip =
            0x0000000080000000, ///< Not replicated. For non replicated
                                ///< properties in replicated structs
        CPF_RepNotify =
            0x0000000100000000, ///< Notify actors when a property is replicated
        CPF_Interp = 0x0000000200000000, ///< interpolatable property for use
                                         ///< with cinematics
        CPF_NonTransactional =
            0x0000000400000000,              ///< Property isn't transacted
        CPF_EditorOnly = 0x0000000800000000, ///< Property should only be loaded
                                             ///< in the editor
        CPF_NoDestructor = 0x0000001000000000, ///< No destructor
        // CPF_								= 0x0000002000000000,	///<
        CPF_AutoWeak =
            0x0000004000000000, ///< Only used for weak pointers, means the
                                ///< export type is autoweak
        CPF_ContainsInstancedReference =
            0x0000008000000000, ///< Property contains component references.
        CPF_AssetRegistrySearchable =
            0x0000010000000000, ///< asset instances will add properties with
                                ///< this flag to the asset registry
                                ///< automatically
        CPF_SimpleDisplay =
            0x0000020000000000, ///< The property is visible by default in the
                                ///< editor details view
        CPF_AdvancedDisplay =
            0x0000040000000000, ///< The property is advanced and not visible by
                                ///< default in the editor details view
        CPF_Protected = 0x0000080000000000, ///< property is protected from the
                                            ///< perspective of script
        CPF_BlueprintCallable =
            0x0000100000000000, ///< MC Delegates only.  Property should be
                                ///< exposed for calling in blueprint code
        CPF_BlueprintAuthorityOnly =
            0x0000200000000000, ///< MC Delegates only.  This delegate accepts
                                ///< (only in blueprint) only events with
                                ///< BlueprintAuthorityOnly.
        CPF_TextExportTransient =
            0x0000400000000000, ///< Property shouldn't be exported to text
                                ///< format (e.g. copy/paste)
        CPF_NonPIEDuplicateTransient =
            0x0000800000000000, ///< Property should only be copied in PIE
        CPF_ExposeOnSpawn =
            0x0001000000000000, ///< Property is exposed on spawn
        CPF_PersistentInstance =
            0x0002000000000000, ///< A object referenced by the property is
                                ///< duplicated like a component. (Each actor
                                ///< should have an own instance.)
        CPF_UObjectWrapper =
            0x0004000000000000, ///< Property was parsed as a wrapper class like
                                ///< TSubclassOf<T>, FScriptInterface etc.,
                                ///< rather than a USomething*
        CPF_HasGetValueTypeHash =
            0x0008000000000000, ///< This property can generate a meaningful
                                ///< hash value.
        CPF_NativeAccessSpecifierPublic =
            0x0010000000000000, ///< Public native access specifier
        CPF_NativeAccessSpecifierProtected =
            0x0020000000000000, ///< Protected native access specifier
        CPF_NativeAccessSpecifierPrivate =
            0x0040000000000000, ///< Private native access specifier
        CPF_SkipSerialization =
            0x0080000000000000, ///< Property shouldn't be serialized, can still
                                ///< be exported to text
};

class UObjectBaseUtility : public UObjectBase
{

      private:
        template <typename ClassType>
        static FORCEINLINE bool
        IsChildOfWorkaround( const ClassType *ObjClass,
                             const ClassType *TestCls ) {
                return ObjClass->IsChildOf( TestCls );
        }
      public:
        template <typename OtherClassType>
        FORCEINLINE bool IsA( OtherClassType SomeBase ) const {
                // We have a cyclic dependency between UObjectBaseUtility and
                // UClass, so we use a template to allow inlining of something
                // we haven't yet seen, because it delays compilation until the
                // function is called.

                // 'static_assert' that this thing is actually a UClass pointer
                // or convertible to it.
                const class UClass *SomeBaseClass = SomeBase;
                (void)SomeBaseClass;
                checkSlow(
                    SomeBaseClass,
                    "IsA(NULL) cannot yield meaningful results" );

                const class UClass *ThisClass = GetClass();

                // Stop the compiler doing some unnecessary branching for
                // nullptr checks
                UE_ASSUME( SomeBaseClass );
                UE_ASSUME( ThisClass );

                return IsChildOfWorkaround( ThisClass, SomeBaseClass );
        }

        template <class T> bool IsA() const { return IsA( T::StaticClass() ); }
};

class UObject : public UObjectBaseUtility {
      public:
        void ProcessEvent( class UFunction *Function, void *Parms ) const;

        /*Theres no checks on this so make sure you check the offset is not equal to 0 or you are positive that this is going to be found*/
        template <typename T>
        T& Get( const std::string &ClassName, const std::string &PropName );

        template<typename T>
        void Set( const std::string &ClassName, const std::string &PropName, const T& Value );

        template <typename Ret, typename... Args>
        Ret CallFunc( const std::string &ClassName,
                      const std::string &FunctionName, Args &&...args );

        class UFunction *GetFunction(const std::string& FunctionName);
};

class UField : public UObject
{
      public:
        UField *Next;
};

class FFieldVariant
{
        union FFieldObjectUnion {
                class FField *Field;
                UObject *Object;
        } Container;

        bool bIsUObject;
};

class FFieldClass
{
      public:
        FName Name;
        uint64_t Id;
        uint64_t CastFlags;
        EClassFlags ClassFlags;
        uint8_t Idk[0x4];
        FFieldClass *SuperClass;
};

class FField
{
      public:
        void *VTable;
        FFieldClass *ClassPrivate;
        FFieldVariant Owner;
        FField *Next;
        FName NamePrivate;
        EObjectFlags FlagsPrivate;
};

class FProperty : public FField
{
      public:
        int32_t ArrayDim;
        int32_t ElementSize;
        EPropertyFlags PropertyFlags;
        uint16_t Repindex;
        uint8_t BlueprintReplicationCondition;
        int32_t Offset_Internal;
        FName RepNotifyFunc;
        FProperty *PropertyLinkNext;
        FProperty *NextRef;
        FProperty *DestructorLinkNext;
        FProperty *PostConstructLinkNext;
};

class FProperty500 : public FField
{
      public:
        uint8_t Pad_Shit[0x14];
        int32 Offset;
        FName RepNotifyFunc;
        FProperty500 *PropertyLinkNext;

        int32 Offset_Internal() {
                return *reinterpret_cast<int32 *>( __int64( this ) + 0x44 );
        }

        EPropertyFlags& GetPropertyFlags() {
                return *reinterpret_cast<EPropertyFlags *>(
                    __int64( this ) + offsetof( FProperty,PropertyFlags ) );
        }
};

class UProperty : public UField
{
      public:
        int32_t ArrayDim;
        int32_t ElementSize;
        EPropertyFlags PropertyFlags;
        uint16_t RepIndex;
        uint8_t BlueprintReplicationCondition;
        int32_t Offset_Internal;
        FName RepNotifyFunc;
        UProperty *PropertyLinkNext;
        UProperty *NextRef;
        UProperty *DestructorLinkNext;
        UProperty *PostConstructorLinkNext;
};

class UBoolProperty : public UProperty
{
      public:
        /** Size of the bitfield/bool property. Equal to ElementSize but used to
         * check if the property has been properly initialized (0-8, where 0
         * means uninitialized). */
        uint8 FieldSize;
        /** Offset from the memeber variable to the byte of the property (0-7).
         */
        uint8 ByteOffset;
        /** Mask of the byte with the property value. */
        uint8 ByteMask;
        /** Mask of the field with the property value. Either equal to ByteMask
         * or 255 in case of 'bool' type. */
        uint8 FieldMask;
};

class FBoolProperty : public FProperty
{
      public:
        /** Size of the bitfield/bool property. Equal to ElementSize but used to
         * check if the property has been properly initialized (0-8, where 0
         * means uninitialized). */
        uint8 FieldSize;
        /** Offset from the memeber variable to the byte of the property (0-7).
         */
        uint8 ByteOffset;
        /** Mask of the byte with the property value. */
        uint8 ByteMask;
        /** Mask of the field with the property value. Either equal to ByteMask
         * or 255 in case of 'bool' type. */
        uint8 FieldMask;
};

class UStruct : public UField
{
      private:
        UStruct *SuperStruct() const;
        UField *Children() const;
        FField *ChildrenProperties() const;
        int32 Size() const;
        int32 MinAlignment() const;
        TArray<uint8_t> &Script() const;
        void *PropertyLink() const;
        void *RefLink() const;
        void *DestructorLink() const;
        void *PostConstructorLink() const;

      public:
        template <typename T>
        void GetPropertyLink( T **OutPropertyLink ) {
                if ( OutPropertyLink )
                        *OutPropertyLink =
                            reinterpret_cast<T *>( PropertyLink() );
        }
        UStruct *GetSuperStruct() const { return SuperStruct(); }
        FField *GetChildrenProperties() const { return ChildrenProperties(); }
        UField *GetChildren() const { return Children(); }
        int32 GetSize() const { return Size(); }

        template <class T> bool IsChildOf() const {
                return IsChildOf( T::StaticClass() );
        }

        bool IsChildOf( const UStruct *SomeBase ) const;
};

class UClass : public UStruct
{
      public:
        UObject *GetClassDefaultObject() const;
        /**
         * This signature intentionally hides the method declared in
         * UObjectBaseUtility to make it private. Call IsChildOf instead; Hidden
         * because calling IsA on a class almost always indicates an error where
         * the caller should use IsChildOf
         */
        bool IsA( const UClass *Parent ) const {
                return UObject::IsA( Parent );
        }
};

#define RESULT_PARAM Z_Param__Result
#define RESULT_DECL void *const RESULT_PARAM

typedef void ( *FNativeFuncPtr )( UObject *Context, void *TheStack,
                                  RESULT_DECL );

class UFunction : public UStruct
{
      private:
        FNativeFuncPtr Func() const;

      public:
        FORCEINLINE FNativeFuncPtr GetNativeFunc() const { return Func(); }
        void SetNativeFunc( FNativeFuncPtr InFunc );
        inline uint32& FunctionFlags() {
                static int FunctionFlagsOffset =
                    SDK::Offsets::Members::UFunction__Exec - 0x28;
                return *reinterpret_cast<uint32 *>( __int64( this ) +
                                                    FunctionFlagsOffset );
        }

      public:
        static UClass *StaticClass() {
                return StaticClassImpl( "Function" );
        }
};


/** Objects flags for internal use (GC, low level UObject code) */
enum class EInternalObjectFlags : int32 {
        None = 0,

        LoaderImport = 1 << 20, ///< Object is ready to be imported by another
                                ///< package during loading
        Garbage = 1 << 21, ///< Garbage from logical point of view and should
                           ///< not be referenced. This flag is mirrored in
                           ///< EObjectFlags as RF_Garbage for performance
        PersistentGarbage = 1
                            << 22, ///< Same as above but referenced through a
                                   ///< persistent reference so it can't be GC'd
        ReachableInCluster =
            1 << 23, ///< External reference to object in cluster exists
        ClusterRoot = 1 << 24, ///< Root of a cluster
        Native = 1 << 25,      ///< Native (UClass only).
        Async = 1 << 26, ///< Object exists only on a different thread than the
                         ///< game thread.
        AsyncLoading = 1 << 27, ///< Object is being asynchronously loaded.
        Unreachable = 1 << 28, ///< Object is not reachable on the object graph.
        PendingKill =
            1 << 29, ///< Objects that are pending destruction (invalid for
                     ///< gameplay but valid objects). This flag is mirrored in
                     ///< EObjectFlags as RF_PendingKill for performance
        RootSet = 1 << 30, ///< Object will not be garbage collected, even if
                           ///< unreferenced.
        PendingConstruction =
            1 << 31, ///< Object didn't have its class constructor called yet
                     ///< (only the UObjectBase one to initialize its most basic
                     ///< members)

        GarbageCollectionKeepFlags =
            Native | Async | AsyncLoading | LoaderImport,
    MirroredFlags =
            Garbage | PendingKill, /// Flags mirrored in EObjectFlags

        //~ Make sure this is up to date!
        AllFlags = LoaderImport | Garbage | PersistentGarbage |
                   ReachableInCluster | ClusterRoot | Native | Async |
                   AsyncLoading | Unreachable | PendingKill | RootSet |
                   PendingConstruction
};

/**
 * Single item in the UObject array.
 */
struct FUObjectItem {
        // Pointer to the allocated object
        class UObjectBase *Object;
        // Internal flags
        int32 Flags;
        // UObject Owner Cluster Index
        int32 ClusterRootIndex;
        // Weak Object Pointer Serial number associated with the object
        int32 SerialNumber;

        FORCEINLINE void SetOwnerIndex( int32 OwnerIndex ) {
                ClusterRootIndex = OwnerIndex;
        }

        FORCEINLINE int32 GetOwnerIndex() const { return ClusterRootIndex; }

        /** Encodes the cluster index in the ClusterRootIndex variable */
        FORCEINLINE void SetClusterIndex( int32 ClusterIndex ) {
                ClusterRootIndex = -ClusterIndex - 1;
        }

        /** Decodes the cluster index from the ClusterRootIndex variable */
        FORCEINLINE int32 GetClusterIndex() const {
                checkSlow( ClusterRootIndex < 0 );
                return -ClusterRootIndex - 1;
        }

        FORCEINLINE int32 GetSerialNumber() const { return SerialNumber; }

        FORCEINLINE EInternalObjectFlags GetFlags() const {
                return EInternalObjectFlags( Flags );
        }

        FORCEINLINE bool HasAnyFlags( EInternalObjectFlags InFlags ) const {
                return !!( Flags & int32( InFlags ) );
        }
        FORCEINLINE bool IsRootSet() const {
                return !!( Flags & int32( EInternalObjectFlags::RootSet ) );
        }

        FORCEINLINE void ResetSerialNumberAndFlags() {
                Flags = 0;
                ClusterRootIndex = 0;
                SerialNumber = 0;
        }
};

/**
 * Fixed size UObject array.
 */
class FFixedUObjectArray {
        /** Static master table to chunks of pointers **/
        FUObjectItem *Objects;
        /** Number of elements we currently have **/
        int32 MaxElements;
        /** Current number of UObject slots */
        int32 NumElements;

      public:
        FFixedUObjectArray() : Objects( nullptr ),
                                         MaxElements( 0 ),
                                         NumElements( 0 ) {}

        ~FFixedUObjectArray() { delete[] Objects; }

        /**
         * Expands the array so that Element[Index] is allocated. New pointers
         *are all zero.
         * @param Index The Index of an element we want to be sure is allocated
         **/
        void PreAllocate( int32 InMaxElements ) {
                check( !Objects );
                Objects = new FUObjectItem[InMaxElements];
                MaxElements = InMaxElements;
        }

        FORCEINLINE FUObjectItem const *
        GetObjectPtr( int32 Index ) const {
                check( Index >= 0 && Index < NumElements );
                return &Objects[Index];
        }

        FORCEINLINE FUObjectItem *GetObjectPtr( int32 Index ) {
                check( Index >= 0 && Index < NumElements );
                return &Objects[Index];
        }

        /**
         * Return the number of elements in the array
         * Thread safe, but you know, someone might have added more elements
         *before this even returns
         * @return	the number of elements in the array
         **/
        FORCEINLINE int32 Num() const { return NumElements; }

        /**
         * Return the number max capacity of the array
         * Thread safe, but you know, someone might have added more elements
         *before this even returns
         * @return	the maximum number of elements in the array
         **/
        FORCEINLINE int32 Capacity() const { return MaxElements; }

        /**
         * Return if this index is valid
         * Thread safe, if it is valid now, it is valid forever. Other threads
         *might be adding during this call.
         * @param	Index	Index to test
         * @return	true, if this is a valid
         **/
        FORCEINLINE bool IsValidIndex( int32 Index ) const {
                return Index < Num() && Index >= 0;
        }
        /**
         * Return a reference to an element
         * @param	Index	Index to return
         * @return	a reference to the pointer to the element
         * Thread safe, if it is valid now, it is valid forever. This might
         *return nullptr, but by then, some other thread might have made it
         *non-nullptr.
         **/
        FORCEINLINE FUObjectItem const &operator[]( int32 Index ) const {
                FUObjectItem const *ItemPtr = GetObjectPtr( Index );
                check( ItemPtr );
                return *ItemPtr;
        }

        FORCEINLINE FUObjectItem &operator[]( int32 Index ) {
                FUObjectItem *ItemPtr = GetObjectPtr( Index );
                check( ItemPtr );
                return *ItemPtr;
        }

        /**
         * Return a naked pointer to the fundamental data structure for debug
         *visualizers.
         **/
        UObjectBase ***GetRootBlockForDebuggerVisualizers() { return nullptr; }
};

/**
 * Simple array type that can be expanded without invalidating existing entries.
 * This is critical to thread safe FNames.
 * @param ElementType Type of the pointer we are storing in the array
 * @param MaxTotalElements absolute maximum number of elements this array can
 *ever hold
 * @param ElementsPerChunk how many elements to allocate in a chunk
 **/
class FChunkedFixedUObjectArray {
        enum {
                NumElementsPerChunk = 64 * 1024,
        };

        /** Master table to chunks of pointers **/
        FUObjectItem **Objects;
        /** If requested, a contiguous memory where all objects are allocated
         * **/
        FUObjectItem *PreAllocatedObjects;
        /** Maximum number of elements **/
        int32 MaxElements;
        /** Number of elements we currently have **/
        int32 NumElements;
        /** Maximum number of chunks **/
        int32 MaxChunks;
        /** Number of chunks we currently have **/
        int32 NumChunks;

      public:
        /** Constructor : Probably not thread safe **/
        FChunkedFixedUObjectArray()
            : Objects( nullptr ), PreAllocatedObjects( nullptr ),
              MaxElements( 0 ), NumElements( 0 ), MaxChunks( 0 ),
              NumChunks( 0 ) {}

        ~FChunkedFixedUObjectArray() {
                if ( !PreAllocatedObjects ) {
                        for ( int32 ChunkIndex = 0; ChunkIndex < MaxChunks;
                              ++ChunkIndex ) {
                                delete[] Objects[ChunkIndex];
                        }
                } else {
                        delete[] PreAllocatedObjects;
                }
                delete[] Objects;
        }

        /**
         * Expands the array so that Element[Index] is allocated. New pointers
         *are all zero.
         * @param Index The Index of an element we want to be sure is allocated
         **/
        void PreAllocate( int32 InMaxElements, bool bPreAllocateChunks ) {
                check( !Objects );
                MaxChunks = InMaxElements / NumElementsPerChunk + 1;
                MaxElements = MaxChunks * NumElementsPerChunk;
                Objects = new FUObjectItem *[MaxChunks];
                ZeroMemory( Objects, sizeof( FUObjectItem * ) * MaxChunks );
                if ( bPreAllocateChunks ) {
                        // Fully allocate all chunks as contiguous memory
                        PreAllocatedObjects = new FUObjectItem[MaxElements];
                        for ( int32 ChunkIndex = 0; ChunkIndex < MaxChunks;
                              ++ChunkIndex ) {
                                Objects[ChunkIndex] =
                                    PreAllocatedObjects +
                                    ChunkIndex * NumElementsPerChunk;
                        }
                        NumChunks = MaxChunks;
                }
        }

        /**
         * Return the number of elements in the array
         * Thread safe, but you know, someone might have added more elements
         *before this even returns
         * @return	the number of elements in the array
         **/
        FORCEINLINE int32 Num() const { return NumElements; }

        /**
         * Return the number max capacity of the array
         * Thread safe, but you know, someone might have added more elements
         *before this even returns
         * @return	the maximum number of elements in the array
         **/
        FORCEINLINE int32 Capacity() const { return MaxElements; }

        /**
         * Return if this index is valid
         * Thread safe, if it is valid now, it is valid forever. Other threads
         *might be adding during this call.
         * @param	Index	Index to test
         * @return	true, if this is a valid
         **/
        FORCEINLINE bool IsValidIndex( int32 Index ) const {
                return Index < Num() && Index >= 0;
        }

        /**
         * Return a pointer to the pointer to a given element
         * @param Index The Index of an element we want to retrieve the
         *pointer-to-pointer for
         **/
        FUObjectItem *
        GetObjectPtr( int32 Index ) const {
                const int32 ChunkIndex = Index / NumElementsPerChunk;
                const int32 WithinChunkIndex = Index % NumElementsPerChunk;
                checkf( IsValidIndex( Index ), TEXT( "IsValidIndex(%d)" ),
                        Index );
                checkf( ChunkIndex < NumChunks,
                        TEXT( "ChunkIndex (%d) < NumChunks (%d)" ), ChunkIndex,
                        NumChunks );
                checkf( Index < MaxElements,
                        TEXT( "Index (%d) < MaxElements (%d)" ), Index,
                        MaxElements );
                FUObjectItem *Chunk = Objects[ChunkIndex];
                check( Chunk );
                return Chunk + WithinChunkIndex;
        }
        FUObjectItem *
        GetObjectPtr( int32 Index ) {
                const int32 ChunkIndex = Index / NumElementsPerChunk;
                const int32 WithinChunkIndex = Index % NumElementsPerChunk;
                checkf( IsValidIndex( Index ), TEXT( "IsValidIndex(%d)" ),
                        Index );
                checkf( ChunkIndex < NumChunks,
                        TEXT( "ChunkIndex (%d) < NumChunks (%d)" ), ChunkIndex,
                        NumChunks );
                checkf( Index < MaxElements,
                        TEXT( "Index (%d) < MaxElements (%d)" ), Index,
                        MaxElements );
                FUObjectItem *Chunk = Objects[ChunkIndex];
                check( Chunk );
                return Chunk + WithinChunkIndex;
        }

        /**
         * Return a reference to an element
         * @param	Index	Index to return
         * @return	a reference to the pointer to the element
         * Thread safe, if it is valid now, it is valid forever. This might
         *return nullptr, but by then, some other thread might have made it
         *non-nullptr.
         **/
        FORCEINLINE FUObjectItem const &operator[]( int32 Index ) const {
                FUObjectItem const *ItemPtr = GetObjectPtr( Index );
                check( ItemPtr );
                return *ItemPtr;
        }
        FORCEINLINE FUObjectItem &operator[]( int32 Index ) {
                FUObjectItem *ItemPtr = GetObjectPtr( Index );
                check( ItemPtr );
                return *ItemPtr;
        }

        /**
         * Return a naked pointer to the fundamental data structure for debug
         *visualizers.
         **/
        FUObjectItem ***GetRootBlockForDebuggerVisualizers() { return nullptr; }

        int64 GetAllocatedSize() const {
                return MaxChunks * sizeof( FUObjectItem * ) +
                       NumChunks * NumElementsPerChunk * sizeof( FUObjectItem );
        }
};

class FUObjectArray {


      public:
        FORCEINLINE int32 ObjectToIndex( const UObject *Object ) const {
                return Object->InternalIndex;
        }

        FUObjectArray() = default;

        FUObjectArray(void* ObjObjectsPtr, bool IsChunked)
        {
                m_ObjObjects = ObjObjectsPtr;
                m_Chunked = IsChunked;
        }

      private:
        void *m_ObjObjects;
        bool m_Chunked;

      public:
        void *GetObjObjects() { return m_ObjObjects; };
        void *GetObjObjects() const { return m_ObjObjects; };

        /**
         * Returns the UObject corresponding to index. Be advised this is only
         * for very low level use.
         *
         * @param Index index of object to return
         * @return Object at this index
         */
        FORCEINLINE FUObjectItem *IndexToObject( int32 Index ) {
                check( Index >= 0 );
                void *ObjObjects = GetObjObjects();
                if ( m_Chunked ) {
                        FChunkedFixedUObjectArray *chunkedObjObjects =
                            (FChunkedFixedUObjectArray *)ObjObjects;
                        if ( Index < chunkedObjObjects->Num() ) {
                                return chunkedObjObjects->GetObjectPtr( Index );
                        }
                } else {
                        FFixedUObjectArray *FixedObjObjects =
                            (FFixedUObjectArray *)ObjObjects;
                        if ( Index < FixedObjObjects->Num() ) {
                                return FixedObjObjects->GetObjectPtr( Index );
                        }
                }

                return nullptr;
        }

        FORCEINLINE FUObjectItem *IndexToObject( int32 Index ) const {
                check( Index >= 0 );
                void *ObjObjects = GetObjObjects();
                if ( m_Chunked ) {
                        FChunkedFixedUObjectArray *chunkedObjObjects =
                            (FChunkedFixedUObjectArray *)ObjObjects;
                        if ( Index < chunkedObjObjects->Num() ) {
                                return chunkedObjObjects->GetObjectPtr( Index );
                        }
                } else {
                        FFixedUObjectArray *FixedObjObjects =
                            (FFixedUObjectArray *)ObjObjects;
                        if ( Index < FixedObjObjects->Num() ) {
                                return FixedObjObjects->GetObjectPtr( Index );
                        }
                }

                return nullptr;
        }

        int64 GetAllocatedSize() const {
                void *ObjObjects = GetObjObjects();
                if ( m_Chunked ) {
                        FChunkedFixedUObjectArray *chunkedObjObjects =
                            (FChunkedFixedUObjectArray *)ObjObjects;
                        return chunkedObjObjects->GetAllocatedSize(); 
                } else {
                        printf( "[FortSDK] Illegal Call on %s on a version "
                                "with FixedObjects",
                                __FUNCTION__ );
                }

                return 0;
        }

        FORCEINLINE int32 GetObjectArrayNum() const
        {
                void *ObjObjects = GetObjObjects();
                if ( m_Chunked ) {
                        FChunkedFixedUObjectArray *chunkedObjObjects =
                            (FChunkedFixedUObjectArray *)ObjObjects;
                        return chunkedObjObjects->Num();
                } else {
                        FFixedUObjectArray *FixedObjObjects =
                            (FFixedUObjectArray *)ObjObjects;
                        return FixedObjObjects->Num();
                }

                return 0;
        }

        FORCEINLINE UObject *FindObject( std::string Name ) const {
                for ( int i = 0; i < this->GetObjectArrayNum(); i++ ) {
                        FUObjectItem* ObjectItem = this->IndexToObject(i);
                        if ( !ObjectItem )
                                continue;
                        UObjectBase *Object = ObjectItem->Object;
                        if ( !Object )
                                continue;
                        if ( Object->GetFName().ToString().ToString() == Name )
                                return (UObject*)Object;
                }
                return nullptr;
        }

        	/**
         * Low level iterator.
         */
        class TIterator {
              public:
                enum EEndTagType { EndTag };

                /**
                 * Constructor
                 *
                 * @param	InArray				the array to iterate on
                 * @param	bOnlyGCedObjects	if true, skip all of the
                 * permanent objects
                 */

                /**
                 * Constructor
                 *
                 * @param	InArray				the array to iterate on
                 * @param	bOnlyGCedObjects	if true, skip all of the
                 * permanent objects
                 */
                TIterator( EEndTagType, const TIterator &InIter )
                    : Array( InIter.Array ) {
                        void *ObjObjects = Array.m_ObjObjects;
                        if ( Array.m_Chunked ) {
                                FChunkedFixedUObjectArray *ChunkedArray =
                                    reinterpret_cast<
                                        FChunkedFixedUObjectArray *>(
                                        ObjObjects );
                                Index = ChunkedArray->Num();
                                CurrentObject = ChunkedArray->GetObjectPtr( 0 );
                        } else {
                                FFixedUObjectArray *FixedArray =
                                    reinterpret_cast<FFixedUObjectArray *>(
                                        ObjObjects );
                                Index = FixedArray->Num();
                                CurrentObject = FixedArray->GetObjectPtr( 0 );
                        }
                }

                /**
                 * Iterator advance
                 */
                FORCEINLINE void operator++() { Advance(); }

                friend bool operator==( const TIterator &Lhs,
                                        const TIterator &Rhs ) {
                        return Lhs.Index == Rhs.Index;
                }
                friend bool operator!=( const TIterator &Lhs,
                                        const TIterator &Rhs ) {
                        return Lhs.Index != Rhs.Index;
                }

                /** Conversion to "bool" returning true if the iterator is
                 * valid. */
                FORCEINLINE explicit operator bool() const {
                        return !!CurrentObject;
                }
                /** inverse of the "bool" operator */
                FORCEINLINE bool operator!() const { return !(bool)*this; }

                FORCEINLINE int32 GetIndex() const { return Index; }

              protected:
                /**
                 * Dereferences the iterator with an ordinary name for clarity
                 * in derived classes
                 *
                 * @return	the UObject at the iterator
                 */
                FORCEINLINE FUObjectItem *GetObject() const {
                        return CurrentObject;
                }
                /**
                 * Iterator advance with ordinary name for clarity in subclasses
                 * @return	true if the iterator points to a valid object, false
                 * if iteration is complete
                 */
                FORCEINLINE bool Advance() {
                        //@todo UE check this for LHS on Index on consoles
                        FUObjectItem *NextObject = nullptr;
                        CurrentObject = nullptr;
                        while ( ++Index < Array.GetObjectArrayNum() ) {

                                NextObject = Array.IndexToObject( Index );
                                if ( NextObject->Object ) {
                                        CurrentObject = NextObject;
                                        return true;
                                }
                        }
                        return false;
                }

                /** Gets the array this iterator iterates over */
                const FUObjectArray &GetIteratedArray() const { return Array; }

              private:
                /** the array that we are iterating on, probably always
                 * GUObjectArray */
                const FUObjectArray &Array;
                /** index of the current element in the object array */
                int32 Index;
                /** Current object */
                mutable FUObjectItem *CurrentObject;
        };

        /**
         * Given a UObject index return the serial number. If it doesn't have a
         * serial number, return 0. Threadsafe.
         * @param Index - UObject Index
         * @return - the serial number for this UObject
         */
        FORCEINLINE int32 GetSerialNumber( int32 Index ) {
                FUObjectItem *ObjectItem = IndexToObject( Index );
                checkSlow( ObjectItem );
                return ObjectItem->GetSerialNumber();
        }

        FORCEINLINE FUObjectItem *
        ObjectToObjectItem( const UObjectBase *Object ) {
                FUObjectItem *ObjectItem =
                    IndexToObject( Object->InternalIndex );
                return ObjectItem;
        }
};
}

template<typename T> T* Cast(SDK::UObject* Base) {
        if ( Base->IsA( T::StaticClass() ) ) {
                return static_cast<T *>( Base );
        }

        return nullptr;
}

static bool GetBool( void *Obj, const SDK::FBoolProperty *Prop ) {
        uint8_t *Byte = reinterpret_cast<uint8_t *>( Obj ) + Prop->Offset_Internal;
        return ( *Byte & Prop->FieldMask ) != 0;
}

static void SetBool( void *Obj, const SDK::FBoolProperty *Prop, bool b ) {
        uint8_t *Byte = reinterpret_cast<uint8_t *>( Obj ) + Prop->Offset_Internal;
        *Byte = b ? ( *Byte | Prop->FieldMask ) : ( *Byte & ~Prop->FieldMask );
}

static bool GetBool( void *Obj, const SDK::UBoolProperty *Prop ) {
        uint8_t *Byte =
            reinterpret_cast<uint8_t *>( Obj ) + Prop->Offset_Internal;
        return ( *Byte & Prop->FieldMask ) != 0;
}

static void SetBool( void *Obj, const SDK::UBoolProperty *Prop, bool b ) {
        uint8_t *Byte =
            reinterpret_cast<uint8_t *>( Obj ) + Prop->Offset_Internal;
        *Byte = b ? ( *Byte | Prop->FieldMask ) : ( *Byte & ~Prop->FieldMask );
}

#define MEMBER_PTR( RetType, MemberName, Offset )                       \
        RetType MemberName() const {                                    \
                return *reinterpret_cast<RetType *>(                           \
                    reinterpret_cast<uintptr_t>( this ) + Offset );            \
        }

#define DEFINE_BOOL( ClassName, PropName )                                     \
        inline bool Get##PropName() {                                          \
                if ( SDK::UEChanges::bUsesFPropertySystem ) {                  \
                        SDK::FBoolProperty *BoolProp =                         \
                            static_cast<SDK::FBoolProperty *>(                 \
                                SDK::PropLibrary                               \
                                    ->GetPropertyByName( (#ClassName + 1),            \
                                                        #PropName )            \
                                    .Prop );                                   \
                        return GetBool( this, BoolProp );                 \
                } else {                                                       \
                        SDK::UBoolProperty *BoolProp =                         \
                            static_cast<SDK::UBoolProperty *>(                 \
                                SDK::PropLibrary                               \
                                    ->GetPropertyByName( (#ClassName + 1),            \
                                                        #PropName )            \
                                    .Prop );                                   \
                        return GetBool( this, BoolProp );                 \
                }                                                              \
        } \
        inline void Set##PropName( bool bValue ) {                             \
                if ( SDK::UEChanges::bUsesFPropertySystem ) {                  \
                        SDK::FBoolProperty *BoolProp =                         \
                            static_cast<SDK::FBoolProperty *>(                 \
                                SDK::PropLibrary                               \
                                    ->GetPropertyByName( (#ClassName + 1),            \
                                                        #PropName )            \
                                    .Prop );                                   \
                        SetBool( this, BoolProp, bValue );                \
                } else {                                                       \
                        SDK::UBoolProperty *BoolProp =                         \
                            static_cast<SDK::UBoolProperty *>(                 \
                                SDK::PropLibrary                               \
                                    ->GetPropertyByName( (#ClassName + 1),            \
                                                        #PropName )            \
                                    .Prop );                                   \
                        SetBool( this, BoolProp, bValue );                \
                }                                                              \
        }

#define DEFINE_MEMBER( Ret, ClassName, PropName )                              \
        inline Ret &Get##PropName() {                                          \
                static int PropOffset = -1;                                    \
                if ( PropOffset == -1 ) {                                      \
                        PropOffset = SDK::PropLibrary                          \
                                         ->GetPropertyByName(                  \
                                             ( #ClassName + 1 ), #PropName )   \
                                         .Offset;                              \
                }                                                              \
                return *reinterpret_cast<Ret *>( uintptr_t( this ) +           \
                                                 PropOffset );                 \
        }                                                                      \
        inline void Set##PropName( Ret Value ) {                               \
                static int PropOffset = -1;                                    \
                if ( PropOffset == -1 ) {                                      \
                        PropOffset = SDK::PropLibrary                          \
                                         ->GetPropertyByName(                  \
                                             ( #ClassName + 1 ), #PropName )   \
                                         .Offset;                              \
                }                                                              \
                *reinterpret_cast<Ret *>( uintptr_t( this ) + PropOffset ) =   \
                    Value;                                                     \
        }

#define DEFINE_PTR( Type, ClassName, PropName )                                \
        inline Type *Get##PropName() {                                         \
                static int PropOffset = -1;                                    \
                if ( PropOffset == -1 ) {                                      \
                        PropOffset = SDK::PropLibrary                          \
                                         ->GetPropertyByName(                  \
                                             ( #ClassName + 1 ), #PropName )   \
                                         .Offset;                              \
                }                                                              \
                return *reinterpret_cast<Type **>( uintptr_t( this ) +          \
                                                  PropOffset );                \
        }                                                                      \
        inline void Set##PropName( Type* Value ) {                              \
                static int PropOffset = -1;                                    \
                if ( PropOffset == -1 ) {                                      \
                        PropOffset = SDK::PropLibrary                          \
                                         ->GetPropertyByName(                  \
                                             ( #ClassName + 1 ), #PropName )   \
                                         .Offset;                              \
                }                                                              \
                *reinterpret_cast<Type **>( uintptr_t( this ) + PropOffset ) =  \
                    Value;                                                     \
        }

