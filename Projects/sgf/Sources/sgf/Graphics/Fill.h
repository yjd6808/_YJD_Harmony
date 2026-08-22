/*
 * 작성자: 윤정도
 * 생성일: 8/16/2026 (게임 오브젝트 시스템)
 * =====================
 * 2D 채움(Fill) — enum 금지, 콜백 기반 커스텀 채움 (엔진 무수정 확장)
 *
 * [무엇인가?]
 * "어떤 기하(콜백)" + "기하 고유 파라미터(텍스처/uv/슬라이스/ratio)"를 값으로
 * 들고 다니는 채움 정의. 단색 채우기만이 아니라 9-패치/게이지/말풍선 같은
 * 형태 생성 전부를 포괄한다.
 *
 * [왜 enum이 아닌가?]
 * enum + switch는 새 채움 추가 시 엔진 헤더 수정 + 재빌드가 필요하다.
 * 콜백(Fill::Custom(fn))은 사용자 코드 하나로 확장이 끝난다.
 *
 * [역할 분담]
 * - Fill: "무엇을" 그릴지 (콜백 + 기하 고유 파라미터)
 * - RenderParams: "어디에, 어떻게" (영역 + 매 호출 색/옵션 + 렌더 버킷)
 * 콜백은 _fill에서 기하 파라미터를, _params에서 영역/색을 읽는다.
 */

#pragma once

#include "jc/Math.h"
#include "jc/Container/Vector.h"
#include "sgf/Graphics/Vertex.h"

NS_SGF_BEGIN

using namespace jc;

class Texture;

// 전방 선언 (콜백 시그니처에서 참조로만 사용)
class Fill;
struct RenderParams;
struct FillResult;

// 렌더 버킷 — 트리 밴드와 무관한 씬 전역 순서 (플러시 순서: Bottom → Default → Top)
enum class RenderLayer
{
	Bottom,		// 씬 전역 최하단 (배경 원경 등)
	Default,	// 트리 밴드 (zOrder) — 기본값
	Top			// 씬 전역 최상단 (HP바/툴팁/이펙트/커서)
};

// 채움 콜백 시그니처 — 이것 하나만 알면 누구나 커스텀 채움을 만들 수 있다.
// _fill = 채움 고유 파라미터 (텍스처/uv/슬라이스/ratio), _params = 매 호출 입력 (영역/색)
using FillCallback = void (*)(const Fill& _fill, const RenderParams& _params, FillResult& _out);

// 채움 정의 — "어떤 기하(콜백)" + "기하 고유 파라미터". 값 타입, 복사로 전달.
class Fill
{
public:
	Fill() = default;						// 무효 채움 (IsValid() == false)

	////////////////////////////////////////////////////////////////////////////////////////
	// 내장 채움 팩토리 (고유 파라미터는 채움이 보관 — 매 호출 변경되는 색은 RenderParams)
	static Fill Solid();												// 단색 — 채움색은 RenderParams.color1_
	static Fill Texture(sgf::Texture* _pTexture,
		const rect& _uv = rect(0.0f, 0.0f, 1.0f, 1.0f));				// 텍스처 쿼드
	static Fill NinePatch(sgf::Texture* _pTexture,
		_f32 _sliceLeft, _f32 _sliceTop, _f32 _sliceRight, _f32 _sliceBottom,
		const rect& _uv = rect(0.0f, 0.0f, 1.0f, 1.0f));				// 9-패치 (텍스처 9분할)
	static Fill Gauge(_f32 _ratio);										// 수평 게이지 (color1=배경, color2=채움)
	static Fill Custom(FillCallback _pCallback, _u32 _userData = 0);	// 외부 정의 — 엔진 무관
	// [엔진 내장 6종(Line/Circle/Polygon/Triangle) 전용] — _pData는 DeclareStatic 호출 동안만 유효
	// (선언 즉시 동기적으로 BuildFill되므로 스택 데이터 전달이 안전하다)
	static Fill CustomData(FillCallback _pCallback, const void* _pData);

	FillCallback GetCallback() const { return pCallback_; }
	// 커스텀 채움이 고유 파라미터를 읽는 창구
	sgf::Texture* GetTexture() const { return pTexture_; }
	const rect&    GetUV() const { return uv_; }
	_f32 GetSliceLeft() const { return slice_[0]; }
	_f32 GetSliceTop() const { return slice_[1]; }
	_f32 GetSliceRight() const { return slice_[2]; }
	_f32 GetSliceBottom() const { return slice_[3]; }
	_f32 GetRatio() const { return ratio_; }
	_u32 GetUserData() const { return userData_; }
	const void* GetData() const { return pData_; }	// [엔진 내장 6종] 콜백이 사용
	bool IsValid() const { return pCallback_ != nullptr; }

private:
	Fill(FillCallback _pCallback, sgf::Texture* _pTexture, const rect& _uv,
		const _f32 (&_slice)[4], _f32 _ratio, _u32 _userData, const void* _pData = nullptr)
		: pCallback_(_pCallback), pTexture_(_pTexture), uv_(_uv)
		, ratio_(_ratio), userData_(_userData), pData_(_pData)
	{
		slice_[0] = _slice[0]; slice_[1] = _slice[1]; slice_[2] = _slice[2]; slice_[3] = _slice[3];
	}

private:
	FillCallback pCallback_ = nullptr;
	sgf::Texture* pTexture_ = nullptr;				// 텍스처 채움 파라미터
	rect uv_ = rect(0.0f, 0.0f, 1.0f, 1.0f);		// 텍스처 영역
	_f32 slice_[4] = { 0.0f, 0.0f, 0.0f, 0.0f };	// 9-패치 슬라이스 (l/t/r/b 픽셀)
	_f32 ratio_ = 1.0f;								// 게이지 비율 (0~1)
	_u32 userData_ = 0;								// 커스텀 파라미터 (Custom 전용)
	const void* pData_ = nullptr;					// [엔진 내장 6종] 콜백이 읽는 데이터 (빌림)
};

// 그리기 입력: "어디에, 어떻게, 뭘로" — 값 타입, 복사로 전달
struct RenderParams
{
	rect  region_;								// 목적지 영역 (노드 로컬 좌표)
	color color1_;								// 주 색상 (단색 채움색, 텍스처 틴트, 게이지 배경)
	color color2_;								// 보조 색상 (게이지 채움색, 그라디언트 등)
	_u32  option_ = 0;							// 채움 방식별 옵션 (매 호출 변경 가능 — 예: 9-패치 스트레치 모드)
	RenderLayer layer_ = RenderLayer::Default;	// 렌더 버킷 — 트리 순서와 무관한 씬 전역 최상단/최하단
	Fill fill_;									// 채움 (콜백 + 고유 파라미터)
};

// 채움 출력: 생성된 버텍스/인덱스. 버퍼는 렌더러가 소유·재사용 (매 호출 할당 없음)
struct FillResult
{
	jc::Vector<VertexPTC> vertices_;	// 영역 → 정점들
	jc::Vector<_u16>      indices_;		// 삼각형 인덱스
	Texture* pTexture_ = nullptr;		// 결과에 사용된 텍스처
};

NS_SGF_END
