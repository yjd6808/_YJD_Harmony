/*
 * 작성자: 윤정도
 * 생성일: 8/14/2023 9:13:09 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/Core.h>
#include <SteinsGate/Common/Type.h>

#include <json.h>

struct JCORE_NOVTABLE RuntimeConfigBase
{
	RuntimeConfigBase();
	virtual ~RuntimeConfigBase() = default;

	void Load();
	void Save();

	virtual void OnLoading(Json::Value& root) = 0;
	virtual void OnSaving(Json::Value& root) = 0;

	void ReadCore(Json::Value& root);
	void WriteCore(Json::Value& root);

	void ShowCommandFilter(JNetwork::Transmission transmission);



	// 특정 커맨드에 대해서 로그를 보여줄지
	// 참고) 여러쓰레드에서 접근하지만, 정확성은 크게 중요치 않다. 성능을 위해 락을 사용하지 않음. 
	// 오류로 프로그램 강종되는 경우가 중간에 Expand가 발생해서 잘못된 메모리를 참조해버리는 경우가 있을 수 있는데
	// 넉넉하게 용량을 지정해놓으면 Expand 될일이 없으므로 상관없다.
	SGVector<Cmd_t> RecvCommandFilter;
	SGVector<Cmd_t> SendCommandFilter;

	// 수신/송신 커맨드 로그를 출역할지 여부
	bool ViewRecvCommand;
	bool ViewSendCommand;

	// 수신/송신 패킷을 16진수 형식의 바이너리 데이터를 출력할지
	bool ViewRecvPacketHex;
	bool ViewSendPacketHex;

	static constexpr char RecvCommandFilterKey[] = "recv_command_filter";
	static constexpr char SendCommandFilterKey[] = "send_command_filter";

	static constexpr char ViewRecvCommandKey[] = "view_recv_command";
	static constexpr char ViewSendCommandKey[] = "view_send_command";

	static constexpr char ViewRecvPacketHexKey[] = "view_recv_packet_hex";
	static constexpr char ViewSendPacketHexKey[] = "view_send_packet_hex";
};