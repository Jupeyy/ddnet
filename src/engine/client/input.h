/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef ENGINE_CLIENT_INPUT_H
#define ENGINE_CLIENT_INPUT_H

#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>

class CInput : public IEngineInput
{
	IEngineGraphics *m_pGraphics;

	int m_InputGrabbed;
	char *m_pClipboardText;

	int64 m_LastRelease;
	int64 m_ReleaseDelta;

	bool m_MouseFocus;
	int m_VideoRestartNeeded;

	void AddEvent(char *pText, int Key, int Flags);
	void Clear();
	bool IsEventValid(CEvent *pEvent) const { return pEvent->m_InputCount == m_InputCounter; };

	// quick access to input
	unsigned short m_aInputCount[g_MaxKeys];	// tw-KEY
	unsigned char m_aInputState[g_MaxKeys];	// SDL_SCANCODE
	int m_InputCounter;

	// IME support
	int m_CountEditingText;
	char m_aEditingText[32];
	int m_EditingCursor;

	bool KeyState(int Key) const;

	IEngineGraphics *Graphics() { return m_pGraphics; }

	std::thread* m_pPollThread;

	volatile bool m_SetNewMouseMode;
	volatile bool m_RelativeMouseMode;

	volatile bool m_Finished;
	void Run();

	std::mutex m_EventMtx;
	std::vector<SDL_Event> m_SDLEvents;

public:
	volatile bool m_InitGraphics;
	volatile int m_InitGraphicsRet;

	CInput();
	~CInput();

	virtual void InitThread(IEngineGraphics* pGraphics);
	virtual void Init();

	bool KeyIsPressed(int Key) const { return KeyState(Key); }
	bool KeyPress(int Key, bool CheckCounter) const { return CheckCounter ? (m_aInputCount[Key] == m_InputCounter) : m_aInputCount[Key]; }

	virtual void MouseRelative(float *x, float *y);
	virtual void MouseModeAbsolute();
	virtual void MouseModeRelative();
	virtual int MouseDoubleClick();
	virtual const char* GetClipboardText();
	virtual void SetClipboardText(const char *Text);

	virtual int Update();
	virtual void NextFrame();

	virtual int VideoRestartNeeded();

	virtual bool GetIMEState();
	virtual void SetIMEState(bool Activate);
	virtual const char* GetIMECandidate();
	virtual int GetEditingCursor();
};

#endif
