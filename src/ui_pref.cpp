#include "stdafx.h"
#include "ui_name_value_edit.h"
#include "ui_pref.h"
#include "scintilla_prop_sets.h"

BOOL CDialogPref::OnInitDialog(HWND hwndFocus, LPARAM lParam)
{
	DoDataExchange();

	SetWindowTheme(m_props.m_hWnd, L"explorer", NULL);

	m_props.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
	m_props.AddColumn(_T("Name"), 0);
	m_props.SetColumnWidth(0, 150);
	m_props.AddColumn(_T("Value"), 1);
	m_props.SetColumnWidth(1, 310);
	LoadProps();

	return TRUE; // set focus to default control
}

HWND CDialogPref::get_wnd()
{
	return m_hWnd;
}

LRESULT CDialogPref::OnPropNMDblClk(LPNMHDR pnmh)
{
	//for ListView - (LPNMITEMACTIVATE)pnmh
	//for StatusBar	- (LPNMMOUSE)pnmh
	LPNMITEMACTIVATE pniv = (LPNMITEMACTIVATE)pnmh;

	if (pniv->iItem >= 0)
	{
		t_sci_prop_set_list& prop_sets = g_sci_prop_sets.val();
		pfc::string8 key, val;

		uGetItemText(pniv->iItem, 0, key);
		uGetItemText(pniv->iItem, 1, val);

		modal_dialog_scope scope;
		if (scope.can_create())
		{
			scope.initialize(m_hWnd);
			CNameValueEdit dlg(key, val);

			if (dlg.DoModal(m_hWnd) == IDOK)
			{
				dlg.GetValue(val);

				for (t_size i = 0; i < prop_sets.get_count(); ++i)
				{
					if (strcmp(prop_sets[i].key, key) == 0)
					{
						prop_sets[i].val = val;
						break;
					}
				}

				m_props.SetItemText(pniv->iItem, 1, pfc::stringcvt::string_wide_from_utf8_fast(val));
				DoDataExchange();
			}
		}
	}

	return 0;
}

t_uint32 CDialogPref::get_state()
{
	return preferences_state::resettable;
}

void CDialogPref::LoadProps(bool reset)
{
	if (reset)
	{
		g_sci_prop_sets.reset();
	}

	pfc::stringcvt::string_wide_from_utf8_fast conv;
	t_sci_prop_set_list& prop_sets = g_sci_prop_sets.val();

	m_props.DeleteAllItems();

	for (t_size i = 0; i < prop_sets.get_count(); ++i)
	{
		conv.convert(prop_sets[i].key);
		m_props.AddItem(i, 0, conv);

		conv.convert(prop_sets[i].val);
		m_props.AddItem(i, 1, conv);
	}

	OnChanged();
}

void CDialogPref::OnButtonExportBnClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl)
{
	pfc::string8_fast filename;
	if (uGetOpenFileName(m_hWnd, "Configuration files|*.cfg", 0, "cfg", "Save as", NULL, filename, TRUE))
	{
		g_sci_prop_sets.export_to_file(filename);
	}
}

void CDialogPref::OnButtonImportBnClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl)
{
	pfc::string8_fast filename;
	if (uGetOpenFileName(m_hWnd, "Configuration files|*.cfg|All files|*.*", 0, "cfg", "Import from", NULL, filename, FALSE))
	{
		g_sci_prop_sets.import_from_file(filename);
	}

	LoadProps();
}

void CDialogPref::OnChanged()
{
	m_callback->on_state_changed();
}

void CDialogPref::OnEditChange(WORD, WORD, HWND)
{
	OnChanged();
}

void CDialogPref::uGetItemText(int nItem, int nSubItem, pfc::string_base& out)
{
	enum
	{
		BUFFER_LEN = 1024
	};
	TCHAR buffer[BUFFER_LEN];

	m_props.GetItemText(nItem, nSubItem, buffer, BUFFER_LEN);
	out.set_string(pfc::stringcvt::string_utf8_from_wide(buffer));
}

void CDialogPref::apply()
{
	OnChanged();
}

void CDialogPref::reset()
{
	LoadProps(true);
}

namespace
{
	preferences_page_factory_t<js_preferences_page_impl> g_pref;
}
