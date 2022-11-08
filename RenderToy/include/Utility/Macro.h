#pragma once

#include <CDX12/DescriptorHeapMngr.h>
#include <CDX12/RenderResourceMngr.h>
#include <ObjectMngr/ObjectMngr.h>
#include <PropertyMngr/PropertyMngr.h>
#include <PropertyMngr/Mesh.h>
#include <PropertyMngr/Transform.h>
#include <PropertyMngr/Material.h>
#include <Utility/GlobalParam.h>

// ******************************************************************************
// Get Global Singleton Mngr

#define GetDescriptorHeapMngr() Chen::CDX12::DescriptorHeapMngr::GetInstance()
#define GetRenderRsrcMngr()     Chen::CDX12::RenderResourceMngr::GetInstance()
#define GetObjectMngr()         Chen::RToy::ObjectMngr::GetInstance()
#define GetPropertyMngr()       Chen::RToy::PropertyMngr::GetInstance()

// Global Param Pass and Save Helper
#define GetGlobalParam()        Chen::RToy::GlobalParam::GetInstance()

// ******************************************************************************


// ******************************************************************************
// Get the pointer to specific property of a object

#define GetTransformOfObj(ptr)        static_cast<Chen::RToy::Transform*>(ptr->GetProperty("Transform"))
#define GetMaterialOfObj(ptr)         static_cast<Chen::RToy::Material*>(ptr->GetProperty("Material"))
#define GetMeshOfObj(ptr)             static_cast<Chen::RToy::Mesh*>(ptr->GetProperty("Mesh"))

#define GetTransformOfObjByID(id)     static_cast<Chen::RToy::Transform*>(GetObjectMngr().GetObj(id)->GetProperty("Transform"))
#define GetMaterialOfObjByID(id)      static_cast<Chen::RToy::Material*>(GetObjectMngr().GetObj(id)->GetProperty("Material"))
#define GetMeshOfObjByID(id)          static_cast<Chen::RToy::Mesh*>(GetObjectMngr().GetObj(id)->GetProperty("Mesh"))

#define GetTransformOfObjByName(name) static_cast<Chen::RToy::Transform*>(GetObjectMngr().GetObj(name)->GetProperty("Transform"))
#define GetMaterialOfObjByName(name)  static_cast<Chen::RToy::Material*>(GetObjectMngr().GetObj(name)->GetProperty("Material"))
#define GetMeshOfObjByName(name)      static_cast<Chen::RToy::Mesh*>(GetObjectMngr().GetObj(name)->GetProperty("Mesh"))

// ******************************************************************************
