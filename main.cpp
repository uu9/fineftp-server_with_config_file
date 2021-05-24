#include <fineftp/server.h>

#include <iostream>
#include <thread>
#include <string>
#include <Windows.h>
#include <sys/stat.h>

bool isFileExists_stat(std::string& name) {
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

LPCWSTR stringToLPCWSTR(std::string orig)
{
	size_t origsize = orig.length() + 1;
	const size_t newsize = 100;
	size_t convertedChars = 0;
	wchar_t *wcstring = (wchar_t *)malloc(sizeof(wchar_t)*(orig.length() - 1));
	mbstowcs_s(&convertedChars, wcstring, origsize, orig.c_str(), _TRUNCATE);

	return wcstring;
}

std::string WCharToMByte(LPCWSTR lpcwszStr) {
	std::string str;
	DWORD dwMinSize = 0;
	LPSTR lpszStr = NULL;
	dwMinSize = WideCharToMultiByte(CP_OEMCP, NULL, lpcwszStr, -1, NULL, 0, NULL, FALSE);
	if (0 == dwMinSize)
	{
		return FALSE;
	}
	lpszStr = new char[dwMinSize];
	WideCharToMultiByte(CP_OEMCP, NULL, lpcwszStr, -1, lpszStr, dwMinSize, NULL, FALSE);
	str = lpszStr;
	delete[] lpszStr;
	return str;
}

int main() {
	std::string status_code;

	LPWSTR username = new WCHAR[20];
	LPWSTR password = new WCHAR[20];
	LPWSTR port = new WCHAR[10];
	LPWSTR root = new WCHAR[MAX_PATH];
	LPCWSTR lpPath = L"./config.ini";
	lstrcpyW(username, L"user");
	lstrcpyW(password, L"user");
	lstrcpyW(port, L"21");
	lstrcpyW(root, L"E:\\tmp\\");
	std::string path_str = WCharToMByte(lpPath);

	if (isFileExists_stat(path_str) == false) {
		WritePrivateProfileStringW(L"CONFIG", L"Username", username, lpPath);
		WritePrivateProfileStringW(L"CONFIG", L"Password", password, lpPath);
		WritePrivateProfileStringW(L"CONFIG", L"Port", port, lpPath);
		WritePrivateProfileStringW(L"CONFIG", L"Root", root, lpPath);
		std::cout << "Generating file 'config.ini'" << std::endl;
	}
	else {
		GetPrivateProfileStringW(L"CONFIG", L"Username", L"user", username, 20, lpPath);
		GetPrivateProfileStringW(L"CONFIG", L"Password", L"user", password, 20, lpPath);
		GetPrivateProfileStringW(L"CONFIG", L"Port", L"21", port, 10, lpPath);
		GetPrivateProfileStringW(L"CONFIG", L"Root", L"E:\\tmp\\", root, MAX_PATH, lpPath);
		std::cout << "Reading file 'config.ini'" << std::endl;
	}
	std::string local_root = WCharToMByte(root); // The backslash at the end is necessary!
	int port_i = _wtoi(port);
	std::string username_s = WCharToMByte(username);
	std::string password_s = WCharToMByte(password);
	fineftp::FtpServer server("0.0.0.0", port_i);
	server.addUser(username_s, password_s, local_root, fineftp::Permission::All);
	server.start(1);
	// Prevent the application from exiting immediatelly
	std::cout << "Starting ftp server 0.0.0.0" << port_i << std::endl;
	std::cout << std::endl
		<< "root:    " << local_root << std::endl
		<< "username:    " << username_s << std::endl
		<< "password:    " << password_s
		<< std::endl << std::endl;
	std::cout << "Input 'exit' to stop ftp server..." << std::endl;
	while (true) {
		std::cin >> status_code;
		if (status_code == "exit") {
			return 0;
		}
	}
}
