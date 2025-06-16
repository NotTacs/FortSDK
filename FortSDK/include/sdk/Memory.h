#pragma once

enum {
        // Default allocator alignment. If the default is specified, the
        // allocator applies to engine rules. Blocks >= 16 bytes will be
        // 16-byte-aligned, Blocks < 16 will be 8-byte aligned. If the allocator
        // does not support allocation alignment, the alignment will be ignored.
        DEFAULT_ALIGNMENT = 0,

        // Minimum allocator alignment
        MIN_ALIGNMENT = 8,
};

namespace SDK
{
class FMemory {
      public:
          static void* Realloc(void* Original, UINT64 Size,
                              UINT32 Alignment = 8 ) {
                static void* ( *InternalRealloc )( void *Original, UINT64 Size,
                                                  UINT32 Alignment ) = decltype(InternalRealloc)(Offsets::FMemory__Realloc);
                  return InternalRealloc( Original, Size, Alignment );
          }
};
}


