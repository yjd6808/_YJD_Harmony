/*
 * 작성자: 윤정도
 * 생성일: 8/4/2026 10:30:00 PM
 * =====================
 * sgf 프로젝트 PCH(Precompiled Header) 파일입니다.
 *
 * [PCH란?]
 *  - 자주 바뀌지 않는 무거운 헤더(Windows.h, d3d11.h, jc 라이브러리 등)를
 *    미리 한 번만 컴파일해두고 재사용하는 기능이다.
 *  - base.props의 ForcedIncludeFiles 설정에 의해 모든 cpp에 자동 포함된다.
 *  - 덕분에 각 cpp 파일 최상단에 매번 무거운 헤더를 적지 않아도 된다.
 *
 * [DirectX11 헤더 설명]
 *  - d3d11.h        : DX11의 핵심. 디바이스/컨텍스트/리소스 인터페이스 정의
 *  - d3dcompiler.h  : HLSL 셰이더 소스를 런타임에 컴파일하는 D3DCompile 함수
 *  - DirectXMath.h  : SIMD 최적화된 수학 라이브러리 (XMMATRIX, XMVECTOR)
 *  - wincodec.h     : WIC(Windows Imaging Component). PNG/JPG 디코딩에 사용
 *
 * [pragma comment(lib, ...)란?]
 *  - 링커에게 "이 라이브러리 파일을 같이 링크해라"라고 코드로 지시하는 방법.
 *  - 프로젝트 설정에 일일이 추가하지 않아도 되어 편리하다.
 */

#pragma once

// Windows.h가 winsock.h 등 불필요한 헤더를 끌고 오지 않도록 차단 (jc와 동일한 규칙)
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <windowsx.h>		// GET_X_LPARAM, GET_Y_LPARAM 등 마우스 좌표 추출 매크로

#include <d3d11.h>			// DirectX 11 핵심 헤더
#include <d3dcompiler.h>	// HLSL 런타임 컴파일 (D3DCompile)
#include <DirectXMath.h>	// XMMATRIX 등 SIMD 수학 (검증용/참고용으로만 사용)
#include <wincodec.h>		// WIC 이미지 디코더 (PNG 로딩)
#include <xaudio2.h>		// XAudio2 사운드 API (SoundEngine)
#include <wrl/client.h>		// Microsoft::WRL::ComPtr (COM 스마트 포인터)

#include <cstdio>
#include <cmath>
#include <cstring>

// DX11 관련 라이브러리 링크
#pragma comment(lib, "d3d11.lib")			// DX11 코어
#pragma comment(lib, "d3dcompiler.lib")		// 셰이더 컴파일러
#pragma comment(lib, "dxgi.lib")			// DXGI (스왑체인/어댑터)
#pragma comment(lib, "windowscodecs.lib")	// WIC (PNG 디코딩)
#pragma comment(lib, "xaudio2.lib")			// XAudio2 (사운드)

// jc 라이브러리 (내가 만든 C++ 기반 라이브러리)
#include "jc/Define.h"
#include "jc/TypeCast.h"
#include "jc/Macro.h"
#include "jc/Type.h"
#include "jc/Assert.h"
#include "jc/Event.h"
#include "jc/Container/Vector.h"
#include "jc/Container/HashMap.h"
#include "jc/Pattern/Singleton.h"
#include "jc/Primitives/String.h"
#include "jc/Utils/Console.h"
#include "jc/Time.h"					// jc::TimeSpan (v2 DeltaTime)
#include "jc/FileSystem/File.h"			// jc 파일 IO
#include "jc/FileSystem/FileStream.h"	// jc 파일 스트림 (WAV 로딩 등)

#include "sgf/Namespace.h"

// COM 스마트 포인터 단축 별칭
// ComPtr는 COM 객체의 Release()를 소멸자에서 자동 호출해주는 스마트 포인터다.
// DX11의 모든 인터페이스(ID3D11Device 등)는 COM 객체이므로 이걸 쓰면
// JC_RELEASE_SAFE를 일일이 호출하지 않아도 메모리 누수가 방지된다.
template <typename T>
using SgfComPtr = Microsoft::WRL::ComPtr<T>;
