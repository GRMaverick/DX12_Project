#pragma once
#include "IDescriptorHeap.h"

namespace Artemis::Renderer::Interfaces
{
	class IGpuResource;
	class ISwapChain;
	class ICommandQueue;
	class ISamplerState;
	class ICommandList;

	enum ResourceFlags
	{
		ResourceFlag_None = 0,
		ResourceFlag_AllowRenderTarget = 0x1,
		ResourceFlag_AllowDepthStencil = 0x2,
		ResourceFlag_AllowUnorderedAccess = 0x4,
		ResourceFlag_DenyShaderResource = 0x8,
		ResourceFlag_AllowCrossAdapter = 0x10,
		ResourceFlag_AllowSimultaneousAccess = 0x20,
		ResourceFlag_VideoDecodeReferenceOnly = 0x40
	};

	enum EFillMode
	{
		FillMode_Wireframe = 2,
		FillMode_Solid= 3
	};

	enum ECullMode
	{
		CullMode_None = 1,
		CullMode_Front = 2,
		CullMode_Back = 3
	};

	enum EConservativeRasterisationMode
	{
		ConservativeRasterisationMode_Off = 0,
		ConservativeRasterisationMode_On = 1
	};

	enum EBlend
	{
		Blend_Zero = 1,
		Blend_One = 2,
		Blend_SrcColour = 3,
		Blend_InverseSrcColour = 4,
		Blend_SrcAlpha = 5,
		Blend_InverseSrcAlpha = 6,
		Blend_DestAlpha = 7,
		Blend_InvDestAlpha = 8,
		Blend_DestColour = 9,
		Blend_InvDestColour = 10,
		Blend_SrcAlphaSat = 11,
		Blend_BlendFactor = 14,
		Blend_InvBlendFactor = 15,
		Blend_Src1Colour = 16,
		Blend_InvSrc1Colour = 17,
		Blend_Src1Alpha = 18,
		Blend_InvSrc1Alpha = 19
	};

	enum EBlendOp
	{
		BlendOp_Add = 1,
		BlendOp_Subtract = 2,
		BlendOp_RevSubtract = 3,
		BlendOp_Min = 4,
		BlendOp_Max = 5
	};

	enum ELogicOp
	{
		LogicOp_Clear = 0,
		LogicOp_Set = (LogicOp_Clear + 1),
		LogicOp_Copy = (LogicOp_Set + 1),
		LogicOp_CopyInverted = (LogicOp_Copy + 1),
		LogicOp_NoOp = (LogicOp_CopyInverted + 1),
		LogicOp_Invert = (LogicOp_NoOp + 1),
		LogicOp_And = (LogicOp_Invert + 1),
		LogicOp_Nand = (LogicOp_And + 1),
		LogicOp_Or = (LogicOp_Nand + 1),
		LogicOp_Nor = (LogicOp_Or + 1),
		LogicOp_Xor = (LogicOp_Nor + 1),
		LogicOp_Equiv = (LogicOp_Xor + 1),
		LogicOp_AndReverse = (LogicOp_Equiv + 1),
		LogicOp_AndInverted = (LogicOp_AndReverse + 1),
		LogicOp_OrReverse = (LogicOp_AndInverted + 1),
		LogicOp_OrInverted = (LogicOp_OrReverse + 1)
	};

	enum EDepthWriteMask
	{
		DepthWriteMask_Zero = 0,
		DepthWriteMask_All = 1
	};

	enum EStencilOp
	{
		StencilOp_Keep = 1,
		StencilOp_Zero = 2,
		StencilOp_Replace = 3,
		StencilOp_IncrSat = 4,
		StencilOp_DecrSat = 5,
		StencilOp_Invert = 6,
		StencilOp_Incr = 7,
		StencilOp_Decr = 8
	};

	enum EComparisonFunc
	{
		ComparisonFunc_Never = 1,
		ComparisonFunc_Less = 2,
		ComparisonFunc_Equal = 3,
		ComparisonFunc_LessEqual = 4,
		ComparisonFunc_Greater = 5,
		ComparisonFunc_NotEqual = 6,
		ComparisonFunc_GreaterEqual = 7,
		ComparisonFunc_Always = 8
	};

	enum ECommandListType
	{
		CommandListType_Direct = 0,
		CommandListType_Bundle = 1,
		CommandListType_Compute = 2,
		CommandListType_Copy = 3,
		CommandListType_VideoDecode = 4,
		CommandListType_VideoProcess = 5,
		CommandListType_VideoEncode = 6
	};

	enum ColourWriteEnable
	{
		ColourWriteEnable_Red = 1,
		ColourWriteEnable_Green = 2,
		ColourWriteEnable_Blue = 4,
		ColourWriteEnable_Alpha = 8,
		ColourWriteEnable_All = (((ColourWriteEnable_Red | ColourWriteEnable_Green) | ColourWriteEnable_Blue) | ColourWriteEnable_Alpha)
	};

	struct DepthStencilOpDesc
	{
		EStencilOp      StencilFailOp;
		EStencilOp      StencilDepthFailOp;
		EStencilOp      StencilPassOp;
		EComparisonFunc StencilFunc;
	};

	struct RenderTargetBlendDesc
	{
		bool           BlendEnable;
		bool           LogicOpEnable;
		EBlend         SrcBlend;
		EBlend         DestBlend;
		EBlendOp       BlendOp;
		EBlend         SrcBlendAlpha;
		EBlend         DestBlendAlpha;
		EBlendOp       BlendOpAlpha;
		ELogicOp       LogicOp;
		unsigned short RenderTargetWriteMask;
	};

	struct DepthStencilDesc
	{
		bool               DepthEnable;
		EDepthWriteMask    DepthWriteMask;
		EComparisonFunc    DepthFunc;
		bool               StencilEnable;
		unsigned short     StencilReadMask;
		unsigned short     StencilWriteMask;
		DepthStencilOpDesc FrontFace;
		DepthStencilOpDesc BackFace;
	};

	struct RasteriserStateDesc
	{
		EFillMode                      FillMode;
		ECullMode                      CullMode;
		bool                           FrontCounterClockwise;
		int                            DepthBias;
		float                          DepthBiasClamp;
		float                          SlopeScaledDepthBias;
		bool                           DepthClipEnable;
		bool                           MultisampleEnable;
		bool                           AntialiasedLineEnable;
		unsigned int                   ForcedSampleCount;
		EConservativeRasterisationMode ConservativeRaster;
	};

	struct BlendDesc
	{
		bool                  AlphaToCoverageEnable;
		bool                  IndependentBlendEnable;
		RenderTargetBlendDesc RenderTarget[8];
	};

	class IGraphicsDevice
	{
	public:
		virtual ~IGraphicsDevice( void )
		{
		};

		virtual const void* GetDeviceObject( void ) const = 0;
		virtual bool        CreateSwapChain( ISwapChain** _ppSwapChain, ICommandQueue* _pCmdQueue, Core::GameWindow* _pWindow, unsigned int _numBackBuffers, const wchar_t* _pDebugName = L"" ) = 0;
		virtual bool        CreateCommandList( ECommandListType _type, ICommandList** _ppCommandList, const wchar_t* _pDebugName = L"" ) const = 0;
		virtual bool        CreateCommandQueue( Interfaces::ECommandListType _type, ICommandQueue** _ppCommandQueue, const wchar_t* _pDebugName = L"" ) const =0;
		virtual bool        CreateDescriptorHeap( Interfaces::DescriptorHeapType _type, IDescriptorHeap** _pDescriptorHeap, Interfaces::DescriptorHeapFlags _flags, unsigned int _numBuffers, const wchar_t* _pDebugName = L"" ) const = 0;

		virtual IGpuResource* CreateVertexBufferResource( ICommandList* _pCommandList, unsigned int _sizeInBytes, unsigned int _strideInBytes, ResourceFlags _flags, const void* _pData, const wchar_t* _pDebugName = L"" ) const = 0;
		virtual IGpuResource* CreateIndexBufferResource( ICommandList* _pCommandList, unsigned int _sizeInBytes, unsigned int _strideInBytes, ResourceFlags _flags, const void* _pData, const wchar_t* _pDebugName = L"" ) const = 0;
		virtual IGpuResource* CreateTexture2D( const wchar_t* _pWstrFilename, ICommandList* _pCommandList, const wchar_t* _pDebugName = L"" ) const = 0;
		virtual IGpuResource* CreateWicTexture2D( const wchar_t* _pWstrFilename, ICommandList* _pCommandList, const wchar_t* _pDebugName = L"" ) const = 0;

		virtual void BeginFrame( void ) = 0;
		virtual void EndFrame( void ) = 0;
		virtual bool FlushState( void ) = 0;

		virtual bool SetMaterial( const char* _pName ) = 0;
		virtual bool SetRenderTarget( void ) = 0;
		virtual bool SetDepthBuffer( void ) = 0;
		virtual bool SetTexture( const char* _pName, IGpuResource* _pTexture ) = 0;
		virtual bool SetConstantBuffer( const char* _pName, IGpuResource* _pCBuffer ) = 0;
		virtual bool SetSamplerState( const char* _pName, Interfaces::ISamplerState* _pSamplerState ) = 0;

		virtual bool SetRasterizerState( const RasteriserStateDesc& _desc ) = 0;
		virtual bool SetBlendState( const BlendDesc& _desc ) = 0;
		virtual bool SetDepthStencilState( const DepthStencilDesc& _desc ) = 0;

		virtual ICommandList*  GetImmediateContext( void ) const = 0;
		virtual ISamplerState* GetDefaultSamplerState( void ) const = 0;
	};
}
