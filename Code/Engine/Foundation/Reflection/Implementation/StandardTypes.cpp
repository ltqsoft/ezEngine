#include <Foundation/PCH.h>
#include <Foundation/Reflection/Implementation/StandardTypes.h>
#include <Foundation/Reflection/Reflection.h>
#include <Foundation/Math/Mat3.h>
#include <Foundation/Math/Mat4.h>
#include <Foundation/Time/Time.h>

EZ_BEGIN_STATIC_REFLECTED_TYPE(ezEnumBase, ezNoBase, 1, ezRTTINoAllocator);
EZ_END_STATIC_REFLECTED_TYPE();

EZ_BEGIN_STATIC_REFLECTED_TYPE(ezBitflagsBase, ezNoBase, 1, ezRTTINoAllocator);
EZ_END_STATIC_REFLECTED_TYPE();

EZ_BEGIN_DYNAMIC_REFLECTED_TYPE(ezReflectedClass, ezNoBase, 1, ezRTTINoAllocator);
EZ_END_DYNAMIC_REFLECTED_TYPE();

// *********************************************
// ***** Standard POD Types for Properties *****

EZ_BEGIN_STATIC_REFLECTED_TYPE(bool, ezNoBase, 1, ezRTTINoAllocator);
EZ_END_STATIC_REFLECTED_TYPE();

EZ_BEGIN_STATIC_REFLECTED_TYPE(float, ezNoBase, 1, ezRTTINoAllocator);
EZ_END_STATIC_REFLECTED_TYPE();

EZ_BEGIN_STATIC_REFLECTED_TYPE(double, ezNoBase, 1, ezRTTINoAllocator);
EZ_END_STATIC_REFLECTED_TYPE();

EZ_BEGIN_STATIC_REFLECTED_TYPE(ezInt8, ezNoBase, 1, ezRTTINoAllocator);
EZ_END_STATIC_REFLECTED_TYPE();

EZ_BEGIN_STATIC_REFLECTED_TYPE(ezUInt8, ezNoBase, 1, ezRTTINoAllocator);
EZ_END_STATIC_REFLECTED_TYPE();

EZ_BEGIN_STATIC_REFLECTED_TYPE(ezInt16, ezNoBase, 1, ezRTTINoAllocator);
EZ_END_STATIC_REFLECTED_TYPE();

EZ_BEGIN_STATIC_REFLECTED_TYPE(ezUInt16, ezNoBase, 1, ezRTTINoAllocator);
EZ_END_STATIC_REFLECTED_TYPE();

EZ_BEGIN_STATIC_REFLECTED_TYPE(ezInt32, ezNoBase, 1, ezRTTINoAllocator);
EZ_END_STATIC_REFLECTED_TYPE();

EZ_BEGIN_STATIC_REFLECTED_TYPE(ezUInt32, ezNoBase, 1, ezRTTINoAllocator);
EZ_END_STATIC_REFLECTED_TYPE();

EZ_BEGIN_STATIC_REFLECTED_TYPE(ezInt64, ezNoBase, 1, ezRTTINoAllocator);
EZ_END_STATIC_REFLECTED_TYPE();

EZ_BEGIN_STATIC_REFLECTED_TYPE(ezUInt64, ezNoBase, 1, ezRTTINoAllocator);
EZ_END_STATIC_REFLECTED_TYPE();

EZ_BEGIN_STATIC_REFLECTED_TYPE(ezConstCharPtr, ezNoBase, 1, ezRTTINoAllocator);
EZ_END_STATIC_REFLECTED_TYPE();

EZ_BEGIN_STATIC_REFLECTED_TYPE(ezTime, ezNoBase, 1, ezRTTINoAllocator);
EZ_END_STATIC_REFLECTED_TYPE();

EZ_BEGIN_STATIC_REFLECTED_TYPE(ezColor, ezNoBase, 1, ezRTTINoAllocator);
  EZ_BEGIN_PROPERTIES
    EZ_MEMBER_PROPERTY("r", r),
    EZ_MEMBER_PROPERTY("g", g),
    EZ_MEMBER_PROPERTY("b", b),
    EZ_MEMBER_PROPERTY("a", a)
  EZ_END_PROPERTIES
EZ_END_STATIC_REFLECTED_TYPE();

EZ_BEGIN_STATIC_REFLECTED_TYPE(ezVec2, ezNoBase, 1, ezRTTINoAllocator);
  EZ_BEGIN_PROPERTIES
    EZ_MEMBER_PROPERTY("x", x),
    EZ_MEMBER_PROPERTY("y", y)
  EZ_END_PROPERTIES
EZ_END_STATIC_REFLECTED_TYPE();

EZ_BEGIN_STATIC_REFLECTED_TYPE(ezVec3, ezNoBase, 1, ezRTTINoAllocator);
  EZ_BEGIN_PROPERTIES
    EZ_MEMBER_PROPERTY("x", x),
    EZ_MEMBER_PROPERTY("y", y),
    EZ_MEMBER_PROPERTY("z", z)
  EZ_END_PROPERTIES
EZ_END_STATIC_REFLECTED_TYPE();

EZ_BEGIN_STATIC_REFLECTED_TYPE(ezVec4, ezNoBase, 1, ezRTTINoAllocator);
  EZ_BEGIN_PROPERTIES
    EZ_MEMBER_PROPERTY("x", x),
    EZ_MEMBER_PROPERTY("y", y),
    EZ_MEMBER_PROPERTY("z", z),
    EZ_MEMBER_PROPERTY("w", w)
  EZ_END_PROPERTIES
EZ_END_STATIC_REFLECTED_TYPE();

EZ_BEGIN_STATIC_REFLECTED_TYPE(ezQuat, ezNoBase, 1, ezRTTINoAllocator);
  EZ_BEGIN_PROPERTIES
    EZ_MEMBER_PROPERTY("v", v),
    EZ_MEMBER_PROPERTY("w", w)
  EZ_END_PROPERTIES
EZ_END_STATIC_REFLECTED_TYPE();

EZ_BEGIN_STATIC_REFLECTED_TYPE(ezMat3, ezNoBase, 1, ezRTTINoAllocator);
EZ_END_STATIC_REFLECTED_TYPE();

EZ_BEGIN_STATIC_REFLECTED_TYPE(ezMat4, ezNoBase, 1, ezRTTINoAllocator);
EZ_END_STATIC_REFLECTED_TYPE();

EZ_BEGIN_STATIC_REFLECTED_TYPE(ezUuid, ezNoBase, 1, ezRTTINoAllocator);
EZ_END_STATIC_REFLECTED_TYPE();

EZ_BEGIN_STATIC_REFLECTED_TYPE(ezVariant, ezNoBase, 1, ezRTTINoAllocator);
EZ_END_STATIC_REFLECTED_TYPE();

EZ_STATICLINK_FILE(Foundation, Foundation_Reflection_Implementation_StandardTypes);

