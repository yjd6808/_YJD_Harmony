/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 8:16:00 AM
 * =====================
 * 텍스처 래퍼 구현부
 *
 * [PNG 로딩 흐름]
 * 1. WIC 팩토리 생성 -> 2. 파일 디코더 생성 -> 3. 첫 프레임 얻기
 * -> 4. RGBA 32비트로 형식 변환 -> 5. 픽셀 복사 -> 6. GPU 텍스처 생성
 *
 * [SVG 로딩 흐름]
 * 1. nanosvg로 파싱(벡터 데이터) -> 2. nanosvgrast로 픽셀화(래스터화)
 * -> 3. CreateFromMemory로 GPU 텍스처 생성
 */

#include "Core.h"
#include "sgf/Graphics/Texture.h"
#include "sgf/Graphics/GraphicDevice.h"

// nanosvg는 선택 사항이다. _Extern 폴더에 헤더가 있으면 SVG 기능이 켜진다.
// (없어도 컴파일은 되며, LoadFromSvgFile이 false를 반환할 뿐이다)
#if __has_include("sgf/_Extern/nanosvg/nanosvg.h")
	#define SGF_HAS_NANOSVG 1
	#define NANOSVG_IMPLEMENTATION
	#define NANOSVGRAST_IMPLEMENTATION
	#include "sgf/_Extern/nanosvg/nanosvg.h"
	#include "sgf/_Extern/nanosvg/nanosvgrast.h"
#else
	#define SGF_HAS_NANOSVG 0
#endif

NS_SGF_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
// 생성자
Texture::Texture()
	: width_(0)
	, height_(0)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
// 소멸자 (ComPtr이 자동 Release)
Texture::~Texture()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
// WIC으로 이미지 파일을 읽어 텍스처 생성
bool Texture::LoadFromFile(GraphicDevice* _pDevice, const wchar_t* _szFilePath)
{
	// COM 초기화. 이미 다른 곳에서 초기화했으면 S_FALSE가 오지만 문제없다.
	// RPC_E_CHANGED_MODE인 경우만 짜짝이 다른 것이므로 CoUninitialize를 생략해야 하지만
	// 튜토리얼 수준에서는 일관되게 COINIT_MULTITHREADED를 사용하므로 고려하지 않는다.
	const HRESULT hrCoInit = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	const bool needUninit = SUCCEEDED(hrCoInit);

	bool result = false;

	// do-while(false) 패턴: 중간 실패 시 break로 빠져나가 마지막 정리 코드를 공유한다.
	do
	{
		// 1. WIC 팩토리 생성: 모든 WIC 객체를 만드는 입구
		SgfComPtr<IWICImagingFactory> pFactory;
		HRESULT hr = CoCreateInstance(
			CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(pFactory.GetAddressOf()));
		if (FAILED(hr)) { break; }

		// 2. 파일 디코더 생성: 확장자에 맞는 코덱(PNG/JPG...)을 자동 선택한다.
		SgfComPtr<IWICBitmapDecoder> pDecoder;
		hr = pFactory->CreateDecoderFromFilename(
			_szFilePath, nullptr, GENERIC_READ,
			WICDecodeMetadataCacheOnDemand, pDecoder.GetAddressOf());
		if (FAILED(hr)) { break; }

		// 3. 첫 프레임 얻기 (GIF 같은 다중 프레임 형식도 첫 장만 사용)
		SgfComPtr<IWICBitmapFrameDecode> pFrame;
		hr = pDecoder->GetFrame(0, pFrame.GetAddressOf());
		if (FAILED(hr)) { break; }

		// 4. RGBA 32비트로 형식 변환
		// PNG는 팔레트/16비트/회색조 등 다양한 내부 형식이 있으므로
		// 어떤 형식이든 GPU가 좋아하는 RGBA 8888로 통일한다.
		SgfComPtr<IWICFormatConverter> pConverter;
		hr = pFactory->CreateFormatConverter(pConverter.GetAddressOf());
		if (FAILED(hr)) { break; }
		hr = pConverter->Initialize(
			pFrame.Get(), GUID_WICPixelFormat32bppRGBA,
			WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom);
		if (FAILED(hr)) { break; }

		// 5. 크기 조회 후 픽셀 복사
		UINT width = 0;
		UINT height = 0;
		hr = pConverter->GetSize(&width, &height);
		if (FAILED(hr) || width == 0 || height == 0) { break; }

		jc::Vector<_u8> pixels(_s32(width) * _s32(height) * 4, _u8(0));
		hr = pConverter->CopyPixels(
			nullptr,					// 전체 영역
			width * 4,					// 한 줄의 바이트 수 (stride)
			width * height * 4,			// 버퍼 전체 크기
			pixels.Source());
		if (FAILED(hr)) { break; }

		// 6. GPU 텍스처 생성
		result = CreateFromMemory(_pDevice, pixels.Source(), _s32(width), _s32(height));
	} while (false);

	if (needUninit)
	{
		CoUninitialize();
	}
	return result;
}

//////////////////////////////////////////////////////////////////////////////////////////
// nanosvg로 SVG 파일을 래스터화해서 텍스처 생성
bool Texture::LoadFromSvgFile(GraphicDevice* _pDevice, const char* _szFilePath, _f32 _scale)
{
#if SGF_HAS_NANOSVG
	// 1. SVG 파싱: 파일을 읽어 벡터 도형 목록으로 변환한다.
	// "px" 단위, 96 DPI는 nanosvg 권장 기본값이다.
	NSVGimage* pImage = nsvgParseFromFile(_szFilePath, "px", 96.0f);
	if (pImage == nullptr)
	{
		return false;
	}

	// 2. 출력 픽셀 크기 계산 (최소 1px 보장)
	const _s32 width = (_s32(pImage->width * _scale) > 0) ? _s32(pImage->width * _scale) : 1;
	const _s32 height = (_s32(pImage->height * _scale) > 0) ? _s32(pImage->height * _scale) : 1;

	// 3. 래스터라이저 생성 후 픽셀화
	bool result = false;
	NSVGrasterizer* pRasterizer = nsvgCreateRasterizer();
	if (pRasterizer != nullptr)
	{
		jc::Vector<_u8> pixels(width * height * 4, _u8(0));
		nsvgRasterize(
			pRasterizer,
			pImage,
			0.0f, 0.0f,			// 출력 오프셋
			_scale,				// 확대 배율
			pixels.Source(),
			width, height,
			width * 4);			// stride

		// 4. GPU 텍스처 생성
		result = CreateFromMemory(_pDevice, pixels.Source(), width, height);
		nsvgDeleteRasterizer(pRasterizer);
	}

	nsvgDelete(pImage);
	return result;
#else
	// nanosvg 미설치: _Extern/nanosvg/README.md 참고 (사용하지 않는 인자 경고 제거)
	(void)_pDevice;
	(void)_szFilePath;
	(void)_scale;
	return false;
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////
// 메모리의 RGBA 픽셀 배열로 텍스처 생성
bool Texture::CreateFromMemory(GraphicDevice* _pDevice, const _u8* _pPixels, _s32 _width, _s32 _height)
{
	// 재사용(재초기화) 대비: 기존 텍스처 뷰를 먼저 정리한다. (GetAddressOf 덮어쓰기 누수 방지)
	pShaderResourceView_.Reset();

	// 텍스처 설정
	D3D11_TEXTURE2D_DESC td = {};
	td.Width = UINT(_width);
	td.Height = UINT(_height);
	td.MipLevels = 1;								// 밉맵 없음 (튜토리얼 단순화)
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;			// RGBA 각 8비트
	td.SampleDesc.Count = 1;
	td.Usage = D3D11_USAGE_IMMUTABLE;				// 생성 후 변경 불가 (가장 빠름)
	td.BindFlags = D3D11_BIND_SHADER_RESOURCE;		// 셰이더 입력 용도

	// 초기 픽셀 데이터
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = _pPixels;
	sd.SysMemPitch = UINT(_width) * 4;				// 한 줄의 바이트 수

	SgfComPtr<ID3D11Texture2D> pTexture;
	HRESULT hr = _pDevice->Device()->CreateTexture2D(&td, &sd, pTexture.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	// 셰이더가 이 텍스처를 읽을 수 있게 해주는 뷰(SRV) 생성
	hr = _pDevice->Device()->CreateShaderResourceView(
		pTexture.Get(), nullptr, pShaderResourceView_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	width_ = _width;
	height_ = _height;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// PS 단계 지정 슬롯에 텍스처 장착
void Texture::Bind(GraphicDevice* _pDevice, UINT _slot)
{
	ID3D11ShaderResourceView* pViews[] = { pShaderResourceView_.Get() };
	_pDevice->Context()->PSSetShaderResources(_slot, 1, pViews);
}

NS_SGF_END
