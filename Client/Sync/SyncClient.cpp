// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) websocket4opc@gmail.com. All rights reserved

#include <Windows.h>
#include <winhttp.h>
#include <stdio.h>

VOID receive(HINTERNET hWebSocketHandle, BYTE* pbCurrentBufferPointer) {

    BYTE* pbInitial = pbCurrentBufferPointer;
	DWORD dwBytesTransferred = 0;
    WINHTTP_WEB_SOCKET_BUFFER_TYPE bufferType;
    DWORD dwBufferLength = 1024;;

    do
    {
        DWORD dwError = WinHttpWebSocketReceive(hWebSocketHandle, pbCurrentBufferPointer, dwBufferLength, &dwBytesTransferred, &bufferType);

        if (dwError != ERROR_SUCCESS)
        {
            return;
        }
        
       if (bufferType == WINHTTP_WEB_SOCKET_BINARY_FRAGMENT_BUFFER_TYPE || bufferType == WINHTTP_WEB_SOCKET_UTF8_FRAGMENT_BUFFER_TYPE)
           printf("%c\n", *pbCurrentBufferPointer);

        pbCurrentBufferPointer += dwBytesTransferred;
        dwBufferLength -= dwBytesTransferred;

    } while (bufferType == WINHTTP_WEB_SOCKET_BINARY_FRAGMENT_BUFFER_TYPE || bufferType == WINHTTP_WEB_SOCKET_UTF8_FRAGMENT_BUFFER_TYPE);

	pbCurrentBufferPointer = pbInitial;
}

int __cdecl wmain()
{
	HINTERNET hConnectionHandle = NULL, hRequestHandle = NULL, hWebSocketHandle = NULL;
	BYTE rgbBuffer[1024]{ }, rgbCommand[] = u8"How are you doing/你好吗/Cómo estás/お元気ですか/어떻게 지내세요", rgbClose[] = u8"Bye/再见/Adiós/さよなら/안녕";
    BOOL fStatus = FALSE;
    DWORD dwError = ERROR_SUCCESS;

    SetConsoleOutputCP(CP_UTF8);

    // Create session, connection and request handles.
    HINTERNET hSessionHandle = WinHttpOpen(L"WebSocket sample", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, NULL, NULL, 0);

    if (hSessionHandle == NULL)
    {
        dwError = GetLastError();
        goto quit;
    }

    hConnectionHandle = WinHttpConnect(hSessionHandle, L"localhost", INTERNET_DEFAULT_HTTP_PORT, 0);

    if (hConnectionHandle == NULL)
    {
        dwError = GetLastError();
        goto quit;
    }

    hRequestHandle = WinHttpOpenRequest(hConnectionHandle, L"GET", L"/interactws/main.iws", NULL, NULL, NULL, 0);

    if (hRequestHandle == NULL)
    {
        dwError = GetLastError();
        goto quit;
    }

    // Request protocol upgrade from http to websocket.
#pragma prefast(suppress:6387, "WINHTTP_OPTION_UPGRADE_TO_WEB_SOCKET does not take any arguments.")

    fStatus = WinHttpSetOption(hRequestHandle, WINHTTP_OPTION_UPGRADE_TO_WEB_SOCKET, NULL, 0);

    if (!fStatus)
    {
        dwError = GetLastError();
        goto quit;
    }

    fStatus = WinHttpSendRequest(hRequestHandle, WINHTTP_NO_ADDITIONAL_HEADERS, 0, NULL, 0, 0, 0);

    if (!fStatus)
    {
        dwError = GetLastError();
        goto quit;
    }

    fStatus = WinHttpReceiveResponse(hRequestHandle, 0);
    if (!fStatus)
    {
        dwError = GetLastError();
        goto quit;
    }

    hWebSocketHandle = WinHttpWebSocketCompleteUpgrade(hRequestHandle, NULL);
    if (hWebSocketHandle == NULL)
    {
        dwError = GetLastError();
        goto quit;
    }
    
    // Get connected message from server
    receive(hWebSocketHandle, rgbBuffer);
    
	printf("%s\n", rgbBuffer);
	memset(rgbBuffer, NULL, sizeof rgbBuffer);
    
    printf("\n*****   Set server in Binary mode and use fragmentation with size 1 byte in response   *****\n\n");
    
    // Set server in Binary mode in response
    dwError = WinHttpWebSocketSend(hWebSocketHandle, WINHTTP_WEB_SOCKET_BINARY_MESSAGE_BUFFER_TYPE, (PVOID)"Mode: Binary", (DWORD)strlen("Mode: Binary"));

    if (dwError != ERROR_SUCCESS)
    {
        goto quit;
    }

    // Set server to use fragmentation in response
    dwError = WinHttpWebSocketSend(hWebSocketHandle, WINHTTP_WEB_SOCKET_BINARY_MESSAGE_BUFFER_TYPE, (PVOID)"Fragment: 1", (DWORD)strlen("Fragment: 1"));

    if (dwError != ERROR_SUCCESS)
    {
        goto quit;
    }
    
    Sleep(1000);
    // Send data in Binary mode from client
	dwError = WinHttpWebSocketSend(hWebSocketHandle, WINHTTP_WEB_SOCKET_BINARY_MESSAGE_BUFFER_TYPE, rgbCommand, sizeof rgbCommand);

    if (dwError != ERROR_SUCCESS)
    {
        goto quit;
    }

    receive(hWebSocketHandle, rgbBuffer);

	printf("%s\n", rgbBuffer);
	memset(rgbBuffer, NULL, sizeof rgbBuffer);

    printf("\n*****   Set server in UTF8 mode and use configured fragmentation as before   *****\n\n");
   
    // Set server in UTF8 mode in response
	dwError = WinHttpWebSocketSend(hWebSocketHandle, WINHTTP_WEB_SOCKET_UTF8_MESSAGE_BUFFER_TYPE, (PVOID)"Mode: UTF8", (DWORD)strlen("Mode: UTF8"));

    if (dwError != ERROR_SUCCESS)
    {
        goto quit;
    }
    
    // Send data in Binary mode from client
    dwError = WinHttpWebSocketSend(hWebSocketHandle, WINHTTP_WEB_SOCKET_BINARY_MESSAGE_BUFFER_TYPE, rgbCommand, sizeof rgbCommand);

    if (dwError != ERROR_SUCCESS)
    {
        goto quit;
    }

    receive(hWebSocketHandle, rgbBuffer);

	printf("%s\n", rgbBuffer);
	memset(rgbBuffer, NULL, sizeof rgbBuffer);

    printf("\n*****   Turn off fragmentation and use UTF8 as set before in server response   *****\n\n");
    
    // Turn off fragmentation in server response
    dwError = WinHttpWebSocketSend(hWebSocketHandle, WINHTTP_WEB_SOCKET_BINARY_MESSAGE_BUFFER_TYPE, (PVOID)"Fragment: 0", (DWORD)strlen("Fragment: 0"));

    if (dwError != ERROR_SUCCESS)
    {
        goto quit;
    }
    
    // Send data in UTF8 mode from client
    dwError = WinHttpWebSocketSend(hWebSocketHandle, WINHTTP_WEB_SOCKET_UTF8_MESSAGE_BUFFER_TYPE, rgbCommand, sizeof rgbCommand);

    if (dwError != ERROR_SUCCESS)
    {
        goto quit;
    }

    receive(hWebSocketHandle, rgbBuffer);

	printf("%s\n", rgbBuffer);
	memset(rgbBuffer, NULL, sizeof rgbBuffer);

quit:

    printf("\n*****   Close connection with reason description   *****\n\n");
    
    // Gracefully close the connection.
    WinHttpWebSocketClose(hWebSocketHandle, WINHTTP_WEB_SOCKET_SUCCESS_CLOSE_STATUS, rgbClose, sizeof rgbClose);

    BYTE rgbCloseReasonBuffer[123]{};
    DWORD dwCloseReasonLength = 0;
    USHORT usStatus = 0;

    WinHttpWebSocketQueryCloseStatus(hWebSocketHandle, &usStatus, rgbCloseReasonBuffer, ARRAYSIZE(rgbCloseReasonBuffer), &dwCloseReasonLength);

	printf("The server closed the connection with status code: '%d' and reason: '%s'\n", (int)usStatus, rgbCloseReasonBuffer);

    if (hRequestHandle != NULL)
    {
        WinHttpCloseHandle(hRequestHandle);
        hRequestHandle = NULL;
    }

    if (hWebSocketHandle != NULL)
    {
        WinHttpCloseHandle(hWebSocketHandle);
        hWebSocketHandle = NULL;
    }

    if (hConnectionHandle != NULL)
    {
        WinHttpCloseHandle(hConnectionHandle);
        hConnectionHandle = NULL;
    }

    if (hSessionHandle != NULL)
    {
        WinHttpCloseHandle(hSessionHandle);
        hSessionHandle = NULL;
    }

    return 0;
}