#include "SerialPort.h"


void dataProvider();												
void dataReceiver();														
PROCESS_INFORMATION CreateNewProcess(char* path, char* commandLine = NULL);


int main(int argc, char* argv[])
{
	if (argc > 1) {
		dataReceiver();
	} else {
		char commandLine[MAX_BUFFER_SIZE];
		sprintf(commandLine, " %d", 1);
		PROCESS_INFORMATION dataRecieverProcess = CreateNewProcess(argv[0], commandLine);
		dataProvider();
	}
}



void dataProvider() {
	std::cout << "\t#####  Start chatting  #####\n\n";

	HANDLE semaphore = CreateSemaphoreA(NULL, 0, 1, "binSemaphore");
	SerialPort dataProvider = SerialPort("COM1");
	dataProvider.init();

	std::string message;
	while (true) {
		std::cout << "Enter message: ";
		std::cin.clear();
		getline(std::cin, message);
	
		ReleaseSemaphore(semaphore, 1, NULL);
		dataProvider.write(message);
		WaitForSingleObject(semaphore, INFINITE);						

		std::cout << "Message was received from COM2: ";
		for (auto symbol : dataProvider.read()) {
			std::cout << symbol;
			Sleep(200);
		}
		std::cout << std::endl;
	}

	dataProvider.disconnect();
	CloseHandle(semaphore);
	return;
}

void dataReceiver() {
	HANDLE semaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, "binSemaphore");

	SerialPort dataReceiver = SerialPort("COM2");
	dataReceiver.init();
	std::string message;
	while (true) {
		WaitForSingleObject(semaphore, INFINITE);

		std::cout << "Message was recieved from COM1: ";
		for (auto symbol : dataReceiver.read()) {
			std::cout << symbol;
			Sleep(200);
		}
		std::cout << std::endl;

		std::cout << "Reply: ";
		getline(std::cin, message);
		dataReceiver.write(message);

		Sleep(400);
		ReleaseSemaphore(semaphore, 1, NULL);
	}
	dataReceiver.disconnect();
	CloseHandle(semaphore);
	return;
}

PROCESS_INFORMATION CreateNewProcess(char* path, char* commandLine) {
	STARTUPINFO startupInfo;
	ZeroMemory(&startupInfo, sizeof(STARTUPINFO));

	PROCESS_INFORMATION procInfo;
	ZeroMemory(&procInfo, sizeof(PROCESS_INFORMATION));

	if (CreateProcessA(
		path,
		commandLine,	
		NULL,
		NULL,
		TRUE,
		CREATE_NEW_CONSOLE,
		NULL,
		NULL,
		&startupInfo,
		&procInfo) == FALSE) {
		return procInfo;
	}
	return procInfo;
}