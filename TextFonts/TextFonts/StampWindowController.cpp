#include "StampWindowController.h"
#include "RectangleStampDrawer.h"

namespace Stamp
{
	const UINT StampWindowController::WM_DRAW_STAMP = WM_USER;

	LRESULT StampWindowController::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_SIZE:
			m_sStampDrawer->UpdateStampSize();
			PostDrawStampMessage();
			return DefWindowProc(m_hWnd, message, wParam, lParam);
		case WM_CHAR:
			switch (wParam)
			{
			case VK_BACK:
				if (m_sStampDrawer->GetTextLength() > 0)
				{
					m_sStampDrawer->DeleteLastSymbol();
					PostDrawStampMessage();
				}
				break;
			case VK_RETURN:
			case VK_ESCAPE:
				break;
			default:
				m_sStampDrawer->AddSymbol((TCHAR)wParam);
				PostDrawStampMessage();
			}
			break;
		case WM_KEYDOWN:
			switch (wParam)
			{
			case VK_F2:
				if (LoadStampBackground() == LR_OK)
				{
					PostDrawStampMessage();
				}
				break;
			case VK_F3:
				if (m_sStampDrawer->HasStampImage())
				{
					m_sStampDrawer->DeleteBackgroundImage();
					PostDrawStampMessage();
				}
				break;
			}
			break;
		case WM_MOUSEWHEEL:
			if (GET_KEYSTATE_WPARAM(wParam) == MK_SHIFT)
			{
				if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
				{
					if (m_sStampDrawer->IncrementStampIndent())
					{
						PostSizeMessage();
					}
				}
				else
				{
					if (m_sStampDrawer->DecrementStampIndent())
					{
						PostSizeMessage();
					}
				}
			}
			else
			{
				if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
				{
					if (m_sStampDrawer->IncrementFontSize())
					{
						PostDrawStampMessage();
					}
				}
				else
				{
					if (m_sStampDrawer->DecrementFontSize())
					{
						PostDrawStampMessage();
					}
				}
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_DRAW_STAMP:
			m_sStampDrawer->Draw();
			break;
		default:
			return DefWindowProc(m_hWnd, message, wParam, lParam);
		}
		return 0;
	}

	COLORREF StampWindowController::GetBackgroundColor()
	{
		return m_crBackgroundColor;
	}

	void StampWindowController::SetBackgroundColor(COLORREF crBackgroundColor)
	{
		m_crBackgroundColor = crBackgroundColor;
	}

	COLORREF StampWindowController::GetDefaultBackgroundColor()
	{
		return GetSysColor(COLOR_WINDOW);
	}

	LoadResult StampWindowController::LoadStampBackground()
	{
		char psFileName[MAX_PATH] = { '\0' };

		OPENFILENAME oOpenFileName;
		oOpenFileName.lStructSize = sizeof(OPENFILENAME);
		oOpenFileName.hwndOwner = m_hWnd;
		oOpenFileName.hInstance = NULL;
		oOpenFileName.lpstrFilter = "Images\0*.bmp;*.gif;*.jpeg;*.png;*.tiff;*.exif;*.wmf;*.emf;*.jpg\0\0";
		oOpenFileName.lpstrCustomFilter = NULL;
		oOpenFileName.nFilterIndex = 1;
		oOpenFileName.lpstrFile = psFileName;
		oOpenFileName.nMaxFile = sizeof(psFileName);
		oOpenFileName.lpstrFileTitle = NULL;
		oOpenFileName.lpstrInitialDir = NULL;
		oOpenFileName.lpstrTitle = "Select stamp background";
		oOpenFileName.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
		oOpenFileName.lpstrDefExt = NULL;

		if (GetOpenFileName(&oOpenFileName))
		{
			if (m_sStampDrawer->LoadBackgroundImage(psFileName))
			{
				return LR_OK;
			}
			else
			{
				return LR_ERROR;
			}
		}
		return LR_CANCELLED_BY_USER;
	}

	BOOL StampWindowController::PostDrawStampMessage()
	{
		return PostMessage(m_hWnd, WM_DRAW_STAMP, NULL, NULL);
	}

	BOOL StampWindowController::PostSizeMessage()
	{
		return PostMessage(m_hWnd, WM_SIZE, NULL, NULL);
	}

	StampWindowController::StampWindowController(HWND hWnd) 
		: StampWindowController::StampWindowController(hWnd, GetDefaultBackgroundColor())
	{ }

	StampWindowController::StampWindowController(HWND hWnd, COLORREF crBackgroundColor) 
		: m_sStampDrawer(new RectangleStampDrawer(hWnd, crBackgroundColor)), 
		m_crBackgroundColor(crBackgroundColor), m_hWnd(hWnd)
	{ }

	StampWindowController::~StampWindowController()
	{
		if (m_sStampDrawer != NULL)
		{
			delete m_sStampDrawer;
		}
	}
}
