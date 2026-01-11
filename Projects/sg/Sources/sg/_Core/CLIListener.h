#pragma once

#define REGISTER_CLI_FUNCTION(name, function, desc) \
	bool function(int _argc, jc::String* _argv, jc::String& _error); \
	static CLIListener::RegistrationFunctor JC_CONCAT_COUNTER(__cli_reg_fn_){ name, function, desc };

class CLIListener final
{
public:
	using TCLI_Callback = jc::Func<int, int, jc::String*, jc::String&>;
	using TCLI_CallbackList = jc::Vector<jc::Pair<jc::String, TCLI_Callback>>;
	using TCLI_Table = jc::TreeMap<jc::String, TCLI_CallbackList>;

	struct RegisteredFunction
	{
		jc::String		name_;
		TCLI_Callback	function_;
		jc::String		desc_;
	};

	struct RegistrationFunctor
	{
		RegistrationFunctor(const jc::String& _name, const TCLI_Callback& _function, const jc::String& _desc)
		{
			CLIListener::RegisterFunction(_name, _function, _desc);
		}
	};

	CLIListener();

	/**
	 * \brief 커맨드 실행
	 * \param _argc 커맨드 이름을 포함한 인자 갯수
	 * \param _pArgv _pArgv[0] 커맨드 이름, _pArgv[1,2, ...] 커맨드 인자
	 * \param _table
	 * \return 이벤트 터널링 여부
	 */
	bool ParseInput(int _argc, jc::String* _argv);

	bool GetDesc(const jc::String& _key, OUT jc::String& _desc);
	bool ShowDesc(const jc::String& _key);
	void ShowAllFunctions();

	static int ToNumber(const jc::String& _numString, int _argIndex);

private:
	TCLI_Table cliTable_;

	static jc::Vector<RegisteredFunction>& Registry();
	static void RegisterFunction(const jc::String& _name, const TCLI_Callback& _function, const jc::String& _desc);
};
