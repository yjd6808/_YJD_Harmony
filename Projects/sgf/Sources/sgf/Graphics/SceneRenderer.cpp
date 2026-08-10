/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 6:00:00 PM
 * =====================
 * 씬 렌더러 구현부
 */

#include "Core.h"
#include "sgf/Graphics/SceneRenderer.h"
#include "sgf/Graphics/GraphicDevice.h"
#include "sgf/Graphics/GraphicContext.h"
#include "sgf/Graphics/Mesh.h"
#include "sgf/Graphics/Material.h"
#include "sgf/Graphics/ResourceMgr.h"

NS_SGF_BEGIN

using namespace jc;

//////////////////////////////////////////////////////////////////////////////////////////
SceneRenderer::SceneRenderer()
	: pDevice_(nullptr)
	, lastMeshKey_(INVALID_RESOURCE_KEY)
	, lastMaterialKey_(INVALID_RESOURCE_KEY)
	, pLastMesh_(nullptr)
	, pLastMaterial_(nullptr)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
SceneRenderer::~SceneRenderer()
{
	Finalize();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool SceneRenderer::Initialize(GraphicDevice* _pDevice)
{
	pDevice_ = _pDevice;

	if (!frameCb_.Create(_pDevice)) { return false; }
	if (!objectCb_.Create(_pDevice)) { return false; }
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void SceneRenderer::Finalize()
{
	pDevice_ = nullptr;
	lastMeshKey_ = INVALID_RESOURCE_KEY;
	lastMaterialKey_ = INVALID_RESOURCE_KEY;
	pLastMesh_ = nullptr;
	pLastMaterial_ = nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
void SceneRenderer::BeginScene(const FrameConstants& _frame)
{
	jc_assert_msg(pDevice_ != nullptr, "Initialize 이후에만 사용할 수 있습니다.");

	GraphicContext& context = pDevice_->GetContext();

	frameCb_.Update(pDevice_, _frame);
	context.SetConstantBuffer(ShaderStage::ssVertex, 0, frameCb_.Raw());
	context.SetConstantBuffer(ShaderStage::ssPixel, 0, frameCb_.Raw());

	// 프레임이 바뀌면 이전 프레임의 해서 캐시는 무효화한다.
	// (프레임 사이에 리소스가 제거/교체되었을 수 있음)
	lastMeshKey_ = INVALID_RESOURCE_KEY;
	lastMaterialKey_ = INVALID_RESOURCE_KEY;
	pLastMesh_ = nullptr;
	pLastMaterial_ = nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
void SceneRenderer::Draw(const RenderObject& _object)
{
	if (!_object.bVisible_)
	{
		return;
	}

	// 키 -> 포인터 해서 (직전 키와 같으면 Find 생략)
	if (_object.meshKey_ != lastMeshKey_)
	{
		pLastMesh_ = g_cResourceMgr.Find<Mesh>(_object.meshKey_);
		lastMeshKey_ = _object.meshKey_;
	}
	if (_object.materialKey_ != lastMaterialKey_)
	{
		pLastMaterial_ = g_cResourceMgr.Find<Material>(_object.materialKey_);
		lastMaterialKey_ = _object.materialKey_;
	}

	if (pLastMesh_ == nullptr || pLastMaterial_ == nullptr)
	{
		jc_assert_msg(false, "RenderObject의 메시/머티리얼 키가 유효하지 않습니다.");
		return;
	}

	Draw(pLastMesh_, pLastMaterial_, _object.world_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void SceneRenderer::Draw(Mesh* _pMesh, Material* _pMaterial, const mat4& _world)
{
	jc_assert_msg(pDevice_ != nullptr, "Initialize 이후에만 사용할 수 있습니다.");
	jc_assert_msg(_pMesh != nullptr && _pMaterial != nullptr, "메시/머티리얼이 비어있습니다.");

	GraphicContext& context = pDevice_->GetContext();

	// 1. 어떻게 그릴지 (셰이더/상태/텍스처/b2)
	if (!_pMaterial->Bind(context))
	{
		return;
	}

	// 2. 무엇을 그릴지 (VB/IB/레이아웃/토폴로지)
	_pMesh->Bind(context);

	// 3. 어디에 그릴지 (b1 월드 행렬)
	ObjectConstants object;
	object.world_ = _world;
	objectCb_.Update(pDevice_, object);
	context.SetConstantBuffer(ShaderStage::ssVertex, 1, objectCb_.Raw());

	// 4. 드로우 호출
	_pMesh->Draw(context);
}

NS_SGF_END
