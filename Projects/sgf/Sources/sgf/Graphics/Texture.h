/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 8:14:00 AM
 * =====================
 * 텍스처 래퍼 (PNG: WIC 로더 / SVG: nanosvg 래스터라이저)
 *
 * [텍스처란?]
 * GPU가 삼각형 위에 붙일 수 있는 이미지. 파일(PNG 등)을 읽어서
 * 픽셀 배열(RGBA)로 풀고, 그걸 GPU 텍스처로 올린 뒤 SRV를 통해 셰이더에서 읽는다.
 *
 * [WIC란?]
 * Windows Imaging Component. 윈도우 내장 이미지 디코더로
 * PNG/JPG/BMP/GIF 등을 별도 라이브러리 없이 읽을 수 있다.
 *
 * [SVG 처리]
 * SVG는 벡터(수식) 이미지라 GPU에 바로 올릴 수 없다.
 * nanosvg로 원하는 크기의 픽셀 배열로 변환(래스터화)한 뒤 올린다.
 * nanosvg 헤더가 _Extern 폴더에 없으면 SVG 기능만 비활성화된다.
 */

#pragma once

#include "jc/Math.h"

#include "sgf/Graphics/IResource.h"
#include "sgf/Graphics/GraphicsEnums.h"

NS_SGF_BEGIN

class GraphicDevice;
class GraphicContext;

// IResource를 상속받아 ResourceMgr에 등록/검색할 수 있다.
class Texture : public ResourceBase
{
	SGF_RESOURCE_TYPE(rtTexture)

public:
	Texture();
	~Texture();

	// PNG/JPG/BMP 등 일반 이미지 파일을 WIC으로 읽어 텍스처를 만든다.
	// @param _szFilePath: 이미지 파일 경로 (유니코드)
	// @return 성공 여부
	bool LoadFromFile(GraphicDevice* _pDevice, const jc::String& _szFilePath);

	// SVG 파일을 nanosvg로 래스터화해서 텍스처를 만든다.
	// @param _szFilePath: SVG 파일 경로 (멀티바이트)
	// @param _scale: 확대 배율 (1.0 = SVG 원본 크기)
	// @return 성공 여부 (nanosvg 미설치 시 항상 false)
	bool LoadFromSvgFile(GraphicDevice* _pDevice, const jc::String& _szFilePath, _f32 _scale = 1.0f);

	// 메모리의 RGBA 픽셀 배열로 직접 텍스처를 만든다.
	// (절차적 생성 텍스처, 단색 텍스처 등에 사용)
	// @param _pPixels: RGBA 순서 8비트씩, 한 줄에 _width*4 바이트
	// @param _format: 픽셀 포맷 (기본 pfRgba8)
	bool CreateFromMemory(GraphicDevice* _pDevice, const _u8* _pPixels, _s32 _width, _s32 _height, PixelFormat _format = PixelFormat::pfRgba8);

	// PS 단계 지정 슬롯에 이 텍스처를 장착한다.
	// @param _slot: 셰이더의 register(t0), register(t1)... 번호
	void Bind(GraphicContext& _context, _u32 _slot = 0);

	_s32 Width() const { return width_; }
	_s32 Height() const { return height_; }
	bool IsValid() const { return pShaderResourceView_ != nullptr; }

	// GraphicContext 바인딩 캐시용 원본 핸들
	ID3D11ShaderResourceView* Srv() const { return pShaderResourceView_.Get(); }

private:
	SgfComPtr<ID3D11ShaderResourceView> pShaderResourceView_;	// 셰이더가 읽는 뷰
	_s32 width_;												// 가로 픽셀 수
	_s32 height_;												// 세로 픽셀 수
};

NS_SGF_END
