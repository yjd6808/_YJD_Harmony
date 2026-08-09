/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 9:00:00 AM
 * =====================
 * 튜토리얼 목록 정의
 * 이 파일의 배열 순서가 곧 콘솔 목차 번호다. (1번부터 순서대로 학습 권장)
 */

#include "Core.h"
#include "sgfr/TutorialRegistry.h"

#include "sgfr/01_LinearAlgebra/01_LinearAlgebra_Main.h"
#include "sgfr/02_WindowAndMessageLoop/02_WindowAndMessageLoop_Main.h"
#include "sgfr/03_DeviceAndSwapChain/03_DeviceAndSwapChain_Main.h"
#include "sgfr/04_RenderingPipeline/04_RenderingPipeline_Main.h"
#include "sgfr/05_HLSL/05_HLSL_Main.h"
#include "sgfr/06_VertexIndexBuffer/06_VertexIndexBuffer_Main.h"
#include "sgfr/07_WorldTransform/07_WorldTransform_Main.h"
#include "sgfr/08_Camera/08_Camera_Main.h"
#include "sgfr/09_Cube3D/09_Cube3D_Main.h"
#include "sgfr/10_Lighting/10_Lighting_Main.h"
#include "sgfr/11_PngTextureDraw/11_PngTextureDraw_Main.h"
#include "sgfr/12_SvgTextureDraw/12_SvgTextureDraw_Main.h"
#include "sgfr/13_Input/13_Input_Main.h"
#include "sgfr/14_SceneGraph/14_SceneGraph_Main.h"
#include "sgfr/15_SamplerFiltering/15_SamplerFiltering_Main.h"
#include "sgfr/16_BlendState/16_BlendState_Main.h"
#include "sgfr/17_RasterizerState/17_RasterizerState_Main.h"
#include "sgfr/18_ShadingModel/18_ShadingModel_Main.h"
#include "sgfr/19_RenderTarget/19_RenderTarget_Main.h"
#include "sgfr/20_PostProcess/20_PostProcess_Main.h"
#include "sgfr/21_ShadowMapping/21_ShadowMapping_Main.h"
#include "sgfr/22_SpriteAnimation/22_SpriteAnimation_Main.h"

// 튜토리얼 목록 (번호 순서 = 학습 순서)
namespace sgfr
{

static const TutorialEntry s_Tutorials[] =
{
	{ "선형대수 (벡터/행렬 기초)",                LinearAlgebra_Main },
	{ "윈도우와 메시지 루프",                       WindowAndMessageLoop_Main },
	{ "디바이스와 스왒체인 (화면 지우기)",          DeviceAndSwapChain_Main },
	{ "렌더링 파이프라인 (첫 삼각형)",             RenderingPipeline_Main },
	{ "HLSL 셰이더 (색상 보간과 애니메이션)",       HLSL_Main },
	{ "정점/인덱스 버퍼 (사각형 그리기)",           VertexIndexBuffer_Main },
	{ "월드 변환 (태양-지구-달 계층 구조)",        WorldTransform_Main },
	{ "카메라 좌표계 (WASD 이동/줌)",              Camera_Main },
	{ "3D 큐브 (깊이 버퍼와 원근 투영)",           Cube3D_Main },
	{ "라이팅 (람버트 확산광)",                    Lighting_Main },
	{ "PNG 텍스처 그리기 (WIC 디코딩)",           PngTextureDraw_Main },
	{ "SVG 텍스처 그리기 (nanosvg 래스터)",        SvgTextureDraw_Main },
	{ "입력 처리 (키보드/마우스 이벤트)",          Input_Main },
	{ "씬 & 사운드 (Application/Director/SoundEngine)", SceneGraph_Main },
	{ "샘플러와 필터링 (Point/Linear, Wrap/Clamp/Mirror)", SamplerFiltering_Main },
	{ "블렌드 스테이트 (불투명/알파/가산/곱셈)",   BlendState_Main },
	{ "래스터라이저 스테이트 (와이어프레임/컸링)",  RasterizerState_Main },
	{ "셰이딩 모델 (램버트/퐁/블린-퐁)",          ShadingModel_Main },
	{ "렌더 타깃 (미니맵)",                        RenderTarget_Main },
	{ "포스트 프로세싱 (화면 후보정)",             PostProcess_Main },
	{ "그림자 매핑 (쉰도우 맵)",                   ShadowMapping_Main },
	{ "스프라이트 애니메이션 (시트/트위닝)",        SpriteAnimation_Main },
};

// 등록된 튜토리얼 개수를 반환한다.
int TutorialCount()
{
	return (int)(sizeof(s_Tutorials) / sizeof(s_Tutorials[0]));
}

// _index(0부터 시작)번째 튜토리얼 정보를 반환한다.
const TutorialEntry& TutorialAt(int _index)
{
	jc_assert(_index >= 0 && _index < TutorialCount());
	return s_Tutorials[_index];
}

} // namespace sgfr
