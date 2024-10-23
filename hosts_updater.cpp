 
/*
MIT License

Copyright (c) 2024

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction.

[Full MIT License Text]
*/

#include <windows.h>
#include <winhttp.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <ctime>
#include <regex>

#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "Advapi32.lib") 

// Constants
const std::wstring HOSTS_FILE_PATH = L"C:\\Windows\\System32\\drivers\\etc\\hosts";
const std::wstring BACKUP_FOLDER = L"C:\\Windows\\System32\\drivers\\etc\\backups\\";
const std::wstring HOSTS_URL = L"https://winhelp2002.mvps.org/hosts.txt";
const std::wstring START_MARKER = L"# BEGIN HOSTS BLOCKER ENTRIES";
const std::wstring END_MARKER = L"# END HOSTS BLOCKER ENTRIES";

// Function declarations
bool IsElevated();
void ShowMenu();
void AddOrUpdateHosts();
void RemoveHostsEntries();
void DownloadHostsFile(const std::wstring& downloadPath);
std::wstring GetTimestamp();
void FlushDNSCache();

int main() {
    if (!IsElevated()) {
        std::cerr << "This program must be run as an administrator.\n";
        system("pause");
        return 1;
    }

    ShowMenu();

    return 0;
}

// Function to check if the program is running with administrative privileges
bool IsElevated() {
    BOOL fRet = FALSE;
    HANDLE hToken = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        TOKEN_ELEVATION Elevation;
        DWORD cbSize = sizeof(TOKEN_ELEVATION);
        if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
            fRet = Elevation.TokenIsElevated;
        }
    }
    if (hToken) {
        CloseHandle(hToken);
    }
    return fRet;
}

// Function to display the menu
void ShowMenu() {
    int choice = 0;
    do {
        std::cout << "--------------------------------------------\n";
        std::cout << "Hosts File Updater Script\n";
        std::cout << "--------------------------------------------\n";
        std::cout << "1) Add/Update hosts entries\n";
        std::cout << "2) Remove hosts entries\n";
        std::cout << "3) Exit\n";
        std::cout << "--------------------------------------------\n";
        std::cout << "Please select an option [1-3]: ";
        std::cin >> choice;
        std::cin.ignore(); // Clear newline from input buffer
        switch (choice) {
            case 1:
                AddOrUpdateHosts();
                break;
            case 2:
                RemoveHostsEntries();
                break;
            case 3:
                std::cout << "Exiting.\n";
                break;
            default:
                std::cout << "Invalid option. Please try again.\n";
                break;
        }
    } while (choice != 3);
}

// Function to add or update hosts entries
void AddOrUpdateHosts() {
    std::wstring timestamp = GetTimestamp();
    std::wstring backupPath = BACKUP_FOLDER + L"hosts.backup." + timestamp;

    // Ensure backup directory exists
    CreateDirectoryW(BACKUP_FOLDER.c_str(), NULL);

    // Backup the existing hosts file
    if (!CopyFileW(HOSTS_FILE_PATH.c_str(), backupPath.c_str(), FALSE)) {
        std::wcerr << L"Failed to create a backup of the hosts file.\n";
        return;
    }
    std::wcout << L"Backup created at: " << backupPath << L"\n";

    // Download the new hosts entries
    std::wstring downloadPath = L"C:\\Windows\\Temp\\hosts_download.txt";
    DownloadHostsFile(downloadPath);

    // Read the downloaded hosts entries
    std::wifstream downloadedFile(downloadPath);
    if (!downloadedFile.is_open()) {
        std::wcerr << L"Failed to open the downloaded hosts file.\n";
        return;
    }
    std::wstringstream newEntries;
    newEntries << START_MARKER << L"\n";
    newEntries << downloadedFile.rdbuf();
    newEntries << L"\n" << END_MARKER << L"\n";
    downloadedFile.close();

    // Read the existing hosts file
    std::wifstream hostsFileIn(HOSTS_FILE_PATH);
    if (!hostsFileIn.is_open()) {
        std::wcerr << L"Failed to open the hosts file for reading.\n";
        return;
    }
    std::wstringstream hostsContent;
    hostsContent << hostsFileIn.rdbuf();
    hostsFileIn.close();

    // Remove existing entries added by this program
    std::wregex pattern(START_MARKER + L"[\\s\\S]*?" + END_MARKER + L"\n?");
    std::wstring updatedContent = std::regex_replace(hostsContent.str(), pattern, L"");

    // Append new entries
    updatedContent += newEntries.str();

    // Write the updated hosts file
    std::wofstream hostsFileOut(HOSTS_FILE_PATH, std::ios::trunc);
    if (!hostsFileOut.is_open()) {
        std::wcerr << L"Failed to open the hosts file for writing.\n";
        return;
    }
    hostsFileOut << updatedContent;
    hostsFileOut.close();

    std::wcout << L"Hosts file has been updated.\n";

    // Flush DNS cache
    FlushDNSCache();
}

// Function to remove hosts entries added by this program
void RemoveHostsEntries() {
    // Read the existing hosts file
    std::wifstream hostsFileIn(HOSTS_FILE_PATH);
    if (!hostsFileIn.is_open()) {
        std::wcerr << L"Failed to open the hosts file for reading.\n";
        return;
    }
    std::wstringstream hostsContent;
    hostsContent << hostsFileIn.rdbuf();
    hostsFileIn.close();

    // Remove entries added by this program
    std::wregex pattern(START_MARKER + L"[\\s\\S]*?" + END_MARKER + L"\n?");
    std::wstring updatedContent = std::regex_replace(hostsContent.str(), pattern, L"");

    // Write the updated hosts file
    std::wofstream hostsFileOut(HOSTS_FILE_PATH, std::ios::trunc);
    if (!hostsFileOut.is_open()) {
        std::wcerr << L"Failed to open the hosts file for writing.\n";
        return;
    }
    hostsFileOut << updatedContent;
    hostsFileOut.close();

    std::wcout << L"Hosts entries have been removed.\n";

    // Flush DNS cache
    FlushDNSCache();
}

// Function to download the hosts file from MVPS
void DownloadHostsFile(const std::wstring& downloadPath) {
    HINTERNET hSession = WinHttpOpen(L"HostsUpdater/1.0",
                                     WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                     WINHTTP_NO_PROXY_NAME,
                                     WINHTTP_NO_PROXY_BYPASS, 0);

    if (!hSession) {
        std::wcerr << L"Failed to open WinHTTP session.\n";
        return;
    }

    URL_COMPONENTS urlComp = { 0 };
    urlComp.dwStructSize = sizeof(urlComp);
    wchar_t hostName[256] = { 0 }, urlPath[1024] = { 0 };
    urlComp.lpszHostName = hostName;
    urlComp.dwHostNameLength = sizeof(hostName) / sizeof(hostName[0]);
    urlComp.lpszUrlPath = urlPath;
    urlComp.dwUrlPathLength = sizeof(urlPath) / sizeof(urlPath[0]);

    if (!WinHttpCrackUrl(HOSTS_URL.c_str(), (DWORD)HOSTS_URL.length(), 0, &urlComp)) {
        std::wcerr << L"Invalid URL.\n";
        WinHttpCloseHandle(hSession);
        return;
    }

    HINTERNET hConnect = WinHttpConnect(hSession, urlComp.lpszHostName, urlComp.nPort, 0);
    if (!hConnect) {
        std::wcerr << L"Failed to connect to host.\n";
        WinHttpCloseHandle(hSession);
        return;
    }

    DWORD flags = (urlComp.nScheme == INTERNET_SCHEME_HTTPS) ? WINHTTP_FLAG_SECURE : 0;

    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", urlComp.lpszUrlPath,
                                            NULL, WINHTTP_NO_REFERER,
                                            WINHTTP_DEFAULT_ACCEPT_TYPES,
                                            flags);

    if (!hRequest) {
        std::wcerr << L"Failed to open HTTP request.\n";
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return;
    }

    if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                            WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) {
        std::wcerr << L"Failed to send HTTP request.\n";
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return;
    }

    if (!WinHttpReceiveResponse(hRequest, NULL)) {
        std::wcerr << L"Failed to receive HTTP response.\n";
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return;
    }

    std::wofstream outFile(downloadPath, std::ios::trunc);
    if (!outFile.is_open()) {
        std::wcerr << L"Failed to open output file for writing.\n";
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return;
    }

    DWORD bytesAvailable = 0;
    DWORD bytesRead = 0;
    char buffer[8192];

    do {
        if (!WinHttpQueryDataAvailable(hRequest, &bytesAvailable)) {
            std::wcerr << L"Error querying data availability.\n";
            break;
        }

        if (bytesAvailable == 0)
            break;

        if (bytesAvailable > sizeof(buffer))
            bytesAvailable = sizeof(buffer);

        if (!WinHttpReadData(hRequest, buffer, bytesAvailable, &bytesRead)) {
            std::wcerr << L"Error reading data.\n";
            break;
        }

        if (bytesRead == 0)
            break;

        std::string data(buffer, bytesRead);
        outFile << std::wstring(data.begin(), data.end());
    } while (bytesRead > 0);

    outFile.close();
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    std::wcout << L"Downloaded hosts file to: " << downloadPath << L"\n";
}

// Function to get the current timestamp
std::wstring GetTimestamp() {
    std::time_t now = std::time(nullptr);
    std::tm localTime;
    localtime_s(&localTime, &now);
    wchar_t buffer[20];
    wcsftime(buffer, sizeof(buffer), L"%Y%m%d%H%M%S", &localTime);
    return buffer;
}

// Function to flush the DNS cache
void FlushDNSCache() {
    std::wcout << L"Flushing DNS cache...\n";
    system("ipconfig /flushdns");
    std::wcout << L"DNS cache flushed.\n";
}
