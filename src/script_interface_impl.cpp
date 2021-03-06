#include "stdafx.h"
#include "drop_impl.h"
#include "helpers.h"
#include "host.h"
#include "host_timer_dispatcher.h"
#include "panel_manager.h"
#include "popup_msg.h"
#include "script_interface_impl.h"
#include "stats.h"
#include "ui_input_box.h"

#include <stackblur.h>

using namespace pfc::stringcvt;

ContextMenuManager::ContextMenuManager() {}
ContextMenuManager::~ContextMenuManager() {}
void ContextMenuManager::FinalRelease()
{
	m_cm.release();
}

STDMETHODIMP ContextMenuManager::BuildMenu(IMenuObj* p, int base_id)
{
	if (m_cm.is_empty()) return E_POINTER;

	HMENU menuid;
	p->get__ID(&menuid);
	contextmenu_node* parent = m_cm->get_root();
	m_cm->win32_build_menu(menuid, parent, base_id, -1);
	return S_OK;
}

STDMETHODIMP ContextMenuManager::ExecuteByID(UINT id, VARIANT_BOOL* p)
{
	if (m_cm.is_empty() || !p) return E_POINTER;

	*p = TO_VARIANT_BOOL(m_cm->execute_by_id(id));
	return S_OK;
}

STDMETHODIMP ContextMenuManager::InitContext(IFbMetadbHandleList* handles)
{
	metadb_handle_list* handles_ptr = NULL;
	handles->get__ptr((void**)&handles_ptr);
	contextmenu_manager::g_create(m_cm);
	m_cm->init_context(*handles_ptr, contextmenu_manager::flag_show_shortcuts);
	return S_OK;
}

STDMETHODIMP ContextMenuManager::InitContextPlaylist()
{
	contextmenu_manager::g_create(m_cm);
	m_cm->init_context_playlist(contextmenu_manager::flag_show_shortcuts);
	return S_OK;
}

STDMETHODIMP ContextMenuManager::InitNowPlaying()
{
	contextmenu_manager::g_create(m_cm);
	m_cm->init_context_now_playing(contextmenu_manager::flag_show_shortcuts);
	return S_OK;
}

DropSourceAction::DropSourceAction()
{
	Reset();
}

DropSourceAction::~DropSourceAction() {}
void DropSourceAction::FinalRelease() {}

STDMETHODIMP DropSourceAction::get_Effect(UINT* p)
{
	if (!p) return E_POINTER;

	*p = m_effect;
	return S_OK;
}

STDMETHODIMP DropSourceAction::put_Base(UINT base)
{
	m_base = base;
	return S_OK;
}

STDMETHODIMP DropSourceAction::put_Effect(UINT effect)
{
	m_effect = effect;
	return S_OK;
}

STDMETHODIMP DropSourceAction::put_Playlist(UINT id)
{
	m_playlist_idx = id;
	return S_OK;
}

STDMETHODIMP DropSourceAction::put_ToSelect(VARIANT_BOOL select)
{
	m_to_select = select != VARIANT_FALSE;
	return S_OK;
}

FbFileInfo::FbFileInfo(file_info_impl* p_info_ptr) : m_info_ptr(p_info_ptr) {}
FbFileInfo::~FbFileInfo() {}
void FbFileInfo::FinalRelease()
{
	if (m_info_ptr)
	{
		delete m_info_ptr;
		m_info_ptr = NULL;
	}
}

STDMETHODIMP FbFileInfo::get__ptr(void** pp)
{
	if (!pp) return E_POINTER;

	*pp = m_info_ptr;
	return S_OK;
}

STDMETHODIMP FbFileInfo::InfoFind(BSTR name, int* p)
{
	if (!m_info_ptr || !p) return E_POINTER;

	*p = m_info_ptr->info_find(string_utf8_from_wide(name));
	return S_OK;
}

STDMETHODIMP FbFileInfo::InfoName(UINT idx, BSTR* p)
{
	if (!m_info_ptr || !p) return E_POINTER;

	if (idx < m_info_ptr->info_get_count())
	{
		*p = SysAllocString(string_wide_from_utf8_fast(m_info_ptr->info_enum_name(idx)));
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP FbFileInfo::InfoValue(UINT idx, BSTR* p)
{
	if (!m_info_ptr || !p) return E_POINTER;

	if (idx < m_info_ptr->info_get_count())
	{
		*p = SysAllocString(string_wide_from_utf8_fast(m_info_ptr->info_enum_value(idx)));
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP FbFileInfo::MetaFind(BSTR name, int* p)
{
	if (!m_info_ptr || !p) return E_POINTER;

	*p = m_info_ptr->meta_find(string_utf8_from_wide(name));
	return S_OK;
}

STDMETHODIMP FbFileInfo::MetaName(UINT idx, BSTR* p)
{
	if (!m_info_ptr || !p) return E_POINTER;

	if (idx < m_info_ptr->meta_get_count())
	{
		*p = SysAllocString(string_wide_from_utf8_fast(m_info_ptr->meta_enum_name(idx)));
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP FbFileInfo::MetaValue(UINT idx, UINT vidx, BSTR* p)
{
	if (!m_info_ptr || !p) return E_POINTER;

	if (idx < m_info_ptr->meta_get_count() && vidx < m_info_ptr->meta_enum_value_count(idx))
	{
		*p = SysAllocString(string_wide_from_utf8_fast(m_info_ptr->meta_enum_value(idx, vidx)));
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP FbFileInfo::MetaValueCount(UINT idx, UINT* p)
{
	if (!m_info_ptr || !p) return E_POINTER;

	if (idx < m_info_ptr->meta_get_count())
	{
		*p = m_info_ptr->meta_enum_value_count(idx);
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP FbFileInfo::get_InfoCount(UINT* p)
{
	if (!m_info_ptr || !p) return E_POINTER;

	*p = m_info_ptr->info_get_count();
	return S_OK;
}

STDMETHODIMP FbFileInfo::get_MetaCount(UINT* p)
{
	if (!m_info_ptr || !p) return E_POINTER;

	*p = m_info_ptr->meta_get_count();
	return S_OK;
}

FbMetadbHandle::FbMetadbHandle(const metadb_handle_ptr& src) : m_handle(src) {}
FbMetadbHandle::FbMetadbHandle(metadb_handle* src) : m_handle(src) {}
FbMetadbHandle::~FbMetadbHandle() {}
void FbMetadbHandle::FinalRelease()
{
	m_handle.release();
}

STDMETHODIMP FbMetadbHandle::get__ptr(void** pp)
{
	if (!pp) return E_POINTER;

	*pp = m_handle.get_ptr();
	return S_OK;
}

STDMETHODIMP FbMetadbHandle::ClearStats()
{
	if (m_handle.is_empty()) return E_POINTER;

	metadb_index_hash hash;
	if (stats::g_client->hashHandle(m_handle, hash))
	{
		stats::set(hash, stats::fields());
	}
	return S_OK;
}

STDMETHODIMP FbMetadbHandle::Compare(IFbMetadbHandle* handle, VARIANT_BOOL* p)
{
	if (m_handle.is_empty() || !p) return E_POINTER;

	metadb_handle* ptr = NULL;
	handle->get__ptr((void**)&ptr);
	*p = TO_VARIANT_BOOL(ptr == m_handle.get_ptr());
	return S_OK;
}

STDMETHODIMP FbMetadbHandle::GetAlbumArt(UINT art_id, VARIANT* p)
{
	if (m_handle.is_empty() || !p) return E_POINTER;

	helpers::array helper;
	if (!helper.create(2)) return E_OUTOFMEMORY;

	_variant_t var1, var2;
	var1.vt = VT_DISPATCH;
	var1.pdispVal = NULL;
	var2.vt = VT_BSTR;
	var2.bstrVal = NULL;

	IGdiBitmap* bitmap = NULL;
	pfc::string8_fast image_path;

	if (SUCCEEDED(helpers::get_album_art(m_handle, art_id, &bitmap, &image_path)))
	{
		var1.pdispVal = bitmap;
		var2.bstrVal = SysAllocString(string_wide_from_utf8_fast(image_path));
		if (!helper.put_item(0, var1)) return E_OUTOFMEMORY;
		if (!helper.put_item(1, var2)) return E_OUTOFMEMORY;
	}
	p->vt = VT_ARRAY | VT_VARIANT;
	p->parray = helper.get_ptr();
	return S_OK;
}

STDMETHODIMP FbMetadbHandle::GetAlbumArtEmbedded(UINT art_id, IGdiBitmap** pp)
{
	if (m_handle.is_empty() || !pp) return E_POINTER;

	return helpers::get_album_art_embedded(m_handle, art_id, pp);
}

STDMETHODIMP FbMetadbHandle::GetFileInfo(IFbFileInfo** pp)
{
	if (m_handle.is_empty() || !pp) return E_POINTER;

	file_info_impl* info_ptr = new file_info_impl;
	m_handle->get_info(*info_ptr);
	*pp = new com_object_impl_t<FbFileInfo>(info_ptr);
	return S_OK;
}

STDMETHODIMP FbMetadbHandle::IsInMediaLibrary(VARIANT_BOOL* p)
{
	if (m_handle.is_empty() || !p) return E_POINTER;

	*p = TO_VARIANT_BOOL(library_manager::get()->is_item_in_library(m_handle));
	return S_OK;
}

STDMETHODIMP FbMetadbHandle::RefreshStats()
{
	if (m_handle.is_empty()) return E_POINTER;

	metadb_index_hash hash;
	if (stats::g_client->hashHandle(m_handle, hash))
	{
		stats::theAPI()->dispatch_refresh(g_guid_jsp_metadb_index, hash);
	}
	return S_OK;
}

STDMETHODIMP FbMetadbHandle::RunContextCommand(BSTR command, VARIANT_BOOL* p)
{
	if (m_handle.is_empty() || !p) return E_POINTER;

	*p = TO_VARIANT_BOOL(helpers::execute_context_command_by_name_SEH(string_utf8_from_wide(command), pfc::list_single_ref_t<metadb_handle_ptr>(m_handle)));
	return S_OK;
}

STDMETHODIMP FbMetadbHandle::SetFirstPlayed(BSTR first_played)
{
	if (m_handle.is_empty()) return E_POINTER;

	metadb_index_hash hash;
	if (stats::g_client->hashHandle(m_handle, hash))
	{
		stats::fields tmp = stats::get(hash);
		string_utf8_from_wide fp(first_played);
		if (!tmp.first_played.equals(fp))
		{
			tmp.first_played = fp;
			stats::set(hash, tmp);
		}
	}
	return S_OK;
}

STDMETHODIMP FbMetadbHandle::SetLastPlayed(BSTR last_played)
{
	if (m_handle.is_empty()) return E_POINTER;

	metadb_index_hash hash;
	if (stats::g_client->hashHandle(m_handle, hash))
	{
		stats::fields tmp = stats::get(hash);
		string_utf8_from_wide lp(last_played);
		if (!tmp.last_played.equals(lp))
		{
			tmp.last_played = lp;
			stats::set(hash, tmp);
		}
	}
	return S_OK;
}

STDMETHODIMP FbMetadbHandle::SetLoved(UINT loved)
{
	if (m_handle.is_empty()) return E_POINTER;

	metadb_index_hash hash;
	if (stats::g_client->hashHandle(m_handle, hash))
	{
		stats::fields tmp = stats::get(hash);
		if (tmp.loved != loved)
		{
			tmp.loved = loved;
			stats::set(hash, tmp);
		}
	}
	return S_OK;
}

STDMETHODIMP FbMetadbHandle::SetPlaycount(UINT playcount)
{
	if (m_handle.is_empty()) return E_POINTER;

	metadb_index_hash hash;
	if (stats::g_client->hashHandle(m_handle, hash))
	{
		stats::fields tmp = stats::get(hash);
		if (tmp.playcount != playcount)
		{
			tmp.playcount = playcount;
			stats::set(hash, tmp);
		}
	}
	return S_OK;
}

STDMETHODIMP FbMetadbHandle::SetRating(UINT rating)
{
	if (m_handle.is_empty()) return E_POINTER;

	metadb_index_hash hash;
	if (stats::g_client->hashHandle(m_handle, hash))
	{
		stats::fields tmp = stats::get(hash);
		if (tmp.rating != rating)
		{
			tmp.rating = rating;
			stats::set(hash, tmp);
		}
	}
	return S_OK;
}

STDMETHODIMP FbMetadbHandle::get_FileSize(LONGLONG* p)
{
	if (m_handle.is_empty() || !p) return E_POINTER;

	*p = m_handle->get_filesize();
	return S_OK;
}

STDMETHODIMP FbMetadbHandle::get_Length(double* p)
{
	if (m_handle.is_empty() || !p) return E_POINTER;

	*p = m_handle->get_length();
	return S_OK;
}

STDMETHODIMP FbMetadbHandle::get_Path(BSTR* p)
{
	if (m_handle.is_empty() || !p) return E_POINTER;

	*p = SysAllocString(string_wide_from_utf8_fast(file_path_display(m_handle->get_path())));
	return S_OK;
}

STDMETHODIMP FbMetadbHandle::get_RawPath(BSTR* p)
{
	if (m_handle.is_empty() || !p) return E_POINTER;

	*p = SysAllocString(string_wide_from_utf8_fast(m_handle->get_path()));
	return S_OK;
}

STDMETHODIMP FbMetadbHandle::get_SubSong(UINT* p)
{
	if (m_handle.is_empty() || !p) return E_POINTER;

	*p = m_handle->get_subsong_index();
	return S_OK;
}

FbMetadbHandleList::FbMetadbHandleList(metadb_handle_list_cref handles) : m_handles(handles) {}
FbMetadbHandleList::~FbMetadbHandleList() {}
void FbMetadbHandleList::FinalRelease()
{
	m_handles.remove_all();
}

STDMETHODIMP FbMetadbHandleList::get__ptr(void** pp)
{
	if (!pp) return E_POINTER;

	*pp = &m_handles;
	return S_OK;
}

STDMETHODIMP FbMetadbHandleList::AddItem(IFbMetadbHandle* handle)
{
	metadb_handle* ptr = NULL;
	handle->get__ptr((void**)&ptr);
	m_handles.add_item(ptr);
	return S_OK;
}

STDMETHODIMP FbMetadbHandleList::AddItems(IFbMetadbHandleList* handles)
{
	metadb_handle_list* handles_ptr = NULL;
	handles->get__ptr((void**)&handles_ptr);
	m_handles.add_items(*handles_ptr);
	return S_OK;
}

STDMETHODIMP FbMetadbHandleList::AttachImage(BSTR image_path, UINT art_id)
{
	if (m_handles.get_count() == 0) return E_POINTER;

	GUID what = helpers::convert_artid_to_guid(art_id);
	album_art_data_ptr data;

	try
	{
		string_utf8_from_wide path(image_path);
		if (!filesystem::g_is_remote_or_unrecognized(path))
		{
			file::ptr file;
			abort_callback_dummy abort;
			filesystem::g_open(file, path, filesystem::open_mode_read, abort);
			if (file.is_valid())
			{
				auto tmp = fb2k::service_new<album_art_data_impl>();
				tmp->from_stream(file.get_ptr(), t_size(file->get_size_ex(abort)), abort);
				data = tmp;
			}
		}
	}
	catch (...) {}

	if (data.is_valid())
	{
		auto cb = fb2k::service_new<helpers::embed_thread>(0, data, m_handles, what);
		threaded_process::get()->run_modeless(cb, threaded_process::flag_show_progress | threaded_process::flag_show_delayed | threaded_process::flag_show_item, core_api::get_main_window(), "Embedding image...");
	}
	return S_OK;
}

STDMETHODIMP FbMetadbHandleList::BSearch(IFbMetadbHandle* handle, int* p)
{
	if (!p) return E_POINTER;

	metadb_handle* ptr = NULL;
	handle->get__ptr((void**)&ptr);
	*p = m_handles.bsearch_by_pointer(ptr);
	return S_OK;
}

STDMETHODIMP FbMetadbHandleList::CalcTotalDuration(double* p)
{
	if (!p) return E_POINTER;

	*p = m_handles.calc_total_duration();
	return S_OK;
}

STDMETHODIMP FbMetadbHandleList::CalcTotalSize(LONGLONG* p)
{
	if (!p) return E_POINTER;

	*p = metadb_handle_list_helper::calc_total_size(m_handles, true);
	return S_OK;
}

STDMETHODIMP FbMetadbHandleList::Clone(IFbMetadbHandleList** pp)
{
	if (!pp) return E_POINTER;

	*pp = new com_object_impl_t<FbMetadbHandleList>(m_handles);
	return S_OK;
}

STDMETHODIMP FbMetadbHandleList::Convert(VARIANT* p)
{
	if (!p) return E_POINTER;

	LONG count = m_handles.get_count();
	helpers::array helper;
	if (!helper.create(count)) return E_OUTOFMEMORY;

	for (LONG i = 0; i < count; ++i)
	{
		_variant_t var;
		var.vt = VT_DISPATCH;
		var.pdispVal = new com_object_impl_t<FbMetadbHandle>(m_handles[i]);
		if (!helper.put_item(i, var)) return E_OUTOFMEMORY;
	}
	p->vt = VT_ARRAY | VT_VARIANT;
	p->parray = helper.get_ptr();
	return S_OK;
}

STDMETHODIMP FbMetadbHandleList::CopyToClipboard(VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	*p = VARIANT_FALSE;
	pfc::com_ptr_t<IDataObject> pDO = ole_interaction::get()->create_dataobject(m_handles);
	if (SUCCEEDED(OleSetClipboard(pDO.get_ptr())))
	{
		*p = VARIANT_TRUE;
	}
	return S_OK;
}

STDMETHODIMP FbMetadbHandleList::DoDragDrop(UINT okEffects, UINT* p)
{
	if (!p) return E_POINTER;

	if (m_handles.get_count() == 0 || okEffects == DROPEFFECT_NONE)
	{
		*p = DROPEFFECT_NONE;
		return S_OK;
	}

	pfc::com_ptr_t<IDataObject> pDO = ole_interaction::get()->create_dataobject(m_handles);
	pfc::com_ptr_t<IDropSourceImpl> pIDropSource = new IDropSourceImpl();

	DWORD returnEffect;
	HRESULT hr = SHDoDragDrop(NULL, pDO.get_ptr(), pIDropSource.get_ptr(), okEffects, &returnEffect);

	*p = hr == DRAGDROP_S_CANCEL ? DROPEFFECT_NONE : returnEffect;
	return S_OK;
}

STDMETHODIMP FbMetadbHandleList::Find(IFbMetadbHandle* handle, int* p)
{
	if (!p) return E_POINTER;

	metadb_handle* ptr = NULL;
	handle->get__ptr((void**)&ptr);
	*p = m_handles.find_item(ptr);
	return S_OK;
}

STDMETHODIMP FbMetadbHandleList::GetLibraryRelativePaths(VARIANT* p)
{
	if (!p) return E_POINTER;

	LONG count = m_handles.get_count();
	helpers::array helper;
	if (!helper.create(count)) return E_OUTOFMEMORY;

	pfc::string8_fastalloc temp;
	temp.prealloc(512);

	auto api = library_manager::get();

	for (LONG i = 0; i < count; ++i)
	{
		metadb_handle_ptr item = m_handles[i];
		if (!api->get_relative_path(item, temp)) temp = "";
		_variant_t var;
		var.vt = VT_BSTR;
		var.bstrVal = SysAllocString(string_wide_from_utf8_fast(temp));
		if (!helper.put_item(i, var)) return E_OUTOFMEMORY;
	}
	p->vt = VT_ARRAY | VT_VARIANT;
	p->parray = helper.get_ptr();
	return S_OK;
}

STDMETHODIMP FbMetadbHandleList::GetQueryItems(BSTR query, IFbMetadbHandleList** pp)
{
	if (!pp) return E_POINTER;

	metadb_handle_list dst_list = m_handles;
	search_filter_v2::ptr filter;

	try
	{
		filter = search_filter_manager_v2::get()->create_ex(string_utf8_from_wide(query), fb2k::service_new<completion_notify_dummy>(), search_filter_manager_v2::KFlagSuppressNotify);
	}
	catch (...)
	{
		return E_FAIL;
	}

	pfc::array_t<bool> mask;
	mask.set_size(dst_list.get_count());
	filter->test_multi(dst_list, mask.get_ptr());
	dst_list.filter_mask(mask.get_ptr());
	*pp = new com_object_impl_t<FbMetadbHandleList>(dst_list);
	return S_OK;
}

STDMETHODIMP FbMetadbHandleList::InsertItem(UINT index, IFbMetadbHandle* handle)
{
	metadb_handle* ptr = NULL;
	handle->get__ptr((void**)&ptr);
	m_handles.insert_item(ptr, index);
	return S_OK;
}

STDMETHODIMP FbMetadbHandleList::InsertItems(UINT index, IFbMetadbHandleList* handles)
{
	metadb_handle_list* handles_ptr = NULL;
	handles->get__ptr((void**)&handles_ptr);
	m_handles.insert_items(*handles_ptr, index);
	return S_OK;
}

STDMETHODIMP FbMetadbHandleList::MakeDifference(IFbMetadbHandleList* handles)
{
	metadb_handle_list* handles_ptr = NULL;
	handles->get__ptr((void**)&handles_ptr);

	metadb_handle_list_ref handles_ref = *handles_ptr;
	metadb_handle_list result;
	t_size walk1 = 0;
	t_size walk2 = 0;
	t_size last1 = m_handles.get_count();
	t_size last2 = handles_ptr->get_count();

	while (walk1 != last1 && walk2 != last2)
	{
		if (m_handles[walk1] < handles_ref[walk2])
		{
			result.add_item(m_handles[walk1]);
			++walk1;
		}
		else if (handles_ref[walk2] < m_handles[walk1])
		{
			++walk2;
		}
		else
		{
			++walk1;
			++walk2;
		}
	}

	m_handles = result;
	return S_OK;
}

STDMETHODIMP FbMetadbHandleList::MakeIntersection(IFbMetadbHandleList* handles)
{
	metadb_handle_list* handles_ptr = NULL;
	handles->get__ptr((void**)&handles_ptr);

	metadb_handle_list_ref handles_ref = *handles_ptr;
	metadb_handle_list result;
	t_size walk1 = 0;
	t_size walk2 = 0;
	t_size last1 = m_handles.get_count();
	t_size last2 = handles_ptr->get_count();

	while (walk1 != last1 && walk2 != last2)
	{
		if (m_handles[walk1] < handles_ref[walk2])
			++walk1;
		else if (handles_ref[walk2] < m_handles[walk1])
			++walk2;
		else
		{
			result.add_item(m_handles[walk1]);
			++walk1;
			++walk2;
		}
	}

	m_handles = result;
	return S_OK;
}

STDMETHODIMP FbMetadbHandleList::MakeUnion(IFbMetadbHandleList* handles)
{
	metadb_handle_list* handles_ptr = NULL;
	handles->get__ptr((void**)&handles_ptr);

	m_handles.add_items(*handles_ptr);
	m_handles.sort_by_pointer_remove_duplicates();
	return S_OK;
}

STDMETHODIMP FbMetadbHandleList::RefreshStats()
{
	pfc::avltree_t<metadb_index_hash> tmp;
	for (t_size i = 0; i < m_handles.get_count(); ++i)
	{
		metadb_index_hash hash;
		if (stats::g_client->hashHandle(m_handles[i], hash))
		{
			tmp += hash;
		}
	}
	pfc::list_t<metadb_index_hash> hashes;
	for (auto iter = tmp.first(); iter.is_valid(); ++iter)
	{
		const metadb_index_hash hash = *iter;
		hashes += hash;
	}
	stats::theAPI()->dispatch_refresh(g_guid_jsp_metadb_index, hashes);
	return S_OK;
}

STDMETHODIMP FbMetadbHandleList::Remove(IFbMetadbHandle* handle)
{
	metadb_handle* ptr = NULL;
	handle->get__ptr((void**)&ptr);
	m_handles.remove_item(ptr);
	return S_OK;
}

STDMETHODIMP FbMetadbHandleList::RemoveAll()
{
	m_handles.remove_all();
	return S_OK;
}

STDMETHODIMP FbMetadbHandleList::RemoveAttachedImage(UINT art_id)
{
	if (m_handles.get_count() == 0) return E_POINTER;

	GUID what = helpers::convert_artid_to_guid(art_id);

	auto cb = fb2k::service_new<helpers::embed_thread>(1, album_art_data_ptr(), m_handles, what);
	threaded_process::get()->run_modeless(cb, threaded_process::flag_show_progress | threaded_process::flag_show_delayed | threaded_process::flag_show_item, core_api::get_main_window(), "Removing images...");
	return S_OK;
}

STDMETHODIMP FbMetadbHandleList::RemoveAttachedImages()
{
	if (m_handles.get_count() == 0) return E_POINTER;

	auto cb = fb2k::service_new<helpers::embed_thread>(2, album_art_data_ptr(), m_handles, pfc::guid_null);
	threaded_process::get()->run_modeless(cb, threaded_process::flag_show_progress | threaded_process::flag_show_delayed | threaded_process::flag_show_item, core_api::get_main_window(), "Removing images...");
	return S_OK;
}

STDMETHODIMP FbMetadbHandleList::RemoveByIdx(UINT index)
{
	if (index < m_handles.get_count())
	{
		m_handles.remove_by_idx(index);
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP FbMetadbHandleList::RemoveFromIdx(UINT from, UINT count)
{
	m_handles.remove_from_idx(from, count);
	return S_OK;
}

STDMETHODIMP FbMetadbHandleList::RunContextCommand(BSTR command, VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	*p = TO_VARIANT_BOOL(helpers::execute_context_command_by_name_SEH(string_utf8_from_wide(command), m_handles));
	return S_OK;
}

STDMETHODIMP FbMetadbHandleList::Sort()
{
	m_handles.sort_by_pointer_remove_duplicates();
	return S_OK;
}

STDMETHODIMP FbMetadbHandleList::SortByFormat(BSTR pattern, int direction)
{
	titleformat_object::ptr obj;
	titleformat_compiler::get()->compile_safe(obj, string_utf8_from_wide(pattern));
	m_handles.sort_by_format(obj, NULL, direction);
	return S_OK;
}

STDMETHODIMP FbMetadbHandleList::SortByPath()
{
	m_handles.sort_by_path();
	return S_OK;
}

STDMETHODIMP FbMetadbHandleList::SortByRelativePath()
{
	// lifted from metadb_handle_list.cpp - adds subsong index for better sorting. github issue #16
	auto api = library_manager::get();
	t_size i, count = m_handles.get_count();

	pfc::array_t<helpers::custom_sort_data> data;
	data.set_size(count);

	pfc::string8_fastalloc temp;
	temp.prealloc(512);

	for (i = 0; i < count; ++i)
	{
		metadb_handle_ptr item = m_handles[i];
		if (!api->get_relative_path(item, temp)) temp = "";
		temp << item->get_subsong_index();
		data[i].index = i;
		data[i].text = helpers::make_sort_string(temp);
	}

	pfc::sort_t(data, helpers::custom_sort_compare<1>, count);
	order_helper order(count);

	for (i = 0; i < count; ++i)
	{
		order[i] = data[i].index;
		delete[] data[i].text;
	}

	m_handles.reorder(order.get_ptr());
	return S_OK;
}

STDMETHODIMP FbMetadbHandleList::UpdateFileInfo(BSTR str)
{
	t_size count = m_handles.get_count();
	if (count == 0) return E_POINTER;

	bool is_array;
	json j;

	try
	{
		j = json::parse(string_utf8_from_wide(str).get_ptr());
	}
	catch (...)
	{
		return E_INVALIDARG;
	}

	if (j.is_array() && j.size() == count)
	{
		is_array = true;
	}
	else if (j.is_object() && j.size() > 0)
	{
		is_array = false;
	}
	else
	{
		return E_INVALIDARG;
	}

	pfc::list_t<file_info_impl> info;
	info.set_size(count);

	for (t_size i = 0; i < count; ++i)
	{
		json obj = is_array ? j[i] : j;
		if (!obj.is_object() || obj.size() == 0) return E_INVALIDARG;

		metadb_handle_ptr item = m_handles[i];
		item->get_info(info[i]);

		for (json::iterator it = obj.begin(); it != obj.end(); ++it)
		{
			pfc::string8_fast key = (it.key()).c_str();
			if (key.is_empty()) return E_INVALIDARG;

			info[i].meta_remove_field(key);

			if (it.value().is_array())
			{
				for (json::iterator ita = it.value().begin(); ita != it.value().end(); ++ita)
				{
					pfc::string8_fast value = helpers::iterator_to_string8(ita);
					if (!value.is_empty())
					{
						info[i].meta_add(key, value);
					}
				}
			}
			else
			{
				pfc::string8_fast value = helpers::iterator_to_string8(it);
				if (!value.is_empty())
				{
					info[i].meta_set(key, value);
				}
			}
		}
	}

	metadb_io_v2::get()->update_info_async_simple(
		m_handles,
		pfc::ptr_list_const_array_t<const file_info, file_info_impl *>(info.get_ptr(), info.get_count()),
		core_api::get_main_window(),
		metadb_io_v2::op_flag_delay_ui,
		NULL
	);

	return S_OK;
}

STDMETHODIMP FbMetadbHandleList::get_Count(UINT* p)
{
	if (!p) return E_POINTER;

	*p = m_handles.get_count();
	return S_OK;
}

STDMETHODIMP FbMetadbHandleList::get_Item(UINT index, IFbMetadbHandle** pp)
{
	if (!pp) return E_POINTER;

	if (index < m_handles.get_count())
	{
		*pp = new com_object_impl_t<FbMetadbHandle>(m_handles.get_item_ref(index));
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP FbMetadbHandleList::put_Item(UINT index, IFbMetadbHandle* handle)
{
	if (index < m_handles.get_count())
	{
		metadb_handle* ptr = NULL;
		handle->get__ptr((void**)&ptr);
		m_handles.replace_item(index, ptr);
		return S_OK;
	}
	return E_INVALIDARG;
}

FbPlayingItemLocation::FbPlayingItemLocation(bool isValid, t_size playlistIndex, t_size playlistItemIndex) : m_isValid(isValid), m_playlistIndex(playlistIndex), m_playlistItemIndex(playlistItemIndex) {}

STDMETHODIMP FbPlayingItemLocation::get_IsValid(VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	*p = TO_VARIANT_BOOL(m_isValid);
	return S_OK;
}

STDMETHODIMP FbPlayingItemLocation::get_PlaylistIndex(int* p)
{
	if (!p) return E_POINTER;

	*p = m_playlistIndex;
	return S_OK;
}

STDMETHODIMP FbPlayingItemLocation::get_PlaylistItemIndex(int* p)
{
	if (!p) return E_POINTER;

	*p = m_playlistItemIndex;
	return S_OK;
}

FbPlaylistManager::FbPlaylistManager() {}
FbPlaylistManager::~FbPlaylistManager() {}

STDMETHODIMP FbPlaylistManager::AddLocations(UINT playlistIndex, VARIANT locations, VARIANT_BOOL select)
{
	auto api = playlist_manager::get();

	if (playlistIndex < api->get_playlist_count() && !api->playlist_lock_is_present(playlistIndex))
	{
		helpers::array helper;
		if (!helper.convert(&locations)) return E_INVALIDARG;
		LONG count = helper.get_count();
		pfc::string_list_impl list;
		for (LONG i = 0; i < count; ++i)
		{
			_variant_t var;
			if (!helper.get_item(i, var, VT_BSTR)) return E_INVALIDARG;
			list.add_item(string_utf8_from_wide(var.bstrVal));
		}

		playlist_incoming_item_filter_v2::get()->process_locations_async(
			list,
			playlist_incoming_item_filter_v2::op_flag_no_filter | playlist_incoming_item_filter_v2::op_flag_delay_ui,
			NULL,
			NULL,
			NULL,
			fb2k::service_new<helpers::js_process_locations>(select != VARIANT_FALSE, api->playlist_get_item_count(playlistIndex), playlistIndex));

		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP FbPlaylistManager::ClearPlaylist(UINT playlistIndex)
{
	playlist_manager::get()->playlist_clear(playlistIndex);
	return S_OK;
}

STDMETHODIMP FbPlaylistManager::ClearPlaylistSelection(UINT playlistIndex)
{
	playlist_manager::get()->playlist_clear_selection(playlistIndex);
	return S_OK;
}

STDMETHODIMP FbPlaylistManager::CreateAutoPlaylist(UINT playlistIndex, BSTR name, BSTR query, BSTR sort, UINT flags, int* p)
{
	if (!p) return E_POINTER;

	int pos;
	CreatePlaylist(playlistIndex, name, &pos);
	if (pos == pfc_infinite)
	{
		*p = pos;
	}
	else
	{
		try
		{
			autoplaylist_manager::get()->add_client_simple(string_utf8_from_wide(query), string_utf8_from_wide(sort), pos, flags);
			*p = pos;
		}
		catch (...)
		{
			playlist_manager::get()->remove_playlist(pos);
			*p = pfc_infinite;
		}
	}
	return S_OK;
}

STDMETHODIMP FbPlaylistManager::CreatePlaylist(UINT playlistIndex, BSTR name, int* p)
{
	if (!p) return E_POINTER;

	auto api = playlist_manager::get();
	string_utf8_from_wide uname(name);

	if (uname.is_empty())
	{
		*p = api->create_playlist_autoname(playlistIndex);
	}
	else
	{
		*p = api->create_playlist(uname, uname.length(), playlistIndex);
	}
	return S_OK;
}

STDMETHODIMP FbPlaylistManager::DuplicatePlaylist(UINT from, BSTR name, UINT* p)
{
	if (!p) return E_POINTER;

	auto api = playlist_manager_v4::get();

	if (from < api->get_playlist_count())
	{
		metadb_handle_list contents;
		api->playlist_get_all_items(from, contents);

		pfc::string8_fast uname = string_utf8_from_wide(name);
		if (uname.is_empty())
		{
			api->playlist_get_name(from, uname);
		}

		stream_reader_dummy dummy_reader;
		abort_callback_dummy abort;
		*p = api->create_playlist_ex(uname.get_ptr(), uname.get_length(), from + 1, contents, &dummy_reader, abort);
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP FbPlaylistManager::ExecutePlaylistDefaultAction(UINT playlistIndex, UINT playlistItemIndex, VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	*p = TO_VARIANT_BOOL(playlist_manager::get()->playlist_execute_default_action(playlistIndex, playlistItemIndex));
	return S_OK;
}

STDMETHODIMP FbPlaylistManager::FindOrCreatePlaylist(BSTR name, VARIANT_BOOL unlocked, int* p)
{
	if (!p) return E_POINTER;

	auto api = playlist_manager::get();
	string_utf8_from_wide uname(name);

	if (unlocked != VARIANT_FALSE)
	{
		*p = api->find_or_create_playlist_unlocked(uname);
	}
	else
	{
		*p = api->find_or_create_playlist(uname);
	}
	return S_OK;
}

STDMETHODIMP FbPlaylistManager::FindPlaylist(BSTR name, int* p)
{
	if (!p) return E_POINTER;

	*p = playlist_manager::get()->find_playlist(string_utf8_from_wide(name));
	return S_OK;
}

STDMETHODIMP FbPlaylistManager::GetPlayingItemLocation(IFbPlayingItemLocation** pp)
{
	if (!pp) return E_POINTER;

	t_size playlistIndex;
	t_size playlistItemIndex;
	if (playlist_manager::get()->get_playing_item_location(&playlistIndex, &playlistItemIndex))
	{
		*pp = new com_object_impl_t<FbPlayingItemLocation>(true, playlistIndex, playlistItemIndex);
	}
	else
	{
		*pp = new com_object_impl_t<FbPlayingItemLocation>(false, -1, -1);
	}
	return S_OK;
}

STDMETHODIMP FbPlaylistManager::GetPlaylistFocusItemIndex(UINT playlistIndex, int* p)
{
	if (!p) return E_POINTER;

	*p = playlist_manager::get()->playlist_get_focus_item(playlistIndex);
	return S_OK;
}

STDMETHODIMP FbPlaylistManager::GetPlaylistItemCount(UINT playlistIndex, UINT* p)
{
	if (!p) return E_POINTER;

	*p = playlist_manager::get()->playlist_get_item_count(playlistIndex);
	return S_OK;
}

STDMETHODIMP FbPlaylistManager::GetPlaylistItems(UINT playlistIndex, IFbMetadbHandleList** pp)
{
	if (!pp) return E_POINTER;

	metadb_handle_list items;
	playlist_manager::get()->playlist_get_all_items(playlistIndex, items);
	*pp = new com_object_impl_t<FbMetadbHandleList>(items);
	return S_OK;
}

STDMETHODIMP FbPlaylistManager::GetPlaylistName(UINT playlistIndex, BSTR* p)
{
	if (!p) return E_POINTER;

	pfc::string8_fast temp;
	playlist_manager::get()->playlist_get_name(playlistIndex, temp);
	*p = SysAllocString(string_wide_from_utf8_fast(temp));
	return S_OK;
}

STDMETHODIMP FbPlaylistManager::GetPlaylistSelectedItems(UINT playlistIndex, IFbMetadbHandleList** pp)
{
	if (!pp) return E_POINTER;

	metadb_handle_list items;
	playlist_manager::get()->playlist_get_selected_items(playlistIndex, items);
	*pp = new com_object_impl_t<FbMetadbHandleList>(items);
	return S_OK;
}

STDMETHODIMP FbPlaylistManager::GetRecyclerItems(UINT index, IFbMetadbHandleList** pp)
{
	if (!pp) return E_POINTER;

	auto api = playlist_manager_v3::get();
	if (index < api->recycler_get_count())
	{
		metadb_handle_list handles;
		api->recycler_get_content(index, handles);
		*pp = new com_object_impl_t<FbMetadbHandleList>(handles);
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP FbPlaylistManager::GetRecyclerName(UINT index, BSTR* p)
{
	if (!p) return E_POINTER;

	auto api = playlist_manager_v3::get();
	if (index < api->recycler_get_count())
	{
		pfc::string8_fast name;
		api->recycler_get_name(index, name);
		*p = SysAllocString(string_wide_from_utf8_fast(name));
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP FbPlaylistManager::InsertPlaylistItems(UINT playlistIndex, UINT base, IFbMetadbHandleList* handles, VARIANT_BOOL select)
{
	metadb_handle_list* handles_ptr = NULL;
	handles->get__ptr((void**)&handles_ptr);
	pfc::bit_array_val selection(select != VARIANT_FALSE);
	playlist_manager::get()->playlist_insert_items(playlistIndex, base, *handles_ptr, selection);
	return S_OK;
}

STDMETHODIMP FbPlaylistManager::InsertPlaylistItemsFilter(UINT playlistIndex, UINT base, IFbMetadbHandleList* handles, VARIANT_BOOL select)
{
	metadb_handle_list* handles_ptr = NULL;
	handles->get__ptr((void**)&handles_ptr);
	playlist_manager::get()->playlist_insert_items_filter(playlistIndex, base, *handles_ptr, select != VARIANT_FALSE);
	return S_OK;
}

STDMETHODIMP FbPlaylistManager::IsAutoPlaylist(UINT playlistIndex, VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	if (playlistIndex < playlist_manager::get()->get_playlist_count())
	{
		*p = TO_VARIANT_BOOL(autoplaylist_manager::get()->is_client_present(playlistIndex));
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP FbPlaylistManager::IsPlaylistItemSelected(UINT playlistIndex, UINT playlistItemIndex, VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	*p = TO_VARIANT_BOOL(playlist_manager::get()->playlist_is_item_selected(playlistIndex, playlistItemIndex));
	return S_OK;
}

STDMETHODIMP FbPlaylistManager::IsPlaylistLocked(UINT playlistIndex, VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	auto api = playlist_manager::get();
	if (playlistIndex < api->get_playlist_count())
	{
		*p = TO_VARIANT_BOOL(api->playlist_lock_is_present(playlistIndex));
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP FbPlaylistManager::MovePlaylist(UINT from, UINT to, VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	*p = VARIANT_FALSE;
	auto api = playlist_manager::get();
	t_size count = api->get_playlist_count();
	order_helper order(count);

	if (from < count && to < count)
	{
		int inc = (from < to) ? 1 : -1;

		for (t_size i = from; i != to; i += inc)
		{
			order[i] = order[i + inc];
		}

		order[to] = from;

		*p = TO_VARIANT_BOOL(api->reorder(order.get_ptr(), order.get_count()));
	}
	return S_OK;
}

STDMETHODIMP FbPlaylistManager::MovePlaylistSelection(UINT playlistIndex, int delta, VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	*p = TO_VARIANT_BOOL(playlist_manager::get()->playlist_move_selection(playlistIndex, delta));
	return S_OK;
}

STDMETHODIMP FbPlaylistManager::RecyclerPurge(VARIANT affectedItems)
{
	auto api = playlist_manager_v3::get();
	pfc::bit_array_bittable affected(api->recycler_get_count());
	helpers::array helper;
	if (!helper.convert_to_bit_array(affectedItems, affected)) return E_INVALIDARG;
	if (helper.get_count() > 0)
	{
		api->recycler_purge(affected);
	}
	return S_OK;
}

STDMETHODIMP FbPlaylistManager::RecyclerRestore(UINT index)
{
	auto api = playlist_manager_v3::get();
	if (index < api->recycler_get_count())
	{
		api->recycler_restore(index);
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP FbPlaylistManager::RemovePlaylist(UINT playlistIndex, VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	*p = TO_VARIANT_BOOL(playlist_manager::get()->remove_playlist(playlistIndex));
	return S_OK;
}

STDMETHODIMP FbPlaylistManager::RemovePlaylistSelection(UINT playlistIndex, VARIANT_BOOL crop)
{
	playlist_manager::get()->playlist_remove_selection(playlistIndex, crop != VARIANT_FALSE);
	return S_OK;
}

STDMETHODIMP FbPlaylistManager::RemovePlaylistSwitch(UINT playlistIndex, VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	*p = TO_VARIANT_BOOL(playlist_manager::get()->remove_playlist_switch(playlistIndex));
	return S_OK;
}

STDMETHODIMP FbPlaylistManager::RenamePlaylist(UINT playlistIndex, BSTR name, VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	string_utf8_from_wide uname(name);
	*p = TO_VARIANT_BOOL(playlist_manager::get()->playlist_rename(playlistIndex, uname, uname.length()));
	return S_OK;
}

STDMETHODIMP FbPlaylistManager::SetActivePlaylistContext()
{
	ui_edit_context_manager::get()->set_context_active_playlist();
	return S_OK;
}

STDMETHODIMP FbPlaylistManager::SetPlaylistFocusItem(UINT playlistIndex, UINT playlistItemIndex)
{
	playlist_manager::get()->playlist_set_focus_item(playlistIndex, playlistItemIndex);
	return S_OK;
}

STDMETHODIMP FbPlaylistManager::SetPlaylistFocusItemByHandle(UINT playlistIndex, IFbMetadbHandle* handle)
{
	metadb_handle* ptr = NULL;
	handle->get__ptr((void**)&ptr);
	playlist_manager::get()->playlist_set_focus_by_handle(playlistIndex, ptr);
	return S_OK;
}

STDMETHODIMP FbPlaylistManager::SetPlaylistSelection(UINT playlistIndex, VARIANT affectedItems, VARIANT_BOOL state)
{
	auto api = playlist_manager::get();
	if (playlistIndex < api->get_playlist_count())
	{
		pfc::bit_array_bittable affected(api->playlist_get_item_count(playlistIndex));
		helpers::array helper;
		if (!helper.convert_to_bit_array(affectedItems, affected)) return E_INVALIDARG;
		if (helper.get_count() > 0)
		{
			pfc::bit_array_val status(state != VARIANT_FALSE);
			api->playlist_set_selection(playlistIndex, affected, status);
		}
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP FbPlaylistManager::SetPlaylistSelectionSingle(UINT playlistIndex, UINT playlistItemIndex, VARIANT_BOOL state)
{
	playlist_manager::get()->playlist_set_selection_single(playlistIndex, playlistItemIndex, state != VARIANT_FALSE);
	return S_OK;
}

STDMETHODIMP FbPlaylistManager::ShowAutoPlaylistUI(UINT playlistIndex, VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	if (playlistIndex < playlist_manager::get()->get_playlist_count())
	{
		*p = VARIANT_FALSE;

		auto api = autoplaylist_manager::get();
		if (api->is_client_present(playlistIndex))
		{
			autoplaylist_client_ptr client = api->query_client(playlistIndex);
			client->show_ui(playlistIndex);
			*p = VARIANT_TRUE;
		}
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP FbPlaylistManager::SortByFormat(UINT playlistIndex, BSTR pattern, VARIANT_BOOL selOnly, VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	string_utf8_from_wide upattern(pattern);
	*p = TO_VARIANT_BOOL(playlist_manager::get()->playlist_sort_by_format(playlistIndex, upattern.is_empty() ? NULL : upattern.get_ptr(), selOnly != VARIANT_FALSE));
	return S_OK;
}

STDMETHODIMP FbPlaylistManager::SortByFormatV2(UINT playlistIndex, BSTR pattern, int direction, VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	auto api = playlist_manager::get();
	metadb_handle_list handles;
	api->playlist_get_all_items(playlistIndex, handles);

	pfc::array_t<t_size> order;
	order.set_count(handles.get_count());

	titleformat_object::ptr obj;
	titleformat_compiler::get()->compile_safe(obj, string_utf8_from_wide(pattern));
	abort_callback_dummy abort;

	metadb_handle_list_helper::sort_by_format_get_order_v2(handles, order.get_ptr(), obj, NULL, direction, abort);

	*p = TO_VARIANT_BOOL(api->playlist_reorder_items(playlistIndex, order.get_ptr(), order.get_count()));
	return S_OK;
}

STDMETHODIMP FbPlaylistManager::SortPlaylistsByName(int direction)
{
	auto api = playlist_manager::get();
	t_size i, count = api->get_playlist_count();

	pfc::array_t<helpers::custom_sort_data> data;
	data.set_size(count);

	pfc::string8_fastalloc temp;
	temp.prealloc(512);

	for (i = 0; i < count; ++i)
	{
		api->playlist_get_name(i, temp);
		data[i].index = i;
		data[i].text = helpers::make_sort_string(temp);
	}

	pfc::sort_t(data, direction > 0 ? helpers::custom_sort_compare<1> : helpers::custom_sort_compare<-1>, count);
	order_helper order(count);

	for (i = 0; i < count; ++i)
	{
		order[i] = data[i].index;
		delete[] data[i].text;
	}

	api->reorder(order.get_ptr(), order.get_count());
	return S_OK;
}

STDMETHODIMP FbPlaylistManager::UndoBackup(UINT playlistIndex)
{
	playlist_manager::get()->playlist_undo_backup(playlistIndex);
	return S_OK;
}

STDMETHODIMP FbPlaylistManager::get_ActivePlaylist(int* p)
{
	if (!p) return E_POINTER;

	*p = playlist_manager::get()->get_active_playlist();
	return S_OK;
}

STDMETHODIMP FbPlaylistManager::get_PlaybackOrder(UINT* p)
{
	if (!p) return E_POINTER;

	*p = playlist_manager::get()->playback_order_get_active();
	return S_OK;
}

STDMETHODIMP FbPlaylistManager::get_PlayingPlaylist(int* p)
{
	if (!p) return E_POINTER;

	*p = playlist_manager::get()->get_playing_playlist();
	return S_OK;
}

STDMETHODIMP FbPlaylistManager::get_PlaylistCount(UINT* p)
{
	if (!p) return E_POINTER;

	*p = playlist_manager::get()->get_playlist_count();
	return S_OK;
}

STDMETHODIMP FbPlaylistManager::get_RecyclerCount(UINT* p)
{
	if (!p) return E_POINTER;

	*p = playlist_manager_v3::get()->recycler_get_count();
	return S_OK;
}

STDMETHODIMP FbPlaylistManager::put_ActivePlaylist(UINT playlistIndex)
{
	auto api = playlist_manager::get();
	if (playlistIndex < api->get_playlist_count())
	{
		api->set_active_playlist(playlistIndex);
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP FbPlaylistManager::put_PlaybackOrder(UINT p)
{
	auto api = playlist_manager::get();
	if (p < api->playback_order_get_count())
	{
		api->playback_order_set_active(p);
		return S_OK;
	}
	return E_INVALIDARG;
}

FbProfiler::FbProfiler(const char* p_name) : m_name(p_name)
{
	m_timer.start();
}

FbProfiler::~FbProfiler() {}

STDMETHODIMP FbProfiler::Print()
{
	FB2K_console_formatter() << JSP_NAME_VERSION ": FbProfiler (" << m_name << "): " << (int)(m_timer.query() * 1000) << " ms";
	return S_OK;
}

STDMETHODIMP FbProfiler::Reset()
{
	m_timer.start();
	return S_OK;
}

STDMETHODIMP FbProfiler::get_Time(int* p)
{
	if (!p) return E_POINTER;

	*p = (int)(m_timer.query() * 1000);
	return S_OK;
}

FbTitleFormat::FbTitleFormat(BSTR pattern)
{
	titleformat_compiler::get()->compile_safe(m_obj, string_utf8_from_wide(pattern));
}

FbTitleFormat::~FbTitleFormat() {}

void FbTitleFormat::FinalRelease()
{
	m_obj.release();
}

STDMETHODIMP FbTitleFormat::get__ptr(void** pp)
{
	if (!pp) return E_POINTER;

	*pp = m_obj.get_ptr();
	return S_OK;
}

STDMETHODIMP FbTitleFormat::Eval(BSTR* p)
{
	if (m_obj.is_empty() || !p) return E_POINTER;

	pfc::string8_fast text;
	playback_control::get()->playback_format_title(NULL, text, m_obj, NULL, playback_control::display_level_all);
	*p = SysAllocString(string_wide_from_utf8_fast(text));
	return S_OK;
}

STDMETHODIMP FbTitleFormat::EvalWithMetadb(IFbMetadbHandle* handle, BSTR* p)
{
	if (m_obj.is_empty() || !p) return E_POINTER;

	metadb_handle* ptr = NULL;
	handle->get__ptr((void**)&ptr);

	pfc::string8_fast text;
	ptr->format_title(NULL, text, m_obj, NULL);

	*p = SysAllocString(string_wide_from_utf8_fast(text));
	return S_OK;
}

STDMETHODIMP FbTitleFormat::EvalWithMetadbs(IFbMetadbHandleList* handles, VARIANT* p)
{
	if (m_obj.is_empty() || !p) return E_POINTER;

	metadb_handle_list* handles_ptr = NULL;
	handles->get__ptr((void**)&handles_ptr);

	metadb_handle_list_ref handles_ref = *handles_ptr;
	LONG count = handles_ref.get_count();
	helpers::array helper;
	if (!helper.create(count)) return E_OUTOFMEMORY;

	for (LONG i = 0; i < count; ++i)
	{
		pfc::string8_fast text;
		handles_ref[i]->format_title(NULL, text, m_obj, NULL);
		_variant_t var;
		var.vt = VT_BSTR;
		var.bstrVal = SysAllocString(string_wide_from_utf8_fast(text));
		if (!helper.put_item(i, var)) return E_OUTOFMEMORY;
	}
	p->vt = VT_ARRAY | VT_VARIANT;
	p->parray = helper.get_ptr();
	return S_OK;
}

FbTooltip::FbTooltip(HWND p_wndparent, const panel_tooltip_param_ptr& p_param_ptr) : m_wndparent(p_wndparent), m_panel_tooltip_param_ptr(p_param_ptr), m_tip_buffer(SysAllocString(PFC_WIDESTRING(JSP_NAME)))
{
	m_wndtooltip = CreateWindowEx(
		WS_EX_TOPMOST,
		TOOLTIPS_CLASS,
		NULL,
		WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		p_wndparent,
		NULL,
		core_api::get_my_instance(),
		NULL);

	// Original position
	SetWindowPos(m_wndtooltip, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

	// Set up tooltip information.
	memset(&m_ti, 0, sizeof(m_ti));

	m_ti.cbSize = sizeof(m_ti);
	m_ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS | TTF_TRANSPARENT;
	m_ti.hinst = core_api::get_my_instance();
	m_ti.hwnd = p_wndparent;
	m_ti.uId = (UINT_PTR)p_wndparent;
	m_ti.lpszText = m_tip_buffer;

	HFONT font = CreateFont(
		-(int)m_panel_tooltip_param_ptr->font_size,
		0,
		0,
		0,
		(m_panel_tooltip_param_ptr->font_style & Gdiplus::FontStyleBold) ? FW_BOLD : FW_NORMAL,
		(m_panel_tooltip_param_ptr->font_style & Gdiplus::FontStyleItalic) ? TRUE : FALSE,
		(m_panel_tooltip_param_ptr->font_style & Gdiplus::FontStyleUnderline) ? TRUE : FALSE,
		(m_panel_tooltip_param_ptr->font_style & Gdiplus::FontStyleStrikeout) ? TRUE : FALSE,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		m_panel_tooltip_param_ptr->font_name);

	SendMessage(m_wndtooltip, TTM_ADDTOOL, 0, (LPARAM)&m_ti);
	SendMessage(m_wndtooltip, TTM_ACTIVATE, FALSE, 0);
	SendMessage(m_wndtooltip, WM_SETFONT, (WPARAM)font, MAKELPARAM(FALSE, 0));

	m_panel_tooltip_param_ptr->tooltip_hwnd = m_wndtooltip;
	m_panel_tooltip_param_ptr->tooltip_size.cx = -1;
	m_panel_tooltip_param_ptr->tooltip_size.cy = -1;
}

FbTooltip::~FbTooltip() {}

void FbTooltip::FinalRelease()
{
	if (m_wndtooltip && IsWindow(m_wndtooltip))
	{
		DestroyWindow(m_wndtooltip);
		m_wndtooltip = NULL;
	}

	if (m_tip_buffer)
	{
		SysFreeString(m_tip_buffer);
		m_tip_buffer = NULL;
	}
}

STDMETHODIMP FbTooltip::get_Text(BSTR* p)
{
	if (!p) return E_POINTER;

	*p = SysAllocString(m_tip_buffer);
	return S_OK;
}

STDMETHODIMP FbTooltip::put_Text(BSTR text)
{
	SysReAllocString(&m_tip_buffer, text);
	m_ti.lpszText = m_tip_buffer;
	SendMessage(m_wndtooltip, TTM_SETTOOLINFO, 0, (LPARAM)&m_ti);
	return S_OK;
}

STDMETHODIMP FbTooltip::put_TrackActivate(VARIANT_BOOL activate)
{
	if (activate)
	{
		m_ti.uFlags |= TTF_TRACK | TTF_ABSOLUTE;
	}
	else
	{
		m_ti.uFlags &= ~(TTF_TRACK | TTF_ABSOLUTE);
	}

	SendMessage(m_wndtooltip, TTM_TRACKACTIVATE, activate != VARIANT_FALSE ? TRUE : FALSE, (LPARAM)&m_ti);
	return S_OK;
}

STDMETHODIMP FbTooltip::Activate()
{
	SendMessage(m_wndtooltip, TTM_ACTIVATE, TRUE, 0);
	return S_OK;
}

STDMETHODIMP FbTooltip::Deactivate()
{
	SendMessage(m_wndtooltip, TTM_ACTIVATE, FALSE, 0);
	return S_OK;
}

STDMETHODIMP FbTooltip::SetMaxWidth(int width)
{
	SendMessage(m_wndtooltip, TTM_SETMAXTIPWIDTH, 0, width);
	return S_OK;
}

STDMETHODIMP FbTooltip::GetDelayTime(int type, int* p)
{
	if (!p) return E_POINTER;
	if (type < TTDT_AUTOMATIC || type > TTDT_INITIAL) return E_INVALIDARG;

	*p = SendMessage(m_wndtooltip, TTM_GETDELAYTIME, type, 0);
	return S_OK;
}

STDMETHODIMP FbTooltip::SetDelayTime(int type, int time)
{
	if (type < TTDT_AUTOMATIC || type > TTDT_INITIAL) return E_INVALIDARG;

	SendMessage(m_wndtooltip, TTM_SETDELAYTIME, type, time);
	return S_OK;
}

STDMETHODIMP FbTooltip::TrackPosition(int x, int y)
{
	POINT pt = { x, y };
	ClientToScreen(m_wndparent, &pt);
	SendMessage(m_wndtooltip, TTM_TRACKPOSITION, 0, MAKELONG(pt.x, pt.y));
	return S_OK;
}

FbUiSelectionHolder::FbUiSelectionHolder(const ui_selection_holder::ptr& holder) : m_holder(holder) {}
FbUiSelectionHolder::~FbUiSelectionHolder() {}
void FbUiSelectionHolder::FinalRelease()
{
	m_holder.release();
}

STDMETHODIMP FbUiSelectionHolder::SetPlaylistSelectionTracking()
{
	m_holder->set_playlist_selection_tracking();
	return S_OK;
}

STDMETHODIMP FbUiSelectionHolder::SetPlaylistTracking()
{
	m_holder->set_playlist_tracking();
	return S_OK;
}

STDMETHODIMP FbUiSelectionHolder::SetSelection(IFbMetadbHandleList* handles)
{
	metadb_handle_list* handles_ptr = NULL;
	handles->get__ptr((void**)&handles_ptr);
	m_holder->set_selection(*handles_ptr);
	return S_OK;
}

FbUtils::FbUtils() {}
FbUtils::~FbUtils() {}

STDMETHODIMP FbUtils::AcquireUiSelectionHolder(IFbUiSelectionHolder** pp)
{
	if (!pp) return E_POINTER;

	ui_selection_holder::ptr holder = ui_selection_manager::get()->acquire();
	*pp = new com_object_impl_t<FbUiSelectionHolder>(holder);
	return S_OK;
}

STDMETHODIMP FbUtils::AddDirectory()
{
	standard_commands::main_add_directory();
	return S_OK;
}

STDMETHODIMP FbUtils::AddFiles()
{
	standard_commands::main_add_files();
	return S_OK;
}

STDMETHODIMP FbUtils::CheckClipboardContents(UINT window_id, VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	*p = VARIANT_FALSE;
	pfc::com_ptr_t<IDataObject> pDO;
	HRESULT hr = OleGetClipboard(pDO.receive_ptr());
	if (SUCCEEDED(hr))
	{
		bool native;
		DWORD drop_effect = DROPEFFECT_COPY;
		hr = ole_interaction::get()->check_dataobject(pDO, drop_effect, native);
		*p = TO_VARIANT_BOOL(SUCCEEDED(hr));
	}
	return S_OK;
}

STDMETHODIMP FbUtils::ClearPlaylist()
{
	standard_commands::main_clear_playlist();
	return S_OK;
}

STDMETHODIMP FbUtils::CreateContextMenuManager(IContextMenuManager** pp)
{
	if (!pp) return E_POINTER;

	*pp = new com_object_impl_t<ContextMenuManager>();
	return S_OK;
}

STDMETHODIMP FbUtils::CreateHandleList(VARIANT handle, IFbMetadbHandleList** pp)
{
	if (!pp) return E_POINTER;

	metadb_handle_list items;
	IDispatch* temp = NULL;

	if (handle.vt == VT_DISPATCH && handle.pdispVal && SUCCEEDED(handle.pdispVal->QueryInterface(__uuidof(IFbMetadbHandle), (void**)&temp)))
	{
		IDispatchPtr handle_s = temp;
		void* ptr = NULL;
		reinterpret_cast<IFbMetadbHandle *>(handle_s.GetInterfacePtr())->get__ptr(&ptr);
		if (!ptr) return E_INVALIDARG;
		items.add_item(reinterpret_cast<metadb_handle*>(ptr));
	}
	*pp = new com_object_impl_t<FbMetadbHandleList>(items);
	return S_OK;
}

STDMETHODIMP FbUtils::CreateMainMenuManager(IMainMenuManager** pp)
{
	if (!pp) return E_POINTER;

	*pp = new com_object_impl_t<MainMenuManager>();
	return S_OK;
}

STDMETHODIMP FbUtils::CreateProfiler(BSTR name, IFbProfiler** pp)
{
	if (!pp) return E_POINTER;

	*pp = new com_object_impl_t<FbProfiler>(string_utf8_from_wide(name));
	return S_OK;
}

STDMETHODIMP FbUtils::Exit()
{
	standard_commands::main_exit();
	return S_OK;
}

STDMETHODIMP FbUtils::GetClipboardContents(UINT window_id, IFbMetadbHandleList** pp)
{
	if (!pp) return E_POINTER;

	auto api = ole_interaction::get();
	pfc::com_ptr_t<IDataObject> pDO;
	metadb_handle_list items;

	HRESULT hr = OleGetClipboard(pDO.receive_ptr());
	if (SUCCEEDED(hr))
	{
		DWORD drop_effect = DROPEFFECT_COPY;
		bool native;
		hr = api->check_dataobject(pDO, drop_effect, native);
		if (SUCCEEDED(hr))
		{
			dropped_files_data_impl data;
			hr = api->parse_dataobject(pDO, data);
			if (SUCCEEDED(hr))
			{
				data.to_handles(items, native, (HWND)window_id);
			}
		}
	}

	*pp = new com_object_impl_t<FbMetadbHandleList>(items);
	return S_OK;
}

STDMETHODIMP FbUtils::GetDSPPresets(VARIANT* p)
{
	if (!p) return E_POINTER;

	auto api = dsp_config_manager_v2::get();
	LONG count = api->get_preset_count();
	helpers::array helper;
	if (!helper.create(count)) return E_OUTOFMEMORY;
	pfc::string8_fast name;

	for (LONG i = 0; i < count; ++i)
	{
		api->get_preset_name(i, name);
		_variant_t var;
		var.vt = VT_BSTR;
		var.bstrVal = SysAllocString(string_wide_from_utf8_fast(name));
		if (!helper.put_item(i, var)) return E_OUTOFMEMORY;
	}
	p->vt = VT_ARRAY | VT_VARIANT;
	p->parray = helper.get_ptr();
	return S_OK;
}

STDMETHODIMP FbUtils::GetFocusItem(IFbMetadbHandle** pp)
{
	if (!pp) return E_POINTER;

	*pp = NULL;
	metadb_handle_ptr metadb;
	if (playlist_manager::get()->activeplaylist_get_focus_item_handle(metadb))
	{
		*pp = new com_object_impl_t<FbMetadbHandle>(metadb);
	}
	return S_OK;
}

STDMETHODIMP FbUtils::GetLibraryItems(IFbMetadbHandleList** pp)
{
	if (!pp) return E_POINTER;

	metadb_handle_list items;
	library_manager::get()->get_all_items(items);
	*pp = new com_object_impl_t<FbMetadbHandleList>(items);
	return S_OK;
}

STDMETHODIMP FbUtils::GetNowPlaying(IFbMetadbHandle** pp)
{
	if (!pp) return E_POINTER;

	*pp = NULL;
	metadb_handle_ptr metadb;
	if (playback_control::get()->get_now_playing(metadb))
	{
		*pp = new com_object_impl_t<FbMetadbHandle>(metadb);
	}
	return S_OK;
}

STDMETHODIMP FbUtils::GetOutputDevices(BSTR* p)
{
	if (!p) return E_POINTER;

	json j = json::array();
	auto api = output_manager_v2::get();
	outputCoreConfig_t config;
	api->getCoreConfig(config);

	api->listDevices([&j, &config](pfc::string8&& name, auto&& output_id, auto&& device_id) {
		pfc::string8 output_string, device_string;
		output_string << "{" << pfc::print_guid(output_id) << "}";
		device_string << "{" << pfc::print_guid(device_id) << "}";

		j.push_back({
			{ "name", name.get_ptr() },
			{ "output_id", output_string.get_ptr() },
			{ "device_id", device_string.get_ptr() },
			{ "active", config.m_output == output_id && config.m_device == device_id }
		});
	});
	*p = SysAllocString(string_wide_from_utf8_fast((j.dump()).c_str()));
	return S_OK;
}

STDMETHODIMP FbUtils::GetSelection(IFbMetadbHandle** pp)
{
	if (!pp) return E_POINTER;

	*pp = NULL;
	metadb_handle_list items;
	ui_selection_manager::get()->get_selection(items);

	if (items.get_count() > 0)
	{
		*pp = new com_object_impl_t<FbMetadbHandle>(items[0]);
	}
	return S_OK;
}

STDMETHODIMP FbUtils::GetSelections(UINT flags, IFbMetadbHandleList** pp)
{
	if (!pp) return E_POINTER;

	metadb_handle_list items;
	ui_selection_manager_v2::get()->get_selection(items, flags);
	*pp = new com_object_impl_t<FbMetadbHandleList>(items);
	return S_OK;
}

STDMETHODIMP FbUtils::GetSelectionType(UINT* p)
{
	if (!p) return E_POINTER;

	const GUID* guids[] = {
		&contextmenu_item::caller_undefined,
		&contextmenu_item::caller_active_playlist_selection,
		&contextmenu_item::caller_active_playlist,
		&contextmenu_item::caller_playlist_manager,
		&contextmenu_item::caller_now_playing,
		&contextmenu_item::caller_keyboard_shortcut_list,
		&contextmenu_item::caller_media_library_viewer,
	};

	*p = 0;
	GUID type = ui_selection_manager_v2::get()->get_selection_type(0);

	for (t_size i = 0; i < _countof(guids); ++i)
	{
		if (*guids[i] == type)
		{
			*p = i;
			break;
		}
	}

	return S_OK;
}

STDMETHODIMP FbUtils::IsLibraryEnabled(VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	*p = TO_VARIANT_BOOL(library_manager::get()->is_library_enabled());
	return S_OK;
}

STDMETHODIMP FbUtils::LoadPlaylist()
{
	standard_commands::main_load_playlist();
	return S_OK;
}

STDMETHODIMP FbUtils::Next()
{
	standard_commands::main_next();
	return S_OK;
}

STDMETHODIMP FbUtils::Pause()
{
	standard_commands::main_pause();
	return S_OK;
}

STDMETHODIMP FbUtils::Play()
{
	standard_commands::main_play();
	return S_OK;
}

STDMETHODIMP FbUtils::PlayOrPause()
{
	standard_commands::main_play_or_pause();
	return S_OK;
}

STDMETHODIMP FbUtils::Prev()
{
	standard_commands::main_previous();
	return S_OK;
}

STDMETHODIMP FbUtils::Random()
{
	standard_commands::main_random();
	return S_OK;
}

STDMETHODIMP FbUtils::RunContextCommand(BSTR command, VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	*p = TO_VARIANT_BOOL(helpers::execute_context_command_by_name_SEH(string_utf8_from_wide(command), metadb_handle_list()));
	return S_OK;
}

STDMETHODIMP FbUtils::RunMainMenuCommand(BSTR command, VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	*p = TO_VARIANT_BOOL(helpers::execute_mainmenu_command_by_name_SEH(string_utf8_from_wide(command)));
	return S_OK;
}

STDMETHODIMP FbUtils::SaveIndex()
{
	try
	{
		stats::theAPI()->save_index_data(g_guid_jsp_metadb_index);
	}
	catch (...)
	{
		FB2K_console_formatter() << JSP_NAME_VERSION ": Save index fail.";
	}
	return S_OK;
}

STDMETHODIMP FbUtils::SavePlaylist()
{
	standard_commands::main_save_playlist();
	return S_OK;
}

STDMETHODIMP FbUtils::SetOutputDevice(BSTR output, BSTR device)
{
	GUID output_id, device_id;
	if (CLSIDFromString(output, &output_id) == NOERROR && CLSIDFromString(device, &device_id) == NOERROR)
	{
		output_manager_v2::get()->setCoreConfigDevice(output_id, device_id);
	}
	return S_OK;
}

STDMETHODIMP FbUtils::ShowConsole()
{
	const GUID guid_main_show_console = { 0x5b652d25, 0xce44, 0x4737,{ 0x99, 0xbb, 0xa3, 0xcf, 0x2a, 0xeb, 0x35, 0xcc } };
	standard_commands::run_main(guid_main_show_console);
	return S_OK;
}

STDMETHODIMP FbUtils::ShowLibrarySearchUI(BSTR query)
{
	if (!query) return E_INVALIDARG;

	library_search_ui::get()->show(string_utf8_from_wide(query));
	return S_OK;
}

STDMETHODIMP FbUtils::ShowPopupMessage(BSTR msg, BSTR title)
{
	popup_msg::g_show(string_utf8_from_wide(msg), string_utf8_from_wide(title));
	return S_OK;
}

STDMETHODIMP FbUtils::ShowPreferences()
{
	standard_commands::main_preferences();
	return S_OK;
}

STDMETHODIMP FbUtils::Stop()
{
	standard_commands::main_stop();
	return S_OK;
}

STDMETHODIMP FbUtils::TitleFormat(BSTR expression, IFbTitleFormat** pp)
{
	if (!pp) return E_POINTER;

	*pp = new com_object_impl_t<FbTitleFormat>(expression);
	return S_OK;
}

STDMETHODIMP FbUtils::VolumeDown()
{
	standard_commands::main_volume_down();
	return S_OK;
}

STDMETHODIMP FbUtils::VolumeMute()
{
	standard_commands::main_volume_mute();
	return S_OK;
}

STDMETHODIMP FbUtils::VolumeUp()
{
	standard_commands::main_volume_up();
	return S_OK;
}

STDMETHODIMP FbUtils::get_AlwaysOnTop(VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	*p = TO_VARIANT_BOOL(config_object::g_get_data_bool_simple(standard_config_objects::bool_ui_always_on_top, false));
	return S_OK;
}

STDMETHODIMP FbUtils::get_ComponentPath(BSTR* p)
{
	if (!p) return E_POINTER;

	*p = SysAllocString(string_wide_from_utf8_fast(helpers::get_fb2k_component_path()));
	return S_OK;
}

STDMETHODIMP FbUtils::get_CursorFollowPlayback(VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	*p = TO_VARIANT_BOOL(config_object::g_get_data_bool_simple(standard_config_objects::bool_cursor_follows_playback, false));
	return S_OK;
}

STDMETHODIMP FbUtils::get_DSP(int* p)
{
	if (!p) return E_POINTER;

	*p = dsp_config_manager_v2::get()->get_selected_preset();
	return S_OK;
}

STDMETHODIMP FbUtils::get_FoobarPath(BSTR* p)
{
	if (!p) return E_POINTER;

	*p = SysAllocString(string_wide_from_utf8_fast(helpers::get_fb2k_path()));
	return S_OK;
}

STDMETHODIMP FbUtils::get_IsPaused(VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	*p = TO_VARIANT_BOOL(playback_control::get()->is_paused());
	return S_OK;
}

STDMETHODIMP FbUtils::get_IsPlaying(VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	*p = TO_VARIANT_BOOL(playback_control::get()->is_playing());
	return S_OK;
}

STDMETHODIMP FbUtils::get_PlaybackFollowCursor(VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	*p = TO_VARIANT_BOOL(config_object::g_get_data_bool_simple(standard_config_objects::bool_playback_follows_cursor, false));
	return S_OK;
}

STDMETHODIMP FbUtils::get_PlaybackLength(double* p)
{
	if (!p) return E_POINTER;

	*p = playback_control::get()->playback_get_length();
	return S_OK;
}

STDMETHODIMP FbUtils::get_PlaybackTime(double* p)
{
	if (!p) return E_POINTER;

	*p = playback_control::get()->playback_get_position();
	return S_OK;
}

STDMETHODIMP FbUtils::get_ProfilePath(BSTR* p)
{
	if (!p) return E_POINTER;

	*p = SysAllocString(string_wide_from_utf8_fast(helpers::get_profile_path()));
	return S_OK;
}

STDMETHODIMP FbUtils::get_ReplaygainMode(UINT* p)
{
	if (!p) return E_POINTER;

	t_replaygain_config rg;
	replaygain_manager::get()->get_core_settings(rg);
	*p = rg.m_source_mode;
	return S_OK;
}

STDMETHODIMP FbUtils::get_StopAfterCurrent(VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	*p = TO_VARIANT_BOOL(playback_control::get()->get_stop_after_current());
	return S_OK;
}

STDMETHODIMP FbUtils::get_Volume(float* p)
{
	if (!p) return E_POINTER;

	*p = playback_control::get()->get_volume();
	return S_OK;
}

STDMETHODIMP FbUtils::put_AlwaysOnTop(VARIANT_BOOL p)
{
	config_object::g_set_data_bool(standard_config_objects::bool_ui_always_on_top, p != VARIANT_FALSE);
	return S_OK;
}

STDMETHODIMP FbUtils::put_CursorFollowPlayback(VARIANT_BOOL p)
{
	config_object::g_set_data_bool(standard_config_objects::bool_cursor_follows_playback, p != VARIANT_FALSE);
	return S_OK;
}

STDMETHODIMP FbUtils::put_DSP(UINT p)
{
	auto api = dsp_config_manager_v2::get();
	if (p < api->get_preset_count())
	{
		api->select_preset(p);
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP FbUtils::put_PlaybackFollowCursor(VARIANT_BOOL p)
{
	config_object::g_set_data_bool(standard_config_objects::bool_playback_follows_cursor, p != VARIANT_FALSE);
	return S_OK;
}

STDMETHODIMP FbUtils::put_PlaybackTime(double time)
{
	playback_control::get()->playback_seek(time);
	return S_OK;
}

STDMETHODIMP FbUtils::put_ReplaygainMode(UINT p)
{
	switch (p)
	{
	case 0:
		standard_commands::main_rg_disable();
		break;
	case 1:
		standard_commands::main_rg_set_track();
		break;
	case 2:
		standard_commands::main_rg_set_album();
		break;
	case 3:
		standard_commands::run_main(standard_commands::guid_main_rg_byorder);
		break;
	default:
		return E_INVALIDARG;
	}

	playback_control_v3::get()->restart();
	return S_OK;
}

STDMETHODIMP FbUtils::put_StopAfterCurrent(VARIANT_BOOL p)
{
	playback_control::get()->set_stop_after_current(p != VARIANT_FALSE);
	return S_OK;
}

STDMETHODIMP FbUtils::put_Volume(float value)
{
	playback_control::get()->set_volume(value);
	return S_OK;
}

FbWindow::FbWindow(HostComm* p) : m_host(p) {}
FbWindow::~FbWindow() {}

STDMETHODIMP FbWindow::ClearInterval(UINT intervalID)
{
	HostTimerDispatcher::Get().killTimer(intervalID);
	return S_OK;
}

STDMETHODIMP FbWindow::ClearTimeout(UINT timeoutID)
{
	HostTimerDispatcher::Get().killTimer(timeoutID);
	return S_OK;
}

STDMETHODIMP FbWindow::CreatePopupMenu(IMenuObj** pp)
{
	if (!pp) return E_POINTER;

	*pp = new com_object_impl_t<MenuObj>(m_host->GetHWND());
	return S_OK;
}

STDMETHODIMP FbWindow::CreateThemeManager(BSTR classid, IThemeManager** pp)
{
	if (!pp) return E_POINTER;

	IThemeManager* ptheme = NULL;

	try
	{
		ptheme = new com_object_impl_t<ThemeManager>(m_host->GetHWND(), classid);
	}
	catch (...)
	{
		if (ptheme)
		{
			ptheme->Dispose();
			delete ptheme;
			ptheme = NULL;
		}
	}

	*pp = ptheme;
	return S_OK;
}

STDMETHODIMP FbWindow::CreateTooltip(BSTR name, float pxSize, int style, IFbTooltip** pp)
{
	if (!pp) return E_POINTER;

	const auto& tooltip_param = m_host->PanelTooltipParam();
	tooltip_param->font_name = name;
	tooltip_param->font_size = pxSize;
	tooltip_param->font_style = style;
	*pp = new com_object_impl_t<FbTooltip>(m_host->GetHWND(), tooltip_param);
	return S_OK;
}

STDMETHODIMP FbWindow::GetColourCUI(UINT type, int* p)
{
	if (!p) return E_POINTER;
	if (m_host->GetInstanceType() != HostComm::KInstanceTypeCUI) return E_NOTIMPL;

	*p = m_host->GetColourUI(type);
	return S_OK;
}

STDMETHODIMP FbWindow::GetColourDUI(UINT type, int* p)
{
	if (!p) return E_POINTER;
	if (m_host->GetInstanceType() != HostComm::KInstanceTypeDUI) return E_NOTIMPL;

	*p = m_host->GetColourUI(type);
	return S_OK;
}

STDMETHODIMP FbWindow::GetFontCUI(UINT type, IGdiFont** pp)
{
	if (!pp) return E_POINTER;
	if (m_host->GetInstanceType() != HostComm::KInstanceTypeCUI) return E_NOTIMPL;

	*pp = NULL;
	HFONT hFont = m_host->GetFontUI(type);
	if (hFont)
	{
		Gdiplus::Font* font = new Gdiplus::Font(m_host->GetHDC(), hFont);
		if (helpers::ensure_gdiplus_object(font))
		{
			*pp = new com_object_impl_t<GdiFont>(font, hFont);
		}
		else
		{
			if (font) delete font;
			font = NULL;
		}
	}
	return S_OK;
}

STDMETHODIMP FbWindow::GetFontDUI(UINT type, IGdiFont** pp)
{
	if (!pp) return E_POINTER;
	if (m_host->GetInstanceType() != HostComm::KInstanceTypeDUI) return E_NOTIMPL;

	*pp = NULL;
	HFONT hFont = m_host->GetFontUI(type);
	if (hFont)
	{
		Gdiplus::Font* font = new Gdiplus::Font(m_host->GetHDC(), hFont);
		if (helpers::ensure_gdiplus_object(font))
		{
			*pp = new com_object_impl_t<GdiFont>(font, hFont, false);
		}
		else
		{
			if (font) delete font;
			font = NULL;
		}
	}
	return S_OK;
}

STDMETHODIMP FbWindow::GetProperty(BSTR name, VARIANT defaultval, VARIANT* p)
{
	if (!p) return E_POINTER;

	HRESULT hr;
	_variant_t var;
	string_utf8_from_wide uname(name);

	if (m_host->get_config_prop().get_config_item(uname, var))
	{
		hr = VariantCopy(p, &var);
	}
	else
	{
		m_host->get_config_prop().set_config_item(uname, defaultval);
		hr = VariantCopy(p, &defaultval);
	}

	if (FAILED(hr))
		p = NULL;

	return S_OK;
}

STDMETHODIMP FbWindow::NotifyOthers(BSTR name, VARIANT info)
{
	if (info.vt & VT_BYREF) return E_INVALIDARG;

	_variant_t var;
	if (FAILED(VariantCopy(&var, &info))) return E_INVALIDARG;

	simple_callback_data_2<_bstr_t, _variant_t>* notify_data = new simple_callback_data_2<_bstr_t, _variant_t>(name, NULL);
	notify_data->m_item2.Attach(var.Detach());
	panel_manager::instance().send_msg_to_others_pointer(m_host->GetHWND(), CALLBACK_UWM_ON_NOTIFY_DATA, notify_data);
	return S_OK;
}

STDMETHODIMP FbWindow::Reload()
{
	PostMessage(m_host->GetHWND(), UWM_RELOAD, 0, 0);
	return S_OK;
}

STDMETHODIMP FbWindow::Repaint()
{
	m_host->Repaint();
	return S_OK;
}

STDMETHODIMP FbWindow::RepaintRect(int x, int y, int w, int h)
{
	m_host->RepaintRect(x, y, w, h);
	return S_OK;
}

STDMETHODIMP FbWindow::SetCursor(UINT id)
{
	::SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(id)));
	return S_OK;
}

STDMETHODIMP FbWindow::SetInterval(IDispatch* func, int delay, UINT* p)
{
	if (!p) return E_POINTER;

	*p = HostTimerDispatcher::Get().setInterval(m_host->GetHWND(), delay, func);
	return S_OK;
}

STDMETHODIMP FbWindow::SetProperty(BSTR name, VARIANT val)
{
	m_host->get_config_prop().set_config_item(string_utf8_from_wide(name), val);
	return S_OK;
}

STDMETHODIMP FbWindow::SetTimeout(IDispatch* func, int delay, UINT* p)
{
	if (!p) return E_POINTER;

	*p = HostTimerDispatcher::Get().setTimeout(m_host->GetHWND(), delay, func);
	return S_OK;
}

STDMETHODIMP FbWindow::ShowConfigure()
{
	PostMessage(m_host->GetHWND(), UWM_SHOW_CONFIGURE, 0, 0);
	return S_OK;
}

STDMETHODIMP FbWindow::ShowProperties()
{
	PostMessage(m_host->GetHWND(), UWM_SHOW_PROPERTIES, 0, 0);
	return S_OK;
}

STDMETHODIMP FbWindow::get_Height(int* p)
{
	if (!p) return E_POINTER;

	*p = m_host->GetHeight();
	return S_OK;
}

STDMETHODIMP FbWindow::get_ID(UINT* p)
{
	if (!p) return E_POINTER;

	*p = (UINT)m_host->GetHWND();
	return S_OK;
}

STDMETHODIMP FbWindow::get_InstanceType(UINT* p)
{
	if (!p) return E_POINTER;

	*p = m_host->GetInstanceType();
	return S_OK;
}

STDMETHODIMP FbWindow::get_IsTransparent(VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	*p = TO_VARIANT_BOOL(m_host->get_pseudo_transparent());
	return S_OK;
}

STDMETHODIMP FbWindow::get_IsVisible(VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	*p = TO_VARIANT_BOOL(IsWindowVisible(m_host->GetHWND()));
	return S_OK;
}

STDMETHODIMP FbWindow::get_MaxHeight(UINT* p)
{
	if (!p) return E_POINTER;

	*p = m_host->MaxSize().y;
	return S_OK;
}

STDMETHODIMP FbWindow::get_MaxWidth(UINT* p)
{
	if (!p) return E_POINTER;

	*p = m_host->MaxSize().x;
	return S_OK;
}

STDMETHODIMP FbWindow::get_MinHeight(UINT* p)
{
	if (!p) return E_POINTER;

	*p = m_host->MinSize().y;
	return S_OK;
}

STDMETHODIMP FbWindow::get_MinWidth(UINT* p)
{
	if (!p) return E_POINTER;

	*p = m_host->MinSize().x;
	return S_OK;
}

STDMETHODIMP FbWindow::get_Name(BSTR* p)
{
	if (!p) return E_POINTER;

	pfc::string8_fast name = m_host->ScriptInfo().name;
	if (name.is_empty())
	{
		name = pfc::print_guid(m_host->GetGUID());
	}
	*p = SysAllocString(string_wide_from_utf8_fast(name));
	return S_OK;
}

STDMETHODIMP FbWindow::get_Width(int* p)
{
	if (!p) return E_POINTER;

	*p = m_host->GetWidth();
	return S_OK;
}

STDMETHODIMP FbWindow::put_MaxHeight(UINT height)
{
	m_host->MaxSize().y = height;
	PostMessage(m_host->GetHWND(), UWM_SIZE_LIMIT_CHANGED, 0, uie::size_limit_maximum_height);
	return S_OK;
}

STDMETHODIMP FbWindow::put_MaxWidth(UINT width)
{
	m_host->MaxSize().x = width;
	PostMessage(m_host->GetHWND(), UWM_SIZE_LIMIT_CHANGED, 0, uie::size_limit_maximum_width);
	return S_OK;
}

STDMETHODIMP FbWindow::put_MinHeight(UINT height)
{
	m_host->MinSize().y = height;
	PostMessage(m_host->GetHWND(), UWM_SIZE_LIMIT_CHANGED, 0, uie::size_limit_minimum_height);
	return S_OK;
}

STDMETHODIMP FbWindow::put_MinWidth(UINT width)
{
	m_host->MinSize().x = width;
	PostMessage(m_host->GetHWND(), UWM_SIZE_LIMIT_CHANGED, 0, uie::size_limit_minimum_width);
	return S_OK;
}

GdiBitmap::GdiBitmap(Gdiplus::Bitmap* p) : GdiObj<IGdiBitmap, Gdiplus::Bitmap>(p) {}

STDMETHODIMP GdiBitmap::ApplyAlpha(BYTE alpha, IGdiBitmap** pp)
{
	if (!m_ptr || !pp) return E_POINTER;

	t_size width = m_ptr->GetWidth();
	t_size height = m_ptr->GetHeight();
	Gdiplus::Bitmap* out = new Gdiplus::Bitmap(width, height, PixelFormat32bppPARGB);
	Gdiplus::Graphics g(out);
	Gdiplus::ImageAttributes ia;
	Gdiplus::ColorMatrix cm = { 0.0 };
	Gdiplus::Rect rc(0, 0, width, height);

	cm.m[0][0] = cm.m[1][1] = cm.m[2][2] = cm.m[4][4] = 1.0;
	cm.m[3][3] = static_cast<float>(alpha) / 255;
	ia.SetColorMatrix(&cm);

	g.DrawImage(m_ptr, rc, 0, 0, width, height, Gdiplus::UnitPixel, &ia);

	*pp = new com_object_impl_t<GdiBitmap>(out);
	return S_OK;
}

STDMETHODIMP GdiBitmap::ApplyMask(IGdiBitmap* mask, VARIANT_BOOL* p)
{
	if (!m_ptr || !p) return E_POINTER;

	*p = VARIANT_FALSE;
	Gdiplus::Bitmap* bitmap_mask = NULL;
	mask->get__ptr((void**)&bitmap_mask);

	if (!bitmap_mask || bitmap_mask->GetHeight() != m_ptr->GetHeight() || bitmap_mask->GetWidth() != m_ptr->GetWidth())
	{
		return E_INVALIDARG;
	}

	Gdiplus::Rect rect(0, 0, m_ptr->GetWidth(), m_ptr->GetHeight());
	Gdiplus::BitmapData bmpdata_mask = { 0 }, bmpdata_dst = { 0 };

	if (bitmap_mask->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bmpdata_mask) != Gdiplus::Ok)
	{
		return S_OK;
	}

	if (m_ptr->LockBits(&rect, Gdiplus::ImageLockModeRead | Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &bmpdata_dst) != Gdiplus::Ok)
	{
		bitmap_mask->UnlockBits(&bmpdata_mask);
		return S_OK;
	}

	const int width = rect.Width;
	const int height = rect.Height;
	const int size = width * height;
	//const int size_threshold = 512;
	t_uint32* p_mask = reinterpret_cast<t_uint32 *>(bmpdata_mask.Scan0);
	t_uint32* p_dst = reinterpret_cast<t_uint32 *>(bmpdata_dst.Scan0);
	const t_uint32* p_mask_end = p_mask + rect.Width * rect.Height;
	t_uint32 alpha;

	while (p_mask < p_mask_end)
	{
		// Method 1:
		//alpha = (~*p_mask & 0xff) * (*p_dst >> 24) + 0x80;
		//*p_dst = ((((alpha >> 8) + alpha) & 0xff00) << 16) | (*p_dst & 0xffffff);
		// Method 2
		alpha = (((~*p_mask & 0xff) * (*p_dst >> 24)) << 16) & 0xff000000;
		*p_dst = alpha | (*p_dst & 0xffffff);

		++p_mask;
		++p_dst;
	}

	m_ptr->UnlockBits(&bmpdata_dst);
	bitmap_mask->UnlockBits(&bmpdata_mask);

	*p = VARIANT_TRUE;
	return S_OK;
}

STDMETHODIMP GdiBitmap::Clone(float x, float y, float w, float h, IGdiBitmap** pp)
{
	if (!m_ptr || !pp) return E_POINTER;

	*pp = NULL;
	Gdiplus::Bitmap* img = m_ptr->Clone(x, y, w, h, PixelFormat32bppPARGB);
	if (helpers::ensure_gdiplus_object(img))
	{
		*pp = new com_object_impl_t<GdiBitmap>(img);
	}
	else
	{
		if (img) delete img;
		img = NULL;
	}

	return S_OK;
}

STDMETHODIMP GdiBitmap::CreateRawBitmap(IGdiRawBitmap** pp)
{
	if (!m_ptr || !pp) return E_POINTER;

	*pp = new com_object_impl_t<GdiRawBitmap>(m_ptr);
	return S_OK;
}

STDMETHODIMP GdiBitmap::GetGraphics(IGdiGraphics** pp)
{
	if (!m_ptr || !pp) return E_POINTER;

	Gdiplus::Graphics* g = new Gdiplus::Graphics(m_ptr);
	*pp = new com_object_impl_t<GdiGraphics>();
	(*pp)->put__ptr(g);
	return S_OK;
}

STDMETHODIMP GdiBitmap::ReleaseGraphics(IGdiGraphics* p)
{
	if (p)
	{
		Gdiplus::Graphics* g = NULL;
		p->get__ptr((void**)&g);
		p->put__ptr(NULL);
		if (g) delete g;
	}

	return S_OK;
}

STDMETHODIMP GdiBitmap::Resize(UINT w, UINT h, int interpolationMode, IGdiBitmap** pp)
{
	if (!m_ptr || !pp) return E_POINTER;

	Gdiplus::Bitmap* bitmap = new Gdiplus::Bitmap(w, h, PixelFormat32bppPARGB);
	Gdiplus::Graphics g(bitmap);
	g.SetInterpolationMode((Gdiplus::InterpolationMode)interpolationMode);
	g.DrawImage(m_ptr, 0, 0, w, h);
	*pp = new com_object_impl_t<GdiBitmap>(bitmap);
	return S_OK;
}

STDMETHODIMP GdiBitmap::RotateFlip(UINT mode)
{
	if (!m_ptr) return E_POINTER;

	m_ptr->RotateFlip((Gdiplus::RotateFlipType)mode);
	return S_OK;
}

STDMETHODIMP GdiBitmap::SaveAs(BSTR path, BSTR format, VARIANT_BOOL* p)
{
	if (!m_ptr || !p) return E_POINTER;

	CLSID clsid_encoder;
	int ret = helpers::get_encoder_clsid(format, &clsid_encoder);

	if (ret > -1)
	{
		m_ptr->Save(path, &clsid_encoder);
		*p = TO_VARIANT_BOOL(m_ptr->GetLastStatus() == Gdiplus::Ok);
	}
	else
	{
		*p = VARIANT_FALSE;
	}

	return S_OK;
}

STDMETHODIMP GdiBitmap::StackBlur(int radius)
{
	if (!m_ptr) return E_POINTER;

	stack_blur_filter(*m_ptr, radius);
	return S_OK;
}

STDMETHODIMP GdiBitmap::get_Height(UINT* p)
{
	if (!m_ptr || !p) return E_POINTER;

	*p = m_ptr->GetHeight();
	return S_OK;
}

STDMETHODIMP GdiBitmap::get_Width(UINT* p)
{
	if (!m_ptr || !p) return E_POINTER;

	*p = m_ptr->GetWidth();
	return S_OK;
}

GdiFont::GdiFont(Gdiplus::Font* p, HFONT hFont, bool managed) : GdiObj<IGdiFont, Gdiplus::Font>(p), m_hFont(hFont), m_managed(managed) {}
GdiFont::~GdiFont() {}
void GdiFont::FinalRelease()
{
	if (m_hFont && m_managed)
	{
		DeleteFont(m_hFont);
		m_hFont = NULL;
	}

	// call parent
	GdiObj<IGdiFont, Gdiplus::Font>::FinalRelease();
}

STDMETHODIMP GdiFont::get__HFont(UINT* p)
{
	if (!m_ptr || !p) return E_POINTER;

	*p = (UINT)m_hFont;
	return S_OK;
}

STDMETHODIMP GdiFont::get_Height(UINT* p)
{
	if (!m_ptr || !p) return E_POINTER;

	Gdiplus::Bitmap img(1, 1, PixelFormat32bppPARGB);
	Gdiplus::Graphics g(&img);
	*p = (UINT)m_ptr->GetHeight(&g);
	return S_OK;
}

STDMETHODIMP GdiFont::get_Name(BSTR* p)
{
	if (!m_ptr || !p) return E_POINTER;

	WCHAR name[LF_FACESIZE] = { 0 };
	Gdiplus::FontFamily fontFamily;
	m_ptr->GetFamily(&fontFamily);
	fontFamily.GetFamilyName(name, LANG_NEUTRAL);
	*p = SysAllocString(name);
	return S_OK;
}

STDMETHODIMP GdiFont::get_Size(float* p)
{
	if (!m_ptr || !p) return E_POINTER;

	*p = m_ptr->GetSize();
	return S_OK;
}

STDMETHODIMP GdiFont::get_Style(int* p)
{
	if (!m_ptr || !p) return E_POINTER;

	*p = m_ptr->GetStyle();
	return S_OK;
}

GdiGraphics::GdiGraphics() : GdiObj<IGdiGraphics, Gdiplus::Graphics>(NULL) {}

void GdiGraphics::GetRoundRectPath(Gdiplus::GraphicsPath& gp, Gdiplus::RectF& rect, float arc_width, float arc_height)
{
	float arc_dia_w = arc_width * 2;
	float arc_dia_h = arc_height * 2;
	Gdiplus::RectF corner(rect.X, rect.Y, arc_dia_w, arc_dia_h);

	gp.Reset();

	// top left
	gp.AddArc(corner, 180, 90);

	// top right
	corner.X += (rect.Width - arc_dia_w);
	gp.AddArc(corner, 270, 90);

	// bottom right
	corner.Y += (rect.Height - arc_dia_h);
	gp.AddArc(corner, 0, 90);

	// bottom left
	corner.X -= (rect.Width - arc_dia_w);
	gp.AddArc(corner, 90, 90);

	gp.CloseFigure();
}

STDMETHODIMP GdiGraphics::put__ptr(void* p)
{
	m_ptr = (Gdiplus::Graphics *)p;
	return S_OK;
}

STDMETHODIMP GdiGraphics::CalcTextHeight(BSTR str, IGdiFont* font, UINT* p)
{
	if (!m_ptr || !p) return E_POINTER;

	HFONT hFont = NULL;
	font->get__HFont((UINT*)&hFont);
	HDC dc = m_ptr->GetHDC();
	HFONT oldfont = SelectFont(dc, hFont);

	*p = helpers::get_text_height(dc, str, SysStringLen(str));
	SelectFont(dc, oldfont);
	m_ptr->ReleaseHDC(dc);
	return S_OK;
}

STDMETHODIMP GdiGraphics::CalcTextWidth(BSTR str, IGdiFont* font, UINT* p)
{
	if (!m_ptr || !p) return E_POINTER;

	HFONT hFont = NULL;
	font->get__HFont((UINT*)&hFont);
	HDC dc = m_ptr->GetHDC();
	HFONT oldfont = SelectFont(dc, hFont);

	*p = helpers::get_text_width(dc, str, SysStringLen(str));
	SelectFont(dc, oldfont);
	m_ptr->ReleaseHDC(dc);
	return S_OK;
}

STDMETHODIMP GdiGraphics::DrawEllipse(float x, float y, float w, float h, float line_width, VARIANT colour)
{
	if (!m_ptr) return E_POINTER;

	Gdiplus::Pen pen(helpers::get_colour_from_variant(colour), line_width);
	m_ptr->DrawEllipse(&pen, x, y, w, h);
	return S_OK;
}

STDMETHODIMP GdiGraphics::DrawImage(IGdiBitmap* image, float dstX, float dstY, float dstW, float dstH, float srcX, float srcY, float srcW, float srcH, float angle, BYTE alpha)
{
	if (!m_ptr) return E_POINTER;

	Gdiplus::Bitmap* img = NULL;
	image->get__ptr((void**)&img);
	Gdiplus::Matrix old_m;

	if (angle != 0.0)
	{
		Gdiplus::Matrix m;
		Gdiplus::PointF pt;

		pt.X = dstX + dstW / 2;
		pt.Y = dstY + dstH / 2;
		m.RotateAt(angle, pt);
		m_ptr->GetTransform(&old_m);
		m_ptr->SetTransform(&m);
	}

	if (alpha != (BYTE)~0)
	{
		Gdiplus::ImageAttributes ia;
		Gdiplus::ColorMatrix cm = { 0.0f };

		cm.m[0][0] = cm.m[1][1] = cm.m[2][2] = cm.m[4][4] = 1.0f;
		cm.m[3][3] = static_cast<float>(alpha) / 255;

		ia.SetColorMatrix(&cm);

		m_ptr->DrawImage(img, Gdiplus::RectF(dstX, dstY, dstW, dstH), srcX, srcY, srcW, srcH, Gdiplus::UnitPixel, &ia);
	}
	else
	{
		m_ptr->DrawImage(img, Gdiplus::RectF(dstX, dstY, dstW, dstH), srcX, srcY, srcW, srcH, Gdiplus::UnitPixel);
	}

	if (angle != 0.0)
	{
		m_ptr->SetTransform(&old_m);
	}

	return S_OK;
}

STDMETHODIMP GdiGraphics::DrawLine(float x1, float y1, float x2, float y2, float line_width, VARIANT colour)
{
	if (!m_ptr) return E_POINTER;

	Gdiplus::Pen pen(helpers::get_colour_from_variant(colour), line_width);
	m_ptr->DrawLine(&pen, x1, y1, x2, y2);
	return S_OK;
}

STDMETHODIMP GdiGraphics::DrawPolygon(VARIANT colour, float line_width, VARIANT points)
{
	if (!m_ptr) return E_POINTER;

	helpers::array helper;
	if (!helper.convert(&points)) return E_INVALIDARG;
	LONG count = helper.get_count();
	if (count % 2 != 0) return E_INVALIDARG;

	pfc::array_t<Gdiplus::PointF> point_array;
	point_array.set_count(count / 2);

	for (LONG i = 0; i < count / 2; ++i)
	{
		_variant_t varX, varY;

		if (!helper.get_item(i * 2, varX, VT_R4)) return E_INVALIDARG;
		if (!helper.get_item((i * 2) + 1, varY, VT_R4)) return E_INVALIDARG;

		point_array[i].X = varX.fltVal;
		point_array[i].Y = varY.fltVal;
	}

	Gdiplus::Pen pen(helpers::get_colour_from_variant(colour), line_width);
	m_ptr->DrawPolygon(&pen, point_array.get_ptr(), point_array.get_count());
	return S_OK;
}

STDMETHODIMP GdiGraphics::DrawRect(float x, float y, float w, float h, float line_width, VARIANT colour)
{
	if (!m_ptr) return E_POINTER;

	Gdiplus::Pen pen(helpers::get_colour_from_variant(colour), line_width);
	m_ptr->DrawRectangle(&pen, x, y, w, h);
	return S_OK;
}

STDMETHODIMP GdiGraphics::DrawRoundRect(float x, float y, float w, float h, float arc_width, float arc_height, float line_width, VARIANT colour)
{
	if (!m_ptr) return E_POINTER;

	if (2 * arc_width > w || 2 * arc_height > h) return E_INVALIDARG;

	Gdiplus::Pen pen(helpers::get_colour_from_variant(colour), line_width);
	Gdiplus::GraphicsPath gp;
	Gdiplus::RectF rect(x, y, w, h);
	GetRoundRectPath(gp, rect, arc_width, arc_height);
	pen.SetStartCap(Gdiplus::LineCapRound);
	pen.SetEndCap(Gdiplus::LineCapRound);
	m_ptr->DrawPath(&pen, &gp);
	return S_OK;
}

STDMETHODIMP GdiGraphics::DrawString(BSTR str, IGdiFont* font, VARIANT colour, float x, float y, float w, float h, int flags)
{
	if (!m_ptr) return E_POINTER;

	Gdiplus::Font* fn = NULL;
	font->get__ptr((void**)&fn);
	Gdiplus::SolidBrush br(helpers::get_colour_from_variant(colour));
	Gdiplus::StringFormat fmt(Gdiplus::StringFormat::GenericTypographic());

	if (flags != 0)
	{
		fmt.SetAlignment((Gdiplus::StringAlignment)((flags >> 28) & 0x3)); //0xf0000000
		fmt.SetLineAlignment((Gdiplus::StringAlignment)((flags >> 24) & 0x3)); //0x0f000000
		fmt.SetTrimming((Gdiplus::StringTrimming)((flags >> 20) & 0x7)); //0x00f00000
		fmt.SetFormatFlags((Gdiplus::StringFormatFlags)(flags & 0x7FFF)); //0x0000ffff
	}

	m_ptr->DrawString(str, -1, fn, Gdiplus::RectF(x, y, w, h), &fmt, &br);
	return S_OK;
}

STDMETHODIMP GdiGraphics::EstimateLineWrap(BSTR str, IGdiFont* font, int max_width, VARIANT* p)
{
	if (!m_ptr || !p) return E_POINTER;

	HFONT hFont = NULL;
	font->get__HFont((UINT*)&hFont);
	HDC dc = m_ptr->GetHDC();
	HFONT oldfont = SelectFont(dc, hFont);

	pfc::list_t<helpers::wrapped_item> result;
	estimate_line_wrap(dc, str, SysStringLen(str), max_width, result);
	SelectFont(dc, oldfont);
	m_ptr->ReleaseHDC(dc);

	LONG count = result.get_count() * 2;
	helpers::array helper;
	if (!helper.create(count)) return E_OUTOFMEMORY;
	for (LONG i = 0; i < count / 2; ++i)
	{
		_variant_t var1, var2;
		var1.vt = VT_BSTR;
		var1.bstrVal = result[i].text;
		var2.vt = VT_I4;
		var2.lVal = result[i].width;
		if (!helper.put_item(i * 2, var1)) return E_OUTOFMEMORY;
		if (!helper.put_item((i * 2) + 1, var2)) return E_OUTOFMEMORY;
	}
	p->vt = VT_ARRAY | VT_VARIANT;
	p->parray = helper.get_ptr();
	return S_OK;
}

STDMETHODIMP GdiGraphics::FillEllipse(float x, float y, float w, float h, VARIANT colour)
{
	if (!m_ptr) return E_POINTER;

	Gdiplus::SolidBrush br(helpers::get_colour_from_variant(colour));
	m_ptr->FillEllipse(&br, x, y, w, h);
	return S_OK;
}

STDMETHODIMP GdiGraphics::FillGradRect(float x, float y, float w, float h, float angle, VARIANT colour1, VARIANT colour2, float focus)
{
	if (!m_ptr) return E_POINTER;

	Gdiplus::RectF rect(x, y, w, h);
	Gdiplus::LinearGradientBrush brush(rect, helpers::get_colour_from_variant(colour1), helpers::get_colour_from_variant(colour2), angle, TRUE);
	brush.SetBlendTriangularShape(focus);
	m_ptr->FillRectangle(&brush, rect);
	return S_OK;
}

STDMETHODIMP GdiGraphics::FillPolygon(VARIANT colour, int fillmode, VARIANT points)
{
	if (!m_ptr) return E_POINTER;

	helpers::array helper;
	if (!helper.convert(&points)) return E_INVALIDARG;
	LONG count = helper.get_count();
	if (count % 2 != 0) return E_INVALIDARG;

	pfc::array_t<Gdiplus::PointF> point_array;
	point_array.set_count(count / 2);

	for (LONG i = 0; i < count / 2; ++i)
	{
		_variant_t varX, varY;

		if (!helper.get_item(i * 2, varX, VT_R4)) return E_INVALIDARG;
		if (!helper.get_item((i * 2) + 1, varY, VT_R4)) return E_INVALIDARG;

		point_array[i].X = varX.fltVal;
		point_array[i].Y = varY.fltVal;
	}

	Gdiplus::SolidBrush br(helpers::get_colour_from_variant(colour));
	m_ptr->FillPolygon(&br, point_array.get_ptr(), point_array.get_count(), (Gdiplus::FillMode)fillmode);
	return S_OK;
}

STDMETHODIMP GdiGraphics::FillRoundRect(float x, float y, float w, float h, float arc_width, float arc_height, VARIANT colour)
{
	if (!m_ptr) return E_POINTER;

	if (2 * arc_width > w || 2 * arc_height > h) return E_INVALIDARG;

	Gdiplus::SolidBrush br(helpers::get_colour_from_variant(colour));
	Gdiplus::GraphicsPath gp;
	Gdiplus::RectF rect(x, y, w, h);
	GetRoundRectPath(gp, rect, arc_width, arc_height);
	m_ptr->FillPath(&br, &gp);
	return S_OK;
}

STDMETHODIMP GdiGraphics::FillSolidRect(float x, float y, float w, float h, VARIANT colour)
{
	if (!m_ptr) return E_POINTER;

	Gdiplus::SolidBrush brush(helpers::get_colour_from_variant(colour));
	m_ptr->FillRectangle(&brush, x, y, w, h);
	return S_OK;
}

STDMETHODIMP GdiGraphics::GdiAlphaBlend(IGdiRawBitmap* bitmap, int dstX, int dstY, int dstW, int dstH, int srcX, int srcY, int srcW, int srcH, BYTE alpha)
{
	if (!m_ptr) return E_POINTER;

	HDC src_dc = NULL;
	bitmap->get__Handle(&src_dc);
	HDC dc = m_ptr->GetHDC();
	BLENDFUNCTION bf = { AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA };

	::GdiAlphaBlend(dc, dstX, dstY, dstW, dstH, src_dc, srcX, srcY, srcW, srcH, bf);
	m_ptr->ReleaseHDC(dc);
	return S_OK;
}

STDMETHODIMP GdiGraphics::GdiDrawBitmap(IGdiRawBitmap* bitmap, int dstX, int dstY, int dstW, int dstH, int srcX, int srcY, int srcW, int srcH)
{
	if (!m_ptr) return E_POINTER;

	HDC src_dc = NULL;
	bitmap->get__Handle(&src_dc);
	HDC dc = m_ptr->GetHDC();

	if (dstW == srcW && dstH == srcH)
	{
		BitBlt(dc, dstX, dstY, dstW, dstH, src_dc, srcX, srcY, SRCCOPY);
	}
	else
	{
		SetStretchBltMode(dc, HALFTONE);
		SetBrushOrgEx(dc, 0, 0, NULL);
		StretchBlt(dc, dstX, dstY, dstW, dstH, src_dc, srcX, srcY, srcW, srcH, SRCCOPY);
	}

	m_ptr->ReleaseHDC(dc);
	return S_OK;
}

STDMETHODIMP GdiGraphics::GdiDrawText(BSTR str, IGdiFont* font, VARIANT colour, int x, int y, int w, int h, int format)
{
	if (!m_ptr) return E_POINTER;

	HFONT hFont = NULL;
	font->get__HFont((UINT*)&hFont);
	HDC dc = m_ptr->GetHDC();
	HFONT oldfont = SelectFont(dc, hFont);

	RECT rc = { x, y, x + w, y + h };
	DRAWTEXTPARAMS dpt = { sizeof(DRAWTEXTPARAMS), 4, 0, 0, 0 };

	SetTextColor(dc, helpers::convert_argb_to_colorref(helpers::get_colour_from_variant(colour)));
	SetBkMode(dc, TRANSPARENT);
	SetTextAlign(dc, TA_LEFT | TA_TOP | TA_NOUPDATECP);

	// Remove DT_MODIFYSTRING flag
	if (format & DT_MODIFYSTRING)
	{
		format &= ~DT_MODIFYSTRING;
	}

	// Well, magic :P
	if (format & DT_CALCRECT)
	{
		RECT rc_calc = { 0 }, rc_old = { 0 };

		memcpy(&rc_calc, &rc, sizeof(RECT));
		memcpy(&rc_old, &rc, sizeof(RECT));

		DrawText(dc, str, -1, &rc_calc, format);

		format &= ~DT_CALCRECT;

		// adjust vertical align
		if (format & DT_VCENTER)
		{
			rc.top = rc_old.top + (((rc_old.bottom - rc_old.top) - (rc_calc.bottom - rc_calc.top)) >> 1);
			rc.bottom = rc.top + (rc_calc.bottom - rc_calc.top);
		}
		else if (format & DT_BOTTOM)
		{
			rc.top = rc_old.bottom - (rc_calc.bottom - rc_calc.top);
		}
	}

	DrawTextEx(dc, str, -1, &rc, format, &dpt);
	SelectFont(dc, oldfont);
	m_ptr->ReleaseHDC(dc);
	return S_OK;
}

STDMETHODIMP GdiGraphics::SetInterpolationMode(int mode)
{
	if (!m_ptr) return E_POINTER;

	m_ptr->SetInterpolationMode((Gdiplus::InterpolationMode)mode);
	return S_OK;
}

STDMETHODIMP GdiGraphics::SetSmoothingMode(int mode)
{
	if (!m_ptr) return E_POINTER;

	m_ptr->SetSmoothingMode((Gdiplus::SmoothingMode)mode);
	return S_OK;
}

STDMETHODIMP GdiGraphics::SetTextRenderingHint(UINT mode)
{
	if (!m_ptr) return E_POINTER;

	m_ptr->SetTextRenderingHint((Gdiplus::TextRenderingHint)mode);
	return S_OK;
}

GdiRawBitmap::GdiRawBitmap(Gdiplus::Bitmap* p_bmp)
{
	PFC_ASSERT(p_bmp != NULL);
	m_width = p_bmp->GetWidth();
	m_height = p_bmp->GetHeight();

	m_hdc = CreateCompatibleDC(NULL);
	m_hbmp = helpers::create_hbitmap_from_gdiplus_bitmap(p_bmp);
	m_hbmpold = SelectBitmap(m_hdc, m_hbmp);
}

GdiRawBitmap::~GdiRawBitmap() {}

void GdiRawBitmap::FinalRelease()
{
	if (m_hdc)
	{
		SelectBitmap(m_hdc, m_hbmpold);
		DeleteDC(m_hdc);
		m_hdc = NULL;
	}

	if (m_hbmp)
	{
		DeleteBitmap(m_hbmp);
		m_hbmp = NULL;
	}
}

STDMETHODIMP GdiRawBitmap::get_Height(UINT* p)
{
	if (!m_hdc || !p) return E_POINTER;

	*p = m_height;
	return S_OK;
}

STDMETHODIMP GdiRawBitmap::get_Width(UINT* p)
{
	if (!m_hdc || !p) return E_POINTER;

	*p = m_width;
	return S_OK;
}

STDMETHODIMP GdiRawBitmap::get__Handle(HDC* p)
{
	if (!m_hdc || !p) return E_POINTER;

	*p = m_hdc;
	return S_OK;
}

STDMETHODIMP GdiUtils::CreateImage(int w, int h, IGdiBitmap** pp)
{
	if (!pp) return E_POINTER;

	*pp = NULL;
	Gdiplus::Bitmap* img = new Gdiplus::Bitmap(w, h, PixelFormat32bppPARGB);
	if (helpers::ensure_gdiplus_object(img))
	{
		*pp = new com_object_impl_t<GdiBitmap>(img);
	}
	else
	{
		if (img) delete img;
		img = NULL;
	}

	return S_OK;
}

STDMETHODIMP GdiUtils::Font(BSTR name, float pxSize, int style, IGdiFont** pp)
{
	if (!pp) return E_POINTER;

	Gdiplus::Font* font = new Gdiplus::Font(name, pxSize, style, Gdiplus::UnitPixel);
	if (helpers::ensure_gdiplus_object(font))
	{
		// Generate HFONT
		// The benefit of replacing Gdiplus::Font::GetLogFontW is that you can get it work with CCF/OpenType fonts.
		HFONT hFont = CreateFont(
			-(int)pxSize,
			0,
			0,
			0,
			(style & Gdiplus::FontStyleBold) ? FW_BOLD : FW_NORMAL,
			(style & Gdiplus::FontStyleItalic) ? TRUE : FALSE,
			(style & Gdiplus::FontStyleUnderline) ? TRUE : FALSE,
			(style & Gdiplus::FontStyleStrikeout) ? TRUE : FALSE,
			DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE,
			name);
		*pp = new com_object_impl_t<GdiFont>(font, hFont);
	}
	else
	{
		if (font) delete font;
		*pp = NULL;
	}

	return S_OK;
}

STDMETHODIMP GdiUtils::Image(BSTR path, IGdiBitmap** pp)
{
	if (!pp) return E_POINTER;

	*pp = helpers::load_image(path);
	return S_OK;
}

STDMETHODIMP GdiUtils::LoadImageAsync(UINT window_id, BSTR path, UINT* p)
{
	if (!p) return E_POINTER;

	unsigned cookie = 0;

	try
	{
		helpers::load_image_async* task = new helpers::load_image_async((HWND)window_id, path);

		if (simple_thread_pool::instance().enqueue(task))
			cookie = reinterpret_cast<unsigned>(task);
		else
			delete task;
	}
	catch (...) {}

	*p = cookie;
	return S_OK;
}

JSConsole::JSConsole() {}
JSConsole::~JSConsole() {}

STDMETHODIMP JSConsole::Log(SAFEARRAY* p)
{
	pfc::string8_fast str;
	LONG nLBound = 0, nUBound = -1;
	HRESULT hr;

	if (FAILED(hr = SafeArrayGetLBound(p, 1, &nLBound)))
		return hr;

	if (FAILED(hr = SafeArrayGetUBound(p, 1, &nUBound)))
		return hr;

	for (LONG i = nLBound; i <= nUBound; ++i)
	{
		_variant_t var;
		LONG n = i;

		if (FAILED(SafeArrayGetElement(p, &n, &var)))
			continue;

		if (FAILED(hr = VariantChangeType(&var, &var, VARIANT_ALPHABOOL, VT_BSTR)))
			continue;

		str.add_string(string_utf8_from_wide(var.bstrVal));

		if (i < nUBound)
		{
			str.add_byte(' ');
		}
	}

	console::info(str);
	return S_OK;
}

JSUtils::JSUtils() {}
JSUtils::~JSUtils() {}

STDMETHODIMP JSUtils::CheckComponent(BSTR name, VARIANT_BOOL is_dll, VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	service_enum_t<componentversion> e;
	componentversion::ptr ptr;
	string_utf8_from_wide uname(name);
	pfc::string8_fast temp;

	*p = VARIANT_FALSE;

	while (e.next(ptr))
	{
		if (is_dll != VARIANT_FALSE)
		{
			ptr->get_file_name(temp);
		}
		else
		{
			ptr->get_component_name(temp);
		}

		if (_stricmp(temp, uname) == 0)
		{
			*p = VARIANT_TRUE;
			break;
		}
	}

	return S_OK;
}

STDMETHODIMP JSUtils::CheckFont(BSTR name, VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	WCHAR family_name_eng[LF_FACESIZE] = { 0 };
	WCHAR family_name_loc[LF_FACESIZE] = { 0 };
	Gdiplus::InstalledFontCollection font_collection;
	Gdiplus::FontFamily* font_families;
	int count = font_collection.GetFamilyCount();
	int recv;

	*p = VARIANT_FALSE;
	font_families = new Gdiplus::FontFamily[count];
	font_collection.GetFamilies(count, font_families, &recv);

	if (recv == count)
	{
		// Find
		for (int i = 0; i < count; ++i)
		{
			font_families[i].GetFamilyName(family_name_eng, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US));
			font_families[i].GetFamilyName(family_name_loc);

			if (_wcsicmp(name, family_name_loc) == 0 || _wcsicmp(name, family_name_eng) == 0)
			{
				*p = VARIANT_TRUE;
				break;
			}
		}
	}

	delete[] font_families;
	return S_OK;
}

STDMETHODIMP JSUtils::ColourPicker(UINT window_id, int default_colour, int* p)
{
	if (!p) return E_POINTER;

	COLORREF COLOR = helpers::convert_argb_to_colorref(default_colour);
	COLORREF COLORS[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	uChooseColor(&COLOR, (HWND)window_id, &COLORS[0]);
	*p = helpers::convert_colorref_to_argb(COLOR);
	return S_OK;
}

STDMETHODIMP JSUtils::FormatDuration(double seconds, BSTR* p)
{
	if (!p) return E_POINTER;

	pfc::string8_fast str = pfc::format_time_ex(seconds, 0);
	*p = SysAllocString(string_wide_from_utf8_fast(str));
	return S_OK;
}

STDMETHODIMP JSUtils::FormatFileSize(LONGLONG bytes, BSTR* p)
{
	if (!p) return E_POINTER;

	pfc::string8_fast str = pfc::format_file_size_short(bytes);
	*p = SysAllocString(string_wide_from_utf8_fast(str));
	return S_OK;
}

STDMETHODIMP JSUtils::GetAlbumArtAsync(UINT window_id, IFbMetadbHandle* handle, UINT art_id, UINT* p)
{
	if (!p) return E_POINTER;

	unsigned cookie = 0;
	metadb_handle* ptr = NULL;
	handle->get__ptr((void**)&ptr);

	if (ptr)
	{
		try
		{
			helpers::album_art_async* task = new helpers::album_art_async((HWND)window_id, ptr, art_id);

			if (simple_thread_pool::instance().enqueue(task))
				cookie = reinterpret_cast<unsigned>(task);
			else
				delete task;
		}
		catch (...)
		{
			cookie = 0;
		}
	}
	else
	{
		cookie = 0;
	}

	*p = cookie;
	return S_OK;
}

STDMETHODIMP JSUtils::GetSysColour(UINT index, int* p)
{
	if (!p) return E_POINTER;

	*p = 0;
	if (::GetSysColorBrush(index) != NULL)
	{
		*p = helpers::convert_colorref_to_argb(::GetSysColor(index));
	}
	return S_OK;
}

STDMETHODIMP JSUtils::GetSystemMetrics(UINT index, int* p)
{
	if (!p) return E_POINTER;

	*p = ::GetSystemMetrics(index);
	return S_OK;
}

STDMETHODIMP JSUtils::InputBox(UINT window_id, BSTR prompt, BSTR caption, BSTR def, VARIANT_BOOL error_on_cancel, BSTR* p)
{
	if (!p) return E_POINTER;

	modal_dialog_scope scope;
	if (scope.can_create())
	{
		scope.initialize(HWND(window_id));

		string_utf8_from_wide uprompt(prompt);
		string_utf8_from_wide ucaption(caption);
		string_utf8_from_wide udef(def);

		CInputBox dlg(uprompt, ucaption, udef);
		int status = dlg.DoModal(HWND(window_id));
		if (status == IDOK)
		{
			pfc::string8 val;
			dlg.GetValue(val);
			*p = SysAllocString(string_wide_from_utf8_fast(val));
		}
		else if (status == IDCANCEL)
		{
			if (error_on_cancel != VARIANT_FALSE)
			{
				return E_FAIL;
			}
			*p = SysAllocString(def);
		}
	}
	return S_OK;
}

STDMETHODIMP JSUtils::IsKeyPressed(UINT vkey, VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	*p = TO_VARIANT_BOOL(::IsKeyPressed(vkey));
	return S_OK;
}

STDMETHODIMP JSUtils::PathWildcardMatch(BSTR pattern, BSTR str, VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	*p = TO_VARIANT_BOOL(PathMatchSpec(str, pattern));
	return S_OK;
}

STDMETHODIMP JSUtils::ReadINI(BSTR filename, BSTR section, BSTR key, VARIANT defaultval, BSTR* p)
{
	if (!p) return E_POINTER;

	enum
	{
		BUFFER_LEN = 255
	};
	TCHAR buff[BUFFER_LEN] = { 0 };

	GetPrivateProfileString(section, key, NULL, buff, BUFFER_LEN, filename);

	if (!buff[0])
	{
		_variant_t var;

		if (SUCCEEDED(VariantChangeType(&var, &defaultval, 0, VT_BSTR)))
		{
			*p = SysAllocString(var.bstrVal);
			return S_OK;
		}
	}
	*p = SysAllocString(buff);
	return S_OK;
}

STDMETHODIMP JSUtils::ReadTextFile(BSTR filename, BSTR* p)
{
	if (!p) return E_POINTER;

	*p = NULL;
	pfc::array_t<wchar_t> content;
	if (helpers::read_file_wide(filename, content))
	{
		*p = SysAllocString(content.get_ptr());
	}
	return S_OK;
}

STDMETHODIMP JSUtils::WriteINI(BSTR filename, BSTR section, BSTR key, VARIANT val, VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	_variant_t var;
	HRESULT hr;
	if (FAILED(hr = VariantChangeType(&var, &val, 0, VT_BSTR))) return hr;
	*p = TO_VARIANT_BOOL(WritePrivateProfileString(section, key, var.bstrVal, filename));
	return S_OK;
}

STDMETHODIMP JSUtils::WriteTextFile(BSTR filename, BSTR content, VARIANT_BOOL write_bom, VARIANT_BOOL* p)
{
	if (!p) return E_POINTER;

	*p = VARIANT_FALSE;
	if (content != NULL)
	{
		*p = TO_VARIANT_BOOL(helpers::write_file(string_utf8_from_wide(filename), string_utf8_from_wide(content), write_bom != VARIANT_FALSE));
	}
	return S_OK;
}

MainMenuManager::MainMenuManager() {}
MainMenuManager::~MainMenuManager() {}
void MainMenuManager::FinalRelease()
{
	m_mm.release();
}

STDMETHODIMP MainMenuManager::BuildMenu(IMenuObj* p, int base_id, int count)
{
	if (m_mm.is_empty()) return E_POINTER;

	HMENU menuid;
	p->get__ID(&menuid);

	// HACK: workaround for foo_menu_addons
	try
	{
		m_mm->generate_menu_win32(menuid, base_id, count, mainmenu_manager::flag_show_shortcuts);
	}
	catch (...) {}

	return S_OK;
}

STDMETHODIMP MainMenuManager::ExecuteByID(UINT id, VARIANT_BOOL* p)
{
	if (m_mm.is_empty() || !p) return E_POINTER;

	*p = TO_VARIANT_BOOL(m_mm->execute_command(id));
	return S_OK;
}

STDMETHODIMP MainMenuManager::Init(BSTR root_name)
{
	struct t_valid_root_name
	{
		const wchar_t* name;
		const GUID* guid;
	};

	// In mainmenu_groups:
	// static const GUID file,view,edit,playback,library,help;
	const t_valid_root_name valid_root_names[] =
	{
		{ L"file", &mainmenu_groups::file },
		{ L"view", &mainmenu_groups::view },
		{ L"edit", &mainmenu_groups::edit },
		{ L"playback", &mainmenu_groups::playback },
		{ L"library", &mainmenu_groups::library },
		{ L"help", &mainmenu_groups::help }
	};

	// Find
	for (int i = 0; i < _countof(valid_root_names); ++i)
	{
		if (_wcsicmp(root_name, valid_root_names[i].name) == 0)
		{
			// found
			m_mm = standard_api_create_t<mainmenu_manager>();
			m_mm->instantiate(*valid_root_names[i].guid);
			return S_OK;
		}
	}
	return E_INVALIDARG;
}

MenuObj::MenuObj(HWND wnd_parent) : m_wnd_parent(wnd_parent), m_has_detached(false)
{
	m_hMenu = ::CreatePopupMenu();
}

MenuObj::~MenuObj() {}

void MenuObj::FinalRelease()
{
	if (!m_has_detached && m_hMenu && IsMenu(m_hMenu))
	{
		DestroyMenu(m_hMenu);
		m_hMenu = NULL;
	}
}

STDMETHODIMP MenuObj::get__ID(HMENU* p)
{
	if (!m_hMenu || !p) return E_POINTER;

	*p = m_hMenu;
	return S_OK;
}

STDMETHODIMP MenuObj::AppendMenuItem(UINT flags, UINT item_id, BSTR text)
{
	if (!m_hMenu) return E_POINTER;
	if (flags & MF_POPUP) return E_INVALIDARG;

	::AppendMenu(m_hMenu, flags, item_id, text);
	return S_OK;
}

STDMETHODIMP MenuObj::AppendMenuSeparator()
{
	if (!m_hMenu) return E_POINTER;

	::AppendMenu(m_hMenu, MF_SEPARATOR, 0, 0);
	return S_OK;
}

STDMETHODIMP MenuObj::AppendTo(IMenuObj* parent, UINT flags, BSTR text)
{
	if (!m_hMenu) return E_POINTER;

	MenuObj* pMenuParent = static_cast<MenuObj *>(parent);
	if (::AppendMenu(pMenuParent->m_hMenu, flags | MF_STRING | MF_POPUP, UINT_PTR(m_hMenu), text))
	{
		m_has_detached = true;
	}
	return S_OK;
}

STDMETHODIMP MenuObj::CheckMenuItem(UINT item_id, VARIANT_BOOL check)
{
	if (!m_hMenu) return E_POINTER;

	::CheckMenuItem(m_hMenu, item_id, check != VARIANT_FALSE ? MF_CHECKED : MF_UNCHECKED);
	return S_OK;
}

STDMETHODIMP MenuObj::CheckMenuRadioItem(UINT first, UINT last, UINT selected)
{
	if (!m_hMenu) return E_POINTER;

	::CheckMenuRadioItem(m_hMenu, first, last, selected, MF_BYCOMMAND);
	return S_OK;
}

STDMETHODIMP MenuObj::TrackPopupMenu(int x, int y, UINT flags, UINT* p)
{
	if (!m_hMenu || !p) return E_POINTER;

	// Only include specified flags
	flags |= TPM_NONOTIFY | TPM_RETURNCMD | TPM_RIGHTBUTTON;
	flags &= ~TPM_RECURSE;

	POINT pt = { x, y };
	ClientToScreen(m_wnd_parent, &pt);
	*p = ::TrackPopupMenu(m_hMenu, flags, pt.x, pt.y, 0, m_wnd_parent, 0);
	return S_OK;
}

ThemeManager::ThemeManager(HWND hwnd, BSTR classlist) : m_theme(NULL), m_partid(0), m_stateid(0)
{
	m_theme = OpenThemeData(hwnd, classlist);

	if (!m_theme) throw pfc::exception_invalid_params();
}

ThemeManager::~ThemeManager() {}

void ThemeManager::FinalRelease()
{
	if (m_theme)
	{
		CloseThemeData(m_theme);
		m_theme = NULL;
	}
}

STDMETHODIMP ThemeManager::DrawThemeBackground(IGdiGraphics* gr, int x, int y, int w, int h, int clip_x, int clip_y, int clip_w, int clip_h)
{
	if (!m_theme) return E_POINTER;

	Gdiplus::Graphics* graphics = NULL;
	gr->get__ptr((void**)&graphics);

	RECT rc = { x, y, x + w, y + h };
	RECT clip_rc = { clip_x, clip_y, clip_x + clip_w, clip_y + clip_h };
	LPCRECT pclip_rc = &clip_rc;
	HDC dc = graphics->GetHDC();

	if (clip_x == 0 && clip_y == 0 && clip_w == 0 && clip_h == 0)
	{
		pclip_rc = NULL;
	}

	HRESULT hr = ::DrawThemeBackground(m_theme, dc, m_partid, m_stateid, &rc, pclip_rc);

	graphics->ReleaseHDC(dc);
	return hr;
}

STDMETHODIMP ThemeManager::IsThemePartDefined(int partid, int stateid, VARIANT_BOOL* p)
{
	if (!m_theme || !p) return E_POINTER;

	*p = TO_VARIANT_BOOL(::IsThemePartDefined(m_theme, partid, stateid));
	return S_OK;
}

STDMETHODIMP ThemeManager::SetPartAndStateID(int partid, int stateid)
{
	if (!m_theme) return E_POINTER;

	m_partid = partid;
	m_stateid = stateid;
	return S_OK;
}
