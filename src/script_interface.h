#pragma once
#pragma warning(disable:4467)

[module(name = "foo_jscript_panel3", version = "1.8")];

extern ITypeLibPtr g_typelib;

[
	dual,
	object,
	pointer_default(unique),
	library_block,
	uuid("2e0bae19-3afe-473a-a703-0feb2d714655")
]
__interface IDisposable : IDispatch
{
	STDMETHOD(Dispose)();
};

[
	object,
	dual,
	pointer_default(unique),
	library_block,
	uuid("4ff021ab-17bc-43de-9dbe-2d0edec1e095")
]
__interface IFbTooltip : IDisposable
{
	STDMETHOD(Activate)();
	STDMETHOD(Deactivate)();
	STDMETHOD(GetDelayTime)(int type, [out, retval] int* p);
	STDMETHOD(SetDelayTime)(int type, int time);
	STDMETHOD(SetMaxWidth)(int width);
	STDMETHOD(TrackPosition)(int x, int y);
	[propget] STDMETHOD(Text)([out, retval] BSTR* p);
	[propput] STDMETHOD(Text)(BSTR text);
	[propput] STDMETHOD(TrackActivate)(VARIANT_BOOL activate);
};

[
	object,
	dual,
	pointer_default(unique),
	library_block,
	uuid("77e72064-1fb6-4754-a076-1dc517a6787b")
]
__interface IGdiObj : IDisposable
{
	STDMETHOD(get__ptr)([out, retval] void** pp);
};

[
	object,
	dual,
	pointer_default(unique),
	library_block,
	uuid("6fa87441-9f53-4a3f-999a-19509e3c92d7")
]
__interface IGdiFont : IGdiObj
{
	STDMETHOD(get__HFont)([out, retval] UINT* p);
	[propget] STDMETHOD(Height)([out, retval] UINT* p);
	[propget] STDMETHOD(Name)([out, retval] BSTR* p);
	[propget] STDMETHOD(Size)([out, retval] float* p);
	[propget] STDMETHOD(Style)([out, retval] int* p);
};

[
	object,
	dual,
	pointer_default(unique),
	library_block,
	uuid("22d1f519-5d6e-4d5c-80e3-8fde0d1b946b")
]
__interface IGdiRawBitmap : IDisposable
{
	STDMETHOD(get__Handle)([out, retval] HDC* p);
	[propget] STDMETHOD(Height)([out, retval] UINT* p);
	[propget] STDMETHOD(Width)([out, retval] UINT* p);
};

[
	object,
	dual,
	pointer_default(unique),
	library_block,
	uuid("7efbd443-4f6f-4cb2-9eee-882b9b19cbf6")
]
__interface IGdiBitmap : IGdiObj
{
	STDMETHOD(ApplyAlpha)(BYTE alpha, [out, retval] IGdiBitmap** pp);
	STDMETHOD(ApplyMask)(IGdiBitmap* mask, [out, retval] VARIANT_BOOL* p);
	STDMETHOD(Clone)(float x, float y, float w, float h, [out, retval] IGdiBitmap** pp);
	STDMETHOD(CreateRawBitmap)([out, retval] IGdiRawBitmap** pp);
	STDMETHOD(GetGraphics)([out, retval] __interface IGdiGraphics** pp);
	STDMETHOD(ReleaseGraphics)(__interface IGdiGraphics* p);
	STDMETHOD(Resize)(UINT w, UINT h, [defaultvalue(0)] int interpolationMode, [out, retval] IGdiBitmap** pp);
	STDMETHOD(RotateFlip)(UINT mode);
	STDMETHOD(SaveAs)(BSTR path, [defaultvalue("image/png")] BSTR format, [out, retval] VARIANT_BOOL* p);
	STDMETHOD(StackBlur)(int radius);
	[propget] STDMETHOD(Height)([out, retval] UINT* p);
	[propget] STDMETHOD(Width)([out, retval] UINT* p);
};

[
	object,
	dual,
	pointer_default(unique),
	library_block,
	uuid("9d6e404f-5ba7-4470-88d5-eb5980dffc07")
]
__interface IGdiGraphics : IGdiObj
{
	STDMETHOD(put__ptr)(void* p);
	STDMETHOD(CalcTextHeight)(BSTR str, IGdiFont* font, [out, retval] UINT* p);
	STDMETHOD(CalcTextWidth)(BSTR str, IGdiFont* font, [out, retval] UINT* p);
	STDMETHOD(DrawEllipse)(float x, float y, float w, float h, float line_width, VARIANT colour);
	STDMETHOD(DrawImage)(IGdiBitmap* image, float dstX, float dstY, float dstW, float dstH, float srcX, float srcY, float srcW, float srcH, [defaultvalue(0)] float angle, [defaultvalue(255)] BYTE alpha);
	STDMETHOD(DrawLine)(float x1, float y1, float x2, float y2, float line_width, VARIANT colour);
	STDMETHOD(DrawPolygon)(VARIANT colour, float line_width, VARIANT points);
	STDMETHOD(DrawRect)(float x, float y, float w, float h, float line_width, VARIANT colour);
	STDMETHOD(DrawRoundRect)(float x, float y, float w, float h, float arc_width, float arc_height, float line_width, VARIANT colour);
	STDMETHOD(DrawString)(BSTR str, IGdiFont* font, VARIANT colour, float x, float y, float w, float h, [defaultvalue(0)] int flags);
	STDMETHOD(EstimateLineWrap)(BSTR str, IGdiFont* font, int max_width, [out, retval] VARIANT* p);
	STDMETHOD(FillEllipse)(float x, float y, float w, float h, VARIANT colour);
	STDMETHOD(FillGradRect)(float x, float y, float w, float h, float angle, VARIANT colour1, VARIANT colour2, [defaultvalue(1)] float focus);
	STDMETHOD(FillPolygon)(VARIANT colour, int fillmode, VARIANT points);
	STDMETHOD(FillRoundRect)(float x, float y, float w, float h, float arc_width, float arc_height, VARIANT colour);
	STDMETHOD(FillSolidRect)(float x, float y, float w, float h, VARIANT colour);
	STDMETHOD(GdiAlphaBlend)(IGdiRawBitmap* bitmap, int dstX, int dstY, int dstW, int dstH, int srcX, int srcY, int srcW, int srcH, [defaultvalue(255)] BYTE alpha);
	STDMETHOD(GdiDrawBitmap)(IGdiRawBitmap* bitmap, int dstX, int dstY, int dstW, int dstH, int srcX, int srcY, int srcW, int srcH);
	STDMETHOD(GdiDrawText)(BSTR str, IGdiFont* font, VARIANT colour, int x, int y, int w, int h, [defaultvalue(0)] int format);
	STDMETHOD(SetInterpolationMode)(int mode);
	STDMETHOD(SetSmoothingMode)(int mode);
	STDMETHOD(SetTextRenderingHint)(UINT mode);
};

_COM_SMARTPTR_TYPEDEF(IGdiGraphics, __uuidof(IGdiGraphics));

[
	object,
	dual,
	pointer_default(unique),
	library_block,
	uuid("351e3e75-8f27-4afd-b7e0-5409cf8f4947")
]
__interface IGdiUtils : IDispatch
{
	STDMETHOD(CreateImage)(int w, int h, [out, retval] IGdiBitmap** pp);
	STDMETHOD(Font)(BSTR name, float pxSize, [defaultvalue(0)] int style, [out, retval] IGdiFont** pp);
	STDMETHOD(Image)(BSTR path, [out, retval] IGdiBitmap** pp);
	STDMETHOD(LoadImageAsync)(UINT window_id, BSTR path, [out, retval] UINT* p);
};

_COM_SMARTPTR_TYPEDEF(IGdiUtils, __uuidof(IGdiUtils));

[
	object,
	dual,
	pointer_default(unique),
	library_block,
	uuid("7c39dcf1-4e41-4a61-b06b-fb52107e4409")
]
__interface IFbFileInfo : IDisposable
{
	STDMETHOD(get__ptr)([out, retval] void** pp);
	STDMETHOD(InfoFind)(BSTR name, [out, retval] int* p);
	STDMETHOD(InfoName)(UINT idx, [out, retval] BSTR* p);
	STDMETHOD(InfoValue)(UINT idx, [out, retval] BSTR* p);
	STDMETHOD(MetaFind)(BSTR name, [out, retval] int* p);
	STDMETHOD(MetaName)(UINT idx, [out, retval] BSTR* p);
	STDMETHOD(MetaValue)(UINT idx, UINT vidx, [out, retval] BSTR* p);
	STDMETHOD(MetaValueCount)(UINT idx, [out, retval] UINT* p);
	[propget] STDMETHOD(InfoCount)([out, retval] UINT* p);
	[propget] STDMETHOD(MetaCount)([out, retval] UINT* p);
};

[
	object,
	dual,
	pointer_default(unique),
	library_block,
	uuid("0e1980d3-916a-482e-af87-578bcb1a4a25")
]
__interface IFbMetadbHandle : IDisposable
{
	STDMETHOD(get__ptr)([out, retval] void** pp);
	STDMETHOD(ClearStats)();
	STDMETHOD(Compare)(IFbMetadbHandle* handle, [out, retval] VARIANT_BOOL* p);
	STDMETHOD(GetAlbumArt)([defaultvalue(0)] UINT art_id, [out, retval] VARIANT* p);
	STDMETHOD(GetAlbumArtEmbedded)([defaultvalue(0)] UINT art_id, [out, retval] IGdiBitmap** pp);
	STDMETHOD(GetFileInfo)([out, retval] IFbFileInfo** pp);
	STDMETHOD(IsInMediaLibrary)([out, retval] VARIANT_BOOL* p);
	STDMETHOD(RefreshStats)();
	STDMETHOD(RunContextCommand)(BSTR command, [out, retval] VARIANT_BOOL* p);
	STDMETHOD(SetFirstPlayed)(BSTR first_played);
	STDMETHOD(SetLastPlayed)(BSTR last_played);
	STDMETHOD(SetLoved)(UINT loved);
	STDMETHOD(SetPlaycount)(UINT playcount);
	STDMETHOD(SetRating)(UINT rating);
	[propget] STDMETHOD(FileSize)([out, retval] LONGLONG* p);
	[propget] STDMETHOD(Length)([out, retval] double* p);
	[propget] STDMETHOD(Path)([out, retval] BSTR* p);
	[propget] STDMETHOD(RawPath)([out, retval] BSTR* p);
	[propget] STDMETHOD(SubSong)([out, retval] UINT* p);
};

[
	object,
	dual,
	pointer_default(unique),
	library_block,
	uuid("64528708-ae09-49dd-8e8d-1417fe9a9f09")
]
__interface IFbMetadbHandleList : IDisposable
{
	STDMETHOD(get__ptr)([out, retval] void** pp);
	STDMETHOD(AddItem)(IFbMetadbHandle* handle);
	STDMETHOD(AddItems)(IFbMetadbHandleList* handles);
	STDMETHOD(AttachImage)(BSTR image_path, [defaultvalue(0)] UINT art_id);
	STDMETHOD(BSearch)(IFbMetadbHandle* handle, [out, retval] int* p);
	STDMETHOD(CalcTotalDuration)([out, retval] double* p);
	STDMETHOD(CalcTotalSize)([out, retval] LONGLONG* p);
	STDMETHOD(Clone)([out, retval] IFbMetadbHandleList** pp);
	STDMETHOD(Convert)([out, retval] VARIANT* p);
	STDMETHOD(CopyToClipboard)([out, retval] VARIANT_BOOL* p);
	STDMETHOD(DoDragDrop)(UINT okEffects, [out, retval] UINT* p);
	STDMETHOD(Find)(IFbMetadbHandle* handle, [out, retval] int* p);
	STDMETHOD(GetLibraryRelativePaths)([out, retval] VARIANT* p);
	STDMETHOD(GetQueryItems)(BSTR query, [out, retval] IFbMetadbHandleList** pp);
	STDMETHOD(InsertItem)(UINT index, IFbMetadbHandle* handle);
	STDMETHOD(InsertItems)(UINT index, IFbMetadbHandleList* handles);
	STDMETHOD(MakeDifference)(IFbMetadbHandleList* handles);
	STDMETHOD(MakeIntersection)(IFbMetadbHandleList* handles);
	STDMETHOD(MakeUnion)(IFbMetadbHandleList* handles);
	STDMETHOD(RefreshStats)();
	STDMETHOD(Remove)(IFbMetadbHandle* handle);
	STDMETHOD(RemoveAll)();
	STDMETHOD(RemoveAttachedImage)([defaultvalue(0)] UINT art_id);
	STDMETHOD(RemoveAttachedImages)();
	STDMETHOD(RemoveByIdx)(UINT index);
	STDMETHOD(RemoveFromIdx)(UINT from, UINT count);
	STDMETHOD(RunContextCommand)(BSTR command, [out, retval] VARIANT_BOOL* p);
	STDMETHOD(Sort)();
	STDMETHOD(SortByFormat)(BSTR pattern, int direction);
	STDMETHOD(SortByPath)();
	STDMETHOD(SortByRelativePath)();
	STDMETHOD(UpdateFileInfo)(BSTR str);
	[propget] STDMETHOD(Count)([out, retval] UINT* p);
	[propget] STDMETHOD(Item)(UINT index, [out, retval] IFbMetadbHandle** pp);
	[propput] STDMETHOD(Item)(UINT index, IFbMetadbHandle* handle);
};

[
	object,
	dual,
	pointer_default(unique),
	library_block,
	uuid("998d8666-b446-4e92-8e8f-797d3cce4b7e")
]
__interface IFbTitleFormat : IDisposable
{
	STDMETHOD(get__ptr)([out, retval] void** pp);
	STDMETHOD(Eval)([out, retval] BSTR* p);
	STDMETHOD(EvalWithMetadb)(IFbMetadbHandle* handle, [out, retval] BSTR* p);
	STDMETHOD(EvalWithMetadbs)(IFbMetadbHandleList* handles, [out, retval] VARIANT* p);
};

[
	object,
	dual,
	pointer_default(unique),
	library_block,
	uuid("1e9f95ae-63be-49dc-a395-ee386e8eb202")
]
__interface IMenuObj : IDisposable
{
	STDMETHOD(get__ID)([out, retval] HMENU* p);
	STDMETHOD(AppendMenuItem)(UINT flags, UINT item_id, BSTR text);
	STDMETHOD(AppendMenuSeparator)();
	STDMETHOD(AppendTo)(IMenuObj* parent, UINT flags, BSTR text);
	STDMETHOD(CheckMenuItem)(UINT item_id, VARIANT_BOOL check);
	STDMETHOD(CheckMenuRadioItem)(UINT first, UINT last, UINT selected);
	STDMETHOD(TrackPopupMenu)(int x, int y, [defaultvalue(0)] UINT flags, [out, retval] UINT* p);
};

[
	object,
	dual,
	pointer_default(unique),
	library_block,
	uuid("0e1bc833-b9f8-44b1-8240-57fff04602ad")
]
__interface IContextMenuManager : IDisposable
{
	STDMETHOD(BuildMenu)(IMenuObj* p, int base_id);
	STDMETHOD(ExecuteByID)(UINT id, [out, retval] VARIANT_BOOL* p);
	STDMETHOD(InitContext)(IFbMetadbHandleList* handles);
	STDMETHOD(InitContextPlaylist)();
	STDMETHOD(InitNowPlaying)();
};

[
	object,
	dual,
	pointer_default(unique),
	library_block,
	uuid("4a357221-1b75-4379-8de7-6a865bbfad10")
]
__interface IMainMenuManager : IDisposable
{
	STDMETHOD(BuildMenu)(IMenuObj* p, int base_id, int count);
	STDMETHOD(ExecuteByID)(UINT id, [out, retval] VARIANT_BOOL* p);
	STDMETHOD(Init)(BSTR root_name);
};

[
	object,
	dual,
	pointer_default(unique),
	library_block,
	uuid("2d7436ad-6527-4154-a3c7-361ab8b88f5c")
]
__interface IFbProfiler : IDispatch
{
	STDMETHOD(Reset)();
	STDMETHOD(Print)();
	[propget] STDMETHOD(Time)([out, retval] int* p);
};

[
	object,
	dual,
	pointer_default(unique),
	library_block,
	uuid("1f40f9e1-c0fb-4021-80de-37c4d0a26f45")
]
__interface IFbUiSelectionHolder : IDisposable
{
	STDMETHOD(SetPlaylistSelectionTracking)();
	STDMETHOD(SetPlaylistTracking)();
	STDMETHOD(SetSelection)(IFbMetadbHandleList* handles);
};

[
	object,
	dual,
	pointer_default(unique),
	library_block,
	uuid("bae2e084-6545-4a17-9795-1496a4ee2741")
]
__interface IFbUtils : IDispatch
{
	STDMETHOD(AcquireUiSelectionHolder)([out, retval] IFbUiSelectionHolder** pp);
	STDMETHOD(AddDirectory)();
	STDMETHOD(AddFiles)();
	STDMETHOD(CheckClipboardContents)(UINT window_id, [out, retval] VARIANT_BOOL* p);
	STDMETHOD(ClearPlaylist)();
	STDMETHOD(CreateContextMenuManager)([out, retval] IContextMenuManager** pp);
	STDMETHOD(CreateHandleList)([optional] VARIANT handle, [out, retval] IFbMetadbHandleList** pp);
	STDMETHOD(CreateMainMenuManager)([out, retval] IMainMenuManager** pp);
	STDMETHOD(CreateProfiler)([defaultvalue("")] BSTR name, [out, retval] IFbProfiler** pp);
	STDMETHOD(Exit)();
	STDMETHOD(GetClipboardContents)(UINT window_id, [out, retval] IFbMetadbHandleList** pp);
	STDMETHOD(GetDSPPresets)([out, retval] VARIANT* p);
	STDMETHOD(GetFocusItem)([out, retval] IFbMetadbHandle** pp);
	STDMETHOD(GetLibraryItems)([out, retval] IFbMetadbHandleList** pp);
	STDMETHOD(GetNowPlaying)([out, retval] IFbMetadbHandle** pp);
	STDMETHOD(GetOutputDevices)([out, retval] BSTR* p);
	STDMETHOD(GetSelection)([out, retval] IFbMetadbHandle** pp);
	STDMETHOD(GetSelections)([defaultvalue(0)] UINT flags, [out, retval] IFbMetadbHandleList** pp);
	STDMETHOD(GetSelectionType)([out, retval] UINT* p);
	STDMETHOD(IsLibraryEnabled)([out, retval] VARIANT_BOOL* p);
	STDMETHOD(LoadPlaylist)();
	STDMETHOD(Next)();
	STDMETHOD(Pause)();
	STDMETHOD(Play)();
	STDMETHOD(PlayOrPause)();
	STDMETHOD(Prev)();
	STDMETHOD(Random)();
	STDMETHOD(RunContextCommand)(BSTR command, [out, retval] VARIANT_BOOL* p);
	STDMETHOD(RunMainMenuCommand)(BSTR command, [out, retval] VARIANT_BOOL* p);
	STDMETHOD(SaveIndex)();
	STDMETHOD(SavePlaylist)();
	STDMETHOD(SetOutputDevice)(BSTR output, BSTR device);
	STDMETHOD(ShowConsole)();
	STDMETHOD(ShowLibrarySearchUI)(BSTR query);
	STDMETHOD(ShowPopupMessage)(BSTR msg, [defaultvalue(JSP_NAME)] BSTR title);
	STDMETHOD(ShowPreferences)();
	STDMETHOD(Stop)();
	STDMETHOD(TitleFormat)(BSTR expression, [out, retval] IFbTitleFormat** pp);
	STDMETHOD(VolumeDown)();
	STDMETHOD(VolumeMute)();
	STDMETHOD(VolumeUp)();
	[propget] STDMETHOD(AlwaysOnTop)([out, retval] VARIANT_BOOL* p);
	[propget] STDMETHOD(ComponentPath)([out, retval] BSTR* p);
	[propget] STDMETHOD(CursorFollowPlayback)([out, retval] VARIANT_BOOL* p);
	[propget] STDMETHOD(DSP)([out, retval] int* p);
	[propget] STDMETHOD(FoobarPath)([out, retval] BSTR* p);
	[propget] STDMETHOD(IsPaused)([out, retval] VARIANT_BOOL* p);
	[propget] STDMETHOD(IsPlaying)([out, retval] VARIANT_BOOL* p);
	[propget] STDMETHOD(PlaybackFollowCursor)([out, retval] VARIANT_BOOL* p);
	[propget] STDMETHOD(PlaybackLength)([out, retval] double* p);
	[propget] STDMETHOD(PlaybackTime)([out, retval] double* p);
	[propget] STDMETHOD(ProfilePath)([out, retval] BSTR* p);
	[propget] STDMETHOD(ReplaygainMode)([out, retval] UINT* p);
	[propget] STDMETHOD(StopAfterCurrent)([out, retval] VARIANT_BOOL* p);
	[propget] STDMETHOD(Volume)([out, retval] float* p);
	[propput] STDMETHOD(AlwaysOnTop)(VARIANT_BOOL p);
	[propput] STDMETHOD(CursorFollowPlayback)(VARIANT_BOOL p);
	[propput] STDMETHOD(DSP)(UINT p);
	[propput] STDMETHOD(PlaybackFollowCursor)(VARIANT_BOOL p);
	[propput] STDMETHOD(PlaybackTime)(double time);
	[propput] STDMETHOD(ReplaygainMode)(UINT p);
	[propput] STDMETHOD(StopAfterCurrent)(VARIANT_BOOL p);
	[propput] STDMETHOD(Volume)(float value);
};

_COM_SMARTPTR_TYPEDEF(IFbUtils, __uuidof(IFbUtils));

[
	object,
	dual,
	pointer_default(unique),
	library_block,
	uuid("8a14d6a2-4582-4398-a6af-2206f2dabbbe")
]
__interface IThemeManager : IDisposable
{
	STDMETHOD(DrawThemeBackground)(IGdiGraphics* gr, int x, int y, int w, int h, [defaultvalue(0)] int clip_x, [defaultvalue(0)] int clip_y, [defaultvalue(0)] int clip_w, [defaultvalue(0)] int clip_h);
	STDMETHOD(IsThemePartDefined)(int partid, [defaultvalue(0)] int stateid, [out, retval] VARIANT_BOOL* p);
	STDMETHOD(SetPartAndStateID)(int partid, int stateid);
};

[
	object,
	dual,
	pointer_default(unique),
	library_block,
	uuid("91830eda-b5f2-4061-9923-7880192a2734")
]
__interface IDropSourceAction : IDisposable
{
	[propget] STDMETHOD(Effect)([out, retval] UINT* p);
	[propput] STDMETHOD(Base)(UINT base);
	[propput] STDMETHOD(Effect)(UINT effect);
	[propput] STDMETHOD(Playlist)(UINT id);
	[propput] STDMETHOD(ToSelect)(VARIANT_BOOL to_select);
};

[
	object,
	dual,
	pointer_default(unique),
	library_block,
	uuid("81e1f0c0-1dfe-4996-abd9-ba98dff69e4c")
]
__interface IFbWindow : IDispatch
{
	STDMETHOD(ClearInterval)(UINT intervalID);
	STDMETHOD(ClearTimeout)(UINT timeoutID);
	STDMETHOD(CreatePopupMenu)([out, retval] IMenuObj** pp);
	STDMETHOD(CreateThemeManager)(BSTR classid, [out, retval] IThemeManager** pp);
	STDMETHOD(CreateTooltip)([defaultvalue("Segoe UI")] BSTR name, [defaultvalue(12)] float pxSize, [defaultvalue(0)] int style, [out, retval] __interface IFbTooltip** pp);
	STDMETHOD(GetColourCUI)(UINT type, [out, retval] int* p);
	STDMETHOD(GetColourDUI)(UINT type, [out, retval] int* p);
	STDMETHOD(GetFontCUI)(UINT type, [out, retval] IGdiFont** pp);
	STDMETHOD(GetFontDUI)(UINT type, [out, retval] IGdiFont** pp);
	STDMETHOD(GetProperty)(BSTR name, [optional] VARIANT defaultval, [out, retval] VARIANT* p);
	STDMETHOD(NotifyOthers)(BSTR name, VARIANT info);
	STDMETHOD(Reload)();
	STDMETHOD(Repaint)();
	STDMETHOD(RepaintRect)(int x, int y, int w, int h);
	STDMETHOD(SetCursor)(UINT id);
	STDMETHOD(SetInterval)(IDispatch* func, int delay, [out, retval] UINT* p);
	STDMETHOD(SetProperty)(BSTR name, VARIANT val);
	STDMETHOD(SetTimeout)(IDispatch* func, int delay, [out, retval] UINT* p);
	STDMETHOD(ShowConfigure)();
	STDMETHOD(ShowProperties)();
	[propget] STDMETHOD(Height)([out, retval] int* p);
	[propget] STDMETHOD(ID)([out, retval] UINT* p);
	[propget] STDMETHOD(InstanceType)([out, retval] UINT* p);
	[propget] STDMETHOD(IsTransparent)([out, retval] VARIANT_BOOL* p);
	[propget] STDMETHOD(IsVisible)([out, retval] VARIANT_BOOL* p);
	[propget] STDMETHOD(MaxHeight)([out, retval] UINT* p);
	[propget] STDMETHOD(MaxWidth)([out, retval] UINT* p);
	[propget] STDMETHOD(MinHeight)([out, retval] UINT* p);
	[propget] STDMETHOD(MinWidth)([out, retval] UINT* p);
	[propget] STDMETHOD(Name)([out, retval] BSTR* p);
	[propget] STDMETHOD(Width)([out, retval] int* p);
	[propput] STDMETHOD(MaxHeight)(UINT height);
	[propput] STDMETHOD(MaxWidth)(UINT width);
	[propput] STDMETHOD(MinHeight)(UINT height);
	[propput] STDMETHOD(MinWidth)(UINT width);
};

_COM_SMARTPTR_TYPEDEF(IFbWindow, __uuidof(IFbWindow));

[
	object,
	dual,
	pointer_default(unique),
	library_block,
	uuid("690a35f6-ba53-46f9-91f8-4327204c6c62")
]
__interface IJSConsole : IDispatch
{
	[vararg] STDMETHOD(Log)([satype(VARIANT)] SAFEARRAY* p);
};

_COM_SMARTPTR_TYPEDEF(IJSConsole, __uuidof(IJSConsole));

[
	object,
	dual,
	pointer_default(unique),
	library_block,
	uuid("d53e81cd-0157-4cfe-a618-1F88d48dc0b7")
]
__interface IJSUtils : IDispatch
{
	STDMETHOD(CheckComponent)(BSTR name, [defaultvalue(-1)] VARIANT_BOOL is_dll, [out, retval] VARIANT_BOOL* p);
	STDMETHOD(CheckFont)(BSTR name, [out, retval] VARIANT_BOOL* p);
	STDMETHOD(ColourPicker)(UINT window_id, int default_colour, [out, retval] int* p);
	STDMETHOD(FormatDuration)(double seconds, [out, retval] BSTR* p);
	STDMETHOD(FormatFileSize)(LONGLONG bytes, [out, retval] BSTR* p);
	STDMETHOD(GetAlbumArtAsync)(UINT window_id, IFbMetadbHandle* handle, [defaultvalue(0)] UINT art_id, [out, retval] UINT* p);
	STDMETHOD(GetSysColour)(UINT index, [out, retval] int* p);
	STDMETHOD(GetSystemMetrics)(UINT index, [out, retval] int* p);
	STDMETHOD(InputBox)(UINT window_id, BSTR prompt, BSTR caption, [defaultvalue("")] BSTR def, [defaultvalue(0)] VARIANT_BOOL error_on_cancel, [out, retval] BSTR* p);
	STDMETHOD(IsKeyPressed)(UINT vkey, [out, retval] VARIANT_BOOL* p);
	STDMETHOD(PathWildcardMatch)(BSTR pattern, BSTR str, [out, retval] VARIANT_BOOL* p);
	STDMETHOD(ReadINI)(BSTR filename, BSTR section, BSTR key, [optional] VARIANT defaultval, [out, retval] BSTR* p);
	STDMETHOD(ReadTextFile)(BSTR filename, [out, retval] BSTR* p);
	STDMETHOD(WriteINI)(BSTR filename, BSTR section, BSTR key, VARIANT val, [out, retval] VARIANT_BOOL* p);
	STDMETHOD(WriteTextFile)(BSTR filename, BSTR content, [defaultvalue(-1)] VARIANT_BOOL write_bom, [out, retval] VARIANT_BOOL* p);
};

_COM_SMARTPTR_TYPEDEF(IJSUtils, __uuidof(IJSUtils));

[
	object,
	dual,
	pointer_default(unique),
	library_block,
	uuid("84212840-c0c5-4625-8fc4-2cc20e4bbcc8")
]
__interface IFbPlaylistManager : IDispatch
{
	STDMETHOD(AddLocations)(UINT playlistIndex, VARIANT locations, [defaultvalue(0)] VARIANT_BOOL select);
	STDMETHOD(ClearPlaylist)(UINT playlistIndex);
	STDMETHOD(ClearPlaylistSelection)(UINT playlistIndex);
	STDMETHOD(CreateAutoPlaylist)(UINT playlistIndex, BSTR name, BSTR query, [defaultvalue("")] BSTR sort, [defaultvalue(0)] UINT flags, [out, retval] int* p);
	STDMETHOD(CreatePlaylist)(UINT playlistIndex, BSTR name, [out, retval] int* p);
	STDMETHOD(DuplicatePlaylist)(UINT from, BSTR name, [out, retval] UINT* p);
	STDMETHOD(ExecutePlaylistDefaultAction)(UINT playlistIndex, UINT playlistItemIndex, [out, retval] VARIANT_BOOL* p);
	STDMETHOD(FindOrCreatePlaylist)(BSTR name, VARIANT_BOOL unlocked, [out, retval] int* p);
	STDMETHOD(FindPlaylist)(BSTR name, [out, retval] int* p);
	STDMETHOD(GetPlayingItemLocation)([out, retval] __interface IFbPlayingItemLocation** pp);
	STDMETHOD(GetPlaylistFocusItemIndex)(UINT playlistIndex, [out, retval] int* p);
	STDMETHOD(GetPlaylistItems)(UINT playlistIndex, [out, retval] IFbMetadbHandleList** pp);
	STDMETHOD(GetPlaylistName)(UINT playlistIndex, [out, retval] BSTR* p);
	STDMETHOD(GetPlaylistSelectedItems)(UINT playlistIndex, [out, retval] IFbMetadbHandleList** pp);
	STDMETHOD(GetPlaylistItemCount)(UINT playlistIndex, [out, retval] UINT* p);
	STDMETHOD(GetRecyclerItems)(UINT index, [out, retval] IFbMetadbHandleList** pp);
	STDMETHOD(GetRecyclerName)(UINT index, [out, retval] BSTR* p);
	STDMETHOD(InsertPlaylistItems)(UINT playlistIndex, UINT base, IFbMetadbHandleList* handles, [defaultvalue(0)] VARIANT_BOOL select);
	STDMETHOD(InsertPlaylistItemsFilter)(UINT playlistIndex, UINT base, IFbMetadbHandleList* handles, [defaultvalue(0)] VARIANT_BOOL select);
	STDMETHOD(IsAutoPlaylist)(UINT playlistIndex, [out, retval] VARIANT_BOOL* p);
	STDMETHOD(IsPlaylistItemSelected)(UINT playlistIndex, UINT playlistItemIndex, [out, retval] VARIANT_BOOL* p);
	STDMETHOD(IsPlaylistLocked)(UINT playlistIndex, [out, retval] VARIANT_BOOL* p);
	STDMETHOD(MovePlaylist)(UINT from, UINT to, [out, retval] VARIANT_BOOL* p);
	STDMETHOD(MovePlaylistSelection)(UINT playlistIndex, int delta, [out, retval] VARIANT_BOOL* p);
	STDMETHOD(RecyclerPurge)(VARIANT affectedItems);
	STDMETHOD(RecyclerRestore)(UINT index);
	STDMETHOD(RemovePlaylist)(UINT playlistIndex, [out, retval] VARIANT_BOOL* p);
	STDMETHOD(RemovePlaylistSelection)(UINT playlistIndex, [defaultvalue(0)] VARIANT_BOOL crop);
	STDMETHOD(RemovePlaylistSwitch)(UINT playlistIndex, [out, retval] VARIANT_BOOL* p);
	STDMETHOD(RenamePlaylist)(UINT playlistIndex, BSTR name, [out, retval] VARIANT_BOOL* p);
	STDMETHOD(SetActivePlaylistContext)();
	STDMETHOD(SetPlaylistFocusItem)(UINT playlistIndex, UINT playlistItemIndex);
	STDMETHOD(SetPlaylistFocusItemByHandle)(UINT playlistIndex, IFbMetadbHandle* handle);
	STDMETHOD(SetPlaylistSelection)(UINT playlistIndex, VARIANT affectedItems, VARIANT_BOOL state);
	STDMETHOD(SetPlaylistSelectionSingle)(UINT playlistIndex, UINT playlistItemIndex, VARIANT_BOOL state);
	STDMETHOD(ShowAutoPlaylistUI)(UINT playlistIndex, [out, retval] VARIANT_BOOL* p);
	STDMETHOD(SortByFormat)(UINT playlistIndex, BSTR pattern, [defaultvalue(0)] VARIANT_BOOL selOnly, [out, retval] VARIANT_BOOL* p);
	STDMETHOD(SortByFormatV2)(UINT playlistIndex, BSTR pattern, [defaultvalue(1)] int direction, [out, retval] VARIANT_BOOL* p);
	STDMETHOD(SortPlaylistsByName)([defaultvalue(1)] int direction);
	STDMETHOD(UndoBackup)(UINT playlistIndex);
	[propget] STDMETHOD(ActivePlaylist)([out, retval] int* p);
	[propget] STDMETHOD(PlaybackOrder)([out, retval] UINT* p);
	[propget] STDMETHOD(PlayingPlaylist)([out, retval] int* p);
	[propget] STDMETHOD(PlaylistCount)([out, retval] UINT* p);
	[propget] STDMETHOD(RecyclerCount)([out, retval] UINT* p);
	[propput] STDMETHOD(ActivePlaylist)(UINT playlistIndex);
	[propput] STDMETHOD(PlaybackOrder)(UINT order);
};

_COM_SMARTPTR_TYPEDEF(IFbPlaylistManager, __uuidof(IFbPlaylistManager));

[
	object,
	dual,
	pointer_default(unique),
	library_block,
	uuid("0f54464f-0b86-4419-83c0-b6f612d85fb0")
]
__interface IFbPlayingItemLocation : IDispatch
{
	[propget] STDMETHOD(IsValid)([out, retval] VARIANT_BOOL* p);
	[propget] STDMETHOD(PlaylistIndex)([out, retval] int* p);
	[propget] STDMETHOD(PlaylistItemIndex)([out, retval] int* p);
};
