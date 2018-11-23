#pragma once
#include "com_tools.h"
#include "script_interface.h"

struct panel_tooltip_param
{
	panel_tooltip_param() : tooltip_hwnd(0) {}

	HWND tooltip_hwnd;
	SIZE tooltip_size;
	BSTR font_name;
	float font_size;
	int font_style;
};

typedef std::shared_ptr<panel_tooltip_param> panel_tooltip_param_ptr;

class ContextMenuManager : public IDisposableImpl4<IContextMenuManager>
{
protected:
	ContextMenuManager();
	virtual ~ContextMenuManager();
	virtual void FinalRelease();

	contextmenu_manager::ptr m_cm;

public:
	STDMETHODIMP BuildMenu(IMenuObj* p, int base_id);
	STDMETHODIMP ExecuteByID(UINT id, VARIANT_BOOL* p);
	STDMETHODIMP InitContext(IFbMetadbHandleList* handles);
	STDMETHODIMP InitContextPlaylist();
	STDMETHODIMP InitNowPlaying();
};

class DropSourceAction : public IDisposableImpl4<IDropSourceAction>
{
protected:
	DropSourceAction();
	virtual ~DropSourceAction();
	virtual void FinalRelease();

	DWORD m_effect;
	bool m_to_select;
	t_size m_base;
	t_size m_playlist_idx;

public:
	DWORD& Effect()
	{
		return m_effect;
	}

	bool& ToSelect()
	{
		return m_to_select;
	}

	t_size& Base()
	{
		return m_base;
	}

	t_size& Playlist()
	{
		return m_playlist_idx;
	}

	void Reset()
	{
		m_playlist_idx = 0;
		m_base = 0;
		m_to_select = true;
		m_effect = DROPEFFECT_NONE;
	}

	STDMETHODIMP get_Effect(UINT* p);
	STDMETHODIMP put_Base(UINT base);
	STDMETHODIMP put_Effect(UINT effect);
	STDMETHODIMP put_Playlist(UINT id);
	STDMETHODIMP put_ToSelect(VARIANT_BOOL to_select);
};

class FbFileInfo : public IDisposableImpl4<IFbFileInfo>
{
protected:
	FbFileInfo(file_info_impl* p_info_ptr);
	virtual ~FbFileInfo();
	virtual void FinalRelease();

	file_info_impl* m_info_ptr;

public:
	STDMETHODIMP get__ptr(void** pp);
	STDMETHODIMP InfoFind(BSTR name, int* p);
	STDMETHODIMP InfoName(UINT idx, BSTR* p);
	STDMETHODIMP InfoValue(UINT idx, BSTR* p);
	STDMETHODIMP MetaFind(BSTR name, int* p);
	STDMETHODIMP MetaName(UINT idx, BSTR* p);
	STDMETHODIMP MetaValue(UINT idx, UINT vidx, BSTR* p);
	STDMETHODIMP MetaValueCount(UINT idx, UINT* p);
	STDMETHODIMP get_InfoCount(UINT* p);
	STDMETHODIMP get_MetaCount(UINT* p);
};

class FbMetadbHandle : public IDisposableImpl4<IFbMetadbHandle>
{
protected:
	FbMetadbHandle(const metadb_handle_ptr& src);
	FbMetadbHandle(metadb_handle* src);
	virtual ~FbMetadbHandle();
	virtual void FinalRelease();

	metadb_handle_ptr m_handle;

public:
	STDMETHODIMP get__ptr(void** pp);
	STDMETHODIMP ClearStats();
	STDMETHODIMP Compare(IFbMetadbHandle* handle, VARIANT_BOOL* p);
	STDMETHODIMP GetAlbumArt(UINT art_id, VARIANT* p);
	STDMETHODIMP GetAlbumArtEmbedded(UINT art_id, IGdiBitmap** pp);
	STDMETHODIMP GetFileInfo(IFbFileInfo** pp);
	STDMETHODIMP IsInMediaLibrary(VARIANT_BOOL* p);
	STDMETHODIMP RefreshStats();
	STDMETHODIMP RunContextCommand(BSTR command, VARIANT_BOOL* p);
	STDMETHODIMP SetFirstPlayed(BSTR first_played);
	STDMETHODIMP SetLastPlayed(BSTR last_played);
	STDMETHODIMP SetLoved(UINT loved);
	STDMETHODIMP SetPlaycount(UINT playcount);
	STDMETHODIMP SetRating(UINT rating);
	STDMETHODIMP get_FileSize(LONGLONG* p);
	STDMETHODIMP get_Length(double* p);
	STDMETHODIMP get_Path(BSTR* p);
	STDMETHODIMP get_RawPath(BSTR* p);
	STDMETHODIMP get_SubSong(UINT* p);
};

class FbMetadbHandleList : public IDisposableImpl4<IFbMetadbHandleList>
{
protected:
	FbMetadbHandleList(metadb_handle_list_cref handles);
	virtual ~FbMetadbHandleList();
	virtual void FinalRelease();

	metadb_handle_list m_handles;

public:
	STDMETHODIMP get__ptr(void** pp);
	STDMETHODIMP AddItem(IFbMetadbHandle* handle);
	STDMETHODIMP AddItems(IFbMetadbHandleList* handles);
	STDMETHODIMP AttachImage(BSTR image_path, UINT art_id);
	STDMETHODIMP BSearch(IFbMetadbHandle* handle, int* p);
	STDMETHODIMP CalcTotalDuration(double* p);
	STDMETHODIMP CalcTotalSize(LONGLONG* p);
	STDMETHODIMP Clone(IFbMetadbHandleList** pp);
	STDMETHODIMP Convert(VARIANT* p);
	STDMETHODIMP CopyToClipboard(VARIANT_BOOL* p);
	STDMETHODIMP DoDragDrop(UINT okEffects, UINT* p);
	STDMETHODIMP Find(IFbMetadbHandle* handle, int* p);
	STDMETHODIMP GetLibraryRelativePaths(VARIANT* p);
	STDMETHODIMP GetQueryItems(BSTR query, IFbMetadbHandleList** pp);
	STDMETHODIMP InsertItem(UINT index, IFbMetadbHandle* handle);
	STDMETHODIMP InsertItems(UINT index, IFbMetadbHandleList* handles);
	STDMETHODIMP MakeDifference(IFbMetadbHandleList* handles);
	STDMETHODIMP MakeIntersection(IFbMetadbHandleList* handles);
	STDMETHODIMP MakeUnion(IFbMetadbHandleList* handles);
	STDMETHODIMP RefreshStats();
	STDMETHODIMP Remove(IFbMetadbHandle* handle);
	STDMETHODIMP RemoveAll();
	STDMETHODIMP RemoveAttachedImage(UINT art_id);
	STDMETHODIMP RemoveAttachedImages();
	STDMETHODIMP RemoveByIdx(UINT index);
	STDMETHODIMP RemoveFromIdx(UINT from, UINT count);
	STDMETHODIMP RunContextCommand(BSTR command, VARIANT_BOOL* p);
	STDMETHODIMP Sort();
	STDMETHODIMP SortByFormat(BSTR pattern, int direction);
	STDMETHODIMP SortByPath();
	STDMETHODIMP SortByRelativePath();
	STDMETHODIMP UpdateFileInfo(BSTR str);
	STDMETHODIMP get_Count(UINT* p);
	STDMETHODIMP get_Item(UINT index, IFbMetadbHandle** pp);
	STDMETHODIMP put_Item(UINT index, IFbMetadbHandle* handle);
};

class FbPlayingItemLocation : public IDispatchImpl3<IFbPlayingItemLocation>
{
protected:
	FbPlayingItemLocation(bool isValid, t_size playlistIndex, t_size playlistItemIndex);

	bool m_isValid;
	t_size m_playlistIndex;
	t_size m_playlistItemIndex;

public:
	STDMETHODIMP get_IsValid(VARIANT_BOOL* p);
	STDMETHODIMP get_PlaylistIndex(int* p);
	STDMETHODIMP get_PlaylistItemIndex(int* p);
};

class FbPlaylistManager : public IDispatchImpl3<IFbPlaylistManager>
{
protected:
	FbPlaylistManager();
	virtual ~FbPlaylistManager();

public:
	STDMETHODIMP AddLocations(UINT playlistIndex, VARIANT locations, VARIANT_BOOL select);
	STDMETHODIMP ClearPlaylist(UINT playlistIndex);
	STDMETHODIMP ClearPlaylistSelection(UINT playlistIndex);
	STDMETHODIMP CreateAutoPlaylist(UINT playlistIndex, BSTR name, BSTR query, BSTR sort, UINT flags, int* p);
	STDMETHODIMP CreatePlaylist(UINT playlistIndex, BSTR name, int* p);
	STDMETHODIMP DuplicatePlaylist(UINT from, BSTR name, UINT* p);
	STDMETHODIMP ExecutePlaylistDefaultAction(UINT playlistIndex, UINT playlistItemIndex, VARIANT_BOOL* p);
	STDMETHODIMP FindOrCreatePlaylist(BSTR name, VARIANT_BOOL unlocked, int* p);
	STDMETHODIMP FindPlaylist(BSTR name, int* p);
	STDMETHODIMP GetPlayingItemLocation(IFbPlayingItemLocation** pp);
	STDMETHODIMP GetPlaylistFocusItemIndex(UINT playlistIndex, int* p);
	STDMETHODIMP GetPlaylistItemCount(UINT playlistIndex, UINT* p);
	STDMETHODIMP GetPlaylistItems(UINT playlistIndex, IFbMetadbHandleList** pp);
	STDMETHODIMP GetPlaylistName(UINT playlistIndex, BSTR* p);
	STDMETHODIMP GetPlaylistSelectedItems(UINT playlistIndex, IFbMetadbHandleList** pp);
	STDMETHODIMP GetRecyclerItems(UINT index, IFbMetadbHandleList** pp);
	STDMETHODIMP GetRecyclerName(UINT index, BSTR* p);
	STDMETHODIMP InsertPlaylistItems(UINT playlistIndex, UINT base, IFbMetadbHandleList* handles, VARIANT_BOOL select);
	STDMETHODIMP InsertPlaylistItemsFilter(UINT playlistIndex, UINT base, IFbMetadbHandleList* handles, VARIANT_BOOL select);
	STDMETHODIMP IsAutoPlaylist(UINT playlistIndex, VARIANT_BOOL* p);
	STDMETHODIMP IsPlaylistItemSelected(UINT playlistIndex, UINT playlistItemIndex, VARIANT_BOOL* p);
	STDMETHODIMP IsPlaylistLocked(UINT playlistIndex, VARIANT_BOOL* p);
	STDMETHODIMP MovePlaylist(UINT from, UINT to, VARIANT_BOOL* p);
	STDMETHODIMP MovePlaylistSelection(UINT playlistIndex, int delta, VARIANT_BOOL* p);
	STDMETHODIMP RecyclerPurge(VARIANT affectedItems);
	STDMETHODIMP RecyclerRestore(UINT index);
	STDMETHODIMP RemovePlaylist(UINT playlistIndex, VARIANT_BOOL* p);
	STDMETHODIMP RemovePlaylistSelection(UINT playlistIndex, VARIANT_BOOL crop);
	STDMETHODIMP RemovePlaylistSwitch(UINT playlistIndex, VARIANT_BOOL* p);
	STDMETHODIMP RenamePlaylist(UINT playlistIndex, BSTR name, VARIANT_BOOL* p);
	STDMETHODIMP SetActivePlaylistContext();
	STDMETHODIMP SetPlaylistFocusItem(UINT playlistIndex, UINT playlistItemIndex);
	STDMETHODIMP SetPlaylistFocusItemByHandle(UINT playlistIndex, IFbMetadbHandle* handle);
	STDMETHODIMP SetPlaylistSelection(UINT playlistIndex, VARIANT affectedItems, VARIANT_BOOL state);
	STDMETHODIMP SetPlaylistSelectionSingle(UINT playlistIndex, UINT playlistItemIndex, VARIANT_BOOL state);
	STDMETHODIMP ShowAutoPlaylistUI(UINT playlistIndex, VARIANT_BOOL* p);
	STDMETHODIMP SortByFormat(UINT playlistIndex, BSTR pattern, VARIANT_BOOL selOnly, VARIANT_BOOL* p);
	STDMETHODIMP SortByFormatV2(UINT playlistIndex, BSTR pattern, int direction, VARIANT_BOOL* p);
	STDMETHODIMP SortPlaylistsByName(int direction);
	STDMETHODIMP UndoBackup(UINT playlistIndex);
	STDMETHODIMP get_ActivePlaylist(int* p);
	STDMETHODIMP get_PlaybackOrder(UINT* p);
	STDMETHODIMP get_PlayingPlaylist(int* p);
	STDMETHODIMP get_PlaylistCount(UINT* p);
	STDMETHODIMP get_RecyclerCount(UINT* p);
	STDMETHODIMP put_ActivePlaylist(UINT playlistIndex);
	STDMETHODIMP put_PlaybackOrder(UINT order);
};

class FbProfiler : public IDispatchImpl3<IFbProfiler>
{
protected:
	FbProfiler(const char* p_name);
	virtual ~FbProfiler();

	pfc::string_simple m_name;
	pfc::hires_timer m_timer;

public:
	STDMETHODIMP Print();
	STDMETHODIMP Reset();
	STDMETHODIMP get_Time(int* p);
};

class FbTitleFormat : public IDisposableImpl4<IFbTitleFormat>
{
protected:
	FbTitleFormat(BSTR pattern);
	virtual ~FbTitleFormat();
	virtual void FinalRelease();

	titleformat_object::ptr m_obj;

public:
	STDMETHODIMP get__ptr(void** pp);
	STDMETHODIMP Eval(BSTR* p);
	STDMETHODIMP EvalWithMetadb(IFbMetadbHandle* handle, BSTR* p);
	STDMETHODIMP EvalWithMetadbs(IFbMetadbHandleList* handles, VARIANT* p);
};

class FbTooltip : public IDisposableImpl4<IFbTooltip>
{
protected:
	FbTooltip(HWND p_wndparent, const panel_tooltip_param_ptr& p_param_ptr);
	virtual ~FbTooltip();
	virtual void FinalRelease();

	BSTR m_tip_buffer;
	HWND m_wndparent;
	HWND m_wndtooltip;
	TOOLINFO m_ti;
	panel_tooltip_param_ptr m_panel_tooltip_param_ptr;

public:
	STDMETHODIMP Activate();
	STDMETHODIMP Deactivate();
	STDMETHODIMP GetDelayTime(int type, int* p);
	STDMETHODIMP SetDelayTime(int type, int time);
	STDMETHODIMP SetMaxWidth(int width);
	STDMETHODIMP TrackPosition(int x, int y);
	STDMETHODIMP get_Text(BSTR* p);
	STDMETHODIMP put_Text(BSTR text);
	STDMETHODIMP put_TrackActivate(VARIANT_BOOL activate);
};

class FbUiSelectionHolder : public IDisposableImpl4<IFbUiSelectionHolder>
{
protected:
	FbUiSelectionHolder(const ui_selection_holder::ptr& holder);
	virtual ~FbUiSelectionHolder();
	virtual void FinalRelease();

	ui_selection_holder::ptr m_holder;

public:
	STDMETHODIMP SetPlaylistSelectionTracking();
	STDMETHODIMP SetPlaylistTracking();
	STDMETHODIMP SetSelection(IFbMetadbHandleList* handles);
};

class FbUtils : public IDispatchImpl3<IFbUtils>
{
protected:
	FbUtils();
	virtual ~FbUtils();

public:
	STDMETHODIMP AcquireUiSelectionHolder(IFbUiSelectionHolder** pp);
	STDMETHODIMP AddDirectory();
	STDMETHODIMP AddFiles();
	STDMETHODIMP CheckClipboardContents(UINT window_id, VARIANT_BOOL* p);
	STDMETHODIMP ClearPlaylist();
	STDMETHODIMP CreateContextMenuManager(IContextMenuManager** pp);
	STDMETHODIMP CreateHandleList(VARIANT handle, IFbMetadbHandleList** pp);
	STDMETHODIMP CreateMainMenuManager(IMainMenuManager** pp);
	STDMETHODIMP CreateProfiler(BSTR name, IFbProfiler** pp);
	STDMETHODIMP Exit();
	STDMETHODIMP GetClipboardContents(UINT window_id, IFbMetadbHandleList** pp);
	STDMETHODIMP GetDSPPresets(VARIANT* p);
	STDMETHODIMP GetFocusItem(IFbMetadbHandle** pp);
	STDMETHODIMP GetLibraryItems(IFbMetadbHandleList** pp);
	STDMETHODIMP GetNowPlaying(IFbMetadbHandle** pp);
	STDMETHODIMP GetOutputDevices(BSTR* p);
	STDMETHODIMP GetSelection(IFbMetadbHandle** pp);
	STDMETHODIMP GetSelections(UINT flags, IFbMetadbHandleList** pp);
	STDMETHODIMP GetSelectionType(UINT* p);
	STDMETHODIMP IsLibraryEnabled(VARIANT_BOOL* p);
	STDMETHODIMP LoadPlaylist();
	STDMETHODIMP Next();
	STDMETHODIMP Pause();
	STDMETHODIMP Play();
	STDMETHODIMP PlayOrPause();
	STDMETHODIMP Prev();
	STDMETHODIMP Random();
	STDMETHODIMP RunContextCommand(BSTR command, VARIANT_BOOL* p);
	STDMETHODIMP RunMainMenuCommand(BSTR command, VARIANT_BOOL* p);
	STDMETHODIMP SaveIndex();
	STDMETHODIMP SavePlaylist();
	STDMETHODIMP SetOutputDevice(BSTR output, BSTR device);
	STDMETHODIMP ShowConsole();
	STDMETHODIMP ShowLibrarySearchUI(BSTR query);
	STDMETHODIMP ShowPopupMessage(BSTR msg, BSTR title);
	STDMETHODIMP ShowPreferences();
	STDMETHODIMP Stop();
	STDMETHODIMP TitleFormat(BSTR expression, IFbTitleFormat** pp);
	STDMETHODIMP VolumeDown();
	STDMETHODIMP VolumeMute();
	STDMETHODIMP VolumeUp();
	STDMETHODIMP get_AlwaysOnTop(VARIANT_BOOL* p);
	STDMETHODIMP get_ComponentPath(BSTR* p);
	STDMETHODIMP get_CursorFollowPlayback(VARIANT_BOOL* p);
	STDMETHODIMP get_DSP(int* p);
	STDMETHODIMP get_FoobarPath(BSTR* p);
	STDMETHODIMP get_IsPaused(VARIANT_BOOL* p);
	STDMETHODIMP get_IsPlaying(VARIANT_BOOL* p);
	STDMETHODIMP get_PlaybackFollowCursor(VARIANT_BOOL* p);
	STDMETHODIMP get_PlaybackLength(double* p);
	STDMETHODIMP get_PlaybackTime(double* p);
	STDMETHODIMP get_ProfilePath(BSTR* p);
	STDMETHODIMP get_ReplaygainMode(UINT* p);
	STDMETHODIMP get_StopAfterCurrent(VARIANT_BOOL* p);
	STDMETHODIMP get_Volume(float* p);
	STDMETHODIMP put_AlwaysOnTop(VARIANT_BOOL p);
	STDMETHODIMP put_CursorFollowPlayback(VARIANT_BOOL p);
	STDMETHODIMP put_DSP(UINT p);
	STDMETHODIMP put_PlaybackFollowCursor(VARIANT_BOOL p);
	STDMETHODIMP put_PlaybackTime(double time);
	STDMETHODIMP put_ReplaygainMode(UINT p);
	STDMETHODIMP put_StopAfterCurrent(VARIANT_BOOL p);
	STDMETHODIMP put_Volume(float value);
};

class HostComm;

class FbWindow : public IDispatchImpl3<IFbWindow>
{
protected:
	FbWindow(HostComm* p);
	virtual ~FbWindow();

public:
	STDMETHODIMP ClearInterval(UINT intervalID);
	STDMETHODIMP ClearTimeout(UINT timeoutID);
	STDMETHODIMP CreatePopupMenu(IMenuObj** pp);
	STDMETHODIMP CreateThemeManager(BSTR classid, IThemeManager** pp);
	STDMETHODIMP CreateTooltip(BSTR name, float pxSize, int style, IFbTooltip** pp);
	STDMETHODIMP GetColourCUI(UINT type, int* p);
	STDMETHODIMP GetColourDUI(UINT type, int* p);
	STDMETHODIMP GetFontCUI(UINT type, IGdiFont** pp);
	STDMETHODIMP GetFontDUI(UINT type, IGdiFont** pp);
	STDMETHODIMP GetProperty(BSTR name, VARIANT defaultval, VARIANT* p);
	STDMETHODIMP NotifyOthers(BSTR name, VARIANT info);
	STDMETHODIMP Reload();
	STDMETHODIMP Repaint();
	STDMETHODIMP RepaintRect(int x, int y, int w, int h);
	STDMETHODIMP SetCursor(UINT id);
	STDMETHODIMP SetInterval(IDispatch* func, int delay, UINT* p);
	STDMETHODIMP SetProperty(BSTR name, VARIANT val);
	STDMETHODIMP SetTimeout(IDispatch* func, int delay, UINT* p);
	STDMETHODIMP ShowConfigure();
	STDMETHODIMP ShowProperties();
	STDMETHODIMP get_Height(int* p);
	STDMETHODIMP get_ID(UINT* p);
	STDMETHODIMP get_InstanceType(UINT* p);
	STDMETHODIMP get_IsTransparent(VARIANT_BOOL* p);
	STDMETHODIMP get_IsVisible(VARIANT_BOOL* p);
	STDMETHODIMP get_MaxHeight(UINT* p);
	STDMETHODIMP get_MaxWidth(UINT* p);
	STDMETHODIMP get_MinHeight(UINT* p);
	STDMETHODIMP get_MinWidth(UINT* p);
	STDMETHODIMP get_Name(BSTR* p);
	STDMETHODIMP get_Width(int* p);
	STDMETHODIMP put_MaxHeight(UINT height);
	STDMETHODIMP put_MaxWidth(UINT width);
	STDMETHODIMP put_MinHeight(UINT height);
	STDMETHODIMP put_MinWidth(UINT width);

private:
	HostComm* m_host;
};

class GdiBitmap : public GdiObj<IGdiBitmap, Gdiplus::Bitmap>
{
protected:
	GdiBitmap(Gdiplus::Bitmap* p);

public:
	STDMETHODIMP ApplyAlpha(BYTE alpha, IGdiBitmap** pp);
	STDMETHODIMP ApplyMask(IGdiBitmap* mask, VARIANT_BOOL* p);
	STDMETHODIMP Clone(float x, float y, float w, float h, IGdiBitmap** pp);
	STDMETHODIMP CreateRawBitmap(IGdiRawBitmap** pp);
	STDMETHODIMP GetGraphics(IGdiGraphics** pp);
	STDMETHODIMP ReleaseGraphics(IGdiGraphics* p);
	STDMETHODIMP Resize(UINT w, UINT h, int interpolationMode, IGdiBitmap** pp);
	STDMETHODIMP RotateFlip(UINT mode);
	STDMETHODIMP SaveAs(BSTR path, BSTR format, VARIANT_BOOL* p);
	STDMETHODIMP StackBlur(int radius);
	STDMETHODIMP get_Height(UINT* p);
	STDMETHODIMP get_Width(UINT* p);
};

class GdiFont : public GdiObj<IGdiFont, Gdiplus::Font>
{
protected:
	GdiFont(Gdiplus::Font* p, HFONT hFont, bool managed = true);
	virtual ~GdiFont();
	virtual void FinalRelease();

	HFONT m_hFont;
	bool m_managed;

public:
	STDMETHODIMP get__HFont(UINT* p);
	STDMETHODIMP get_Height(UINT* p);
	STDMETHODIMP get_Name(BSTR* p);
	STDMETHODIMP get_Size(float* p);
	STDMETHODIMP get_Style(int* p);
};

class GdiGraphics : public GdiObj<IGdiGraphics, Gdiplus::Graphics>
{
protected:
	GdiGraphics();
	static void GetRoundRectPath(Gdiplus::GraphicsPath& gp, Gdiplus::RectF& rect, float arc_width, float arc_height);

public:
	STDMETHODIMP Dispose()
	{
		return E_NOTIMPL;
	}

	STDMETHODIMP put__ptr(void* p);
	STDMETHODIMP CalcTextHeight(BSTR str, IGdiFont* font, UINT* p);
	STDMETHODIMP CalcTextWidth(BSTR str, IGdiFont* font, UINT* p);
	STDMETHODIMP DrawEllipse(float x, float y, float w, float h, float line_width, VARIANT colour);
	STDMETHODIMP DrawImage(IGdiBitmap* image, float dstX, float dstY, float dstW, float dstH, float srcX, float srcY, float srcW, float srcH, float angle, BYTE alpha);
	STDMETHODIMP DrawLine(float x1, float y1, float x2, float y2, float line_width, VARIANT colour);
	STDMETHODIMP DrawPolygon(VARIANT colour, float line_width, VARIANT points);
	STDMETHODIMP DrawRect(float x, float y, float w, float h, float line_width, VARIANT colour);
	STDMETHODIMP DrawRoundRect(float x, float y, float w, float h, float arc_width, float arc_height, float line_width, VARIANT colour);
	STDMETHODIMP DrawString(BSTR str, IGdiFont* font, VARIANT colour, float x, float y, float w, float h, int flags);
	STDMETHODIMP EstimateLineWrap(BSTR str, IGdiFont* font, int max_width, VARIANT* p);
	STDMETHODIMP FillEllipse(float x, float y, float w, float h, VARIANT colour);
	STDMETHODIMP FillGradRect(float x, float y, float w, float h, float angle, VARIANT colour1, VARIANT colour2, float focus);
	STDMETHODIMP FillPolygon(VARIANT colour, int fillmode, VARIANT points);
	STDMETHODIMP FillRoundRect(float x, float y, float w, float h, float arc_width, float arc_height, VARIANT colour);
	STDMETHODIMP FillSolidRect(float x, float y, float w, float h, VARIANT colour);
	STDMETHODIMP GdiAlphaBlend(IGdiRawBitmap* bitmap, int dstX, int dstY, int dstW, int dstH, int srcX, int srcY, int srcW, int srcH, BYTE alpha);
	STDMETHODIMP GdiDrawBitmap(IGdiRawBitmap* bitmap, int dstX, int dstY, int dstW, int dstH, int srcX, int srcY, int srcW, int srcH);
	STDMETHODIMP GdiDrawText(BSTR str, IGdiFont* font, VARIANT colour, int x, int y, int w, int h, int format);
	STDMETHODIMP SetInterpolationMode(int mode);
	STDMETHODIMP SetSmoothingMode(int mode);
	STDMETHODIMP SetTextRenderingHint(UINT mode);
};

class GdiRawBitmap : public IDisposableImpl4<IGdiRawBitmap>
{
protected:
	GdiRawBitmap(Gdiplus::Bitmap* p_bmp);
	virtual ~GdiRawBitmap();
	virtual void FinalRelease();

	HBITMAP m_hbmp, m_hbmpold;
	HDC m_hdc;
	t_size m_height, m_width;

public:
	STDMETHODIMP get__Handle(HDC* p);
	STDMETHODIMP get_Height(UINT* p);
	STDMETHODIMP get_Width(UINT* p);
};

class GdiUtils : public IDispatchImpl3<IGdiUtils>
{
protected:
	GdiUtils() {}
	virtual ~GdiUtils() {}

public:
	STDMETHODIMP CreateImage(int w, int h, IGdiBitmap** pp);
	STDMETHODIMP Font(BSTR name, float pxSize, int style, IGdiFont** pp);
	STDMETHODIMP Image(BSTR path, IGdiBitmap** pp);
	STDMETHODIMP LoadImageAsync(UINT window_id, BSTR path, UINT* p);
};

class JSConsole : public IDispatchImpl3<IJSConsole>
{
protected:
	JSConsole();
	virtual ~JSConsole();

public:
	STDMETHODIMP Log(SAFEARRAY* p);
};

class JSUtils : public IDispatchImpl3<IJSUtils>
{
protected:
	JSUtils();
	virtual ~JSUtils();

public:
	STDMETHODIMP CheckComponent(BSTR name, VARIANT_BOOL is_dll, VARIANT_BOOL* p);
	STDMETHODIMP CheckFont(BSTR name, VARIANT_BOOL* p);
	STDMETHODIMP ColourPicker(UINT window_id, int default_colour, int* p);
	STDMETHODIMP FormatDuration(double seconds, BSTR* p);
	STDMETHODIMP FormatFileSize(LONGLONG bytes, BSTR* p);
	STDMETHODIMP GetAlbumArtAsync(UINT window_id, IFbMetadbHandle* handle, UINT art_id, UINT* p);
	STDMETHODIMP GetSysColour(UINT index, int* p);
	STDMETHODIMP GetSystemMetrics(UINT index, int* p);
	STDMETHODIMP InputBox(UINT window_id, BSTR prompt, BSTR caption, BSTR def, VARIANT_BOOL error_on_cancel, BSTR* p);
	STDMETHODIMP IsKeyPressed(UINT vkey, VARIANT_BOOL* p);
	STDMETHODIMP PathWildcardMatch(BSTR pattern, BSTR str, VARIANT_BOOL* p);
	STDMETHODIMP ReadINI(BSTR filename, BSTR section, BSTR key, VARIANT defaultval, BSTR* p);
	STDMETHODIMP ReadTextFile(BSTR filename, BSTR* p);
	STDMETHODIMP WriteINI(BSTR filename, BSTR section, BSTR key, VARIANT val, VARIANT_BOOL* p);
	STDMETHODIMP WriteTextFile(BSTR filename, BSTR content, VARIANT_BOOL write_bom, VARIANT_BOOL* p);
};

class MainMenuManager : public IDisposableImpl4<IMainMenuManager>
{
protected:
	MainMenuManager();
	virtual ~MainMenuManager();
	virtual void FinalRelease();

	mainmenu_manager::ptr m_mm;

public:
	STDMETHODIMP BuildMenu(IMenuObj* p, int base_id, int count);
	STDMETHODIMP ExecuteByID(UINT id, VARIANT_BOOL* p);
	STDMETHODIMP Init(BSTR root_name);
};

class MenuObj : public IDisposableImpl4<IMenuObj>
{
protected:
	MenuObj(HWND wnd_parent);
	virtual ~MenuObj();
	virtual void FinalRelease();

	HMENU m_hMenu;
	HWND m_wnd_parent;
	bool m_has_detached;

public:
	STDMETHODIMP get__ID(UINT* p);
	STDMETHODIMP AppendMenuItem(UINT flags, UINT item_id, BSTR text);
	STDMETHODIMP AppendMenuSeparator();
	STDMETHODIMP AppendTo(IMenuObj* parent, UINT flags, BSTR text);
	STDMETHODIMP CheckMenuItem(UINT item_id, VARIANT_BOOL check);
	STDMETHODIMP CheckMenuRadioItem(UINT first, UINT last, UINT selected);
	STDMETHODIMP TrackPopupMenu(int x, int y, UINT flags, UINT* p);
};

class ThemeManager : public IDisposableImpl4<IThemeManager>
{
protected:
	ThemeManager(HWND hwnd, BSTR classlist);
	virtual ~ThemeManager();
	virtual void FinalRelease();


	HTHEME m_theme;
	int m_partid;
	int m_stateid;

public:
	STDMETHODIMP DrawThemeBackground(IGdiGraphics* gr, int x, int y, int w, int h, int clip_x, int clip_y, int clip_w, int clip_h);
	STDMETHODIMP IsThemePartDefined(int partid, int stateid, VARIANT_BOOL* p);
	STDMETHODIMP SetPartAndStateID(int partid, int stateid);
};
