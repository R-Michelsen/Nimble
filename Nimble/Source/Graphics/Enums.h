#pragma once

enum class ResourceType
{
	Texture,
	ConstantBuffer,
	StorageBuffer
};

enum class TextureFormat
{
	RGBA32,
	RGB32,
	RG32,
	R32,
	RGBA16,
	RG16,
	R16,
	RGBA8,
	RGB8,
	RG8,
	R8,
	D32,
	D24S8,
	UNKNOWN
};

enum class TextureSize
{
	ScreenRelative,
	Absolute
};

enum ShaderStageFlags : uint32_t
{
	VertexStage = 1 << 0,
	PixelStage = 1 << 1,
	ComputeStage = 1 << 2,
	UnknownStage = 1 << 3
};

enum TextureUsageFlags : uint32_t
{
	TextureReadOnly = 1 << 0,
	TextureReadWrite = 1 << 1,
	TextureRenderTarget = 1 << 2
};

enum BufferUsageFlags : uint32_t
{
	BufferReadOnly = 1 << 0,
	BufferReadWrite = 1 << 1
};

enum class VertexInputAttribute
{
	VERTEX_ATTRIBUTE_POSITION,
	VERTEX_ATTRIBUTE_NORMAL,
	VERTEX_ATTRIBUTE_UV,
	VERTEX_ATTRIBUTE_COLOR,
	VERTEX_ATTRIBUTE_FLOAT,
	VERTEX_ATTRIBUTE_FLOAT3,
	VERTEX_ATTRIBUTE_FLOAT4
};

enum class PipelineDepthState
{
	PIPELINE_DEPTH_DISABLE,
	PIPELINE_DEPTH_ENABLE
};

enum class PipelineRasterizationState
{
	PIPELINE_RASTERIZATION_DEFAULT,
	PIPELINE_RASTERIZATION_NOCULL,
	PIPELINE_RASTERIZATION_SHADOWMAP
};

enum class PipelineColourBlendingState
{
	COLOURBLENDING_DISABLE,
	COLOURBLENDING_ENABLE,
	COLOURBLENDING_NOATTACHMENT
};

enum class PipelineDynamicState
{
	DYNAMICSTATE_VIEWPORTSCISSOR,
	DYNAMICSTATE_DEPTHBIAS,
	DYNAMICSTATE_NONE
};