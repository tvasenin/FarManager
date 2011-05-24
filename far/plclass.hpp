#pragma once

/*
Copyright � 1996 Eugene Roshal
Copyright � 2000 Far Group
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the authors may not be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "language.hpp"
#include "bitflags.hpp"
#include "plugin.hpp"

class AncientPlugin
{
	public:
		virtual ~AncientPlugin() {}
		virtual const GUID& GetGUID(void) = 0;
};

class PluginManager;
struct ExecuteStruct
{
	int id; //function id
	union
	{
		INT_PTR nResult;
		HANDLE hResult;
		BOOL bResult;
	};

	union
	{
		INT_PTR nDefaultResult;
		HANDLE hDefaultResult;
		BOOL bDefaultResult;
	};

	bool bUnloaded;
};

#define EXECUTE_FUNCTION(function, es) \
{ \
	__Prolog(); \
	es.nResult = 0; \
	es.nDefaultResult = 0; \
	es.bUnloaded = false; \
	if ( Opt.ExceptRules ) \
	{ \
		__try \
		{ \
			function; \
		} \
		__except(xfilter(es.id, GetExceptionInformation(), this, 0)) \
		{ \
			m_owner->UnloadPlugin(this, es.id, true); \
			es.bUnloaded = true; \
			es.nResult = es.nDefaultResult; \
			ProcessException=FALSE; \
		} \
	} \
	else \
	{ \
		function; \
	} \
	__Epilog(); \
}

#define EXECUTE_FUNCTION_EX(function, es) EXECUTE_FUNCTION(es.nResult = (INT_PTR)function, es)

#define FUNCTION(id) reinterpret_cast<id##Prototype>(Exports[id])

#define _W(string) L##string
#define W(string) _W(string)

enum EXPORTS_ENUM
{
	iGetGlobalInfo,
	iSetStartupInfo,
	iOpen,
	iClosePanel,
	iGetPluginInfo,
	iGetOpenPanelInfo,
	iGetFindData,
	iFreeFindData,
	iGetVirtualFindData,
	iFreeVirtualFindData,
	iSetDirectory,
	iGetFiles,
	iPutFiles,
	iDeleteFiles,
	iMakeDirectory,
	iProcessHostFile,
	iSetFindList,
	iConfigure,
	iExitFAR,
	iProcessPanelInput,
	iProcessEvent,
	iProcessEditorEvent,
	iCompare,
	iProcessEditorInput,
	iProcessViewerEvent,
	iProcessDialogEvent,
	iProcessSynchroEvent,
#if defined(MANTIS_0000466)
	iProcessMacro,
#endif
#if defined(MANTIS_0001687)
	iProcessConsoleInput,
#endif
	iAnalyse,
	iGetCustomData,
	iFreeCustomData,

	iOpenFilePlugin,
	iGetMinFarVersion,
	i_LAST
};

class Plugin: public AncientPlugin
{
public:
	Plugin(PluginManager *owner, const wchar_t *lpwszModuleName);
	virtual ~Plugin();

	virtual bool GetGlobalInfo(GlobalInfo *Info);
	virtual bool SetStartupInfo(bool &bUnloaded);
	virtual bool CheckMinFarVersion(bool &bUnloaded);
	virtual HANDLE Open(int OpenFrom, const GUID& Guid, INT_PTR Item);
	virtual HANDLE OpenFilePlugin(const wchar_t *Name, const unsigned char *Data, int DataSize, int OpMode);
	virtual int SetFindList(HANDLE hPlugin, const PluginPanelItem *PanelItem, int ItemsNumber);
	virtual int GetFindData(HANDLE hPlugin, PluginPanelItem **pPanelItem, int *pItemsNumber, int OpMode);
	virtual int GetVirtualFindData(HANDLE hPlugin, PluginPanelItem **pPanelItem, int *pItemsNumber, const wchar_t *Path);
	virtual int SetDirectory(HANDLE hPlugin, const wchar_t *Dir, int OpMode);
	virtual int GetFiles(HANDLE hPlugin, PluginPanelItem *PanelItem, int ItemsNumber, int Move, const wchar_t **DestPath, int OpMode);
	virtual int PutFiles(HANDLE hPlugin, PluginPanelItem *PanelItem, int ItemsNumber, int Move, int OpMode);
	virtual int DeleteFiles(HANDLE hPlugin, PluginPanelItem *PanelItem, int ItemsNumber, int OpMode);
	virtual int MakeDirectory(HANDLE hPlugin, const wchar_t **Name, int OpMode);
	virtual int ProcessHostFile(HANDLE hPlugin, PluginPanelItem *PanelItem, int ItemsNumber, int OpMode);
	virtual int ProcessKey(HANDLE hPlugin, const INPUT_RECORD *Rec, bool Pred);
	virtual int ProcessEvent(HANDLE hPlugin, int Event, PVOID Param);
	virtual int Compare(HANDLE hPlugin, const PluginPanelItem *Item1, const PluginPanelItem *Item2, unsigned long Mode);
	virtual int GetCustomData(const wchar_t *FilePath, wchar_t **CustomData);
	virtual void FreeCustomData(wchar_t *CustomData);
	virtual void GetOpenPanelInfo(HANDLE hPlugin, OpenPanelInfo *Info);
	virtual void FreeFindData(HANDLE hPlugin, PluginPanelItem *PanelItem, int ItemsNumber);
	virtual void FreeVirtualFindData(HANDLE hPlugin, PluginPanelItem *PanelItem, int ItemsNumber);
	virtual void ClosePanel(HANDLE hPlugin);
	virtual int ProcessEditorInput(const INPUT_RECORD *D);
	virtual int ProcessEditorEvent(int Event, PVOID Param);
	virtual int ProcessViewerEvent(int Event, PVOID Param);
	virtual int ProcessDialogEvent(int Event, PVOID Param);
	virtual int ProcessSynchroEvent(int Event, PVOID Param);
#if defined(MANTIS_0000466)
	virtual int ProcessMacro(ProcessMacroInfo *Info);
#endif
#if defined(MANTIS_0001687)
	virtual int ProcessConsoleInput(ProcessConsoleInputInfo *Info);
#endif
	virtual int Analyse(const AnalyseInfo *Info);
	virtual bool GetPluginInfo(PluginInfo *pi);
	virtual int Configure(const GUID& Guid);
	virtual void ExitFAR(const ExitInfo *Info);

	virtual bool IsOemPlugin() { return false; }
	virtual const wchar_t *GetHotkeyName() { return m_strGuid; }

	bool HasGetGlobalInfo()       { return Exports[iGetGlobalInfo]!=nullptr; }
	bool HasOpenPanel()           { return Exports[iOpen]!=nullptr; }
	bool HasMakeDirectory()       { return Exports[iMakeDirectory]!=nullptr; }
	bool HasDeleteFiles()         { return Exports[iDeleteFiles]!=nullptr; }
	bool HasPutFiles()            { return Exports[iPutFiles]!=nullptr; }
	bool HasGetFiles()            { return Exports[iGetFiles]!=nullptr; }
	bool HasSetStartupInfo()      { return Exports[iSetStartupInfo]!=nullptr; }
	bool HasClosePanel()          { return Exports[iClosePanel]!=nullptr; }
	bool HasGetPluginInfo()       { return Exports[iGetPluginInfo]!=nullptr; }
	bool HasGetOpenPanelInfo()    { return Exports[iGetOpenPanelInfo]!=nullptr; }
	bool HasGetFindData()         { return Exports[iGetFindData]!=nullptr; }
	bool HasFreeFindData()        { return Exports[iFreeFindData]!=nullptr; }
	bool HasGetVirtualFindData()  { return Exports[iGetVirtualFindData]!=nullptr; }
	bool HasFreeVirtualFindData() { return Exports[iFreeVirtualFindData]!=nullptr; }
	bool HasSetDirectory()        { return Exports[iSetDirectory]!=nullptr; }
	bool HasProcessHostFile()     { return Exports[iProcessHostFile]!=nullptr; }
	bool HasSetFindList()         { return Exports[iSetFindList]!=nullptr; }
	bool HasConfigure()           { return Exports[iConfigure]!=nullptr; }
	bool HasExitFAR()             { return Exports[iExitFAR]!=nullptr; }
	bool HasProcessPanelInput()   { return Exports[iProcessPanelInput]!=nullptr; }
	bool HasProcessEvent()        { return Exports[iProcessEvent]!=nullptr; }
	bool HasProcessEditorEvent()  { return Exports[iProcessEditorEvent]!=nullptr; }
	bool HasCompare()             { return Exports[iCompare]!=nullptr; }
	bool HasProcessEditorInput()  { return Exports[iProcessEditorInput]!=nullptr; }
	bool HasProcessViewerEvent()  { return Exports[iProcessViewerEvent]!=nullptr; }
	bool HasProcessDialogEvent()  { return Exports[iProcessDialogEvent]!=nullptr; }
	bool HasProcessSynchroEvent() { return Exports[iProcessSynchroEvent]!=nullptr; }
#if defined(MANTIS_0000466)
	bool HasProcessMacro()        { return Exports[iProcessMacro]!=nullptr; }
#endif
#if defined(MANTIS_0001687)
	bool HasProcessConsoleInput() { return Exports[iProcessConsoleInput]!=nullptr; }
#endif
	bool HasAnalyse()             { return Exports[iAnalyse]!=nullptr; }
	bool HasGetCustomData()       { return Exports[iGetCustomData]!=nullptr; }
	bool HasFreeCustomData()      { return Exports[iFreeCustomData]!=nullptr; }

	bool HasOpenFilePlugin()      { return Exports[iOpenFilePlugin]!=nullptr; }
	bool HasMinFarVersion()       { return Exports[iGetMinFarVersion]!=nullptr; }

	const string &GetModuleName() { return m_strModuleName; }
	const wchar_t *GetCacheName() { return m_strCacheName; }
	const wchar_t* GetTitle(void) { return strTitle.CPtr(); }
	const GUID& GetGUID(void) { return m_Guid; }
	const wchar_t *GetMsg(int nID) { return PluginLang.GetMsg(nID); }

	bool CheckWorkFlags(DWORD flags) { return WorkFlags.Check(flags)==TRUE; }
	DWORD GetWorkFlags() { return WorkFlags.Flags; }
	DWORD GetFuncFlags() { return FuncFlags.Flags; }

	bool InitLang(const wchar_t *Path) { return PluginLang.Init(Path,!IsOemPlugin()); }
	void CloseLang() { PluginLang.Close(); }

	bool Load();
	int Unload(bool bExitFAR = false);
	bool LoadData();
	bool LoadFromCache(const FAR_FIND_DATA_EX &FindData);
	bool SaveToCache();
	bool IsPanelPlugin();

protected:
	virtual void __Prolog() {};
	virtual void __Epilog() {};

	void* Exports[i_LAST];
	const wchar_t **ExportsNamesW;
	const char **ExportsNamesA;

	PluginManager *m_owner; //BUGBUG
	Language PluginLang;

private:
	void InitExports();
	void ClearExports();
	void SetGuid(const GUID& Guid);

	string strTitle;
	string strDescription;
	string strAuthor;

	string m_strModuleName;
	string m_strCacheName;

	BitFlags WorkFlags;      // ������� ����� �������� �������
	BitFlags FuncFlags;      // ������� ����� ������ ����.������� �������

	HMODULE m_hModule;

	VersionInfo MinFarVersion;
	VersionInfo PluginVersion;

	GUID m_Guid;
	string m_strGuid;
};
