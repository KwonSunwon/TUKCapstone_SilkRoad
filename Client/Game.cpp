#include "pch.h"
#include "Game.h"
#include "Engine.h"
#include "SceneManager.h"
#include "Network.h"
#include "Resource.h"

#pragma region Dialog
bool CALLBACK DialogProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch(iMessage) {
	case WM_INITDIALOG:
		OutputDebugString(L"DialogProc\n");
		return true;
	case WM_COMMAND:
		switch(LOWORD(wParam)) {
		case IDOK:
		{
			char* serverIP = new char(16);
			GetDlgItemTextA(hDlg, IDC_IPADDRESS1, serverIP, 16);
			GET_SINGLE(NetworkManager)->m_hostIP = make_shared<char[]>(16);
			memcpy(GET_SINGLE(NetworkManager)->m_hostIP.get(), serverIP, 16);
			EndDialog(hDlg, IDOK);
			SetEvent(GET_SINGLE(NetworkManager)->m_eventHandle);
			return true;
		}
		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			return true;
		}
		break;
	}
	return false;
}
#pragma endregion

void Game::Init(const WindowInfo& info)
{
	GEngine->Init(info);

	GET_SINGLE(SceneManager)->LoadScene(L"TestScene");

	auto dialog = CreateDialog(NULL, MAKEINTRESOURCE(IDD_DIALOG1), NULL, reinterpret_cast<DLGPROC>(DialogProc));
	GET_SINGLE(NetworkManager)->m_dialog = dialog;
}

void Game::Update()
{
	GEngine->Update();
}
