/////////////////////////////////////////////////////////////////////////////
// Name:		MainFrm.h
// Purpose:
// Author:		Ralph Walden
// Copyright:	Copyright (c) 1998-2019 KeyWorks Software (Ralph Walden)
// License:		Apache License (see LICENSE)
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <ttstr.h>			// ttCStr
#include <ttwtlmsgs.h>		// Macros for mapping Windows messages to functions

extern const char* txtMainClass; // "KeyViewMsgs";

class CMainFrame : public CFrameWindowImpl<CMainFrame>, public CUpdateUI<CMainFrame>,
		public CMessageFilter, public CIdleHandler
{
public:
	DECLARE_FRAME_WND_CLASS(tt::txtTraceClass, IDR_MAINFRAME)

	CTraceView m_view;

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

	BEGIN_UPDATE_UI_MAP(CMainFrame)
		UPDATE_ELEMENT(ID_VIEW_TOOLBAR, UPDUI_MENUPOPUP)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainFrame)
		MSG_COMMAND_HANDLER(IDM_CLEAR, OnClearView)
		MSG_COMMAND_HANDLER(IDM_PREFERENCES, OnPreferences)
		MSG_COMMAND_HANDLER(IDM_FILE_SAVE_AS, OnSaveAs)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)

		MSG_WMP_HANDLER(tt::WMP_CLEAR_TRACE, OnClear)
		MSG_WMP_HANDLER(tt::WMP_TRACE_GENERAL, OnGeneralMsg);
		MSG_WMP_HANDLER(tt::WMP_TRACE_MSG, OnTraceMsg);

		COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
		COMMAND_ID_HANDLER(ID_VIEW_TOOLBAR, OnViewToolBar)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
	END_MSG_MAP()

protected:
	// Msg handlers

	void OnClearView();
	void OnClear(WPARAM /* wParam */, LPARAM /* lParam */);
	void OnPreferences();
	void OnG15Line1(WPARAM /* wParam */, LPARAM /* lParam */);
	void OnSaveAs();
	void OnGeneralMsg(WPARAM wParam, LPARAM lParam);
	void OnTraceMsg(WPARAM wParam, LPARAM lParam);

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	// Class members

	HANDLE	m_hMapping;
	char*	m_pszMap;		 // points to data in shared memory
	ttCStr	m_cszLastMsg;
};
