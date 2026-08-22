/*
 * 작성자: 윤정도
 * 생성일: 8/4/2026 10:30:00 PM
 * 수정일: 8/9/2026 1:00:00 AM (Node/Sprite 제거, SoundEngine/분리된 D3D 부품 추가)
 * 수정일: 8/9/2026 6:20:00 PM (GraphicContext/파이프라인 상태 객체/Mesh/Material/SceneRenderer/ResourceMgr 추가)
 * 수정일: 8/16/2026 (개편: Fill/GameObject/Layer/Transform/Component/Camera2D/3D/Scene2D/3D 추가, SceneRenderer 제거)
 * =====================
 * sgf 엔진 통합 헤더
 *
 * 사용하는 쪽(sgfr, sgcl 등)은 이 헤더 하나만 포함하면 된다.
 *
 * [구성]
 * Math: 벡터/행렬/색상
 * Core: 윈도우 / 타이머(jc::TimeSpan) / 애플리케이션(g_c 매크로)
 * Input: 키보드/마우스 입력
 * Graphics: D3D11 파사드 + 부품(SwapChain/DepthStencilSurface/RenderStates)
 * 셰이더/버퍼/텍스처/렌더타깃/2D렌더러
 * Audio: XAudio2 기반 사운드 엔진 (g_cSound)
 * Scene: 카메라 / 씬 / 디렉터 (g_cDirector)
 */

#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
// Math
#include "jc/Math.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Core
#include "sgf/Core/Window.h"
#include "sgf/Core/FrameTimer.h"
#include "sgf/Core/Application.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Input
#include "sgf/Input/InputManager.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Graphics
#include "sgf/Graphics/RenderStates.h"
#include "sgf/Graphics/SwapChain.h"
#include "sgf/Graphics/DepthStencilSurface.h"
#include "sgf/Graphics/GraphicDevice.h"
#include "sgf/Graphics/Shader.h"
#include "sgf/Graphics/Vertex.h"
#include "sgf/Graphics/Buffers.h"
#include "sgf/Graphics/Texture.h"
#include "sgf/Graphics/RenderTarget.h"
#include "sgf/Graphics/Renderer2D.h"
#include "sgf/Graphics/Renderer3D.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Graphics (파이프라인 객체 + 리소스 계층)
#include "sgf/Graphics/GraphicsEnums.h"
#include "sgf/Graphics/IResource.h"
#include "sgf/Graphics/PrimitiveMeshType.h"
#include "sgf/Graphics/PipelineState.h"
#include "sgf/Graphics/GraphicContext.h"
#include "sgf/Graphics/ShaderProgram.h"
#include "sgf/Graphics/DefaultShaders.h"
#include "sgf/Graphics/Mesh.h"
#include "sgf/Graphics/Material.h"
#include "sgf/Graphics/ResourceMgr.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Graphics (채움 셰이프 시스템)
#include "sgf/Graphics/Fill.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Audio
#include "sgf/Audio/SoundEngine.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Scene (GameObject/컴포넌트 계층)
#include "sgf/Scene/Transform.h"
#include "sgf/Scene/Component.h"
#include "sgf/Scene/GameObject.h"
#include "sgf/Scene/Layer.h"
#include "sgf/Scene/Camera.h"
#include "sgf/Scene/Camera2D.h"
#include "sgf/Scene/Camera3D.h"
#include "sgf/Scene/Scene.h"
#include "sgf/Scene/Scene2D.h"
#include "sgf/Scene/Scene3D.h"
#include "sgf/Scene/Shape2D.h"
#include "sgf/Scene/Shape3D.h"
#include "sgf/Scene/Director.h"
