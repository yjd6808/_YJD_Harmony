/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 6:30:00 PM
 * =====================
 * 03. 디바이스와 컨텍스트 (Device & Context)
 *
 * [이 튜토리얼에서 배우는 것]
 *  1. ID3D11Device(공장)와 ID3D11DeviceContext(조립 라인)의 역할 분리
 *  2. sgf의 GraphicDevice(생성 담당)와 GraphicContext(바인딩/그리기 담당) 구조
 *  3. 상태 캐시: 같은 상태를 두 번 설정하면 실제 D3D 호출이 생략되는 원리
 *
 * [Before/After 비교]
 *  - Before(v2): device.Context()로 원시 ID3D11DeviceContext를 직접 노출.
 *                호출하는 쪽이 중복 바인딩을 신경 쓰지 않으면 매 프레임 낭비가 쌓인다.
 *  - After (v3): GraphicContext가 모든 바인딩을 감싸고, 직전 상태와 같으면
 *                D3D 호출 자체를 건너뛴다. (통계로 직접 확인!)
 *
 * [왜 중요한가?]
 *  D3D11의 상태 바인딩(Set 계열)은 값싸 보이지만 드로우가 수천 번이 되면
 *  런타임 검증/드라이버 오버헤드가 누적된다. 실전 렌더러는 반드시
 *  "직전 상태와 같으면 생략"하는 상태 캐시를 둔다. (23번 튜토리얼에서 심화)
 */

#include "Core.h"
#include "sgfr/03_DeviceAndContext/03_DeviceAndContext_Main.h"

using namespace sgf;
using namespace jc;

// 디바이스/컨텍스트 분리 튜토리얼을 실행한다. (윈도우 없이 디바이스만 만들어 실험)
void DeviceAndContext_Main()
{
	printf("=== 03. 디바이스와 컨텍스트 ===\n\n");
	printf("[역할 분리]\n");
	printf("  ID3D11Device        = 공장   : 버퍼/텍스처/셰이더 같은 리소스를 '만든다'\n");
	printf("  ID3D11DeviceContext = 조립라인: 만들어진 리소스를 파이프라인에 '묶고 그린다'\n\n");
	printf("[sgf v3 대응]\n");
	printf("  GraphicDevice  -> 생성/수명 담당 (Initialize/Finalize, CreateXXX)\n");
	printf("  GraphicContext -> 바인딩/그리기 담당 (SetXXX, Draw/DrawIndexed)\n\n");

	// 1. 창 없이 디바이스만 초기화한다. (v2.1부터 창과 디바이스가 분리되어 가능)
	GraphicDevice device;
	if (!device.Initialize())
	{
		printf("그래픽 디바이스 초기화 실패!\n");
		return;
	}
	printf("1) 디바이스 초기화 성공 (창 없이 디바이스만 생성됨)\n\n");

	// 2. 컨텍스트는 디바이스가 소유한다. 참조로 빌려 쓴다.
	GraphicContext& context = device.GetContext();

	// 3. 상태 캐시 실험: 같은 토폴로지를 연속으로 설정해본다.
	context.ResetStats();
	context.SetPrimitiveTopology(PrimitiveTopology::ptTriangleList);	// (1) 실제 D3D 호출
	context.SetPrimitiveTopology(PrimitiveTopology::ptTriangleList);	// (2) 직전과 같음 -> 생략!
	context.SetPrimitiveTopology(PrimitiveTopology::ptLineList);		// (3) 달라짐 -> 실제 호출

	printf("2) 토폴로지를 [삼각형, 삼각형, 선] 순서로 3번 설정했다.\n");
	printf("   실제 D3D 호출 수   : %llu (예상: 2)\n", context.GetApiCallCount());
	printf("   캐시로 생략된 호출 : %llu (예상: 1)\n\n", context.GetSkippedCallCount());

	printf("[정리]\n");
	printf("  같은 상태를 다시 설정하면 GraphicContext가 알아서 걸러낸다.\n");
	printf("  단, 외부에서 원시 컨텍스트를 직접 만졌다면 InvalidateCache()로 캐시를 비워야 한다.\n");
	printf("  (BeginFrame처럼 엔진 내부에서 상태를 바꾸는 지점이 대표적)\n\n");

	// 4. 정리
	device.Finalize();
	printf("디바이스 정리 완료. (함수가 반환되면 런처가 목차로 돌아간다)\n");
}
