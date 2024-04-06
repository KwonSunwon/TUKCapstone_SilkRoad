#pragma once


class D3D11On12Device
{
public:
	D3D11On12Device();
	~D3D11On12Device();

	void Init(ComPtr<ID3D12Device> device, ComPtr<IDXGIFactory> dxgi, vector<ComPtr<ID3D12Resource>> swapchains, ComPtr<ID3D12CommandQueue> cmdQueue);

	ComPtr<ID3D11DeviceContext> GetD3D11DeviceContext() { return m_d3d11DeviceContext; }
	ComPtr<ID3D11On12Device> GetD3D11on12Device() { return m_d3d11On12Device; }
	ComPtr<ID2D1Factory3> GetD2DFactory() { return m_d2dFactory; }
	ComPtr<ID2D1Device2> GetD2DDevice() { return m_d2dDevice; }
	ComPtr<ID2D1DeviceContext2>	GetD2DDeviceContext() { return m_d2dDeviceContext; }
	ComPtr<ID2D1Bitmap1> GetD3D11On12RT(uint8 index) { return m_d2dRenderTargets[index]; }
	ComPtr<ID3D11Resource> GetWrappedBackBuffer(uint8 index) {return m_wrappedBackBuffers[index]; }

	ComPtr<ID2D1SolidColorBrush> GetSolidColorBrush() { return m_textBrush; }
	ComPtr<IDWriteTextFormat> GetTextFormat() { return m_textFormat; }


private:
	static const UINT FrameCount = 2;

	ComPtr<ID3D11DeviceContext> m_d3d11DeviceContext;
	ComPtr<ID3D11On12Device>	m_d3d11On12Device;
	ComPtr<IDWriteFactory>		m_dWriteFactory;
	ComPtr<ID2D1Factory3>		m_d2dFactory;
	ComPtr<ID2D1Device2>		m_d2dDevice;
	ComPtr<ID2D1DeviceContext2>	m_d2dDeviceContext;
	ComPtr<ID2D1Bitmap1>		m_d2dRenderTargets[FrameCount];
	ComPtr<ID3D11Resource>		m_wrappedBackBuffers[FrameCount];

	// 리소스
	ComPtr<ID2D1SolidColorBrush> m_textBrush;
	ComPtr<IDWriteTextFormat> m_textFormat;
};

