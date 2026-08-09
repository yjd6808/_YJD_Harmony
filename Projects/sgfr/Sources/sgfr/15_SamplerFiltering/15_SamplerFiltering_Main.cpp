/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 1:40:00 PM
 * =====================
 * 15. 샘플러와 필터링 (Sampler & Filtering)
 *
 * [이 튜토리얼에서 배우는 것]
 *  1. 샘플링: 화면 픽셀 하나에 텍스처의 어떤 색을 가져올지 정하는 규칙
 *  2. 필터: Point(최근접) vs Linear(선형 보간)의 화질 차이
 *  3. 주소 모드: UV가 0~1을 벗어났을 때 Wrap/Clamp/Mirror의 차이
 *  4. 엔진의 GraphicDevice::SetSampler로 상태를 실시간 교체하는 법
 *
 * [Before/After 비교 뷰]
 *  화면이 노란 세로 경계선으로 좌/우로 나뉜다.
 *  - 왼쪽  (Before): 기준 상태 = Point + Wrap 고정
 *  - 오른쪽(After) : 현재 선택한 필터/주소 모드 적용
 *  같은 텍스처, 같은 UV인데 샘플러 규칙만 달라서 생기는 차이를 한 화면에서 비교한다!
 *  (셰이더가 픽셀의 화면 x좌표를 보고 s0/s1 샘플러 중 하나를 골라 쓴다)
 *
 * [왜 중요한가?]
 *  게임의 모든 텍스처는 결국 샘플링을 거쳐 화면에 나온다.
 *  도트 그래픽 게임은 Point가 정답이고, 실사풍 게임은 Linear가 기본이다.
 *  바닥 타일링은 Wrap 없이는 불가능하다. 용도에 맞는 선택이 필요하다!
 *
 * [조작법]
 *  - 1/2: 필터 (Point/Linear) - 오른쪽(After) 화면에만 적용
 *  - 3/4/5: 주소 모드 (Wrap/Clamp/Mirror) - 오른쪽(After) 화면에만 적용
 *  - ESC: 종료
 */

#include "Core.h"
#include "sgfr/15_SamplerFiltering/15_SamplerFiltering_Main.h"
#include "sgfr/15_SamplerFiltering/15_SamplerFiltering_Function.h"

using namespace sgf;
using namespace jc;

namespace
{
	// HLSL의 CbSplit(b0)과 메모리 배치가 일치해야 한다. (16바이트 = 16의 배수)
	struct CbSplit
	{
		_f32 splitPixelX_;	// 화면을 좌(Before)/우(After)로 나누는 경계의 픽셀 x좌표
		_f32 padding_[3];	// 16바이트 정렬용 여백
	};

	// 창 제목에 표시할 모드 이름표 (enum 순서와 일치)
	const wchar_t* s_szFilterNames[] = { L"Point(최근접)", L"Linear(선형보간)" };
	const wchar_t* s_szAddressNames[] = { L"Wrap(반복)", L"Clamp(고정)", L"Mirror(거울)" };
}

// 샘플러와 필터링 튜토리얼을 실행한다. (Point/Linear, Wrap/Clamp/Mirror, 좌우 Before/After 비교)
void SamplerFiltering_Main()
{
	PrintSamplerExplanation();

	// 1. 윈도우 + 디바이스 준비
	Window window;
	if (!window.Create(L"15. 샘플러와 필터링 (1/2 필터, 3/4/5 주소, ESC 종료)", 800, 600))
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

	// 2. 고작 32x32짜리 체커 텍스처를 만든다.
	//    (일부러 작게! 크게 확대해야 필터 차이가 눈에 보인다)
	_u8 pixels[32 * 32 * 4];
	FillCheckerPixels(pixels, 32, 32, 4);

	Texture texture;
	if (!texture.CreateFromMemory(&device, pixels, 32, 32))
	{
		printf("텍스처 생성 실패!\n");
		device.Finalize();
		window.Destroy();
		return;
	}

	// 3. UV가 0~3인 사각형 (0~1을 벗어난 UV로 주소 모드를 실험한다)
	VertexPTC vertices[4];
	_u32 indices[6];
	FillUvQuad(vertices, indices, 3.0f);

	VertexBuffer vb;
	IndexBuffer ib;
	if (!vb.Create(&device, vertices, sizeof(VertexPTC), 4) ||
		!ib.Create(&device, indices, 6))
		{
		printf("버퍼 생성 실패!\n");
		device.Finalize();
		window.Destroy();
		return;
	}

	// 4. 셰이더 + 경계 상수 버퍼
	UINT layoutCount = 0;
	const D3D11_INPUT_ELEMENT_DESC* pLayoutDescs = VertexPTC::LayoutDescs(&layoutCount);

	Shader shader;
	ConstantBuffer<CbSplit> cbSplit;
	if (!shader.CompileFromString(&device, TextureQuadShaderSource(), pLayoutDescs, layoutCount) ||
		!cbSplit.Create(&device))
		{
		printf("셰이더/상수 버퍼 생성 실패!\n");
		device.Finalize();
		window.Destroy();
		return;
	}

	// 5. 현재 샘플러 상태 (키 입력으로 바꾼다. 오른쪽 After 화면에만 적용된다)
	GraphicDevice::SamplerFilter filter = GraphicDevice::SamplerFilter::Point;
	GraphicDevice::SamplerAddress address = GraphicDevice::SamplerAddress::Wrap;

	// 창 제목에 현재 모드를 표시하는 보조 람다
	auto UpdateTitle = [&]()
	{
		wchar_t szTitle[256];
		swprintf_s(szTitle, L"15. 샘플러 - 왼쪽: Point+Wrap(기준) | 오른쪽: %s + %s (1/2, 3/4/5, ESC)",
			s_szFilterNames[static_cast<int>(filter)],
			s_szAddressNames[static_cast<int>(address)]);
		window.SetTitle(szTitle);
	};
	UpdateTitle();

	// 6. 렌더 루프
	while (window.PumpMessage())
	{
		if (input.IsKeyPressed(VK_ESCAPE))
		{
			break;
		}

		// 필터/주소 모드 전환 (문자 키 '1'~'5'는 가상 키 코드와 값이 같다)
		bool bChanged = false;
		if (input.IsKeyPressed('1')) { filter = GraphicDevice::SamplerFilter::Point;    bChanged = true; }
		if (input.IsKeyPressed('2')) { filter = GraphicDevice::SamplerFilter::Linear;   bChanged = true; }
		if (input.IsKeyPressed('3')) { address = GraphicDevice::SamplerAddress::Wrap;   bChanged = true; }
		if (input.IsKeyPressed('4')) { address = GraphicDevice::SamplerAddress::Clamp;  bChanged = true; }
		if (input.IsKeyPressed('5')) { address = GraphicDevice::SamplerAddress::Mirror; bChanged = true; }
		if (bChanged)
		{
			UpdateTitle();
		}

		input.NextFrame();

		device.BeginFrame(color(0.08f, 0.08f, 0.1f, 1.0f));

		// [Before] s0 슬롯: 기준 샘플러 (Point + Wrap 고정)
		device.SetSampler(GraphicDevice::SamplerFilter::Point, GraphicDevice::SamplerAddress::Wrap, 0);
		// [After] s1 슬롯: 현재 선택한 샘플러
		// (상태 객체는 엔진이 내부에서 캐싱하므로 매 프레임 불러도 부담이 없다)
		device.SetSampler(filter, address, 1);

		// 화면 절반 위치를 경계로 전달: 픽셀 셰이더가 좌/우를 구분해 s0/s1을 골라 쓴다.
		CbSplit cbS;
		cbS.splitPixelX_ = window.Width() * 0.5f;
		cbS.padding_[0] = cbS.padding_[1] = cbS.padding_[2] = 0.0f;
		cbSplit.UpdateAndBind(&device, cbS, 0);

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
