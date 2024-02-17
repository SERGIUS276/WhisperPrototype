#pragma once

namespace Whisper
{
	using pfnLoadProgress = HRESULT( __stdcall* )( double val, void* pv );
	// Return S_OK to continue, or S_FALSE to fail with "The operation was canceled by the user" status code
	using pfnCancel = HRESULT( __stdcall* )( void* pv );
	
	struct sLoadModelCallbacks
	{
		pfnLoadProgress progress;
		pfnCancel cancel;
		void* pv;
	};
}