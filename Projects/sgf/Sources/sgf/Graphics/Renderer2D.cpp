/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 8:26:00 AM
 * 수정일: 8/16/2026 (예약/그리기 분리 + 스태틱 캐시 + 임의 도형)
 * =====================
 * 2D 배치 렌더러 구현부
 */

#include "Core.h"
#include "sgf/Graphics/Renderer2D.h"
#include "sgf/Graphics/GraphicDevice.h"
#include "sgf/Graphics/Texture.h"
#include "sgf/Graphics/Mesh.h"
#include "sgf/Graphics/Material.h"
#include "sgf/Graphics/ResourceMgr.h"
#include "sgf/Scene/Scene2D.h"
#include "sgf/Core/Application.h"

#include <cmath>

NS_SGF_BEGIN

using namespace jc;

////////////////////////////////////////////////////////////////////////////////////////////
// 내장 HLSL 셰이더
// [셰이더 해설]
// cbuffer: CPU에서 넘겨주는 전역값. row_major로 선언해서
// C++의 행우선 Mat4를 전치 없이 그대로 받는다.
// VSMain: 정점마다 실행. 로컬 좌표 -> 월드(b1) -> 클립 좌표 변환.
// mul(v, M)은 행벡터 x 행렬 순서로, 우리 수학 규약과 일치.
// PSMain: 픽셀마다 실행. 텍스처 색 x 정점 색(틴트) x 머티리얼 색(b2).
// SV_POSITION: "이 값이 클립 좌표다"라고 파이프라인에 알려주는 의미소(Semantic).
// [상수버퍼 규약] (FR-19) — b0 프레임/뷰프로젝션, b1 오브젝트/월드, b2 머티리얼/기본색
static const char* s_szSpriteShader = R"(
cbuffer ConstantBufferFrame : register(b0)
{
	row_major float4x4 viewProjection_;	// 뷰 x 프로젝션 행렬
};

cbuffer ConstantBufferObject : register(b1)
{
	row_major float4x4 world_;			// 월드 행렬 (배치 경로: 단위행렬 고정)
};

cbuffer ConstantBufferMaterial : register(b2)
{
	float4 baseColor_;					// 머티리얼 기본색/틴트 (배치 경로: 흰색 고정)
};

Texture2D texture_ : register(t0);		// 그릴 텍스처
SamplerState sampler_ : register(s0);	// 텍스처 읽기 규칙

struct VSInput
{
	float3 position_ : POSITION;		// 로컬 좌표 (GPU에서 월드 변환 — 불변)
	float2 uv_       : TEXCOORD0;	// 텍스처 좌표
	float4 color_    : COLOR0;		// 틴트 색상
};

struct PSInput
{
	float4 position_ : SV_POSITION;	// 클립 좌표
	float2 uv_       : TEXCOORD0;
	float4 color_    : COLOR0;
};

// 정점 셰이더: 로컬 좌표를 월드(b1) 변환 후 뷰프로젝션(b0)으로 클립 좌표 변환
PSInput VSMain(VSInput input)
{
	PSInput output;
	float4 worldPos = mul(float4(input.position_, 1.0f), world_);
	output.position_ = mul(worldPos, viewProjection_);
	output.uv_ = input.uv_;
	output.color_ = input.color_;
	return output;
}

// 픽셀 셰이더: 텍스처 색상에 틴트와 머티리얼 색을 곱한다.
// 흰색 1x1 텍스처를 쓰면 결과가 틴트 색 그대로가 되므로 단색 도형도 같은 셰이더로 그린다.
float4 PSMain(PSInput input) : SV_TARGET
{
	return texture_.Sample(sampler_, input.uv_) * input.color_ * baseColor_;
}
)";

// 버킷 3패스 — Bottom → Default(트리 밴드) → Top 순으로만 Flush()가 분리된다.
// 같은 텍스처여도 버킷이 다르면 절대 섞이지 않는다 (씬 전역 최상단/최하단 보장).
static const RenderLayer kFlushOrder[] = { RenderLayer::Bottom, RenderLayer::Default, RenderLayer::Top };

// 로그 주기 제한용 프레임 카운터 (60프레임마다 1회만 출력 — A-4)
static _u32 sLogFrame = 0;

//////////////////////////////////////////////////////////////////////////////////////////
Renderer2D::Renderer2D()
	: vertices_(MAX_VERTICES)
	, indices_(MAX_INDICES)
	, pCurrentTexture_(nullptr)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
Renderer2D::~Renderer2D()
{
	Finalize();
}

//////////////////////////////////////////////////////////////////////////////////////////
// BatchRenderer 훅: 스프라이트 셰이더 소스
const char* Renderer2D::ShaderSource() const
{
	return s_szSpriteShader;
}

//////////////////////////////////////////////////////////////////////////////////////////
// BatchRenderer 훅: VertexPTC 레이아웃
const D3D11_INPUT_ELEMENT_DESC* Renderer2D::VertexLayout(UINT* _outCount) const
{
	return VertexPTC::LayoutDescs(_outCount);
}

//////////////////////////////////////////////////////////////////////////////////////////
// BatchRenderer 훅: 셰이더/상수 버퍼 생성 후 호출된다.
// 정점/인덱스 버퍼를 만든다. (인덱스 버퍼는 DYNAMIC — 임의 인덱스)
bool Renderer2D::CreateBatchResources(GraphicDevice* _pDevice)
{
	// 1. DYNAMIC 정점 버퍼 (매 프레임 CPU가 채워 넣는다 — ResourceUsage::ruDynamic → D3D11_USAGE_DYNAMIC)
	if (!vertexBuffer_.Create(_pDevice, nullptr, sizeof(VertexPTC), MAX_VERTICES, ResourceUsage::ruDynamic))
	{
		return false;
	}

	// 2. DYNAMIC 인덱스 버퍼 (— 임의 인덱스/도형 지원, 매 Flush마다 CPU 인덱스를 업데이트)
	if (!indexBuffer_.Create(_pDevice, nullptr, MAX_INDICES, ResourceUsage::ruDynamic))
	{
		return false;
	}

	// 3. 메시 파이프라인 상수 버퍼 (b1 오브젝트 / b2 머티리얼 — Renderer3D와 동일 규약)
	if (!objectCb_.Create(_pDevice)) { return false; }
	if (!materialCb_.Create(_pDevice)) { return false; }

	// 4. (1x1 흰색 텍스처는 ResourceMgr 기본 텍스처(GetDefaultTexture)를 공유한다 — A-2)
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Renderer2D::Finalize()
{
	staticCache_.Clear();
	BatchRenderer::Finalize();
}

//////////////////////////////////////////////////////////////////////////////////////////
// BatchRenderer 훅: Begin() 공통 처리 후 호출된다.
// 2D는 깊이 테스트 끄고(그리는 순서로 적층), 알파 블렌딩을 켠다.
void Renderer2D::OnBegin()
{
	vertices_.Clear();
	indices_.Clear();
	pCurrentTexture_ = nullptr;
	drawCallCount_ = 0;

	pDevice_->SetDepthTest(false);
	pDevice_->SetAlphaBlending(true);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 씬 배치 시작 — 씬 카메라의 ViewProjection으로 배치를 연다.
// ★ 왜 필요한가: 2D 한 프레임의 "문을 여는" 일. 이후 들어오는 모든 정점이 이 카메라 시점으로 변환된다.
// - (용어) 배치(Batch): 그릴 것들을 버퍼에 모아 두는 것. EndScene에서 한꺼번에 GPU로 보낸다.
// - (용어) ViewProjection: 뷰(카메라가 보는 변환) × 투영(화면에 펼치는 변환)을 합친 행렬.
void Renderer2D::BeginScene(Scene2D* _pScene)
{
	jc_assert_msg(_pScene != nullptr, "null 씬으로 배치를 시작할 수 없습니다.");
	if (_pScene == nullptr) return;

	pScene_ = _pScene;
	Begin(_pScene->GetCamera()->ViewProjection());
}

//////////////////////////////////////////////////////////////////////////////////////////
// 씬 배치 종료 — 잔여 세그먼트 플러시 + 배치 종료.
// ★ 왜 필요한가: 모아둔 배치를 GPU로 보내고(Flush) 2D 프레임을 닫는 일.
// 문을 열지 않았는데 닫으려 하면 무시한다 (이중 호출 방지).
void Renderer2D::EndScene()
{
	if (!begun_) return;
	End();		// BatchRenderer::End → Flush + begun_ 해제
}

//////////////////////////////////////////////////////////////////////////////////////////
// 스태틱 선언 — 채움 1회 빌드 → 캐시 저장 → staticId 반환. (OnEnter에서 호출)
// staticId = 캐시 인덱스 + 1 (IdProvider 순수 증가 — 재사용 없음)
_u64 Renderer2D::DeclareStatic(const RenderParams& _params)
{
	StaticSlot slot;
	slot.layer_ = _params.layer_;						// 버킷도 선언 시 고정
	BuildFill(_params.fill_, _params, slot.geometry_);	// 버텍스/인덱스 생성 (CPU 1회)
	_u64 id = staticIdProvider_.Acquire();
	staticCache_.PushBack(jc::Move(slot));					// 인덱스 = id - 1 (선형 검색 없음)
	return id;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 스태틱 예약 — 캐시 조회 → 스태틱 세그먼트에 키만 기록 (복사 없음)
void Renderer2D::RenderStatic(_u64 _staticId)
{
	StaticSlot* pSlot = FindStatic(_staticId);
	if (pSlot == nullptr) return;

	staticItems_.PushBack(DrawItem{ true, _staticId, pSlot->geometry_.pTexture_, pSlot->layer_ });
}

//////////////////////////////////////////////////////////////////////////////////////////
// 다이나믹 예약 — 채움 파라미터 복사 → 다이나믹 세그먼트 (인덱스 참조 — 재배치 안전)
void Renderer2D::RenderDynamic(const RenderParams& _params)
{
	_u64 index = (_u64)dynamicParams_.Size();
	dynamicParams_.PushBack(_params);                  // 수명 안정화 (복사 1회)
	dynamicItems_.PushBack(DrawItem{ false, index, _params.fill_.GetTexture(), _params.layer_ });
}

//////////////////////////////////////////////////////////////////////////////////////////
// 스태틱 세그먼트 드로우 — 버킷 3패스, 캐시 버텍스를 PushTriangles로 배칭.
void Renderer2D::FlushStatic()
{
	_s32 drawnCount = 0;
	for (RenderLayer pass : kFlushOrder)
	{
		for (DrawItem& item : staticItems_)
		{
			if (item.layer_ != pass) continue;
			StaticSlot* pSlot = FindStatic(item.key_);
			if (pSlot == nullptr) continue;
			FillResult& geo = pSlot->geometry_;
			PushTriangles(item.pTexture_, geo.vertices_.Source(), (_u32)geo.vertices_.Size(),
				geo.indices_.Source(), (_u32)geo.indices_.Size());   // 텍스처 배칭 (임의 도형 지원)
			++drawnCount;
		}
		Flush();                 // 버킷 경계마다 GPU 전송 (기본 버킷만 쓰면 1회 — 기존과 동일)
	}
	staticItems_.Clear();
	if ((++sLogFrame % 60) == 1)
		_LogDebug_("[sgf] Renderer2D::FlushStatic — 스태틱 세그먼트 드로우 (%d건)", drawnCount);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 다이나믹 세그먼트 드로우 — 버킷 3패스, 매 프레임 BuildFill 후 배칭.
void Renderer2D::FlushDynamic()
{
	_s32 drawnCount = 0;
	for (RenderLayer pass : kFlushOrder)
	{
		for (DrawItem& item : dynamicItems_)
		{
			if (item.layer_ != pass) continue;
			if (item.key_ >= (_u64)dynamicParams_.Size()) continue;
			RenderParams& params = dynamicParams_[(_s32)item.key_];
			BuildFill(params.fill_, params, scratch_);   // 스크래치 재사용 버퍼
			PushTriangles(item.pTexture_, scratch_.vertices_.Source(), (_u32)scratch_.vertices_.Size(),
				scratch_.indices_.Source(), (_u32)scratch_.indices_.Size());
			++drawnCount;
		}
		Flush();                 // 버킷 경계마다 GPU 전송
	}
	dynamicItems_.Clear();
	dynamicParams_.Clear();
	if ((sLogFrame % 60) == 1)
		_LogDebug_("[sgf] Renderer2D::FlushDynamic — 다이나믹 세그먼트 드로우 (%d건)", drawnCount);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 채움 실행: 콜백 → FillResult (매 호출 Clear — 재사용)
// ★ 왜 필요한가: "Fill(채움 설명서) → 실제 정점/인덱스"로 변환하는 공통 통로.
// - (용어) 콜백: '이 채움을 어떻게 만들지'가 등록된 함수. Solid/Texture/9-패치/게이지마다 각각 콜백이 있다.
// - (용어) FillResult: 콜백이 만들어낸 결과물(버텍스/인덱스/텍스처)을 담는 바구니.
void Renderer2D::BuildFill(const Fill& _fill, const RenderParams& _params, FillResult& _out)
{
	_out.vertices_.Clear();  _out.indices_.Clear();  _out.pTexture_ = nullptr;
	if (!_fill.IsValid()) return;
	_fill.GetCallback()(_fill, _params, _out);      // 콜백이 채움 고유 파라미터 + RenderParams 읽음
}

//////////////////////////////////////////////////////////////////////////////////////////
// 스태틱 캐시 조회 — 인덱스 = id - 1 (IdProvider 순수 증가 — 1, 2, 3, ...)
Renderer2D::StaticSlot* Renderer2D::FindStatic(_u64 _staticId)
{
	if (_staticId == 0) return nullptr;
	const _s64 index = (_s64)_staticId - 1;
	if (index < 0 || index >= staticCache_.Size()) return nullptr;
	return &staticCache_[(_s32)index];
}

//////////////////////////////////////////////////////////////////////////////////////////
// 임의 정점/인덱스를 배치에 추가. 텍스처가 바뀌거나 버퍼가 꽉 차면 자동 Flush.
// ★ 왜 필요한가: 모든 2D 도형이 지나가는 공통 통로. "모아 보내기(배칭)"의 핵심.
// - 같은 텍스처가 계속되면 쌓기만 하고, 텍스처가 바뀌는 순간 이전 배치를 먼저 그린다(Flush).
// - 이렇게 모아 보내는 이유: GPU 드로우콜("그려!" 명령) 횟수가 줄어들면 성능이 좋아진다.
// - (용어) 인덱스 재배치: 도형마다 0부터 시작하는 인덱스에 배치 전체 기준 오프셋을 더해
//   여러 도형을 하나의 큰 버퍼에서 그릴 수 있게 만든다.
void Renderer2D::PushTriangles(Texture* _pTexture, const VertexPTC* _pVertices, _u32 _vertexCount,
	const _u16* _pIndices, _u32 _indexCount)
{
	jc_assert(begun_);
	if (_pVertices == nullptr || _pIndices == nullptr || _vertexCount == 0 || _indexCount == 0) return;

	// 텍스처가 바뀌면 지금까지 모은 배치를 먼저 그린다.
	if (pCurrentTexture_ != nullptr && pCurrentTexture_ != _pTexture)
	{
		Flush();
	}
	// 배치가 꽉 차도 먼저 그린다. (정점/인덱스 각각 — 임의 도형은 비율이 다를 수 있음)
	if (vertices_.Size() + (_s32)_vertexCount > MAX_VERTICES ||
		indices_.Size() + (_s32)_indexCount > MAX_INDICES)
	{
		Flush();
	}

	// 정점 추가 + 인덱스 재배치 (베이스 오프셋을 더해 배치 전체 인덱스로 만든다)
	const _s32 base = vertices_.Size();
	for (_u32 i = 0; i < _vertexCount; ++i)
	{
		vertices_.PushBack(_pVertices[i]);
	}
	for (_u32 i = 0; i < _indexCount; ++i)
	{
		indices_.PushBack((_u32)base + (_u32)_pIndices[i]);
	}

	pCurrentTexture_ = _pTexture;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 사각형 4정점을 배치에 추가 (즉시 그리기용) — PushTriangles로 위임
void Renderer2D::PushQuad(Texture* _pTexture, const VertexPTC (&_vertices)[4])
{
	const _u16 indices[6] = { 0, 1, 2, 0, 2, 3 };
	PushTriangles(_pTexture, _vertices, 4, indices, 6);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 단색 사각형
void Renderer2D::DrawRect(const vec2& _center, const vec2& _size, const color& _color, _f32 _radian)
{
	// 흰색 텍스처 x 색상 틴트 = 단색. 사각형은 회전 포함 DrawSprite와 동일 로직.
	DrawSprite(g_cResourceMgr.GetDefaultTexture(), _center, _size, _color, _radian);
}

// 텍스처 스프라이트
void Renderer2D::DrawSprite(Texture* _pTexture, const vec2& _center, const vec2& _size,
	const color& _tint, _f32 _radian, const vec2& _uvMin, const vec2& _uvMax)
{
	if (_pTexture == nullptr || !_pTexture->IsValid())
	{
		return;
	}

	// 중심 기준 반폭
	const _f32 halfW = _size.x * 0.5f;
	const _f32 halfH = _size.y * 0.5f;

	// 회전 행렬을 직접 풌어 쓴다. (2D 회전은 2x2만 있으면 충분)
	const _f32 c = cosf(_radian);
	const _f32 s = sinf(_radian);

	// 로컬 4구석 (좌하, 우하, 우상, 좌상) - 월드는 Y위쪽+ 기준
	const vec2 locals[4] = {
		vec2(-halfW, -halfH),
		vec2(+halfW, -halfH),
		vec2(+halfW, +halfH),
		vec2(-halfW, +halfH),
	};
	// UV는 텍스처 좌표계(Y아래쪽+)이므로 위아래를 뒤집어 매칭한다.
	const vec2 uvs[4] = {
		vec2(_uvMin.x, _uvMax.y),	// 좌하 정점 <- UV 좌하단
		vec2(_uvMax.x, _uvMax.y),	// 우하 정점 <- UV 우하단
		vec2(_uvMax.x, _uvMin.y),	// 우상 정점 <- UV 우상단
		vec2(_uvMin.x, _uvMin.y),	// 좌상 정점 <- UV 좌상단
	};

	VertexPTC vertices[4];
	for (_s32 i = 0; i < 4; ++i)
	{
		const _f32 x = locals[i].x * c - locals[i].y * s + _center.x;
		const _f32 y = locals[i].x * s + locals[i].y * c + _center.y;
		vertices[i].position_ = vec3(x, y, 0.0f);
		vertices[i].uv_ = uvs[i];
		vertices[i].color_ = _tint;
	}

	PushQuad(_pTexture, vertices);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 선분: 두 점을 잇는 가늘고 긴 사각형을 회전시켜 그린다.
void Renderer2D::DrawLine(const vec2& _from, const vec2& _to, const color& _color, _f32 _thickness)
{
	const _f32 dx = _to.x - _from.x;
	const _f32 dy = _to.y - _from.y;
	const _f32 length = sqrtf(dx * dx + dy * dy);
	if (length <= Epsilon_v)
	{
		return;
	}

	const vec2 center((_from.x + _to.x) * 0.5f, (_from.y + _to.y) * 0.5f);
	const _f32 radian = atan2f(dy, dx);
	DrawRect(center, vec2(length, _thickness), _color, radian);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 원: 중심에서 부채꼴로 뻗치는 삼각형들로 근사한다. (사각형 배치 재사용 — 정점 중복 허용)
void Renderer2D::DrawCircle(const vec2& _center, _f32 _radius, const color& _color, _s32 _segments)
{
	if (_segments < 3)
	{
		_segments = 3;
	}

	for (_s32 i = 0; i < _segments; ++i)
	{
		const _f32 a0 = (jc_math_pi2) * _f32(i) / _f32(_segments);
		const _f32 a1 = (jc_math_pi2) * _f32(i + 1) / _f32(_segments);

		const vec2 p0(_center.x + cosf(a0) * _radius, _center.y + sinf(a0) * _radius);
		const vec2 p1(_center.x + cosf(a1) * _radius, _center.y + sinf(a1) * _radius);

		// 삼각형(중심, p0, p1)을 정점 4개짜리 쿼드로 표현 (마지막 정점 중복)
		VertexPTC vertices[4];
		const vec2 positions[4] = { _center, p0, p1, p1 };
		for (_s32 corner = 0; corner < 4; ++corner)
		{
			vertices[corner].position_ = vec3(positions[corner].x, positions[corner].y, 0.0f);
			vertices[corner].uv_ = vec2(0.0f, 0.0f);
			vertices[corner].color_ = _color;
		}
		PushQuad(g_cResourceMgr.GetDefaultTexture(), vertices);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// 메시 그리기 — 2D 프리미티브 메시(vfPTC2D)를 GPU 변환으로 즉시 드로우한다.
// Renderer3D::Draw와 동일한 4단계 (상태 → Bind → b1 갱신 → Draw) — 정점은 로컬 공간 불변.
// 배치 경로와 달리 오브젝트당 1콜 (인스턴싱은 추후 — C-3). Scene2D::DrawMesh → 이곳.
//
// ★ 이 함수가 실제 렌더링을 수행하는 시점 — 정점이 화면에 그려지는 전체 흐름:
//
// [텍스처가 왜 필요한가 — 정점과 텍스처의 관계]
// - 정점(Vertex)은 "어디에 그릴지"(뼈대)만 알려준다 — 위치/UV/색 정보.
// - 텍스처는 "무엇을 그릴지"(살/피부 — 픽셀 무늬)를 담은 저장고다.
//   GPU 흐름: 정점 → 삼각형 → 래스터라이제이션(삼각형 내부를 픽셀로 채움) → 픽셀마다 색 결정(픽셀 셰이더)
//   여기서 픽셀 색의 원본이 되는 게 텍스처다.
// - 정점 색만으로는 삼각형 내부가 그라데이션(보간)밖에 못 된다. 복잡한 무늬(벽돌/얼굴/글자)를
//   정점으로 표현하려면 "픽셀 1개 = 정점 1개" 수준이 되어 데이터가 폭발한다.
// - 텍스처는 정점 4개 + UV 4개로 수백만 픽셀의 무늬를 표현한다. (정보 압축)
// - 연결 고리는 UV: 정점이 "텍스처의 어느 지점"을 가리키고, GPU가 삼각형 내부 픽셀마다
//   UV를 보간해 그 위치의 픽셀 색을 텍스처에서 샘플링해 화면에 칠한다.
//   (이 엔진의 픽셀 셰이더: texture_.Sample(sampler_, input.uv_) * input.color_ * baseColor_)
//
// [단색 도형 = 흰색 1x1 텍스처 트릭]
// - 단색 도형도 개념적으로는 텍스처가 필요 없다. 하지만 아래의 GetDefaultTexture(1x1 흰색)에
//   틴트 색을 곱해 단색을 만든다. (흰색 × 빨강 = 빨강 — 셰이더 곱셈 규약이 그대로 성립)
// - 단색 전용 셰이더를 두지 않는 이유: 셰이더 전환(비싼 파이프라인 작업) 없이 하나의 규약으로
//   통일하고, 배치 경로에서는 모든 단색 도형이 "같은 텍스처"로 취급되어 서로 한 드로우콜로 묶인다.
// - GetDefaultTexture()는 매번 "조회"일 뿐 생성/변경이 아니다 — ResourceMgr가 1번 생성해 보관하고
//   같은 객체의 포인터만 반환한다. 텍스처는 생성 후 내용이 변하지 않는 불변 리소스이고,
//   매 프레임 같은 텍스처를 Bind해도 GraphicContext의 바인딩 캐시(B-1)가 실제 D3D 호출을 생략한다.
void Renderer2D::DrawMesh(Mesh* _pMesh, Material* _pMaterial, const mat4& _world)
{
	if (_pMesh == nullptr || !_pMesh->Is2D())
	{
		return;
	}
	if (_pMesh->VertexCount() == 0)
	{
		return;
	}

	GraphicContext& context = pDevice_->GetContext();

	// 깊이/블렌드 상태 재확정 — Flush와 같은 이유. 3D와 섞여도 2D는 깊이 끄고 반투명.
	pDevice_->SetDepthTest(false);
	pDevice_->SetAlphaBlending(true);

	// 1. 어떻게 그릴지 — 셰이더 + b0(뷰프로젝션) 장착 (배치 공용 파이프라인)
	ApplyFrameStates();

	// 2. 무엇을 그릴지 — VB/IB/레이아웃/토폴로지 (로컬 정점 그대로)
	_pMesh->Bind(context);

	// 3. 텍스처 해석: 재질 0번 슬롯 → 없으면 기본 흰색 텍스처 (단색 도형)
	Texture* pTexture = g_cResourceMgr.GetDefaultTexture();
	if (_pMaterial != nullptr)
	{
		const _u64 textureKey = _pMaterial->GetTextureKey(0);
		if (textureKey != INVALID_RESOURCE_KEY)
		{
			Texture* pFound = g_cResourceMgr.Find<Texture>(textureKey);
			if (pFound != nullptr)
			{
				pTexture = pFound;
			}
		}
	}
	if (pTexture != nullptr)
	{
		pTexture->Bind(pDevice_, 0);
	}

	// 4. b2 머티리얼 상수 = 재질 기본색 (틴트. 배치 경로는 흰색 고정 — 메시 경로만 갱신)
	MaterialConstants material;
	const color tint = (_pMaterial != nullptr) ? _pMaterial->GetBaseColor() : color::WHITE;
	tint.ToFloat4(material.baseColor_);
	materialCb_.Update(pDevice_, material);
	context.SetConstantBuffer(ShaderStage::ssPixel, 2, materialCb_.Raw());

	// 5. b1 오브젝트 상수 = 월드 행렬 (GPU 변환 — StaticLevel은 행렬 고정으로 전달됨)
	ObjectConstants object;
	object.world_ = _world;
	objectCb_.Update(pDevice_, object);
	context.SetConstantBuffer(ShaderStage::ssVertex, 1, objectCb_.Raw());

	// 6. 드로우 콜 (인덱스 유무에 따라 DrawIndexed/Draw)
	++drawCallCount_;
	_pMesh->Draw(context);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 배치를 GPU로 전송하고 드로우 콜 실행 (정점 + 인덱스 둘 다 업로드)
// ★ 왜 필요한가: 2D 프레임의 "발사" 지점. CPU에 쌓인 배치를 GPU 버퍼로 복사하고 그리라고 명령한다.
// - 깊이/블렌드 상태를 여기서 다시 확정하는 이유: 같은 프레임에 Renderer3D(깊이 켜짐)와 섞여 쓰여도
//   2D는 항상 "깊이 끄고 + 반투명 켜고" 그려지도록 보장하기 위함이다.
void Renderer2D::Flush()
{
	const _s32 vertexCount = vertices_.Size();
	const _s32 indexCount = indices_.Size();
	if (vertexCount == 0 || indexCount == 0)
	{
		return;
	}

	// 깊이/블렌드 상태는 Flush 직전에 확정한다.
	// 같은 프레임에 Renderer3D(깊이 켜고 그림)와 섞여 쓰여도
	// 2D는 항상 "깊이 끄고, 반투명 켜고" 그려지도록 보장하기 위함이다.
	pDevice_->SetDepthTest(false);
	pDevice_->SetAlphaBlending(true);

	// 1. CPU 배치 -> GPU 정점/인덱스 버퍼 복사
	vertexBuffer_.Update(pDevice_, vertices_.Source(), UINT(vertexCount));
	indexBuffer_.Update(pDevice_, indices_.Source(), UINT(indexCount));

	GraphicContext& context = pDevice_->GetContext();

	// 2. 파이프라인 구성: 셰이더/버퍼/텍스처/상수버퍼/토폴로지
	ApplyFrameStates();

	// 배치 정점은 이미 월드좌표(CPU 변환 완료)이므로 b1=단위행렬, b2=흰색 고정.
	// 메시 즉시 드로우가 남긴 b1/b2를 배치 드로우 전에 되돌린다.
	ObjectConstants object;
	object.world_ = mat4::Identity();
	objectCb_.Update(pDevice_, object);
	context.SetConstantBuffer(ShaderStage::ssVertex, 1, objectCb_.Raw());

	MaterialConstants material;	// 기본값 = 흰색 (1,1,1,1)
	materialCb_.Update(pDevice_, material);
	context.SetConstantBuffer(ShaderStage::ssPixel, 2, materialCb_.Raw());

	vertexBuffer_.Bind(pDevice_);
	indexBuffer_.Bind(pDevice_);
	if (pCurrentTexture_ != nullptr)
	{
		pCurrentTexture_->Bind(pDevice_, 0);
	}
	pDevice_->Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 3. 드로우 콜: 인덱스 수만큼 그린다. (임의 도형 지원)
	++drawCallCount_;
	pDevice_->Context()->DrawIndexed(UINT(indexCount), 0, 0);

	// 4. 배치 비우기
	vertices_.Clear();
	indices_.Clear();
}

NS_SGF_END
