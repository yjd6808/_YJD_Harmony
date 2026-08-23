/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 6:30:00 PM
 * =====================
 * 20. 머티리얼 (Material)
 *
 * [이 튜토리얼에서 배우는 것]
 * 1. Material = 셰이더(VS/PS) + 상태 4종 + 텍스처 슬롯 + 머티리얼 상수(b2)의 묶음
 * 2. material.Bind(context) 한 줄이면 파이프라인이 '그릴 준비 완료' 상태가 된다
 * 3. 디폴트 리소스: ResourceMgr가 준비해둔 디폴트 2D 셰이더를 그대로 쓴다
 *
 * [Before/After 비교]
 * - Before: 그리기 직전마다 셰이더/블렌드/샘플러/텍스처를 일일이 바인딩
 * - After: "어떻게 그릴지"를 Material 값 하나에 담아두고 Bind 한 번으로 재사용
 *
 * [조작법]
 * - 1/2: 블렌드 (불투명/알파) - 3/4: 필터 (Point/Linear)
 * - 5: 틴트 색 순환 (머티리얼 상수 b2 갱신)
 * - ESC: 종료
 */

#include "Core.h"
#include "sgfr/Tutorial/20_Material/20_Material_Main.h"

using namespace sgf;
using namespace jc;

namespace
{
	//////////////////////////////////////////////////////////////////////////////////////////

	// 체커보드 텍스처 픽셀을 채운다. (8x8 체커, 한 칸 8픽셀 = 64x64)
	void FillChecker(_u8* _pOutPixels, _s32 _width, _s32 _height)
	{
		for (_s32 y = 0; y < _height; ++y)
		{
			for (_s32 x = 0; x < _width; ++x)
			{
			const bool bWhite = (((x / 8) + (y / 8)) % 2) == 0;
			_u8* pPixel = _pOutPixels + (y * _width + x) * 4;
			pPixel[0] = bWhite ? 255 : 40;	// R
			pPixel[1] = bWhite ? 255 : 40;	// G
			pPixel[2] = bWhite ? 255 : 80;	// B
			pPixel[3] = 255;			// A
			}
		}
	}

	// 틴트 색 후보 (5번 키로 순환)
	const color s_tintColors[] =
	{
		color(0xFF, 0xFF, 0xFF, 0xFF),
		color(0xFF, 0x80, 0x80, 0xFF),
		color(0x80, 0xFF, 0x80, 0xCC),
		color(0x80, 0xB3, 0xFF, 0x80),
	};
}

//////////////////////////////////////////////////////////////////////////////////////////

// 머티리얼 튜토리얼을 실행한다. (한 덩어리 바인딩 + b2 머티리얼 상수)
void Material_Main()
{
	// 1. 윈도우 + 디바이스 + 리소스 매니저 준비
	Window window;
	if (!window.Create("20. 머티리얼 (1/2 블렌드, 3/4 필터, 5 틴트, ESC 종료)", 800, 600))
	{
		jc::Console::WriteLine("윈도우 생성 실패!");
		return;
	}

	InputManager input;
	window.ConnectInput(&input);

	GraphicDevice device;
	if (!device.Initialize())
	{
	jc::Console::WriteLine("그래픽 디바이스 초기화 실패!");
		window.Destroy();
		return;
	}
	if (!device.CreateSwapChain(window.Handle(), window.Width(), window.Height(), PixelFormat::pfRgba8))
	{
	jc::Console::WriteLine("스왑체인 생성 실패!");
	device.Finalize();
	window.Destroy();
	return;
	}

	// 튜토리얼 단독 실행이므로 리소스 매니저도 직접 초기화한다.
	// (Application 기반 게임에서는 엔진이 알아서 초기화해준다)
	if (!g_cResourceMgr.Initialize(&device))
	{
		jc::Console::WriteLine("리소스 매니저 초기화 실패!");
		device.Finalize();
		window.Destroy();
		return;
	}

	GraphicContext& context = device.Context();

	// 2. 체커 텍스처를 만들어 리소스 매니저에 등록한다. (소유권 이전!)
	_u8 pixels[64 * 64 * 4];
	FillChecker(pixels, 64, 64);

	Texture* pChecker = dbg_new Texture();
	if (!pChecker->CreateFromMemory(&device, pixels, 64, 64))
	{
		jc::Console::WriteLine("텍스처 생성 실패!");
		delete pChecker;
		g_cResourceMgr.Finalize();
		device.Finalize();
		window.Destroy();
		return;
	}
	pChecker->SetDebugName("Checker64");
	const _u64 checkerKey = g_cResourceMgr.Add(pChecker);

	// 3. 머티리얼 구성: 디폴트 2D 셰이더 키 + 체커 텍스처 키 + 상태 조합
	Material material;
	if (!material.Initialize(&device))
	{
		jc::Console::WriteLine("머티리얼 초기화 실패!");
		g_cResourceMgr.Finalize();
		device.Finalize();
		window.Destroy();
		return;
	}
	material.SetVertexShaderKey(g_cResourceMgr.GetDefaultVertexShader2DKey());
	material.SetPixelShaderKey(g_cResourceMgr.GetDefaultPixelShader2DKey());
	material.SetTextureKey(0, checkerKey);
	material.SetBlend(BlendMode::bmAlpha);
	material.SetDepth(DepthMode::dmDisabled);
	material.SetRasterizer(CullMode::cmNone);

	// 4. 메시: 디폴트 2D 셰이더의 VertexPTC 레이아웃과 일치하는 내장 쿼드
	Mesh quad;
	if (!quad.InitializeAsQuad2D(&device, g_cResourceMgr.GetDefaultVertexShader2D()))
	{
		jc::Console::WriteLine("쿼드 메시 생성 실패!");
		g_cResourceMgr.Finalize();
		device.Finalize();
		window.Destroy();
		return;
	}

	// 5. 디폴트 2D 셰이더의 b0/b1 규약에 맞춰 상수버퍼 준비
	// (22번부터는 Renderer3D가 이 작업까지 대신해준다)
	ConstantBuffer<FrameConstants> frameCb;
	ConstantBuffer<ObjectConstants> objectCb;
	if (!frameCb.Create(&device) || !objectCb.Create(&device))
	{
		jc::Console::WriteLine("상수버퍼 생성 실패!");
		g_cResourceMgr.Finalize();
		device.Finalize();
		window.Destroy();
		return;
	}

	jc::Console::WriteLine("머티리얼 하나로 셰이더/상태/텍스처가 한 번에 바인딩됩니다!");

	// 6. 렌더 루프
	_s32 tintIndex = 0;

	while (window.PumpMessage())
	{
		if (input.IsKeyPressed(VK_ESCAPE))
		{
			break;
		}
		if (input.IsKeyPressed('1')) { material.SetBlend(BlendMode::bmNone); }
		if (input.IsKeyPressed('2')) { material.SetBlend(BlendMode::bmAlpha); }
		if (input.IsKeyPressed('3')) { material.SetSampler(FilterMode::fmPoint); }
		if (input.IsKeyPressed('4')) { material.SetSampler(FilterMode::fmLinear); }
		if (input.IsKeyPressed('5'))
		{
			tintIndex = (tintIndex + 1) % (_s32)(sizeof(s_tintColors) / sizeof(s_tintColors[0]));
			material.SetBaseColor(s_tintColors[tintIndex]);	// b2 머티리얼 상수 갱신
		}
		input.NextFrame();

		device.BeginFrame(color(0x0F, 0x0F, 0x1A, 0xFF));
		context.InvalidateCache();	// BeginFrame이 원시 상태를 건드렸으므로 캐시를 비운다

		// b0(뷰/투영): NDC를 그대로 쓰려고 단위행렬, b1(월드): 1.2배 확대
		FrameConstants frame;
		frame.view_ = mat4::Identity();
		frame.projection_ = mat4::Identity();
		frame.cameraPosition_ = vec4(0.0f, 0.0f, 0.0f, 1.0f);
		frameCb.Update(device.Context(), frame);
		context.SetConstantBuffer(ShaderStage::ssVertex, 0, frameCb.Raw());

		ObjectConstants object;
		object.world_ = mat4::Scale(1.2f);
		objectCb.Update(device.Context(), object);
		context.SetConstantBuffer(ShaderStage::ssVertex, 1, objectCb.Raw());

		// 핵심! 셰이더/상태 4종/텍스처/b2까지 이 한 줄이 전부 바인딩한다.
		if (material.Bind(context))
		{
			quad.Bind(context);
			quad.Draw(context);
		}

		device.Present(true);
	}

	// 7. 정리: 등록한 텍스처는 매니저 Finalize가 함께 소멸시킨다.
	quad.Finalize();
	material.Finalize();
	g_cResourceMgr.Finalize();
	device.Finalize();
	window.Destroy();
}
