#pragma once

#include "Interfaces/IBufferResource.h"
#include "Interfaces/IGraphicsDevice.h"
#include "Interfaces/ICommandList.h"
#include "Interfaces/IDescriptorHeap.h"

// Interface Aliases
using IAGraphicsDevice = Artemis::Renderer::Interfaces::IGraphicsDevice;
using IADescriptorHeap = Artemis::Renderer::Interfaces::IDescriptorHeap;
using IACommandList = Artemis::Renderer::Interfaces::ICommandList;
using IABufferResource = Artemis::Renderer::Interfaces::IBufferResource;
using IAGpuBufferResource = Artemis::Renderer::Interfaces::IGpuBufferResource;

// Enum Aliases
using EAResourceFlags = Artemis::Renderer::Interfaces::ResourceFlags;
