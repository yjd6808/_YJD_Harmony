/*
 * 작성자: 윤정도
 * 생성일: 8/14/2023 9:13:09 AM
 * =====================
 *
 */


#pragma once

#include <jc/Logger/LoggerAbstract.h>
#include <jc/Container/Properties.h>

//////////////////////////////////////////////////////////////////////////////////////////
struct ClientInfo;
struct AppConfig : jc::SingletonPointer<AppConfig>
{
	AppConfig();
	virtual ~AppConfig();

	// Command Arguments
	jc::String assetPath_;
	jc::String srcDataPath_;
	jc::String resDataPath_;
	jc::String resDataFontPath_;
	jc::String resDataImagePath_;
	jc::String resDataSoundPath_;

	void ReadEnvArgs();
	void LoadConfFile();
	void DeleteConfFile();
	void SaveConfiFile();

	void ReadCore(Json::Value& _root);
	void ReadClient(Json::Value& _clientRoot);
	void WriteCore(Json::Value& _root);
	void WriteClient(Json::Value& _clientRoot);

	void ShowCommandFilter(jnet::Transmission _transmission);
	void FilterCommand(jnet::Transmission _transmission, Cmd_t _cmd);
	void UnfilterCommand(jnet::Transmission _transmission, Cmd_t _cmd);
	bool IsFilteredCommand(jnet::Transmission _transmission, Cmd_t _cmd);

	void ApplyLoggerOption();
	void ApplyNetLoggerOption();
	ClientInfo* GetClientInfo() { return pClientInfo_; }

	// 수신/송신 커맨드 로그를 출역할지 여부
	bool showRecvCommand_;
	bool showSendCommand_;

	// 수신/송신 패킷을 16진수 형식의 바이너리 데이터를 출력할지
	bool showRecvPacketHex_;
	bool showSendPacketHex_;

	jc::ConsoleColor consoleLogColor_[jc::LoggerAbstract::Level::eMax];
	jc::ConsoleColor consoleNetLogColor_[jc::LoggerAbstract::Level::eMax];

	bool showConsoleLog_[jc::LoggerAbstract::Level::eMax];
	bool showConsoleNetLog_[jc::LoggerAbstract::Level::eMax];

	static constexpr char RECV_COMMAND_FILTER_KEY[] = "recv_command_filter";
	static constexpr char SEND_COMMAND_FILTER_KEY[] = "send_command_filter";

	static constexpr char SHOW_RECV_COMMAND_KEY[] = "show_recv_command";
	static constexpr char SHOW_SEND_COMMAND_KEY[] = "show_send_command";

	static constexpr char SHOW_RECV_PACKET_HEX_KEY[] = "show_recv_packet_hex";
	static constexpr char SHOW_SEND_PACKET_HEX_KEY[] = "show_send_packet_hex";

	static constexpr char CONSOLE_LOG_COLOR_KEY[] = "console_log_color";
	static constexpr char CONSOLE_NET_LOG_COLOR_KEY[] = "console_net_log_color";

	static constexpr char SHOW_CONSOLE_LOG_KEY[] = "show_console_log";
	static constexpr char SHOW_CONSOLE_NET_LOG_KEY[] = "show_console_net_log";

	jc::Properties<> props_;
private:
	// 테이블 크기를 크게 잡더라도 버킷 내부에서 확장이 발생할 수 있어서 락이 필요하지만 디버깅용으로만 쓰기땜에 락은 우선보류..
	jc::HashSet<Cmd_t> recvCommandFilter_;
	jc::HashSet<Cmd_t> sendCommandFilter_;
	jc::NormalLock filterLock_;
	ClientInfo* pClientInfo_ = nullptr;
};

JC_DECL_SINGLETON_VAR(AppConfig)
#define g_cAppConfig JC_DECL_SINGLETON_BODY(AppConfig)
#define g_cAppProps		(g_cAppConfig.props_)

