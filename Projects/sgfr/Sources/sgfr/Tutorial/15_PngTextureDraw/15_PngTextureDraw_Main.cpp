/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 10:50:00 AM
 * =====================
 * 15. PNG 텍스처 그리기 (PNG Texture) - WIC 디코딩과 UV 좌표계
 *
 * [이 튜토리얼에서 배우는 것]
 *  1. PNG 파일이 화면에 나오기까지의 과정
 *     PNG 파일 -> [WIC 디코더] -> RGBA 픽셀 배열 -> [GPU 텍스처] -> [샘플링] -> 화면
 *  2. UV 좌표계 (0~1)와 텍스처 샘플링
 *  3. 알파 블렌딩 (반투명 처리)
 *
 * [WIC란?]
 *  Windows Imaging Component. PNG/JPG/BMP 등을 디코딩해주는 Windows 내장 라이브러리.
 *  별도 외부 라이브러리 없이 PNG를 읽을 수 있다. (sgf::Texture::LoadFromFile이 내부에서 사용)
 *
 * [조작법]
 *  - 실행 팏더의 Resources\sample.png를 먼저 찾고, 없으면 체커보드 텍스처를 생성해 그린다.
 *  - ESC로 종료.
 */

#include "Core.h"
#include "sgfr/Tutorial/15_PngTextureDraw/15_PngTextureDraw_Main.h"
#include "sgfr/Tutorial/15_PngTextureDraw/15_PngTextureDraw_Function.h"
#include "sgfr/Common/TutorialCommon.h"	// TextureShaderSource 공용 사용

using namespace sgf;
using namespace jc;

// PNG 텍스처 튜토리얼을 실행한다. (WIC 디코딩 + UV 좌표계)
void PngTextureDraw_Main()
{
	PrintUvExplanation();

	// 1. 윈도우 + 디바이스 준비
	Window window;
	if (!window.Create(L"15. PNG 텍스처 그리기 (ESC로 종료)", 800, 600))
	{
		jc::Console::WriteLine("윈도우 생성 실패!");
		return;
	}

	InputManager input;
	window.ConnectInput(&input);

	GraphicDevice device;
	if (!device.Initialize(window.Handle(), window.Width(), window.Height()))
	{
		jc::Console::WriteLine("그래픽 디바이스 초기화 실패!");
		window.Destroy();
		return;
	}

	// 2. 텍스처 준비
	//    (1순위) 실행 팏더의 Resources\sample.png를 WIC로 디코딩
	//    (2순위) 파일이 없으면 직접 만든 체커보드 픽셀로 텍스처 생성
	Texture texture;
	if (texture.LoadFromFile(&device, L"Resources\\sample.png"))
	{
		jc::Console::Write("Resources\\sample.png 로드 성공! (%d x %d)\n", texture.Width(), texture.Height());
	}
	else if (CreateCheckerboardTexture(&device, &texture))
	{
		jc::Console::Write("sample.png가 없어 체커보드 텍스처로 대체합니다. (%d x %d)\n", texture.Width(), texture.Height());
		jc::Console::WriteLine("(실행 팏더에 Resources\\sample.png를 넣으면 진짜 PNG를 볼 수 있습니다)");
	}
	else
	{
		jc::Console::WriteLine("텍스처 생성 실패!");
		device.Finalize();
		window.Destroy();
		return;
	}

	// 3. UV가 포함된 정점 (VertexPTC: 위치 + UV + 틴트 색상)
	//    UV (0,0)이 왼쪽 위! (NDC의 y축과 반대 방향이니 항상 주의)
	const _f32 aspect = window.AspectRatio();
	const _f32 halfW = 0.5f;
	const _f32 halfH = 0.5f * aspect;	// 화면 비율 보정으로 정사각형 유지
	const VertexPTC vertices[] = {
		{ vec3(-halfW, +halfH, 0.0f), vec2(0.0f, 0.0f), color::White() },	// 왼쪽위
		{ vec3(+halfW, +halfH, 0.0f), vec2(1.0f, 0.0f), color::White() },	// 오른쪽위
		{ vec3(-halfW, -halfH, 0.0f), vec2(0.0f, 1.0f), color::White() },	// 왼쪽아래
		{ vec3(+halfW, -halfH, 0.0f), vec2(1.0f, 1.0f), color::White() },	// 오른쪽아래
	};
	const _u32 indices[] = { 0, 1, 2, 2, 1, 3 };

	VertexBuffer vb;
	IndexBuffer ib;
	if (!vb.Create(&device, vertices, sizeof(VertexPTC), 4) ||
		!ib.Create(&device, indices, 6))
		{
		jc::Console::WriteLine("버퍼 생성 실패!");
		device.Finalize();
		window.Destroy();
		return;
	}

	// 4. 셰이더 컴파일 (텍스처 샘플링 버전)
	UINT layoutCount = 0;
	const D3D11_INPUT_ELEMENT_DESC* pLayoutDescs = VertexPTC::LayoutDescs(&layoutCount);

	Shader shader;
	if (!shader.CompileFromString(&device, TextureShaderSource(), pLayoutDescs, layoutCount))
	{
		jc::Console::WriteLine("셰이더 컴파일 실패!");
		device.Finalize();
		window.Destroy();
		return;
	}

	// 5. 알파 블렌딩 켜기: PNG의 투명/반투명 영역이 올바로 섞이도록 한다.
	//    공식: 최종색 = 새색 x 알파 + 배경색 x (1 - 알파)
	device.SetAlphaBlending(true);

	// 6. 렌더 루프
	while (window.PumpMessage())
	{
		if (input.IsKeyPressed(VK_ESCAPE))
		{
			break;
		}
		input.NextFrame();

		device.BeginFrame(color(0.1f, 0.1f, 0.15f, 1.0f));

		// 텍스처를 t0 슬롯에 묶는다. (셰이더의 register(t0)과 연결)
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
