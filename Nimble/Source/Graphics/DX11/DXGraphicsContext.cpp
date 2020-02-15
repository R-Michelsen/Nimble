#include <NimblePCH.h>

#include "DXCommandBuffer.h"
#include "DXGraphicsContext.h"
#include "DXCommon.h"

#include "../../Math/MathCommon.h"

namespace Nimble 
{
	DXGraphicsContext::DXGraphicsContext(HINSTANCE hInstance, HWND hwnd_) : hwnd(hwnd_)
	{
		UNREFERENCED_PARAMETER(hInstance);

		RECT client;
		GetClientRect(hwnd, &client);
		windowWidth = client.right - client.left;
		windowHeight = client.bottom - client.top;

		uint32_t deviceFlags = 0;
#ifdef _DEBUG
		deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		// Force DirectX 11.1
		eastl::array<D3D_FEATURE_LEVEL, 1> featureLevels 
		{
			D3D_FEATURE_LEVEL_11_1
		};

		DXCHECK(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, deviceFlags,
			featureLevels.data(), static_cast<uint32_t>(featureLevels.size()), D3D11_SDK_VERSION,
			&device, &featureLevel, &deviceContext));

		DXCHECK(device->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&device1)));
		DXCHECK(deviceContext->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&deviceContext1)));
	}

	DXGraphicsContext::~DXGraphicsContext()
	{
		deviceContext->ClearState();
		
		device->Release();
		device1->Release();
		deviceContext->Release();
		deviceContext1->Release();
		swapchain->Release();
		swapchain1->Release();
		renderTargetView->Release();
		depthStencilBuffer->Release();
		depthStencilView->Release();
	}

	void DXGraphicsContext::CreateSwapchain(const bool enableMSAA_)
	{
		enableMSAA = enableMSAA_;
		if (backBuffer != nullptr) backBuffer->Release();
		if (swapchain != nullptr) swapchain->Release();
		if (swapchain1 != nullptr) swapchain1->Release();
		if (renderTargetView != nullptr) renderTargetView->Release();
		if (depthStencilBuffer != nullptr) depthStencilBuffer->Release();
		if (depthStencilView != nullptr) depthStencilView->Release();

		IDXGIDevice* dxgiDevice = nullptr;
		DXCHECK(device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice)));
		IDXGIAdapter* adapter = nullptr;
		DXCHECK(dxgiDevice->GetAdapter(&adapter));
		IDXGIFactory1* dxgiFactory = nullptr;
		DXCHECK(adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory)));
		adapter->Release();

		dxgiDevice->Release();

		IDXGIFactory2* dxgiFactory2 = nullptr;
		DXCHECK(dxgiFactory->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory2)));

		DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};
		swapchainDesc.Width = windowWidth;
		swapchainDesc.Height = windowHeight;
		swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapchainDesc.SampleDesc.Count = 1;
		swapchainDesc.SampleDesc.Quality = 0;
		swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapchainDesc.BufferCount = 2;
		swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

		DXCHECK(dxgiFactory2->CreateSwapChainForHwnd(device, hwnd, &swapchainDesc, nullptr, nullptr, &swapchain1));
		DXCHECK(swapchain1->QueryInterface(__uuidof(IDXGISwapChain), reinterpret_cast<void**>(&swapchain)));

		dxgiFactory2->Release();
		dxgiFactory->Release();
		DXCHECK(swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));

		// Create a default sampler state, this can be used by any texture if there are no specific requirements
		D3D11_SAMPLER_DESC samplerDesc{};
		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MaxAnisotropy = D3D11_DEFAULT_MAX_ANISOTROPY;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		DXCHECK(device->CreateSamplerState(&samplerDesc, &samplerState));

		CreateTargets();
	}

	void DXGraphicsContext::Resize()
	{
		RECT client;
		GetClientRect(hwnd, &client);
		windowWidth = client.right - client.left;
		windowHeight = client.bottom - client.top;

		renderTargetBuffer->Release();
		renderTargetView->Release();
		depthStencilBuffer->Release();
		depthStencilView->Release();
		backBuffer->Release();

		// Resize swapchain and create new render target view from back buffer
		DXCHECK(swapchain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0));

		DXCHECK(swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));

		CreateTargets();
	}

	void DXGraphicsContext::Present()
	{
		deviceContext->ResolveSubresource(backBuffer, 0, renderTargetBuffer, 0, DXGI_FORMAT_R8G8B8A8_UNORM);
		swapchain->Present(0, 0);
	}

	void DXGraphicsContext::PlayBackCommandBuffer(CommandBufferType& commandBuffer)
	{
		commandBuffer.Flush();
		deviceContext->ExecuteCommandList(commandBuffer.commandList, false);
		commandBuffer.commandList->Release();
	}

	void DXGraphicsContext::CreateTargets()
	{
		// Create a default DepthStencil/Render buffers and views
		D3D11_TEXTURE2D_DESC textureDescription{};
		textureDescription.ArraySize = 1;
		textureDescription.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		textureDescription.CPUAccessFlags = 0;
		textureDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		textureDescription.Width = windowWidth;
		textureDescription.Height = windowHeight;
		textureDescription.MipLevels = 1;
		textureDescription.SampleDesc.Count = enableMSAA ? MSAA_SAMPLE_COUNT : 1;
		textureDescription.SampleDesc.Quality = 0;
		textureDescription.Usage = D3D11_USAGE_DEFAULT;

		DXCHECK(device->CreateTexture2D(&textureDescription, nullptr, &depthStencilBuffer));
		textureDescription.BindFlags = D3D11_BIND_RENDER_TARGET;
		textureDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		DXCHECK(device->CreateTexture2D(&textureDescription, nullptr, &renderTargetBuffer));
		DXCHECK(device->CreateRenderTargetView(renderTargetBuffer, nullptr, &renderTargetView));
		DXCHECK(device->CreateDepthStencilView(depthStencilBuffer, nullptr, &depthStencilView));
	}

}