#pragma once

enum t_sci_editor_style_flag
{
	ESF_NONE = 0,
	ESF_FONT = 1 << 0,
	ESF_SIZE = 1 << 1,
	ESF_FORE = 1 << 2,
	ESF_BACK = 1 << 3,
	ESF_BOLD = 1 << 4,
	ESF_ITALICS = 1 << 5,
	ESF_UNDERLINED = 1 << 6,
	ESF_CASEFORCE = 1 << 7,
};

struct t_sci_editor_style
{
	t_sci_editor_style()
	{
		flags = 0;
	}

	unsigned flags;
	bool italics, bold, underlined;
	pfc::string_simple font;
	unsigned size;
	DWORD fore, back;
	int case_force;
};

struct t_sci_prop_set
{
	pfc::string_simple key, defaultval, val;
};

struct t_prop_set_init_table
{
	const char* key;
	const char* defaultval;
};

typedef pfc::list_t<t_sci_prop_set> t_sci_prop_set_list;

class cfg_sci_prop_sets
{
private:
	t_sci_prop_set_list m_data;

	void init_data(const t_prop_set_init_table* p_default);

public:
	explicit inline cfg_sci_prop_sets(const t_prop_set_init_table* p_default)
	{
		init_data(p_default);
	}

	inline t_sci_prop_set_list& val()
	{
		return m_data;
	}
	inline const t_sci_prop_set_list& val() const
	{
		return m_data;
	}
};

extern cfg_sci_prop_sets g_sci_prop_sets;
