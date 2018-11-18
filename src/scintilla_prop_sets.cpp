#include "stdafx.h"
#include "helpers.h"
#include "scintilla_prop_sets.h"

const t_prop_set_init_table prop_sets_init_table[] =
{
	{"style.default", "font:Courier New,size:10"},
	{"style.comment", "fore:#008000"},
	{"style.keyword", "bold,fore:#0000ff"},
	{"style.indentifier", "$(style.default)"},
	{"style.string", "fore:#ff0000"},
	{"style.number", "fore:#ff0000"},
	{"style.operator", "$(style.default)"},
	{"style.linenumber", "font:Courier New,size:8,fore:#2b91af"},
	{"style.bracelight", "bold,fore:#000000,back:#ffee62"},
	{"style.bracebad", "bold,fore:#ff0000"},
	{"style.selection.fore", ""},
	{"style.selection.back", ""},
	{"style.selection.alpha", "256"}, // 256 - SC_ALPHA_NOALPHA
	{"style.caret.fore", ""},
	{"style.caret.width", "1"},
	{"style.caret.line.back", ""},
	{"style.caret.line.back.alpha", "256"},
	{0, 0},
};

cfg_sci_prop_sets g_sci_prop_sets(prop_sets_init_table);

void cfg_sci_prop_sets::init_data(const t_prop_set_init_table* p_default)
{
	t_sci_prop_set temp;

	m_data.remove_all();

	for (int i = 0; p_default[i].key; ++i)
	{
		temp.key = p_default[i].key;
		temp.defaultval = p_default[i].defaultval;
		temp.val = temp.defaultval;
		m_data.add_item(temp);
	}
}
