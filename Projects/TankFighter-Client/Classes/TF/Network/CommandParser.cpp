#define _WINSOCKAPI_

#include <TF/Network/CommandParser.h>

void CommandParser::AddCommand(Int16 cmd, JCore::Action<JNetwork::ICommand*> fn) {
}

bool CommandParser::RunCommand(JNetwork::ICommand* cmd) const {
	return true;
}
