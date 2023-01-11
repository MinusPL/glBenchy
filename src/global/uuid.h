#ifndef SHAREDGLOBALS_H
#define SHAREDGLOBALS_H

#include <cstdint>
#include <string>
#define UUID_SYSTEM_GENERATOR
#include "../uuid/uuid.h"

typedef uuids::uuid UUID;

class UUIDGenerator
{
public:
    static inline UUID GetUUID()
    {
        return uuids::uuid_system_generator{}();
    }

    static inline std::string UUIDToString(UUID id)
    {
        return uuids::to_string(id);
    }
};



#endif