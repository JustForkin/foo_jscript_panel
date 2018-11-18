#pragma once

enum t_version_info
{
	JSP_VERSION_100 = 123, // must start with 123 so we don't break component upgrades
	CONFIG_VERSION_CURRENT = JSP_VERSION_100
};

class prop_kv_config
{
public:
	typedef prop_kv_config t_self;
	typedef pfc::map_t<pfc::string_simple, _variant_t, pfc::comparator_stricmp_ascii> t_map;

	t_map& get_val()
	{
		return m_map;
	}

	bool get_config_item(const char* p_key, VARIANT& p_out);
	static bool g_is_allowed_type(VARTYPE p_vt);
	static void g_load(t_map& data, stream_reader* reader, abort_callback& abort) throw();
	static void g_save(const t_map& data, stream_writer* writer, abort_callback& abort) throw();
	void load(stream_reader* reader, abort_callback& abort) throw();
	void save(stream_writer* writer, abort_callback& abort) const throw();
	void set_config_item(const char* p_key, const VARIANT& p_val);

private:
	t_map m_map;
};

class js_panel_vars
{
public:
	js_panel_vars()
	{
		reset_config();
	}

	GUID& get_config_guid();
	WINDOWPLACEMENT& get_windowplacement();
	bool& get_pseudo_transparent();
	pfc::string_base& get_script_code();
	prop_kv_config& get_config_prop();
	static void get_default_script_code(pfc::string_base& out);
	void load_config(stream_reader* reader, t_size size, abort_callback& abort);
	void reset_config();
	void save_config(stream_writer* writer, abort_callback& abort) const;

private:
	GUID m_config_guid;
	WINDOWPLACEMENT m_wndpl;
	bool m_pseudo_transparent;
	pfc::string8 m_script_code;
	prop_kv_config m_config_prop;
};
