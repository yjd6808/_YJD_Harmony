/*
 * 작성자: 윤정도
 * 생성일: 8/16/2023 6:26:28 AM
 * =====================
 *
 */


#include "Core.h"
#include "CommonCoreHeader.h"
#include "CLIListenerBase.h"

USING_NS_JC;

bool CLIListenerBase::OnInputProcessing(int argc, String* argv) {

	if (argv[0] == "help") {
		return CLI_HelpBase();
	}

	if (argv[0] == RuntimeConfigBase::SendCommandFilterKey) {
		return CLI_SendCommandFilter(argc, argv);
	}

	if (argv[0] == RuntimeConfigBase::RecvCommandFilterKey) {
		return CLI_RecvCommandFilter(argc, argv);
	}

	if (argv[0] == RuntimeConfigBase::ViewSendCommandKey) {
		return CLI_ViewSendCommand(argc, argv);
	}

	if (argv[0] == RuntimeConfigBase::ViewRecvCommandKey) {
		return CLI_ViewRecvCommand(argc, argv);
	}

	if (argv[0] == RuntimeConfigBase::ViewSendPacketHexKey) {
		return CLI_ViewSendPacketHex(argc, argv);
	}

	if (argv[0] == RuntimeConfigBase::ViewRecvPacketHexKey) {
		return CLI_ViewRecvPacketHex(argc, argv);
	}

	if (argv[0] == "save_runtime_config") {
		CoreRuntimeConfigBase_v->Save();
		return false;
	}

	return true;
}

bool CLIListenerBase::CLI_HelpBase() {
	String szHelpText{ 1024 };
	szHelpText += " - send_command_filter [add|remove] [커맨드 ID]\n";
	szHelpText += " - recv_command_filter [add|remove] [커맨드 ID]\n";
	szHelpText += " - send_command_filter [show]\n";
	szHelpText += " - recv_command_filter [show]\n";
	szHelpText += "   송/수신 커맨드의 출력 로그에서 제외할 커맨드 ID를 등록하거나 제거합니다.\n";
	szHelpText += " - view_send_command [1|0]\n";
	szHelpText += " - view_recv_command [1|0]\n";
	szHelpText += "   송/수신한 커맨드 정보를 출력합니다.\n";
	szHelpText += " - view_send_packet_hex [1|0]\n";
	szHelpText += " - view_recv_packet_hex [1|0]";
	szHelpText += "   송/수신한 패킷의 바이너리 데이터를 16진수 형식으로 출력합니다.\n";
	szHelpText += " - save_runtime_config: 런타임 설정을 실행파일 디렉토리에 저장합니다.";
	Console::WriteLine(szHelpText.Source());
	return true;
}

bool CLIListenerBase::CLI_SendCommandFilter(int argc, String* argv) {
	if (argc <= 1) {
		Console::WriteLine("인자 갯수가 올바르지 않습니다.");
		return false;
	}

	if (argv[1] == "add") {
		if (argc <= 2) {
			Console::WriteLine("인자 갯수가 올바르지 않습니다.");
			return false;
		}

		Cmd_t iCmd;
		if (!StringUtil::TryToNumber(iCmd, argv[2].Source())) {
			Console::WriteLine("올바르지 않은 커맨드 ID입니다.");
			return false;
		}

		CoreRuntimeConfigBase_v->SendCommandFilter.PushBack(iCmd);
		Console::WriteLine("송신 커맨드 필터에 %d커맨드 추기완료", iCmd);
	} else if (argv[1] == "remove") {
		if (argc <= 2) {
			Console::WriteLine("인자 갯수가 올바르지 않습니다.");
			return false;
		}

		Cmd_t iCmd;
		if (!StringUtil::TryToNumber(iCmd, argv[2].Source())) {
			Console::WriteLine("올바르지 않은 커맨드 ID입니다.");
			return false;
		}

		CoreRuntimeConfigBase_v->SendCommandFilter.Remove(iCmd);
		Console::WriteLine("송신 커맨드 필터에서 %d커맨드 제거완료", iCmd);
	} else if (argv[1] == "show") {
		CoreRuntimeConfigBase_v->ShowCommandFilter(JNetwork::Transmission::Send);
	} else {
		Console::WriteLine("올바르지 않은 명령입니다.");
	}
	return false;
}

bool CLIListenerBase::CLI_RecvCommandFilter(int argc, String* argv) {
	if (argc <= 1) {
		Console::WriteLine("인자 갯수가 올바르지 않습니다.");
		return false;
	}

	if (argv[1] == "add") {
		if (argc <= 2) {
			Console::WriteLine("인자 갯수가 올바르지 않습니다.");
			return false;
		}

		Cmd_t iCmd;
		if (!StringUtil::TryToNumber(iCmd, argv[2].Source())) {
			Console::WriteLine("올바르지 않은 커맨드 ID입니다.");
			return false;
		}

		CoreRuntimeConfigBase_v->RecvCommandFilter.PushBack(iCmd);
		Console::WriteLine("수신 커맨드 필터에 %d커맨드 추가완료", iCmd);

	} else if (argv[1] == "remove") {
		if (argc <= 2) {
			Console::WriteLine("인자 갯수가 올바르지 않습니다.");
			return false;
		}

		Cmd_t iCmd;
		if (!StringUtil::TryToNumber(iCmd, argv[2].Source())) {
			Console::WriteLine("올바르지 않은 커맨드 ID입니다.");
			return false;
		}

		CoreRuntimeConfigBase_v->RecvCommandFilter.Remove(iCmd);
		Console::WriteLine("수신 커맨드 필터에서 %d커맨드 제거완료", iCmd);
	} else if (argv[1] == "show") {
		CoreRuntimeConfigBase_v->ShowCommandFilter(JNetwork::Transmission::Recv);
	} else {
		Console::WriteLine("올바르지 않은 명령입니다.");
	}
	return false;
}

bool CLIListenerBase::CLI_ViewSendCommand(int argc, String* argv) {
	if (argc <= 1) {
		Console::WriteLine("인자 갯수가 올바르지 않습니다.");
		return false;
	}

	int bEnable;
	if (!StringUtil::TryToNumber(bEnable, argv[1].Source())) {
		Console::WriteLine("올바르지 않은 커맨드 ID입니다.");
		return false;
	}

	CoreRuntimeConfigBase_v->ViewSendCommand = bEnable;

	if (bEnable) {
		Console::WriteLine("송신 커맨드 보기 활성화");
	} else {
		Console::WriteLine("송신 커맨드 보기 비활성화");
	}
	return false;
}

bool CLIListenerBase::CLI_ViewRecvCommand(int argc, String* argv) {
	if (argc <= 1) {
		Console::WriteLine("인자 갯수가 올바르지 않습니다.");
		return false;
	}

	int bEnable;
	if (!StringUtil::TryToNumber(bEnable, argv[1].Source())) {
		Console::WriteLine("올바르지 않은 커맨드 ID입니다.");
		return false;
	}

	CoreRuntimeConfigBase_v->ViewRecvCommand = bEnable;

	if (bEnable) {
		Console::WriteLine("수신 커맨드 보기 활성화");
	} else {
		Console::WriteLine("수신 커맨드 보기 비활성화");
	}
	return false;
}

bool CLIListenerBase::CLI_ViewSendPacketHex(int argc, String* argv) {
	if (argc <= 1) {
		Console::WriteLine("인자 갯수가 올바르지 않습니다.");
		return false;
	}

	int bEnable;
	if (!StringUtil::TryToNumber(bEnable, argv[1].Source())) {
		Console::WriteLine("올바르지 않은 커맨드 ID입니다.");
		return false;
	}

	CoreRuntimeConfigBase_v->ViewSendPacketHex = bEnable;

	if (bEnable) {
		Console::WriteLine("송신 패킷 헥스 보기 활성화");
	} else {
		Console::WriteLine("송신 커맨드 헥스 보기 활성화");
	}

	return false;
}

bool CLIListenerBase::CLI_ViewRecvPacketHex(int argc, String* argv) {
	if (argc <= 1) {
		Console::WriteLine("인자 갯수가 올바르지 않습니다.");
		return false;
	}

	int bEnable;
	if (!StringUtil::TryToNumber(bEnable, argv[1].Source())) {
		Console::WriteLine("올바르지 않은 커맨드 ID입니다.");
		return false;
	}

	CoreRuntimeConfigBase_v->ViewRecvPacketHex = bEnable;

	if (bEnable) {
		Console::WriteLine("수신 패킷 헥스 보기 활성화");
	} else {
		Console::WriteLine("수신 커맨드 헥스 보기 활성화");
	}
	return false;
}
