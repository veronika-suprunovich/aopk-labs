#define _SCL_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <windows.h>
#include <conio.h>
#include <ctime>
#include <iostream>
#include <string>
#include <cstdlib>

#define TIMEOUT 1000
#define MAX_BUFFER_SIZE 20

class SerialPort {
private:
	HANDLE handler;
public:
	SerialPort(std::string port);
	void init();
	void write(std::string message);
	std::string read();
	void disconnect();
};