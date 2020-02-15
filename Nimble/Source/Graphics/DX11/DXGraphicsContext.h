#pragma once

#include <d3d11_3.h>

#include "../BackendTypedefs.h"

namespace Nimble 
{
	constexpr size_t MSAA_SAMPLE_COUNT = 8;

	class DXGraphicsContext
	{
	public:
		uint32_t windowWidth;
		uint32_t windowHeight;

		DXGraphicsContext(HINSTANCE hInstance, HWND hwnd);
		~DXGraphicsContext();

		void Resize();
		void Present();
		void PlayBackCommandBuffer(CommandBufferType& commandBuffer);

		ID3D11SamplerState* const GetDefaultSamplerState() { return samplerState; }
		ID3D11Device1* const GetDevice() { return device1; }
		ID3D11DeviceContext1* const GetImmediateContext() { return deviceContext1; }
		ID3D11RenderTargetView* const GetMainRTV() { return renderTargetView; }
		ID3D11DepthStencilView* const GetMainDSV() { return depthStencilView; }

	private:
		friend class RenderGraph;
		void CreateSwapchain(const bool enableMSAA);
		void CreateTargets();
		

		bool enableMSAA;
		HWND hwnd;

		ID3D11Device* device = nullptr;
		ID3D11Device1* device1 = nullptr;
		ID3D11DeviceContext* deviceContext = nullptr;
		ID3D11DeviceContext1* deviceContext1 = nullptr;
		IDXGISwapChain* swapchain = nullptr;
		IDXGISwapChain1* swapchain1 = nullptr;
		ID3D11RenderTargetView* renderTargetView = nullptr;
		ID3D11Texture2D* backBuffer = nullptr;
		ID3D11Texture2D* renderTargetBuffer = nullptr;
		ID3D11Texture2D* depthStencilBuffer = nullptr;
		ID3D11DepthStencilView* depthStencilView = nullptr;
		ID3D11SamplerState* samplerState = nullptr;

		D3D_FEATURE_LEVEL featureLevel;
		uint32_t multiSampleQualityLevel;
	};
}