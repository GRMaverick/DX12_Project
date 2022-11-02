#pragma once

namespace Artemis::Renderer::Interfaces
{
	class IGraphicsDevice;

	enum DescriptorHeapType
	{
		DescriptorHeapType_CbvSrvUav = 0,
		DescriptorHeapType_Sampler = (DescriptorHeapType_CbvSrvUav + 1),
		DescriptorHeapType_Rtv = (DescriptorHeapType_Sampler + 1),
		DescriptorHeapType_Dsv = (DescriptorHeapType_Rtv + 1),
		DescriptorHeapType_NumTypes = (DescriptorHeapType_Dsv + 1)
	};

	enum DescriptorHeapFlags
	{
		DescriptorHeapFlags_None = 0,
		DescriptorHeapFlags_ShaderVisible = 0x1
	};

	class IDescriptorHeap
	{
	public:
		virtual ~IDescriptorHeap( void )
		{
		}

		virtual void Reset( void ) = 0;

		virtual bool Initialise( const IGraphicsDevice* _pDevice, DescriptorHeapType _type, unsigned int _numDescriptors, DescriptorHeapFlags _flags, const wchar_t* _pDebugName ) = 0;

		virtual void* GetDeviceObject( void ) const = 0;

		virtual unsigned int GetIncrementSize() const = 0;
		virtual unsigned int GetFreeIndex( void ) const = 0;
		virtual unsigned int GetFreeIndexAndIncrement( void ) = 0;
	};
}
