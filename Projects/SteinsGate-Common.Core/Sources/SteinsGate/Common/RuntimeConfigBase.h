/*
 * 작성자: 윤정도
 * 생성일: 8/14/2023 9:13:09 AM
 * =====================
 *
 */


#pragma once

#include <JCore/Logger/LoggerAbstract.h>

#include <SteinsGate/Common/Core.h>
#include <SteinsGate/Common/Type.h>

#include <json.h>

struct JCORE_NOVTABLE RuntimeConfigBase
{
	RuntimeConfigBase();
	virtual ~RuntimeConfigBase() = default;

	void Load();
	void Delete();
	void Save();

	virtual void OnLoading(Json::Value& _root) = 0;
	virtual void OnSaving(Json::Value& _root) = 0;

	void ReadCore(Json::Value& _root);
	void WriteCore(Json::Value& _root);

	void ShowCommandFilter(JNetwork::Transmission _transmission);
	void FilterCommand(JNetwork::Transmission _transmission, Cmd_t _cmd);
	void UnfilterCommand(JNetwork::Transmission _transmission, Cmd_t _cmd);
	bool IsFilteredCommand(JNetwork::Transmission _transmission, Cmd_t _cmd);

	void ApplyLoggerOption();
	void ApplyNetLoggerOption();


	// 수신/송신 커맨드 로그를 출역할지 여부
	bool showRecvCommand_;
	bool showSendCommand_;

	// 수신/송신 패킷을 16진수 형식의 바이너리 데이터를 출력할지
	bool showRecvPacketHex_;
	bool showSendPacketHex_;

	JCore::ConsoleColor consoleLogColor_[JCore::LoggerAbstract::Level::eMax];
	JCore::ConsoleColor consoleNetLogColor_[JCore::LoggerAbstract::Level::eMax];

	bool showConsoleLog_[JCore::LoggerAbstract::Level::eMax];
	bool showConsoleNetLog_[JCore::LoggerAbstract::Level::eMax];

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

private:
	// 테이블 크기를 크게 잡더라도 버킷 내부에서 확장이 발생할 수 있어서 락이 필요하지만 디버깅용으로만 쓰기땜에 락은 우선보류..
	SGHashSet<Cmd_t> recvCommandFilter_;
	SGHashSet<Cmd_t> sendCommandFilter_;
	SGNormalLock filterLock_;
};
