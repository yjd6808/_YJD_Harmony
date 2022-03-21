#pragma once

#include <JNetwork/IPAddress.h>

#include <Windows.h>



class Socket
{
	
public:
	DWORD Bind();
	DWORD Listen();
	DWORD Accept();
	DWORD Connet();
	DWORD Send();
	DWORD SendTo();
	DWORD Receive();
	DWORD ReceiveFrom();

	DWORD SetSendBufferSize();
	DWORD SetRecvBufferSize();
	DWORD GetSendBufferSize();
	DWORD GetRecvBufferSize();
	DWORD GetLingerTimeout();
	DWORD SetNonBlockingMode();
	DWORD SetLinger();


	bool IsNagleEnabled();
	bool IsReuseAddressEnabled();
	bool IsExclusiveReuseAddressEnabled();
	bool IsLingerEnabled();
};