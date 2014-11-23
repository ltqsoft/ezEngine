#include <ToolsFoundation/PCH.h>
#include <ToolsFoundation/Reflection/ReflectedType.h>
#include <ToolsFoundation/Reflection/ToolsReflectionUtils.h>
#include <Foundation/Configuration/Startup.h>


////////////////////////////////////////////////////////////////////////
// ezToolsReflectionUtils public functions
////////////////////////////////////////////////////////////////////////

ezVariant ezToolsReflectionUtils::GetDefaultVariantFromType(ezVariant::Type::Enum type)
{
  switch (type)
  {
  case ezVariant::Type::Invalid:
    return ezVariant();
  case ezVariant::Type::Bool:
    return ezVariant(false);
  case ezVariant::Type::Int8:
    return ezVariant((ezInt8)0);
  case ezVariant::Type::UInt8:
    return ezVariant((ezUInt8)0);
  case ezVariant::Type::Int16:
    return ezVariant((ezInt16)0);
  case ezVariant::Type::UInt16:
    return ezVariant((ezUInt16)0);
  case ezVariant::Type::Int32:
    return ezVariant((ezInt32)0);
  case ezVariant::Type::UInt32:
    return ezVariant((ezUInt32)0);
  case ezVariant::Type::Int64:
    return ezVariant((ezInt64)0);
  case ezVariant::Type::UInt64:
    return ezVariant((ezUInt64)0);
  case ezVariant::Type::Float:
    return ezVariant(0.0f);
  case ezVariant::Type::Double:
    return ezVariant(0.0);
  case ezVariant::Type::Color:
    return ezVariant(ezColor(1.0f, 1.0f, 1.0f));
  case ezVariant::Type::Vector2:
    return ezVariant(ezVec2(0.0f, 0.0f));
  case ezVariant::Type::Vector3:
    return ezVariant(ezVec3(0.0f, 0.0f, 0.0f));
  case ezVariant::Type::Vector4:
    return ezVariant(ezVec4(0.0f, 0.0f, 0.0f, 0.0f));
  case ezVariant::Type::Quaternion:
    return ezVariant(ezQuat(0.0f, 0.0f, 0.0f, 1.0f));
  case ezVariant::Type::Matrix3:
    return ezVariant(ezMat3::IdentityMatrix());
  case ezVariant::Type::Matrix4:
    return ezVariant(ezMat4::IdentityMatrix());
  case ezVariant::Type::String:
    return ezVariant("");
  case ezVariant::Type::Time:
    return ezVariant(ezTime());
  case ezVariant::Type::Uuid:
    return ezVariant(ezUuid());
  case ezVariant::Type::VariantArray:
    return ezVariant();
  case ezVariant::Type::VariantDictionary:
    return ezVariant();
  case ezVariant::Type::ReflectedPointer:
    return ezVariant();
  case ezVariant::Type::VoidPointer:
    return ezVariant();
  case ezVariant::Type::ENUM_COUNT:
    EZ_REPORT_FAILURE("Invalid case statement");
    return ezVariant();
  }
  return ezVariant();
}

void ezToolsReflectionUtils::GetReflectedTypeDescriptorFromRtti(const ezRTTI* pRtti, ezReflectedTypeDescriptor& out_desc)
{
  EZ_ASSERT(pRtti != nullptr, "Type to process must not be null!");
  out_desc.m_sTypeName = pRtti->GetTypeName();
  out_desc.m_sPluginName = pRtti->GetPluginName();
  const ezRTTI* pParentRtti = pRtti->GetParentType();
  out_desc.m_sParentTypeName = pParentRtti ? pParentRtti->GetTypeName() : nullptr;

  out_desc.m_Properties.Clear();
  const ezArrayPtr<ezAbstractProperty*>& rttiProps = pRtti->GetProperties();
  const ezUInt32 uiCount = rttiProps.GetCount();
  out_desc.m_Properties.Reserve(uiCount);

  for (ezUInt32 i = 0; i < uiCount; ++i)
  {
    ezAbstractProperty* prop = rttiProps[i];
    
    switch (prop->GetCategory())
    {
    case ezAbstractProperty::Member:
      {
        ezAbstractMemberProperty* memberProp = static_cast<ezAbstractMemberProperty*>(prop);
        const ezRTTI* pMemberPropRtti = memberProp->GetPropertyType();
        ezVariant::Type::Enum memberType = pMemberPropRtti->GetVariantType();

        ezBitflags<PropertyFlags> memberFlags;
        if (memberProp->IsReadOnly())
          memberFlags.Add(PropertyFlags::IsReadOnly);

        if (memberType == ezVariant::Type::Invalid || memberType == ezVariant::Type::ReflectedPointer)
        {
          out_desc.m_Properties.PushBack(ezReflectedPropertyDescriptor(memberProp->GetPropertyName(), pMemberPropRtti->GetTypeName(), memberFlags));
        }
        else if (memberType >= ezVariant::Type::Bool && memberType <= ezVariant::Type::Uuid)
        {
          memberFlags.Add(PropertyFlags::IsPOD);
          out_desc.m_Properties.PushBack(ezReflectedPropertyDescriptor(memberProp->GetPropertyName(), memberType, memberFlags));
        }
        else
        {
          EZ_ASSERT(false, "Arrays and pointers are not supported yet!");
        }
      }
      break;
    case ezAbstractProperty::Function:
    case ezAbstractProperty::Array:
      break;
    }
  }
}

ezPropertyPath ezToolsReflectionUtils::CreatePropertyPath(const char* pData1, const char* pData2, const char* pData3, const char* pData4, const char* pData5, const char* pData6)
{
  const ezUInt32 uiMaxParams = 6;
  const char* pStrings[uiMaxParams] = { pData1, pData2, pData3, pData4, pData5, pData6 };
  ezUInt32 uiUsedParams = 0;
  for (ezUInt32 i = 0; i < uiMaxParams; ++i)
  {
    if (ezStringUtils::IsNullOrEmpty(pStrings[i]))
      break;

    uiUsedParams++;
  }

  return ezPropertyPath(ezArrayPtr<const char*>(pStrings, uiUsedParams));
}

ezAbstractMemberProperty* ezToolsReflectionUtils::GetMemberPropertyByPath(const ezRTTI*& inout_pRtti, void*& inout_pData, const ezPropertyPath& path)
{
  EZ_ASSERT(path.GetCount() > 0, "ezReflectedTypeDirectAccessor: the given property path is empty!");

  ezAbstractMemberProperty* pCurrentProp = ezReflectionUtils::GetMemberProperty(inout_pRtti, path[0]);

  // Find pointer that contains the final data property.
  const ezUInt32 uiPathSize = path.GetCount();
  for (ezUInt32 i = 1; i < uiPathSize; ++i)
  {
    if (pCurrentProp == nullptr)
      return nullptr;

    inout_pRtti = pCurrentProp->GetPropertyType();
    inout_pData = pCurrentProp->GetPropertyPointer(inout_pData);
    if (inout_pData == nullptr)
      return nullptr;
    
    pCurrentProp = ezReflectionUtils::GetMemberProperty(inout_pRtti, path[i]);
  }

  // pCurrentRtti should now be a type that contains the property that we can access directly.
  // pData should point to the object of type pCurrentRtti.
  // pCurrentProp should be a property defined in 'pCurrentRtti' and it should be a type that is handled by ezVariant.
  return pCurrentProp;
}

void ezToolsReflectionUtils::GetPropertyPathFromString(const char* szPath, ezPropertyPath& out_Path, ezHybridArray<ezString, 6>& out_Storage)
{
  ezStringBuilder temp = szPath;
  temp.Split(false, out_Storage, "/");

  const ezUInt32 uiCount = out_Storage.GetCount();

  for (ezUInt32 i = 0; i < uiCount; i++)
  {
    out_Path.PushBack(out_Storage[i]);
  }
}

ezString ezToolsReflectionUtils::GetStringFromPropertyPath(const ezPropertyPath& Path)
{
  EZ_ASSERT(Path.GetCount() > 0, "Path must not be empty");

  ezStringBuilder pathBuilder(Path[0]);

  const ezUInt32 pathLength = Path.GetCount();
  for (ezUInt32 i = 1; i < pathLength; ++i)
  {
    pathBuilder.Append("/", Path[i]);
  }

  return pathBuilder;
}

