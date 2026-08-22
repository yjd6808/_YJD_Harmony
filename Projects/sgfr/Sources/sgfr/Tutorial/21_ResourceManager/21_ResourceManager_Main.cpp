/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 6:30:00 PM
 * =====================
 * 21. 리소스 매니저 (ResourceMgr)
 *
 * [이 튜토리얼에서 배우는 것]
 * 1. 모든 공유 리소스(IResource)는 _u64 키로 등록/검색/제거한다
 * 2. 키는 jc의 Provider<IdProviderReuse<_u64>>가 발급 - 반납된 키는 재사용(LIFO)
 * 3. 경로 별칭(path)으로 같은 파일의 중복 로드를 막는다
 * 4. Initialize에서 디폴트 리소스 17종이 자동 준비되고, Finalize에서 일괄 소멸된다
 *
 * [Before/After 비교]
 * - Before: 텍스처/셰이더를 각 튜토리얼이 지역 변수로 들고 직접 수명 관리
 * - After: 매니저에 Add하면 소유권이 넘어가고, 이후는 키로만 참조한다
 * ("소유자 = 수명 결정자" 원칙)
 *
 * [참고] 디폴트 리소스 키는 Remove가 금지되어 있다. (jc_assert 발동)
 * 디폴트는 매니저 Finalize에서만 소멸된다.
 */

#include "Core.h"
#include "sgfr/Tutorial/21_ResourceManager/21_ResourceManager_Main.h"

using namespace sgf;
using namespace jc;

namespace
{
	//////////////////////////////////////////////////////////////////////////////////////////

	// 2x2 단색 텍스처를 만든다. (데모용 더미 리소스)
	Texture* CreateSolidTexture(GraphicDevice* _pDevice, _u8 _r, _u8 _g, _u8 _b)
	{
		_u8 pixels[2 * 2 * 4];
		for (_s32 i = 0; i < 4; ++i)
		{
			pixels[i * 4 + 0] = _r;
			pixels[i * 4 + 1] = _g;
			pixels[i * 4 + 2] = _b;
			pixels[i * 4 + 3] = 255;
		}

		Texture* pTexture = dbg_new Texture();
		if (!pTexture->CreateFromMemory(_pDevice, pixels, 2, 2))
		{
			delete pTexture;
			return nullptr;
		}
		return pTexture;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////

// 리소스 매니저 튜토리얼을 실행한다. (콘솔 출력만으로 키 생명주기 관찰)
void ResourceManager_Main()
{
	// 1. 창 없이 디바이스만 초기화한다. (03번과 같은 헤드리스 모드)
	GraphicDevice device;
	if (!device.Initialize())
	{
		jc::Console::WriteLine("그래픽 디바이스 초기화 실패!");
		return;
	}

	// 2. 매니저 초기화 - 이 순간 디폴트 리소스 17종이 자동 등록된다.
	// (2D/3D 버텍스/픽셀 셰이더 4종 + 1x1 흰색 텍스처 1종 + 2D/3D 디폴트 머티리얼 2종
	//  + 프리미티브 메시 10종: 2D 4종(Rect/Circle/Triangle/Line) + 3D 6종(Cube/Sphere/Capsule/Cylinder/Plane/Quad))
	if (!g_cResourceMgr.Initialize(&device))
	{
		jc::Console::WriteLine("리소스 매니저 초기화 실패!");
		device.Finalize();
		return;
	}

	jc::Console::WriteLine("--- 1. 디폴트 리소스 ---");
	jc::Console::Write("초기화 직후 등록 개수: %llu (디폴트 17종)\n", g_cResourceMgr.GetCount());
	jc::Console::Write("디폴트 텍스처 키: %llu\n", g_cResourceMgr.GetDefaultTextureKey());
	jc::Console::Write("디폴트 2D 머티리얼 키: %llu\n", g_cResourceMgr.GetDefaultMaterial2DKey());
	g_cResourceMgr.PrintStatus();

	// 3. 리소스 등록: Add에 넘기는 순간 소유권이 매니저로 넘어간다.
	jc::Console::WriteLine("\n--- 2. 등록/검색/제거 ---");

	Texture* pRed = CreateSolidTexture(&device, 255, 0, 0);
	Texture* pGreen = CreateSolidTexture(&device, 0, 255, 0);
	if (pRed == nullptr || pGreen == nullptr)
	{
		jc::Console::WriteLine("텍스처 생성 실패!");
		delete pRed;
		delete pGreen;
		g_cResourceMgr.Finalize();
		device.Finalize();
		return;
	}
	pRed->SetDebugName("RedTexture");
	pGreen->SetDebugName("GreenTexture");

	const _u64 redKey = g_cResourceMgr.Add(pRed);
	const _u64 greenKey = g_cResourceMgr.Add(pGreen, "memory://green");	// 경로 별칭 부여
	jc::Console::Write("빨간 텍스처 키: %llu, 초록 텍스처 키: %llu\n", redKey, greenKey);

	// 타입 안전 검색: 키로 찾되, 타입이 다르면 nullptr를 돌려준다.
	Texture* pFound = g_cResourceMgr.Find<Texture>(redKey);
	Material* pWrongType = g_cResourceMgr.Find<Material>(redKey);
	jc::Console::Write("Find<Texture>(빨간 키) = %s\n", (pFound == pRed) ? "성공 (같은 포인터)" : "실패");
	jc::Console::Write("Find<Material>(빨간 키) = %s (타입 불일치 보호)\n", (pWrongType == nullptr) ? "nullptr" : "???");

	// 경로로 키 역조회
	const _u64 foundByPath = g_cResourceMgr.FindKeyByPath("memory://green");
	jc::Console::Write("FindKeyByPath(\"memory://green\") = %llu (기대값 %llu)\n", foundByPath, greenKey);

	// 같은 경로로 또 등록하면 중복으로 거부된다. (INVALID_RESOURCE_KEY 반환, 소유권은 호출자에게 그대로)
	Texture* pDuplicated = CreateSolidTexture(&device, 0, 0, 255);
	if (pDuplicated != nullptr)
	{
		const _u64 duplicatedKey = g_cResourceMgr.Add(pDuplicated, "memory://green");
		jc::Console::Write("중복 경로 Add = %llu (0 = INVALID_RESOURCE_KEY, 거부됨)\n", duplicatedKey);
		if (duplicatedKey == INVALID_RESOURCE_KEY)
		{
			delete pDuplicated;	// 거부됐으므로 소유권은 여전히 우리에게 있다
		}
	}

	// 4. 키 재사용 관찰: Remove된 키는 freeList로 돌아가 다음 Add에서 재사용된다(LIFO).
	jc::Console::WriteLine("\n--- 3. 키 재사용 (IdProviderReuse) ---");
	jc::Console::Write("빨간 텍스처 제거 (키 %llu 반납)\n", redKey);
	g_cResourceMgr.Remove(redKey);	// 이 순간 pRed는 소멸된다! (소유자 = 수명 결정자)

	Texture* pYellow = CreateSolidTexture(&device, 255, 255, 0);
	if (pYellow != nullptr)
	{
		pYellow->SetDebugName("YellowTexture");
		const _u64 yellowKey = g_cResourceMgr.Add(pYellow);
		jc::Console::Write("새 텍스처 키: %llu (반납한 키 %llu가 재사용되면 성공!)\n", yellowKey, redKey);
	}

	jc::Console::WriteLine("\n--- 4. 최종 상태 ---");
	g_cResourceMgr.PrintStatus();

	// 5. 정리: Finalize 한 번이면 남은 모든 리소스(디폴트 포함)가 일괄 소멸된다.
	g_cResourceMgr.Finalize();
	device.Finalize();
	jc::Console::WriteLine("\n정리 완료. (남은 리소스는 Finalize가 모두 소멸시켰다)");
}
