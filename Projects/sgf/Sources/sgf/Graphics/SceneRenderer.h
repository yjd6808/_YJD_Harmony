/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 6:00:00 PM
 * =====================
 * 씬 렌더러 (v3: 프레임/오브젝트 상수버퍼 소유 + 렌더 오브젝트 그리기)
 *
 * [상수버퍼 슬롯 규약] (v3 FR-19)
 *  b0 = 프레임 공통 (view/projection/카메라) - BeginScene에서 1회 갱신
 *  b1 = 오브젝트 (world) - Draw마다 갱신
 *  b2 = 머티리얼 - Material이 소유
 *
 * [사용 흐름] (수도 코드의 Render 루프와 동일)
 *  sceneRenderer.BeginScene(frameData);
 *  for (obj : objects) { sceneRenderer.Draw(obj); }
 */

#pragma once

#include "sgf/Graphics/IResource.h"
#include "sgf/Graphics/Buffers.h"
#include "jc/Math.h"

NS_SGF_BEGIN

using namespace jc;

class GraphicDevice;
class GraphicContext;
class Mesh;
class Material;

// b0 프레임 공통 상수 (16바이트 배수: 64+64+16 = 144)
struct FrameConstants
{
	mat4 view_;				// 뷰 행렬
	mat4 projection_;		// 투영 행렬
	vec4 cameraPosition_;	// 카메라 월드 위치 (스페큘러 등에 사용)
};

// b1 오브젝트 상수 (64바이트)
struct ObjectConstants
{
	mat4 world_;			// 월드 행렬
};

// 그려야 할 대상 하나 = 메시(무엇을) + 머티리얼(어떻게) + 월드 행렬(어디에)
struct RenderObject
{
	_u64 meshKey_ = INVALID_RESOURCE_KEY;		// ResourceMgr의 메시 키
	_u64 materialKey_ = INVALID_RESOURCE_KEY;	// ResourceMgr의 머티리얼 키
	mat4 world_;								// 월드 행렬 (기본: 단위행렬)
	bool visible_ = true;						// false면 Draw에서 건너뀜
};

class SceneRenderer
{
public:
	SceneRenderer();
	~SceneRenderer();

	bool Initialize(GraphicDevice* _pDevice);
	void Finalize();

	// 프레임 공통 상수를 갱신하고 b0에 장착한다. (프레임당 1회 호출)
	void BeginScene(const FrameConstants& _frame);

	// 렌더 오브젝트 하나를 그린다. (키 해서 -> 머티리얼/메시 바인딩 -> b1 갱신 -> 드로우)
	void Draw(const RenderObject& _object);

	// 포인터로 직접 그리기 (ResourceMgr를 거치지 않는 경우용)
	void Draw(Mesh* _pMesh, Material* _pMaterial, const mat4& _world);

private:
	GraphicDevice* pDevice_;					// 소유하지 않음
	ConstantBuffer<FrameConstants> frameCb_;	// b0 (이 클래스가 소유)
	ConstantBuffer<ObjectConstants> objectCb_;	// b1 (이 클래스가 소유)

	// 같은 키가 연속될 때 Find를 생략하는 소형 캐시 (v3 D-13)
	_u64 lastMeshKey_;
	_u64 lastMaterialKey_;
	Mesh* pLastMesh_;
	Material* pLastMaterial_;
};

NS_SGF_END
