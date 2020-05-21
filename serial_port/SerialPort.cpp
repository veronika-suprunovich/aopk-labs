#include "SerialPort.h"

SerialPort::SerialPort(std::string port) {
	handler = CreateFileA(
		port.c_str(),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (handler == (HANDLE)-1) {
		handler = 0;
		throw GetLastError();
	}
}

void SerialPort::init() {
	SetCommMask(this->handler, EV_RXCHAR);
	SetupComm(handler, 1500, 1500);


	COMMTIMEOUTS commTimeOuts;
	commTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF;
	commTimeOuts.ReadTotalTimeoutMultiplier = 0;
	commTimeOuts.ReadTotalTimeoutConstant = TIMEOUT;
	commTimeOuts.WriteTotalTimeoutMultiplier = 0;
	commTimeOuts.WriteTotalTimeoutConstant = TIMEOUT;

	if (!SetCommTimeouts(this->handler, &commTimeOuts)) {
		CloseHandle(handler);
		handler = INVALID_HANDLE_VALUE;
		return;
	}

	DCB comParameters{ 0 };
	comParameters.DCBlength = sizeof(DCB);
	GetCommState(handler, &comParameters);
	comParameters.BaudRate = CBR_9600;
	comParameters.ByteSize = 8;
	comParameters.Parity = NOPARITY;
	comParameters.StopBits = ONESTOPBIT;
	comParameters.fAbortOnError = TRUE;
	comParameters.fDtrControl = DTR_CONTROL_DISABLE;
	comParameters.fRtsControl = RTS_CONTROL_DISABLE;
	comParameters.fBinary = TRUE;
	comParameters.fParity = FALSE;
	comParameters.fInX = comParameters.fOutX = FALSE;
	comParameters.XonChar = 0;
	comParameters.XoffChar = (unsigned char)0xFF;
	comParameters.fErrorChar = FALSE;
	comParameters.fNull = FALSE;
	comParameters.fOutxCtsFlow = FALSE;
	comParameters.fOutxDsrFlow = FALSE;
	comParameters.XonLim = 128;
	comParameters.XoffLim = 128;

	if (!SetCommState(handler, &comParameters)) {
		CloseHandle(handler);
		handler = INVALID_HANDLE_VALUE;
		std::cout << GetLastError() << std::endl;
		return;
	}
}

void SerialPort::write(std::string message) {
	DWORD feedback;

	char buffer[MAX_BUFFER_SIZE]{ 0 };
	int bufferSize = sizeof(buffer);

	int size = message.size();
	WriteFile(handler, &size, sizeof(size), &feedback, NULL);

	message.copy(buffer, bufferSize, 0);
	if (!WriteFile(handler, buffer, bufferSize, &feedback, NULL)) {
		CloseHandle(handler);
		throw "Error during writing to the file";
	}
}

std::string SerialPort::read() {
	DWORD feedback;
	int blockNum;
	std::string dest;

	char buffer[MAX_BUFFER_SIZE]{ 0 };
	int bufferSize = sizeof(buffer);

	int size;
	if (!ReadFile(handler, &size, sizeof(size), &feedback, NULL)) {
		CloseHandle(handler);
		throw "Error during reading the file";
	}
	
	if (!ReadFile(handler, buffer, bufferSize, &feedback, NULL)) {
		CloseHandle(handler);
		throw "Error during reading the file";
	}

	dest.append(buffer, bufferSize);
	dest.resize(size);

	return dest;
}

void SerialPort::disconnect() {
	CloseHandle(handler);
}


