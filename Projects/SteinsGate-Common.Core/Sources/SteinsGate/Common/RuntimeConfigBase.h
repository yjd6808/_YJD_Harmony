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

	virtual void OnLoading(Json::Value& root) = 0;
	virtual void OnSaving(Json::Value& root) = 0;

	void ReadCore(Json::Value& root);
	void WriteCore(Json::Value& root);

	void ShowCommandFilter(JNetwork::Transmission transmission);
	void FilterCommand(JNetwork::Transmission transmission, Cmd_t cmd);
	void UnfilterCommand(JNetwork::Transmission transmission, Cmd_t cmd);
	bool IsFilteredCommand(JNetwork::Transmission transmission, Cmd_t cmd);

	void ApplyLoggerOption();
	void ApplyNetLoggerOption();


	// 수신/송신 커맨드 로그를 출역할지 여부
	bool ShowRecvCommand;
	bool ShowSendCommand;

	// 수신/송신 패킷을 16진수 형식의 바이너리 데이터를 출력할지
	bool ShowRecvPacketHex;
	bool ShowSendPacketHex;

	JCore::ConsoleColor ConsoleLogColor[JCore::LoggerAbstract::Level::eMax];
	JCore::ConsoleColor ConsoleNetLogColor[JCore::LoggerAbstract::Level::eMax];

	bool ShowConsoleLog[JCore::LoggerAbstract::Level::eMax];
	bool ShowConsoleNetLog[JCore::LoggerAbstract::Level::eMax];

	static constexpr char RecvCommandFilterKey[] = "recv_command_filter";
	static constexpr char SendCommandFilterKey[] = "send_command_filter";

	static constexpr char ShowRecvCommandKey[] = "show_recv_command";
	static constexpr char ShowSendCommandKey[] = "show_send_command";

	static constexpr char ShowRecvPacketHexKey[] = "show_recv_packet_hex";
	static constexpr char ShowSendPacketHexKey[] = "show_send_packet_hex";

	static constexpr char ConsoleLogColorKey[] = "console_log_color";
	static constexpr char ConsoleNetLogColorKey[] = "console_net_log_color";

	static constexpr char ShowConsoleLogKey[] = "show_console_log";
	static constexpr char ShowConsoleNetLogKey[] = "show_console_net_log";

private:
	// 테이블 크기를 크게 잡더라도 버킷 내부에서 확장이 발생할 수 있어서 락이 필요하지만 디버깅용으로만 쓰기땜에 락은 우선보류..
	SGHashSet<Cmd_t> RecvCommandFilter;
	SGHashSet<Cmd_t> SendCommandFilter;
	SGNormalLock FilterLock;
};
