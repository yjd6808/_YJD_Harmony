/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 11:00:00 AM
 * =====================
 * 16. SVG 텍스처 그리기 (SVG Texture) - nanosvg 래스터화
 *
 * [이 튜토리얼에서 배우는 것]
 * 1. 벡터 그래픽(SVG)과 래스터 그래픽(PNG)의 차이
 * 2. 래스터화(Rasterize): 도형 명령어 -> 픽셀 배열 변환
 * 3. scale 인자로 원하는 해상도로 깨끗하게 키우는 법
 *
 * [준비물: nanosvg]
 * sgf::Texture::LoadFromSvgFile은 단일 헤더 라이브러리 nanosvg를 사용한다.
 * Projects/sgf/Sources/sgf/_Extern/nanosvg/README.md의 안내대로
 * nanosvg.h / nanosvgrast.h 2개 파일을 받아 넣으면 활성화된다.
 * 없으면(SGF_HAS_NANOSVG 미정의) LoadFromSvgFile이 false를 반환하므로
 * 이 튜토리얼은 CPU로 직접 그린 대체 텍스처로 동작한다.
 *
 * [조작법]
 * - 화면에 SVG(또는 대체) 텍스처가 크기를 바꿔가며 그려진다.
 * - 확대되어도 깨끗한 이유: 충분히 큰 해상도로 래스터화했기 때문.
 * - ESC로 종료.
 */

#include "Core.h"
#include "sgf/Graphics/ResourceMgr.h"
#include "sgfr/Tutorial/16_SvgTextureDraw/16_SvgTextureDraw_Main.h"
#include "sgfr/Tutorial/16_SvgTextureDraw/16_SvgTextureDraw_Function.h"
#include "sgfr/Common/TutorialCommon.h"	// TextureShaderSource 공용 사용

using namespace sgf;
using namespace jc;

// SVG 텍스처 튜토리얼을 실행한다. (nanosvg 래스터 + 벡터 vs 래스터)
void SvgTextureDraw_Main()
{
	PrintVectorVsRasterExplanation();

	// 1. 윈도우 + 디바이스 준비
	Window window;
	if (!window.Create("16. SVG 텍스처 그리기 (ESC로 종료)", 800, 600))
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
	if (!g_cResourceMgr.Initialize(&device))
	{
		jc::Console::WriteLine("리소스 매니저 초기화 실패!");
	g_cResourceMgr.Finalize();
		device.Finalize();
		window.Destroy();
		return;
	}

	if (!device.CreateSwapChain(window.Handle(), window.Width(), window.Height(), PixelFormat::pfRgba8))
	{
	jc::Console::WriteLine("스왑체인 생성 실패!");
	g_cResourceMgr.Finalize();
	device.Finalize();
	window.Destroy();
	return;
	}

	// 2. SVG 텍스처 준비
	// (1순위) 예제 SVG 파일을 만들고 nanosvg로 래스터화 (scale 2.0 = 512x512)
	// (2순위) nanosvg 미설치 시 CPU로 직접 그린 대체 텍스처
	const char* szSvgPath = "sample.svg";	// 실행 팏더 기준 상대 경로
	Texture texture;
	bool bLoaded = false;

	if (EnsureSampleSvgFile(szSvgPath))
	{
		// scale 2.0: SVG 원본(256x256)을 2배 해상도(512x512)로 래스터화
		// -> 화면에서 확대되어도 깨끗하다. 이것이 벡터의 힘!
		bLoaded = texture.LoadFromSvgFile(&device, szSvgPath, 2.0f);
	}

	if (bLoaded)
	{
		jc::Console::Write("%s 래스터화 성공! (%d x %d)\n", szSvgPath, texture.Width(), texture.Height());
	}
	else if (CreateFallbackCircleTexture(&device, &texture))
	{
		jc::Console::WriteLine("nanosvg 미설치 -> CPU로 직접 그린 대체 텍스처로 진행합니다.");
		jc::Console::WriteLine("(sgf/_Extern/nanosvg/README.md 참고해서 헤더  2개를 넣으면 진짜 SVG 래스터화를 볼 수 있습니다)");
	}
	else
	{
		jc::Console::WriteLine("텍스처 생성 실패!");
	g_cResourceMgr.Finalize();
		device.Finalize();
		window.Destroy();
		return;
	}

	// 3. 동적 정점 버퍼 (크기가 매 프레임 바뀌므로 bDynamic = true)
	VertexPTC vertices[4] = {};
	const _u32 indices[] = { 0, 1, 2, 2, 1, 3 };

	VertexBuffer vb;
	IndexBuffer ib;
	if (!vb.Create(&device, vertices, 4, VertexPTC::Decl()) ||
		!ib.Create(&device, indices, 6))
		{
		jc::Console::WriteLine("버퍼 생성 실패!");
	g_cResourceMgr.Finalize();
		device.Finalize();
		window.Destroy();
		return;
	}

	// 4. 셰이더는 15번과 완전히 동일 (텍스처 샘플링) -> 재사용!

	_u64 vsShader = device.Context().CreateVertexShader(TextureShaderSource());
	_u64 psShader = device.Context().CreatePixelShader(TextureShaderSource());
	if (vsShader == INVALID_RESOURCE_KEY || psShader == INVALID_RESOURCE_KEY)
	{
		jc::Console::WriteLine("셰이더 컴파일 실패!");
	g_cResourceMgr.Finalize();
		device.Finalize();
		window.Destroy();
		return;
	}

	// 5. 알파 블렌딩 (SVG의 투명 배경이 올바로 보이도록)
	device.Context().SetBlend(BlendMode::bmAlpha);

	FrameTimer timer;
	timer.Reset();
	_f32 elapsed = 0.0f;

	// 6. 렌더 루프
	while (window.PumpMessage())
	{
		if (input.IsKeyPressed(VK_ESCAPE))
		{
			break;
		}
		input.NextFrame();

		timer.Tick();
		elapsed += timer.DeltaTime();

		// 크기가 0.2 ~ 0.9 사이를 천천히 왕복하도록 사인파로 계산
		const _f32 half = 0.55f + 0.35f * sinf(elapsed * 0.8f);
		const _f32 aspect = window.AspectRatio();

		// 매 프레임 정점을 새로 계산해 동적 버퍼를 갱신한다. (06번에서 배운 동적 버퍼 활용)
		vertices[0] = { vec3(-half, +half * aspect, 0.0f), vec2(0.0f, 0.0f), color::WHITE };
		vertices[1] = { vec3(+half, +half * aspect, 0.0f), vec2(1.0f, 0.0f), color::WHITE };
		vertices[2] = { vec3(-half, -half * aspect, 0.0f), vec2(0.0f, 1.0f), color::WHITE };
		vertices[3] = { vec3(+half, -half * aspect, 0.0f), vec2(1.0f, 1.0f), color::WHITE };
		vb.Update(device.Context(), vertices, 4);

		device.BeginFrame(color(0x1F, 0x1F, 0x29, 0xFF));

		texture.Bind(device.Context(), 0);
		vb.Bind(device.Context());
		ib.Bind(device.Context());
		device.Context().SetVertexShader(vsShader);
		device.Context().SetPixelShader(psShader);
		{
		}
		device.Context().SetPrimitiveTopology(PrimitiveTopology::ptTriangleList);
		device.Context().DrawIndexed(6, 0, 0);

		device.Present(true);
	}

	// 7. 정리
	g_cResourceMgr.Finalize();
	device.Finalize();
	window.Destroy();
}

