/*
 * 작성자: 윤정도
 * 생성일: 8/16/2023 6:26:28 AM
 * =====================
 *
 */


#include "CLIListener.h"
#include "CLIFunctions_Linker.h"
#include "AppConfig.h"

USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
CLIListener::CLIListener()
{
	CLI_ForceLinkage();

	auto& registeredFunctions = Registry();
	jc_assert(registeredFunctions.Size() > 0); // 사전 등록된 함수들이 없습니다.

	for (int i = 0; i < registeredFunctions.Size(); ++i)
	{
		const RegisteredFunction& regFunc = registeredFunctions.At(i);
		TCLI_CallbackList* pCallbackList = cliTable_.Find(regFunc.name_.Source());
		if (pCallbackList == nullptr)
		{
			TCLI_CallbackList newCallbackList;
			newCallbackList.PushBack(jc::Pair<jc::String, TCLI_Callback>{ regFunc.desc_, regFunc.function_ });
			cliTable_.Insert(regFunc.name_.Source(), newCallbackList);
		}
		else
		{
			pCallbackList->PushBack(jc::Pair<jc::String, TCLI_Callback>{ regFunc.desc_, regFunc.function_ });
		}
	}
	registeredFunctions.Clear(); // 등록 이후 모두 정리한다.
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CLIListener::ParseInput(int _argc, String* _pArgv)
{
	TCLI_CallbackList* pCallbackList = cliTable_.Find(_pArgv[0].Source());
	if (pCallbackList == nullptr)
		return false;

	jc::String error;
	if (pCallbackList && pCallbackList->Size() > 0)
	{
		for (int i = 0; i < pCallbackList->Size(); ++i)
		{
			if (pCallbackList->At(i).value_(_argc, _pArgv, error))
				break; // false를 반환한 경우 propagation 하지 않음.
			
			if (error.Length() > 0)
			{
				String desc;
				GetDesc(_pArgv[0], desc);
				Console::WriteLine("%s 명령 수행중 오류 발생: %s\n%s", _pArgv[0].SafeSource(), error.SafeSource(), desc.SafeSource());
				break; // 오류가 발생한 경우에도 propagation 하지 않음
			}
		}

		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CLIListener::GetDesc(const jc::String& _key, OUT jc::String& _desc)
{
	TCLI_CallbackList* pCallbackList = cliTable_.Find(_key);
	if (pCallbackList == nullptr)
		return false;

	for (int i = 0; i < pCallbackList->Size(); ++i)
	{
		const jc::Pair<jc::String, TCLI_Callback>& pair = pCallbackList->At(i);
		_desc += pair.key_;
		_desc += "\n";
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CLIListener::ShowDesc(const jc::String& _key)
{
	jc::String desc;
	if (GetDesc(_key, desc))
	{
		Console::WriteLine("%s", desc.Source());
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
jc::Vector<CLIListener::RegisteredFunction>& CLIListener::Registry()
{
	// 정적 멤버로 초기화하지 않는 이유는 정적 초기화 순서 문제를 피하기 위해서이다.
	static jc::Vector<RegisteredFunction> registeredFunctions_;
	return registeredFunctions_;
}

//////////////////////////////////////////////////////////////////////////////////////////
void CLIListener::RegisterFunction(const jc::String& _name, const TCLI_Callback& _function, const jc::String& _desc)
{
	RegisteredFunction regFunc;
	regFunc.name_ = _name;
	regFunc.function_ = _function;
	regFunc.desc_ = _desc;
	Registry().PushBack(regFunc);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CLI_HelpBase(int _argc, jc::String* _pArgv, OUT jc::String& _error)
{
	if (_argc == 1)
	{
		g_cCLIListener.ShowAllFunctions();
		return false;
	}
	else if (_argc == 2)
	{
		jc::String desc;
		if (g_cCLIListener.GetDesc(_pArgv[1], desc))
		{
			Console::WriteLine("%s", desc.Source());
		}
		else
		{
			_error = JC_FMT("%s는 알 수 없는 함수명입니다.", _pArgv[0].SafeSource());
		}
	}
	else
	{
		_error = "인자 갯수가 올바르지 않습니다.";
	}
	return true; // propagation
}

//////////////////////////////////////////////////////////////////////////////////////////
void CLIListener::ShowAllFunctions()
{
	auto it = cliTable_.Begin();

	jc::String text{ 1024 };
	while (it.HasNext())
	{
		jc::Pair<jc::String, TCLI_CallbackList>& list = it.Next();
		text += " - ";
		text += list.key_ + "\n";
	}
	jc::Console::WriteLine(text.SafeSource());
}

//////////////////////////////////////////////////////////////////////////////////////////
int CLIListener::ToNumber(const jc::String& _numString, int _argIndex)
{
	int value;
	if (!StringUtil::TryToNumber(value, _numString.Source()))
	{
		Console::WriteLine("%d번째 인자가 올바르지 않습니다.", _argIndex + 1);
		return InvalidValue_v;
	}
	return value;
}
