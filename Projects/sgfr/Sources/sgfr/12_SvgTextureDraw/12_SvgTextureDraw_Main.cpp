/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 11:00:00 AM
 * =====================
 * 12. SVG 텍스처 그리기 (SVG Texture) - nanosvg 래스터화
 *
 * [이 튜토리얼에서 배우는 것]
 *  1. 벡터 그래픽(SVG)과 래스터 그래픽(PNG)의 차이
 *  2. 래스터화(Rasterize): 도형 명령어 -> 픽셀 배열 변환
 *  3. scale 인자로 원하는 해상도로 깨끗하게 키우는 법
 *
 * [준비물: nanosvg]
 *  sgf::Texture::LoadFromSvgFile은 단일 헤더 라이브러리 nanosvg를 사용한다.
 *  Projects/sgf/Sources/sgf/_Extern/nanosvg/README.md의 안내대로
 *  nanosvg.h / nanosvgrast.h 2개 파일을 받아 넣으면 활성화된다.
 *  없으면(SGF_HAS_NANOSVG 미정의) LoadFromSvgFile이 false를 반환하므로
 *  이 튜토리얼은 CPU로 직접 그린 대체 텍스처로 동작한다.
 *
 * [조작법]
 *  - 화면에 SVG(또는 대체) 텍스처가 크기를 바꿔가며 그려진다.
 *  - 확대되어도 깨끗한 이유: 충분히 큰 해상도로 래스터화했기 때문.
 *  - ESC로 종료.
 */

#include "Core.h"
#include "sgfr/12_SvgTextureDraw/12_SvgTextureDraw_Main.h"
#include "sgfr/12_SvgTextureDraw/12_SvgTextureDraw_Function.h"
#include "sgfr/Common/TutorialCommon.h"	// TextureShaderSource 공용 사용

using namespace sgf;
using namespace jc;

// SVG 텍스처 튜토리얼을 실행한다. (nanosvg 래스터 + 벡터 vs 래스터)
void SvgTextureDraw_Main()
{
	PrintVectorVsRasterExplanation();

	// 1. 윈도우 + 디바이스 준비
	Window window;
	if (!window.Create(L"12. SVG 텍스처 그리기 (ESC로 종료)", 800, 600))
	{
		printf("윈도우 생성 실패!\n");
		return;
	}

	InputManager input;
	window.ConnectInput(&input);

	GraphicDevice device;
	if (!device.Initialize(window.Handle(), window.Width(), window.Height()))
	{
		printf("그래픽 디바이스 초기화 실패!\n");
		window.Destroy();
		return;
	}

	// 2. SVG 텍스처 준비
	//    (1순위) 예제 SVG 파일을 만들고 nanosvg로 래스터화 (scale 2.0 = 512x512)
	//    (2순위) nanosvg 미설치 시 CPU로 직접 그린 대체 텍스처
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
		printf("%s 래스터화 성공! (%d x %d)\n", szSvgPath, texture.Width(), texture.Height());
	}
	else if (CreateFallbackCircleTexture(&device, &texture))
	{
		printf("nanosvg 미설치 -> CPU로 직접 그린 대체 텍스처로 진행합니다.\n");
		printf("(sgf/_Extern/nanosvg/README.md 참고해서 헤더  2개를 넣으면 진짜 SVG 래스터화를 볼 수 있습니다)\n");
	}
	else
	{
		printf("텍스처 생성 실패!\n");
		device.Finalize();
		window.Destroy();
		return;
	}

	// 3. 동적 정점 버퍼 (크기가 매 프레임 바뀌므로 bDynamic = true)
	VertexPTC vertices[4] = {};
	const _u32 indices[] = { 0, 1, 2, 2, 1, 3 };

	VertexBuffer vb;
	IndexBuffer ib;
	if (!vb.Create(&device, vertices, sizeof(VertexPTC), 4, true) ||
		!ib.Create(&device, indices, 6))
		{
		printf("버퍼 생성 실패!\n");
		device.Finalize();
		window.Destroy();
		return;
	}

	// 4. 셰이더는 11번과 완전히 동일 (텍스처 샘플링) -> 재사용!
	UINT layoutCount = 0;
	const D3D11_INPUT_ELEMENT_DESC* pLayoutDescs = VertexPTC::LayoutDescs(&layoutCount);

	Shader shader;
	if (!shader.CompileFromString(&device, TextureShaderSource(), pLayoutDescs, layoutCount))
	{
		printf("셰이더 컴파일 실패!\n");
		device.Finalize();
		window.Destroy();
		return;
	}

	// 5. 알파 블렌딩 (SVG의 투명 배경이 올바로 보이도록)
	device.SetAlphaBlending(true);

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

		// 크기가 0.2 ~ 0.9 사이를 천천히 왛다거리도록 사인파로 계산
		const _f32 half = 0.55f + 0.35f * sinf(elapsed * 0.8f);
		const _f32 aspect = window.AspectRatio();

		// 매 프레임 정점을 새로 계산해 동적 버퍼를 갱신한다. (05번에서 배운 동적 버퍼 활용)
		vertices[0] = { Vec3(-half, +half * aspect, 0.0f), Vec2(0.0f, 0.0f), Color::White() };
		vertices[1] = { Vec3(+half, +half * aspect, 0.0f), Vec2(1.0f, 0.0f), Color::White() };
		vertices[2] = { Vec3(-half, -half * aspect, 0.0f), Vec2(0.0f, 1.0f), Color::White() };
		vertices[3] = { Vec3(+half, -half * aspect, 0.0f), Vec2(1.0f, 1.0f), Color::White() };
		vb.Update(&device, vertices, 4);

		device.BeginFrame(Color(0.12f, 0.12f, 0.16f, 1.0f));

		texture.Bind(&device, 0);
		vb.Bind(&device);
		ib.Bind(&device);
		shader.Bind(&device);
		device.Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		device.Context()->DrawIndexed(6, 0, 0);

		device.EndFrame(true);
	}

	// 7. 정리
	device.Finalize();
	window.Destroy();
}
