#pragma once
#include "IDescriptorHeap.h"
#include "IConstantBufferParameters.h"

namespace Artemis::Core
{
	class GameWindow;
}

namespace Artemis::Renderer::Interfaces
{
	class IGpuResource;
	class ISwapChain;
	class ICommandQueue;
	class ISamplerState;
	class ICommandList;
	struct IMaterial;

	enum class DxgiFormat
	{
		DxgiFormat_UNKNOWN = 0,
		DxgiFormat_R32G32B32A32_TYPELESS = 1,
		DxgiFormat_R32G32B32A32_FLOAT = 2,
		DxgiFormat_R32G32B32A32_UINT = 3,
		DxgiFormat_R32G32B32A32_SINT = 4,
		DxgiFormat_R32G32B32_TYPELESS = 5,
		DxgiFormat_R32G32B32_FLOAT = 6,
		DxgiFormat_R32G32B32_UINT = 7,
		DxgiFormat_R32G32B32_SINT = 8,
		DxgiFormat_R16G16B16A16_TYPELESS = 9,
		DxgiFormat_R16G16B16A16_FLOAT = 10,
		DxgiFormat_R16G16B16A16_UNORM = 11,
		DxgiFormat_R16G16B16A16_UINT = 12,
		DxgiFormat_R16G16B16A16_SNORM = 13,
		DxgiFormat_R16G16B16A16_SINT = 14,
		DxgiFormat_R32G32_TYPELESS = 15,
		DxgiFormat_R32G32_FLOAT = 16,
		DxgiFormat_R32G32_UINT = 17,
		DxgiFormat_R32G32_SINT = 18,
		DxgiFormat_R32G8X24_TYPELESS = 19,
		DxgiFormat_D32_FLOAT_S8X24_UINT = 20,
		DxgiFormat_R32_FLOAT_X8X24_TYPELESS = 21,
		DxgiFormat_X32_TYPELESS_G8X24_UINT = 22,
		DxgiFormat_R10G10B10A2_TYPELESS = 23,
		DxgiFormat_R10G10B10A2_UNORM = 24,
		DxgiFormat_R10G10B10A2_UINT = 25,
		DxgiFormat_R11G11B10_FLOAT = 26,
		DxgiFormat_R8G8B8A8_TYPELESS = 27,
		DxgiFormat_R8G8B8A8_UNORM = 28,
		DxgiFormat_R8G8B8A8_UNORM_SRGB = 29,
		DxgiFormat_R8G8B8A8_UINT = 30,
		DxgiFormat_R8G8B8A8_SNORM = 31,
		DxgiFormat_R8G8B8A8_SINT = 32,
		DxgiFormat_R16G16_TYPELESS = 33,
		DxgiFormat_R16G16_FLOAT = 34,
		DxgiFormat_R16G16_UNORM = 35,
		DxgiFormat_R16G16_UINT = 36,
		DxgiFormat_R16G16_SNORM = 37,
		DxgiFormat_R16G16_SINT = 38,
		DxgiFormat_R32_TYPELESS = 39,
		DxgiFormat_D32_FLOAT = 40,
		DxgiFormat_R32_FLOAT = 41,
		DxgiFormat_R32_UINT = 42,
		DxgiFormat_R32_SINT = 43,
		DxgiFormat_R24G8_TYPELESS = 44,
		DxgiFormat_D24_UNORM_S8_UINT = 45,
		DxgiFormat_R24_UNORM_X8_TYPELESS = 46,
		DxgiFormat_X24_TYPELESS_G8_UINT = 47,
		DxgiFormat_R8G8_TYPELESS = 48,
		DxgiFormat_R8G8_UNORM = 49,
		DxgiFormat_R8G8_UINT = 50,
		DxgiFormat_R8G8_SNORM = 51,
		DxgiFormat_R8G8_SINT = 52,
		DxgiFormat_R16_TYPELESS = 53,
		DxgiFormat_R16_FLOAT = 54,
		DxgiFormat_D16_UNORM = 55,
		DxgiFormat_R16_UNORM = 56,
		DxgiFormat_R16_UINT = 57,
		DxgiFormat_R16_SNORM = 58,
		DxgiFormat_R16_SINT = 59,
		DxgiFormat_R8_TYPELESS = 60,
		DxgiFormat_R8_UNORM = 61,
		DxgiFormat_R8_UINT = 62,
		DxgiFormat_R8_SNORM = 63,
		DxgiFormat_R8_SINT = 64,
		DxgiFormat_A8_UNORM = 65,
		DxgiFormat_R1_UNORM = 66,
		DxgiFormat_R9G9B9E5_SHAREDEXP = 67,
		DxgiFormat_R8G8_B8G8_UNORM = 68,
		DxgiFormat_G8R8_G8B8_UNORM = 69,
		DxgiFormat_BC1_TYPELESS = 70,
		DxgiFormat_BC1_UNORM = 71,
		DxgiFormat_BC1_UNORM_SRGB = 72,
		DxgiFormat_BC2_TYPELESS = 73,
		DxgiFormat_BC2_UNORM = 74,
		DxgiFormat_BC2_UNORM_SRGB = 75,
		DxgiFormat_BC3_TYPELESS = 76,
		DxgiFormat_BC3_UNORM = 77,
		DxgiFormat_BC3_UNORM_SRGB = 78,
		DxgiFormat_BC4_TYPELESS = 79,
		DxgiFormat_BC4_UNORM = 80,
		DxgiFormat_BC4_SNORM = 81,
		DxgiFormat_BC5_TYPELESS = 82,
		DxgiFormat_BC5_UNORM = 83,
		DxgiFormat_BC5_SNORM = 84,
		DxgiFormat_B5G6R5_UNORM = 85,
		DxgiFormat_B5G5R5A1_UNORM = 86,
		DxgiFormat_B8G8R8A8_UNORM = 87,
		DxgiFormat_B8G8R8X8_UNORM = 88,
		DxgiFormat_R10G10B10_XR_BIAS_A2_UNORM = 89,
		DxgiFormat_B8G8R8A8_TYPELESS = 90,
		DxgiFormat_B8G8R8A8_UNORM_SRGB = 91,
		DxgiFormat_B8G8R8X8_TYPELESS = 92,
		DxgiFormat_B8G8R8X8_UNORM_SRGB = 93,
		DxgiFormat_BC6H_TYPELESS = 94,
		DxgiFormat_BC6H_UF16 = 95,
		DxgiFormat_BC6H_SF16 = 96,
		DxgiFormat_BC7_TYPELESS = 97,
		DxgiFormat_BC7_UNORM = 98,
		DxgiFormat_BC7_UNORM_SRGB = 99,
		DxgiFormat_AYUV = 100,
		DxgiFormat_Y410 = 101,
		DxgiFormat_Y416 = 102,
		DxgiFormat_NV12 = 103,
		DxgiFormat_P010 = 104,
		DxgiFormat_P016 = 105,
		DxgiFormat_420_OPAQUE = 106,
		DxgiFormat_YUY2 = 107,
		DxgiFormat_Y210 = 108,
		DxgiFormat_Y216 = 109,
		DxgiFormat_NV11 = 110,
		DxgiFormat_AI44 = 111,
		DxgiFormat_IA44 = 112,
		DxgiFormat_P8 = 113,
		DxgiFormat_A8P8 = 114,
		DxgiFormat_B4G4R4A4_UNORM = 115,
		DxgiFormat_P208 = 130,
		DxgiFormat_V208 = 131,
		DxgiFormat_V408 = 132,
		DxgiFormat_SAMPLER_FEEDBACK_MIN_MIP_OPAQUE = 189,
		DxgiFormat_SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE = 190,
	};

	enum class ResourceFlags
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
		int            BlendEnable;
		int            LogicOpEnable;
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
		int                DepthEnable;
		EDepthWriteMask    DepthWriteMask;
		EComparisonFunc    DepthFunc;
		int                StencilEnable;
		unsigned short     StencilReadMask;
		unsigned short     StencilWriteMask;
		DepthStencilOpDesc FrontFace;
		DepthStencilOpDesc BackFace;
	};

	struct RasteriserStateDesc
	{
		EFillMode                      FillMode;
		ECullMode                      CullMode;
		int                            FrontCounterClockwise;
		int                            DepthBias;
		float                          DepthBiasClamp;
		float                          SlopeScaledDepthBias;
		int                            DepthClipEnable;
		int                            MultisampleEnable;
		int                            AntialiasedLineEnable;
		unsigned int                   ForcedSampleCount;
		EConservativeRasterisationMode ConservativeRaster;
	};

	struct BlendDesc
	{
		int                   AlphaToCoverageEnable;
		int                   IndependentBlendEnable;
		RenderTargetBlendDesc RenderTarget[8];
	};

	struct DeviceStateStats
	{
		unsigned int TextureUpdates        = 0;
		unsigned int ShaderUpdates         = 0;
		unsigned int RenderTargetUpdates   = 0;
		unsigned int DepthBufferUpdates    = 0;
		unsigned int ConstantBufferUpdates = 0;
		unsigned int PipelineStateUpdates  = 0;
		unsigned int RootSignatureUpdates  = 0;
		unsigned int SamplerStateUpdates   = 0;
	};

	class IGraphicsDevice
	{
	public:
		virtual ~IGraphicsDevice( void )
		{
		};

		virtual bool InitialiseImGui( Artemis::Core::GameWindow* _pWindow, const IDescriptorHeap* _pHeap ) const = 0;

		virtual const void* GetDeviceObject( void ) const = 0;
		virtual bool        CreateSwapChain( ISwapChain** _ppSwapChain, ICommandQueue* _pCmdQueue, Artemis::Core::GameWindow* _pWindow, unsigned int _numBackBuffers, const wchar_t* _pDebugName = L"" ) = 0;
		virtual bool        CreateCommandList( ECommandListType _type, ICommandList** _ppCommandList, const wchar_t* _pDebugName = L"" ) const = 0;
		virtual bool        CreateCommandQueue( Interfaces::ECommandListType _type, ICommandQueue** _ppCommandQueue, const wchar_t* _pDebugName = L"" ) const =0;
		virtual bool        CreateDescriptorHeap( Interfaces::DescriptorHeapType _type, IDescriptorHeap** _pDescriptorHeap, Interfaces::DescriptorHeapFlags _flags, unsigned int _numBuffers, const wchar_t* _pDebugName = L"" ) const = 0;

		virtual IGpuResource* CreateVertexBufferResource( ICommandList* _pCommandList, unsigned int _sizeInBytes, unsigned int _strideInBytes, ResourceFlags _flags, const void* _pData, const wchar_t* _pDebugName = L"" ) const = 0;
		virtual IGpuResource* CreateIndexBufferResource( ICommandList* _pCommandList, unsigned int _sizeInBytes, unsigned int _strideInBytes, ResourceFlags _flags, const void* _pData, const wchar_t* _pDebugName = L"" ) const = 0;
		virtual IGpuResource* CreateConstantBufferResource( const IConstantBufferParameters::ConstantBuffer& _params, const wchar_t* _pDebugName = L"" ) const = 0;
		virtual IGpuResource* CreateTexture2D( const wchar_t* _pWstrFilename, ICommandList* _pCommandList, const wchar_t* _pDebugName = L"" ) const = 0;
		virtual IGpuResource* CreateWicTexture2D( const wchar_t* _pWstrFilename, ICommandList* _pCommandList, const wchar_t* _pDebugName = L"" ) const = 0;
		virtual IGpuResource* CreateRenderTargetResource( const Interfaces::IGraphicsDevice* _pDevice, const Interfaces::ICommandList* _pCmdList, const unsigned _width, const unsigned _height, const Interfaces::DxgiFormat _format, Interfaces::IDescriptorHeap* _pRtvHeap, Interfaces::IDescriptorHeap* _pSrvHeap ) const = 0;
		virtual IGpuResource* CreateDepthBufferResource( const Interfaces::IGraphicsDevice* _pDevice, const Interfaces::ICommandList* _pCmdList, const unsigned _width, const unsigned _height, const Interfaces::DxgiFormat _format, Interfaces::IDescriptorHeap* _pDsvHeap, Interfaces::IDescriptorHeap* _pSrvHeap ) const = 0;

		virtual void BeginFrame( void ) = 0;
		virtual void EndFrame( void ) = 0;
		virtual bool FlushState( void ) = 0;

		virtual bool SetMaterial( IMaterial* _pMaterial ) = 0;
		virtual bool SetRenderTarget( void ) = 0;
		virtual bool SetDepthBuffer( void ) = 0;
		virtual bool SetTexture( const char* _pName, IGpuResource* _pTexture ) = 0;
		virtual bool SetConstantBuffer( const char* _pName, IGpuResource* _pCBuffer ) = 0;
		virtual bool SetSamplerState( const char* _pName, Interfaces::ISamplerState* _pSamplerState ) = 0;

		virtual bool SetRasterizerState( const RasteriserStateDesc& _desc ) = 0;
		virtual bool SetBlendState( const BlendDesc& _desc ) = 0;
		virtual bool SetDepthStencilState( const DepthStencilDesc& _desc ) = 0;

		virtual ICommandList*    GetImmediateContext( void ) const = 0;
		virtual ISamplerState*   GetDefaultSamplerState( void ) const = 0;
		virtual DeviceStateStats GetDeviceStats( void ) const = 0;
	};
}
