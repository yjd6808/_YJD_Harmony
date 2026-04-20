/*
 * 작성자 : 윤정도
 *
 * 콘솔 메뉴 아이템 / 옵션
 *
 * [사용 예시]
 *
 *   auto pOption = jc::MakeShared<ConsoleMenuItemOption>();
 *   pOption->inputTitle_        = "수행할 함수를 선택";
 *   pOption->inputLeftBrace_    = "[";
 *   pOption->inputRightBrace_   = "]";
 *   pOption->inputRightPadding_ = " - ";
 *   pOption->pressAnyKeyAfterCallback_ = true;
 *
 *   auto pRoot = jc::MakeShared<ConsoleMenuItem>();
 *   pRoot->AddHeader(
 *       "========================================\n"
 *       "           학습 프로그램 메뉴\n"
 *       "========================================"
 *   )
 *   ->Add("1", "항목 A", [](ConsoleMenuItem&) { ... })
 *   ->Add("2", "항목 B", []()               { ... })
 *   ->AddBack("0", "종료");
 *
 *   ConsoleMenuItem* pChild = nullptr;
 *   if (pRoot->AddMenu("3", "서브메뉴", pChild))
 *   {
 *       pChild->AddHeader("서브메뉴 헤더")
 *              ->Add("1", "서브 항목", [](){})
 *              ->AddBack("0");
 *   }
 *
 *   pRoot->Print(pOption);   // 또는 Console::PrintMenu(pRoot.GetPtr(), pOption.GetPtr())
 */

#pragma once

#include "jc/Utils/Console.h"
#include "jc/Utils/ConsoleColor.h"
#include "jc/Primitives/SmartPtr.h"
#include "jc/Primitives/String.h"
#include "jc/Functional.h"

#include <vector>

NS_JC_BEGIN

// 전방 선언
class ConsoleMenuItem;
struct ConsoleMenuItemOption;

using ConsoleMenuItemPtr       = SharedPtr<ConsoleMenuItem>;
using ConsoleMenuItemOptionPtr = SharedPtr<ConsoleMenuItemOption>;

// ============================================================
// ConsoleMenuItemOption
// ============================================================
struct ConsoleMenuItemOption
{
	// true 이면 inputTitle_ + "> " 를 입력 프롬프트로 표시
	bool userInputTitle_ = true;

	// 입력 프롬프트 타이틀 (userInputTitle_ = true 일 때 사용)
	// ex) inputTitle_ = "수행할 함수를 선택"  ->  "수행할 함수를 선택> "
	jc::String inputTitle_ = jc::String("입력");

	// false: 대소문자 구분 없이 매칭 (기본값)
	// true : 대소문자 정확히 일치해야 매칭
	bool caseMatch_ = false;

	// true 이면 Add("help") 가 "HELP" 로 표시됨
	bool inputForceCase_ = false;

	jc::String inputLeftBrace_;
	jc::String inputRightBrace_;

	// inputFormatPadding_ 이 2 라면
	//   Add("1")  ->  [ 1]
	//   Add("20") ->  [20]
	int inputFormatPadding_ = -1;

	// 키 표시 블록([key]) 앞
	jc::String inputLeftPadding_ = jc::String(" ");

	// 키 표시 블록([key]) 뒤
	jc::String inputRightPadding_ = jc::String(" ");

	ConsoleColor titleColor_        = ConsoleColor::LightGray;
	ConsoleColor menuColor_         = ConsoleColor::LightGray;
	ConsoleColor inputColor_        = ConsoleColor::LightGray;
	ConsoleColor inputBraceColor_   = ConsoleColor::LightGray;
	ConsoleColor inputPaddingColor_ = ConsoleColor::LightGray;

	// Back/Home 항목의 기본 표시 이름
	jc::String defaultMenuName_ = jc::String("NO NAME");
	jc::String backMenuName_ = jc::String("GO HOME");
	jc::String homeMenuName_ = jc::String("GO BACK");

	// 콜백 실행 후 자동으로 "아무 키나 눌러서 계속하세요..." 출력
	bool pressAnyKeyAfterCallback_ = false;
};

#define CONSOLE_MENU_ACTION(_content) [=](ConsoleMenuItem& _menu) { _content; }

// ============================================================
// ConsoleMenuItem
// ============================================================
class ConsoleMenuItem
{
public:
	enum class EntryType
	{
		Normal,   // 일반 항목 (콜백 실행) 또는 서브 메뉴
		Back,     // 부모 메뉴로 이동 (루트에서는 메뉴 종료)
		Home,     // 루트 메뉴로 이동
	};

	struct Entry
	{
		jc::String  key_;
		jc::String  text_;
		EntryType   type_ = EntryType::Normal;
		jc::Action<ConsoleMenuItem&> callback_;
		ConsoleMenuItemPtr pChild_;   // non-null 이면 서브메뉴
	};

public:
	// 메뉴 상단 헤더 텍스트
	jc::String header_;

	// 부모 메뉴 (raw pointer - 순환참조 방지)
	ConsoleMenuItem* pParent_ = nullptr;

	// 메뉴 항목 목록
	std::vector<Entry> entries_;

public:
	// --------------------------------------------------------
	// 빌더 메서드 (메서드 체이닝 지원)
	// --------------------------------------------------------

	// 헤더 텍스트 설정
	ConsoleMenuItem* AddHeader(const jc::String& _text);

	// 일반 항목 추가 - Action<ConsoleMenuItem&> 콜백
	ConsoleMenuItem* Add(const jc::String& _key, const jc::String& _text, jc::Action<ConsoleMenuItem&> _callback);
	ConsoleMenuItem* Add(const jc::String& _key, const jc::String& _text, jc::Action<> _callback);
	

	// 서브메뉴 항목 추가
	// _pOutChild : 생성된 자식 ConsoleMenuItem 의 raw pointer 를 반환 (항상 non-null)
	// 반환값     : 항상 true
	bool AddMenu(const jc::String& _key, const jc::String& _text, ConsoleMenuItem*& _pOutChild);

	// 뒤로 가기 항목 추가 (루트에서 선택하면 메뉴 루프 종료)
	// _key 기본값: 빈 문자열 (Enter 입력으로 매칭)
	ConsoleMenuItem* AddBack(const jc::String& _key = jc::String(), const jc::String& _text = jc::String());

	// 루트 메뉴로 이동 항목 추가
	// _key 기본값: 빈 문자열 (Enter 입력으로 매칭)
	ConsoleMenuItem* AddHome(const jc::String& _key = jc::String(), const jc::String& _text = jc::String());

	// --------------------------------------------------------
	// 메뉴 실행
	// --------------------------------------------------------

	// 인터랙티브 메뉴 루프 실행 (이 항목을 루트로 동작)
	void Show(ConsoleMenuItemOptionPtr _pOption = nullptr);

	// SharedPtr 인자 편의 오버로드
	static void Show(ConsoleMenuItemPtr _pItem, ConsoleMenuItemOptionPtr _pOption = nullptr);
};

NS_END
