/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 9:00:00 AM
 * 수정일: 8/9/2026 6:30:00 PM - v3 개편: 튜토리얼 재배치 + 신규 9종 추가 (총 31개)
 * =====================
 * 튜토리얼 목록 정의
 * 이 파일의 배열 순서가 곧 콘솔 목차 번호다. (1번부터 순서대로 학습 권장)
 */

#include "Core.h"
#include "sgfr/TutorialRegistry.h"

#include "sgfr/Tutorial/01_LinearAlgebra/01_LinearAlgebra_Main.h"
#include "sgfr/Tutorial/02_WindowAndMessageLoop/02_WindowAndMessageLoop_Main.h"
#include "sgfr/Tutorial/03_DeviceAndContext/03_DeviceAndContext_Main.h"
#include "sgfr/Tutorial/04_DeviceAndSwapChain/04_DeviceAndSwapChain_Main.h"
#include "sgfr/Tutorial/05_RenderingPipeline/05_RenderingPipeline_Main.h"
#include "sgfr/Tutorial/06_HLSL/06_HLSL_Main.h"
#include "sgfr/Tutorial/07_VertexIndexBuffer/07_VertexIndexBuffer_Main.h"
#include "sgfr/Tutorial/08_MeshAndTopology/08_MeshAndTopology_Main.h"
#include "sgfr/Tutorial/09_PipelineStateObjects/09_PipelineStateObjects_Main.h"
#include "sgfr/Tutorial/10_WorldTransform/10_WorldTransform_Main.h"
#include "sgfr/Tutorial/11_Camera/11_Camera_Main.h"
#include "sgfr/Tutorial/12_ShaderStagesAndConstants/12_ShaderStagesAndConstants_Main.h"
#include "sgfr/Tutorial/13_Cube3D/13_Cube3D_Main.h"
#include "sgfr/Tutorial/14_Lighting/14_Lighting_Main.h"
#include "sgfr/Tutorial/15_PngTextureDraw/15_PngTextureDraw_Main.h"
#include "sgfr/Tutorial/16_SvgTextureDraw/16_SvgTextureDraw_Main.h"
#include "sgfr/Tutorial/17_SamplerFiltering/17_SamplerFiltering_Main.h"
#include "sgfr/Tutorial/18_BlendState/18_BlendState_Main.h"
#include "sgfr/Tutorial/19_RasterizerState/19_RasterizerState_Main.h"
#include "sgfr/Tutorial/20_Material/20_Material_Main.h"
#include "sgfr/Tutorial/21_ResourceManager/21_ResourceManager_Main.h"
#include "sgfr/Tutorial/22_RenderObjectLoop/22_RenderObjectLoop_Main.h"
#include "sgfr/Tutorial/23_StateCache/23_StateCache_Main.h"
#include "sgfr/Tutorial/24_ShadingModel/24_ShadingModel_Main.h"
#include "sgfr/Tutorial/25_RenderTarget/25_RenderTarget_Main.h"
#include "sgfr/Tutorial/26_PostProcess/26_PostProcess_Main.h"
#include "sgfr/Tutorial/27_ShadowMapping/27_ShadowMapping_Main.h"
#include "sgfr/Tutorial/28_Input/28_Input_Main.h"
#include "sgfr/Tutorial/29_SceneGraph/29_SceneGraph_Main.h"
#include "sgfr/Tutorial/30_SpriteAnimation/30_SpriteAnimation_Main.h"
#include "sgfr/Tutorial/31_PipelineJourney/31_PipelineJourney_Main.h"

// 튜토리얼 목록 (번호 순서 = 학습 순서)
namespace sgfr
{

static const TutorialEntry s_Tutorials[] =
{
	{ "선형대수 (벡터/행렬 기초)",                LinearAlgebra_Main },
	{ "윈도우와 메시지 루프",                       WindowAndMessageLoop_Main },
	{ "디바이스와 컨텍스트 (생성/바인딩 분리)",   DeviceAndContext_Main },
	{ "디바이스와 스왑체인 (화면 지우기)",          DeviceAndSwapChain_Main },
	{ "렌더링 파이프라인 (첫 삼각형)",             RenderingPipeline_Main },
	{ "HLSL 셰이더 (색상 보간과 애니메이션)",       HLSL_Main },
	{ "정점/인덱스 버퍼 (사각형 그리기)",           VertexIndexBuffer_Main },
	{ "메시와 토폴로지 (정점 조립 한 덩어리로)",    MeshAndTopology_Main },
	{ "파이프라인 상태 객체 (래스터/블렌드/깊이/샘플러)", PipelineStateObjects_Main },
	{ "월드 변환 (태양-지구-달 계층 구조)",        WorldTransform_Main },
	{ "카메라 좌표계 (WASD 이동/줌)",              Camera_Main },
	{ "셰이더 스테이지와 상수버퍼 (b0/b1 규약)",    ShaderStagesAndConstants_Main },
	{ "3D 큐브 (깊이 버퍼와 원근 투영)",           Cube3D_Main },
	{ "라이팅 (람버트 확산광)",                    Lighting_Main },
	{ "PNG 텍스처 그리기 (WIC 디코딩)",           PngTextureDraw_Main },
	{ "SVG 텍스처 그리기 (nanosvg 래스터)",        SvgTextureDraw_Main },
	{ "샘플러와 필터링 (Point/Linear, Wrap/Clamp/Mirror)", SamplerFiltering_Main },
	{ "블렌드 스테이트 (불투명/알파/가산/곱셈)",   BlendState_Main },
	{ "래스터라이저 스테이트 (와이어프레임/컬링)",  RasterizerState_Main },
	{ "머티리얼 (셰이더+상태+텍스처 묶음)",         Material_Main },
	{ "리소스 매니저 (키 발급과 수명 관리)",         ResourceManager_Main },
	{ "렌더 오브젝트 루프 (SceneRenderer)",         RenderObjectLoop_Main },
	{ "스테이트 캐시 (중복 바인딩 절감)",           StateCache_Main },
	{ "셰이딩 모델 (람버트/퐁/블린-퐁)",          ShadingModel_Main },
	{ "렌더 타깃 (미니맵)",                        RenderTarget_Main },
	{ "포스트 프로세싱 (화면 후보정)",             PostProcess_Main },
	{ "그림자 매핑 (섀도우 맵)",                   ShadowMapping_Main },
	{ "입력 처리 (키보드/마우스 이벤트)",          Input_Main },
	{ "씬 & 사운드 (Application/Director/SoundEngine)", SceneGraph_Main },
	{ "스프라이트 애니메이션 (시트/트위닝)",        SpriteAnimation_Main },
	{ "파이프라인 여행 (수도 코드 §1~§20 캡스톤)",  PipelineJourney_Main },
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
