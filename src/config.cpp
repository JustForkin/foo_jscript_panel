#include "stdafx.h"
#include "config.h"
#include "resource.h"

bool prop_kv_config::get_config_item(const char* p_key, VARIANT& p_out)
{
	_variant_t val;

	if (m_map.query(p_key, val))
	{
		if (g_is_allowed_type(val.vt) && SUCCEEDED(VariantCopy(&p_out, &val)))
		{
			return true;
		}
		else
		{
			m_map.remove(p_key);
		}
	}

	return false;
}

bool prop_kv_config::g_is_allowed_type(VARTYPE p_vt)
{
	switch (p_vt)
	{
	case VT_UI1:
	case VT_I1:
	case VT_I2:
	case VT_UI2:
	case VT_BOOL:
	case VT_I4:
	case VT_UI4:
	case VT_R4:
	case VT_INT:
	case VT_UINT:
	case VT_I8:
	case VT_UI8:
	case VT_R8:
	case VT_CY:
	case VT_DATE:
	case VT_BSTR:
		return true;
	}

	return false;
}

void prop_kv_config::g_load(t_map& data, stream_reader* reader, abort_callback& abort) throw()
{
	t_size count;

	data.remove_all();

	try
	{
		// Get count
		reader->read_lendian_t(count, abort);

		for (t_size i = 0; i < count; ++i)
		{
			pfc::string8_fast key;
			_variant_t val;
			VARTYPE vt;
			int cbRead = 0;

			// read key
			reader->read_string(key, abort);
			// read vtype
			reader->read_lendian_t(vt, abort);

			switch (vt)
			{
			case VT_UI1:
			case VT_I1:
				cbRead = sizeof(BYTE);
				break;

			case VT_I2:
			case VT_UI2:
			case VT_BOOL:
				cbRead = sizeof(short);
				break;

			case VT_I4:
			case VT_UI4:
			case VT_R4:
			case VT_INT:
			case VT_UINT:
				cbRead = sizeof(LONG);
				break;

			case VT_I8:
			case VT_UI8:
				cbRead = sizeof(LONGLONG);
				break;

			case VT_R8:
			case VT_CY:
			case VT_DATE:
				cbRead = sizeof(double);
				break;
			}

			val.vt = vt;

			if (cbRead != 0)
			{
				reader->read(&val.bVal, cbRead, abort);
			}
			else
			{
				// Read to bstr
				pfc::string8_fast str;

				reader->read_string(str, abort);
				val.bstrVal = SysAllocString(pfc::stringcvt::string_wide_from_utf8_fast(str));
			}

			data[key] = val;
		}
	}
	catch (...) {}
}

void prop_kv_config::g_save(const t_map& data, stream_writer* writer, abort_callback& abort) throw()
{
	try
	{
		// Write count
		writer->write_lendian_t(data.get_count(), abort);

		for (t_map::const_iterator iter = data.first(); iter.is_valid(); ++iter)
		{
			// Write key
			writer->write_string(iter->m_key, abort);
			// Write vt
			writer->write_lendian_t(iter->m_value.vt, abort);
			// Write value
			int cbWrite = 0;

			switch (iter->m_value.vt)
			{
			case VT_UI1:
			case VT_I1:
				cbWrite = sizeof(BYTE);
				break;

			case VT_I2:
			case VT_UI2:
			case VT_BOOL:
				cbWrite = sizeof(short);
				break;

			case VT_I4:
			case VT_UI4:
			case VT_R4:
			case VT_INT:
			case VT_UINT:
				cbWrite = sizeof(LONG);
				break;

			case VT_I8:
			case VT_UI8:
				cbWrite = sizeof(LONGLONG);
				break;

			case VT_R8:
			case VT_CY:
			case VT_DATE:
				cbWrite = sizeof(double);
				break;
			}

			if (cbWrite != 0)
			{
				writer->write(&iter->m_value.bVal, cbWrite, abort);
			}
			else if (iter->m_value.vt == VT_BSTR)
			{
				pfc::stringcvt::string_utf8_from_wide conv = iter->m_value.bstrVal;
				writer->write_string(conv, abort);
			}
		}
	}
	catch (...) {}
}

void prop_kv_config::load(stream_reader* reader, abort_callback& abort) throw()
{
	g_load(m_map, reader, abort);
}

void prop_kv_config::save(stream_writer* writer, abort_callback& abort) const throw()
{
	g_save(m_map, writer, abort);
}

void prop_kv_config::set_config_item(const char* p_key, const VARIANT& p_val)
{
	if (g_is_allowed_type(p_val.vt))
	{
		m_map[p_key] = p_val;
	}
	else
	{
		m_map.remove(p_key);
	}
}

GUID& js_panel_vars::get_config_guid()
{
	return m_config_guid;
}

WINDOWPLACEMENT& js_panel_vars::get_windowplacement()
{
	return m_wndpl;
}

bool& js_panel_vars::get_pseudo_transparent()
{
	return m_pseudo_transparent;
}

pfc::string_base& js_panel_vars::get_script_code()
{
	return m_script_code;
}

prop_kv_config& js_panel_vars::get_config_prop()
{
	return m_config_prop;
}

void js_panel_vars::get_default_script_code(pfc::string_base& out)
{
	out.reset();
	puResource pures = uLoadResource(core_api::get_my_instance(), uMAKEINTRESOURCE(IDR_SCRIPT), "SCRIPT");

	if (pures)
	{
		out.set_string(static_cast<const char*>(pures->GetPointer()), pures->GetSize());
	}
}

void js_panel_vars::load_config(stream_reader* reader, t_size size, abort_callback& abort)
{
	reset_config();

	if (size > 0)
	{
		try
		{
			reader->read_object_t(m_config_guid, abort);
			m_config_prop.load(reader, abort);
			reader->read_object(&m_wndpl, sizeof(m_wndpl), abort);
			reader->read_string(m_script_code, abort);
			reader->read_object_t(m_pseudo_transparent, abort);
		}
		catch (...)
		{
			reset_config();
			FB2K_console_formatter() << "Error: " JSP_NAME_VERSION " Configuration has been corrupted. All settings have been reset.";
		}
	}
}

void js_panel_vars::reset_config()
{
	get_default_script_code(m_script_code);
	m_pseudo_transparent = false;
	m_wndpl.length = 0;
	CoCreateGuid(&m_config_guid);
}

void js_panel_vars::save_config(stream_writer* writer, abort_callback& abort) const
{
	try
	{
		writer->write_object_t(m_config_guid, abort);
		m_config_prop.save(writer, abort);
		writer->write_object(&m_wndpl, sizeof(m_wndpl), abort);
		writer->write_string(m_script_code, abort);
		writer->write_object_t(m_pseudo_transparent, abort);
	}
	catch (...) {}
}
