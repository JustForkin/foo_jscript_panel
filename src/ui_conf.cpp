#include "stdafx.h"
#include "js_panel_window.h"
#include "ui_conf.h"
#include "ui_find.h"
#include "ui_goto.h"
#include "ui_replace.h"

LRESULT CDialogConf::OnCloseCmd(WORD wNotifyCode, WORD wID, HWND hWndCtl)
{
	switch (wID)
	{
	case IDOK:
		Apply();
		EndDialog(IDOK);
		break;

	case IDAPPLY:
		Apply();
		break;

	case IDCANCEL:
		if (m_editorctrl.GetModify())
		{
			int ret = uMessageBox(m_hWnd, "Do you want to apply your changes?", m_caption, MB_ICONWARNING | MB_SETFOREGROUND | MB_YESNOCANCEL);

			switch (ret)
			{
			case IDYES:
				Apply();
				EndDialog(IDOK);
				break;

			case IDCANCEL:
				return 0;
			}
		}

		EndDialog(IDCANCEL);
	}

	return 0;
}

LRESULT CDialogConf::OnInitDialog(HWND hwndFocus, LPARAM lParam)
{
	// Get caption text
	uGetWindowText(m_hWnd, m_caption);

	// Init resize
	DlgResize_Init();

	// Apply window placement
	if (m_parent->get_windowplacement().length == 0)
	{
		m_parent->get_windowplacement().length = sizeof(WINDOWPLACEMENT);

		if (!GetWindowPlacement(&m_parent->get_windowplacement()))
		{
			memset(&m_parent->get_windowplacement(), 0, sizeof(WINDOWPLACEMENT));
		}
	}
	else
	{
		SetWindowPlacement(&m_parent->get_windowplacement());
	}

	// GUID Text
	pfc::string8 guid_text = "GUID: ";
	guid_text += pfc::print_guid(m_parent->get_config_guid());
	uSetWindowText(GetDlgItem(IDC_STATIC_GUID), guid_text);

	// Edit Control
	m_editorctrl.SubclassWindow(GetDlgItem(IDC_EDIT));
	m_editorctrl.SetJScript();
	m_editorctrl.ReadAPI();
	m_editorctrl.SetContent(m_parent->get_script_code(), true);
	m_editorctrl.SetSavePoint();

	// Pseudo Transparent
	if (m_parent->GetInstanceType() == HostComm::KInstanceTypeCUI)
	{
		uButton_SetCheck(m_hWnd, IDC_CHECK_PSEUDO_TRANSPARENT, m_parent->get_pseudo_transparent());
	}
	else
	{
		uButton_SetCheck(m_hWnd, IDC_CHECK_PSEUDO_TRANSPARENT, false);
		GetDlgItem(IDC_CHECK_PSEUDO_TRANSPARENT).EnableWindow(false);
		GetDlgItem(IDC_CHECK_PSEUDO_TRANSPARENT).ShowWindow(false);
	}

	return TRUE; // set focus to default control
}

LRESULT CDialogConf::OnNotify(int idCtrl, LPNMHDR pnmh)
{
	pfc::string8 caption = m_caption;

	switch (pnmh->code)
	{
	case SCN_SAVEPOINTLEFT: // dirty
		caption += " *";
		uSetWindowText(m_hWnd, caption);
		break;
	case SCN_SAVEPOINTREACHED: // not dirty
		uSetWindowText(m_hWnd, caption);
		break;
	}

	SetMsgHandled(FALSE);
	return 0;
}

LRESULT CDialogConf::OnTools(WORD wNotifyCode, WORD wID, HWND hWndCtl)
{
	enum
	{
		kImport = 1,
		kExport,
		kResetDefault,
		kResetCurrent,
	};

	HMENU menu = CreatePopupMenu();
	AppendMenu(menu, MF_STRING, kImport, _T("&Import"));
	AppendMenu(menu, MF_STRING, kExport, _T("E&xport"));
	AppendMenu(menu, MF_SEPARATOR, 0, 0);
	AppendMenu(menu, MF_STRING, kResetDefault, _T("Reset &Default"));
	AppendMenu(menu, MF_STRING, kResetCurrent, _T("Reset &Current"));

	RECT rc = { 0 };
	::GetWindowRect(::GetDlgItem(m_hWnd, IDC_TOOLS), &rc);

	int ret = TrackPopupMenu(menu, TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, rc.left, rc.bottom, 0, m_hWnd, 0);

	switch (ret)
	{
	case kImport:
		OnImport();
		break;

	case kExport:
		OnExport();
		break;

	case kResetDefault:
		OnResetDefault();
		break;

	case kResetCurrent:
		OnResetCurrent();
		break;
	}

	DestroyMenu(menu);
	return 0;
}

LRESULT CDialogConf::OnUwmKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return MatchShortcuts(wParam);
}

LRESULT CDialogConf::OnUwmFindTextChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_lastFlags = wParam;
	m_lastSearchText = reinterpret_cast<const char*>(lParam);
	return 0;
}

bool CDialogConf::MatchShortcuts(unsigned vk)
{
	int modifiers =
		(IsKeyPressed(VK_SHIFT) ? SCMOD_SHIFT : 0) |
		(IsKeyPressed(VK_CONTROL) ? SCMOD_CTRL : 0) |
		(IsKeyPressed(VK_MENU) ? SCMOD_ALT : 0);

	// Hotkeys
	if (modifiers == SCMOD_CTRL)
	{
		switch (vk)
		{
		case 'F':
			OpenFindDialog();
			return true;

		case 'H':
		{
			if (!m_dlgreplace)
			{
				m_dlgreplace = new CDialogReplace(GetDlgItem(IDC_EDIT));

				if (!m_dlgreplace || !m_dlgreplace->Create(m_hWnd))
				{
					break;
				}
			}

			m_dlgreplace->ShowWindow(SW_SHOW);
			m_dlgreplace->SetFocus();
		}
		return true;

		case 'G':
		{
			modal_dialog_scope scope(m_hWnd);
			CDialogGoto dlg(GetDlgItem(IDC_EDIT));
			dlg.DoModal(m_hWnd);
		}
		return true;

		case 'S':
			Apply();
			return true;
		}
	}
	else if (modifiers == 0)
	{
		if (vk == VK_F3)
		{
			// Find next one
			if (!m_lastSearchText.is_empty())
			{
				FindNext(m_hWnd, m_editorctrl.m_hWnd, m_lastFlags, m_lastSearchText);
			}
			else
			{
				OpenFindDialog();
			}
		}
	}
	else if (modifiers == SCMOD_SHIFT)
	{
		if (vk == VK_F3)
		{
			// Find previous one
			if (!m_lastSearchText.is_empty())
			{
				FindPrevious(m_hWnd, m_editorctrl.m_hWnd, m_lastFlags, m_lastSearchText);
			}
			else
			{
				OpenFindDialog();
			}
		}
	}

	return false;
}

bool CDialogConf::FindNext(HWND hWnd, HWND hWndEdit, unsigned flags, const char* which)
{
	::SendMessage(::GetAncestor(hWndEdit, GA_PARENT), UWM_FIND_TEXT_CHANGED, flags, reinterpret_cast<LPARAM>(which));

	SendMessage(hWndEdit, SCI_CHARRIGHT, 0, 0);
	SendMessage(hWndEdit, SCI_SEARCHANCHOR, 0, 0);
	int pos = ::SendMessage(hWndEdit, SCI_SEARCHNEXT, flags, reinterpret_cast<LPARAM>(which));
	return FindResult(hWnd, hWndEdit, pos, which);
}

bool CDialogConf::FindPrevious(HWND hWnd, HWND hWndEdit, unsigned flags, const char* which)
{
	::SendMessage(::GetAncestor(hWndEdit, GA_PARENT), UWM_FIND_TEXT_CHANGED, flags, reinterpret_cast<LPARAM>(which));

	SendMessage(hWndEdit, SCI_SEARCHANCHOR, 0, 0);
	int pos = ::SendMessage(hWndEdit, SCI_SEARCHPREV, flags, reinterpret_cast<LPARAM>(which));
	return FindResult(hWnd, hWndEdit, pos, which);
}

bool CDialogConf::FindResult(HWND hWnd, HWND hWndEdit, int pos, const char* which)
{
	if (pos != -1)
	{
		// Scroll to view
		::SendMessage(hWndEdit, SCI_SCROLLCARET, 0, 0);
		return true;
	}

	pfc::string8 buff = "Cannot find \"";
	buff += which;
	buff += "\"";
	uMessageBox(hWnd, buff.get_ptr(), JSP_NAME, MB_ICONINFORMATION | MB_SETFOREGROUND);
	return false;
}

void CDialogConf::Apply()
{
	pfc::array_t<char> code;
	int len = 0;

	// Get script text
	len = m_editorctrl.GetTextLength();
	code.set_size(len + 1);
	m_editorctrl.GetText(code.get_ptr(), len + 1);

	m_parent->get_pseudo_transparent() = uButton_GetCheck(m_hWnd, IDC_CHECK_PSEUDO_TRANSPARENT);
	m_parent->update_script(code.get_ptr());

	// Wndow position
	GetWindowPlacement(&m_parent->get_windowplacement());

	// Save point
	m_editorctrl.SetSavePoint();
}

void CDialogConf::OnExport()
{
	pfc::string8 filename;

	if (uGetOpenFileName(m_hWnd, "Text files|*.txt|All files|*.*", 0, "txt", "Save as", NULL, filename, TRUE))
	{
		int len = m_editorctrl.GetTextLength();
		pfc::string8_fast text;

		m_editorctrl.GetText(text.lock_buffer(len), len + 1);
		text.unlock_buffer();

		helpers::write_file(filename, text);
	}
}

void CDialogConf::OnImport()
{
	pfc::string8 filename;

	if (uGetOpenFileName(m_hWnd, "Text files|*.txt|JScript files|*.js|All files|*.*", 0, "txt", "Import from", NULL, filename, FALSE))
	{
		pfc::array_t<wchar_t> text;
		helpers::read_file_wide(pfc::stringcvt::string_wide_from_utf8_fast(filename), text);
		m_editorctrl.SetContent(pfc::stringcvt::string_utf8_from_wide(text.get_ptr()));
	}
}

void CDialogConf::OnResetCurrent()
{
	m_editorctrl.SetContent(m_parent->get_script_code());
}

void CDialogConf::OnResetDefault()
{
	pfc::string8 code;
	js_panel_vars::get_default_script_code(code);
	m_editorctrl.SetContent(code);
}

void CDialogConf::OpenFindDialog()
{
	if (!m_dlgfind)
	{
		m_dlgfind = new CDialogFind(GetDlgItem(IDC_EDIT));
		m_dlgfind->Create(m_hWnd);
	}

	m_dlgfind->ShowWindow(SW_SHOW);
	m_dlgfind->SetFocus();
}
