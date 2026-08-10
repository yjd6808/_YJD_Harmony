/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 9:30:00 AM
 * =====================
 * 04. 디바이스와 스왑체인 (Device & SwapChain)
 *
 * [이 튜토리얼에서 배우는 것]
 *  1. ID3D11Device        : GPU 리소스(버퍼/텍스처/셰이더)를 만드는 "공장"
 *  2. ID3D11DeviceContext : 만든 리소스로 그리기 명령을 내리는 "작업반장"
 *  3. IDXGISwapChain      : 백버퍼/프론트버퍼를 교체(Present)하는 "화면 전달자"
 *  4. 더블 버퍼링        : 왜 화면이 깜박이지 않고 부드럽게 보이는가
 *
 * [더블 버퍼링이란?]
 *  화면에 보이는 그림(프론트버퍼)과 지금 그리는 중인 그림(백버퍼)을 분리하고,
 *  다 그리면 둘을 한번에 교체한다. 그리는 과정이 단계별로 노출되지 않아
 *  깜박임/찢어짐(tearing)이 사라진다. 이 교체를 해주는 게 EndFrame()의 Present다.
 *
 * [화면 지우기(Clear)는 왜 필요한가?]
 *  지난 프레임에 그린 그림이 백버퍼에 그대로 남아있기 때문에,
 *  매 프레임 시작 시 단색으로 밀어버리고 새로 그린다. (칠판 지우기)
 *
 * [조작법]
 *  - 그냥 바라보면 된다. 배경색이 무지개처럼 변한다.
 *  - ESC 또는 창 닫기로 종료한다.
 */

#include "Core.h"
#include "sgfr/04_DeviceAndSwapChain/04_DeviceAndSwapChain_Main.h"
#include "sgfr/04_DeviceAndSwapChain/04_DeviceAndSwapChain_Function.h"

using namespace sgf;

// DX11 디바이스/스왑체인 튜토리얼을 실행한다. (무지개빛 화면 지우기)
void DeviceAndSwapChain_Main()
{
	// 1. 윈도우 생성 (02번 튜토리얼과 동일)
	Window window;
	if (!window.Create(L"04. 디바이스와 스왑체인 (ESC로 종료)", 800, 600))
	{
		printf("윈도우 생성 실패!\n");
		return;
	}

	InputManager input;
	window.ConnectInput(&input);

	// 2. 그래픽 디바이스 초기화
	//    GraphicDevice::Initialize 내부에서 일어나는 일:
	//    (1) D3D11CreateDeviceAndSwapChain : Device + Context + SwapChain 한번에 생성
	//    (2) 백버퍼에 대한 RenderTargetView 생성 ("여기에 그려라"는 표시)
	//    (3) 깊이 버퍼와 DepthStencilView 생성 (3D에서 앞뒤 판정용)
	//    (4) 블렌드/깊이/래스터라이저/샘플러 상태 객체 생성
	//    (5) 뷰포트 설정 (NDC -> 화면 픽셀 좌표 매핑 영역)
	GraphicDevice device;
	if (!device.Initialize(window.Handle(), window.Width(), window.Height()))
	{
		printf("그래픽 디바이스 초기화 실패!\n");
		window.Destroy();
		return;
	}

	printf("디바이스 초기화 성공! 배경색이 부드럽게 변합니다. (ESC로 종료)\n");

	// 3. 시간 측정용 타이머
	FrameTimer timer;
	timer.Reset();

	_f32 elapsed = 0.0f;	// 누적 시간(초)

	// 4. 렌더 루프: "지우기 -> (그리기) -> 보여주기"의 반복
	while (window.PumpMessage())
	{
		if (input.IsKeyPressed(VK_ESCAPE))
		{
			break;
		}
		input.NextFrame();

		// 지난 프레임으로부터 몇 초 지났는지 측정 (프레임 독립적 애니메이션의 핵심)
		timer.Tick();
		elapsed += timer.DeltaTime();

		// BeginFrame: 백버퍼를 지정한 색으로 지우고 그리기 대상으로 묶는다.
		device.BeginFrame(RainbowColor(elapsed));

		// (이번 튜토리얼은 아무것도 그리지 않는다. 화면 지우기 그 자체가 주제!)

		// EndFrame: 백버퍼와 프론트버퍼를 교체한다. (Present)
		// 인자 true = 수직동기(VSync). 모니터 주사율(보통 60Hz)에 맞춰 대기한다.
		device.EndFrame(true);
	}

	// 5. 정리: 생성의 역순으로 해제한다.
	device.Finalize();
	window.Destroy();
}
