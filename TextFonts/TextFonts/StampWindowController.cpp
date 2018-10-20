#include "StampWindowController.h"
#include "RectangleStampDrawer.h"

namespace Stamp
{
	const UINT StampWindowController::WM_DRAW_STAMP = WM_USER;

	LRESULT StampWindowController::HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (m_asStampDrawers == NULL)
		{
			return DefWindowProc(hWnd, message, wParam, lParam);
		}

		switch (message)
		{
		case WM_SIZE:
			m_asStampDrawers[m_uCurDrawer]->UpdateStampSize();
			PostDrawStampMessage(hWnd);
			return DefWindowProc(hWnd, message, wParam, lParam);
		case WM_CHAR:
			switch (wParam)
			{
			case VK_BACK:
				if (m_asStampDrawers[m_uCurDrawer]->GetTextLength() > 0)
				{
					m_asStampDrawers[m_uCurDrawer]->DeleteLastSymbol();
					PostDrawStampMessage(hWnd);
				}
				break;
			case VK_RETURN:
			case VK_ESCAPE:
				break;
			default:
				m_asStampDrawers[m_uCurDrawer]->AddSymbol((TCHAR)wParam);
				PostDrawStampMessage(hWnd);
			}
			break;
		case WM_KEYDOWN:
			switch (wParam)
			{
			case VK_F2:
				if (LoadStampBackground(hWnd) == LR_OK)
				{
					PostDrawStampMessage(hWnd);
				}
				break;
			case VK_F3:
				if (m_asStampDrawers[m_uCurDrawer]->HasStampImage())
				{
					m_asStampDrawers[m_uCurDrawer]->DeleteBackgroundImage();
					PostDrawStampMessage(hWnd);
				}
				break;
			case VK_F4:
				NextDrawer();
				PostDrawStampMessage(hWnd);
				break;
			}
			break;
		case WM_MOUSEWHEEL:
			if (GET_KEYSTATE_WPARAM(wParam) == MK_SHIFT)
			{
				if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
				{
					if (m_asStampDrawers[m_uCurDrawer]->IncrementStampIndent())
					{
						PostSizeMessage(hWnd);
					}
				}
				else
				{
					if (m_asStampDrawers[m_uCurDrawer]->DecrementStampIndent())
					{
						PostSizeMessage(hWnd);
					}
				}
			}
			else
			{
				if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
				{
					if (m_asStampDrawers[m_uCurDrawer]->IncrementFontSize())
					{
						PostDrawStampMessage(hWnd);
					}
				}
				else
				{
					if (m_asStampDrawers[m_uCurDrawer]->DecrementFontSize())
					{
						PostDrawStampMessage(hWnd);
					}
				}
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_DRAW_STAMP:
			m_asStampDrawers[m_uCurDrawer]->Draw();
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		return 0;
	}

	COLORREF StampWindowController::GetDefaultBackgroundColor()
	{
		return GetSysColor(COLOR_WINDOW);
	}

	void StampWindowController::ClearDrawers()
	{
		if (m_asStampDrawers != NULL)
		{
			for (size_t uDrawer = 0; uDrawer < m_uDrawersCount; uDrawer++)
			{
				delete m_asStampDrawers[uDrawer];
			}
			free(m_asStampDrawers);
			m_asStampDrawers = NULL;
			m_uDrawersCount = 0;
		}
	}

	BOOL StampWindowController::AddDrawer(StampDrawer *sStampDrawer)
	{
		if (sStampDrawer == NULL)
		{
			return FALSE;
		}
		StampDrawer **asNewStampDrawers = (StampDrawer **)realloc(m_asStampDrawers, (++m_uDrawersCount) * sizeof(StampDrawer *));
		if (asNewStampDrawers == NULL)
		{
			return FALSE;
		}
		else
		{
			asNewStampDrawers[m_uDrawersCount - 1] = sStampDrawer;
			m_asStampDrawers = asNewStampDrawers;
			return TRUE;
		}
	}

	size_t StampWindowController::NextDrawer()
	{
		if ((m_asStampDrawers == NULL) || (m_uDrawersCount < 2))
		{
			return 0;
		}

		m_asStampDrawers[(m_uCurDrawer + 1) % m_uDrawersCount]->SetByAnother(m_asStampDrawers[m_uCurDrawer]);
		m_uCurDrawer = (m_uCurDrawer + 1) % m_uDrawersCount;
		return m_uCurDrawer;
	}

	size_t StampWindowController::GetDrawersCount()
	{
		return m_uDrawersCount;
	}

	LoadResult StampWindowController::LoadStampBackground(HWND hWnd)
	{
		char psFileName[MAX_PATH] = { '\0' };

		OPENFILENAME oOpenFileName;
		oOpenFileName.lStructSize = sizeof(OPENFILENAME);
		oOpenFileName.hwndOwner = hWnd;
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
			if (m_asStampDrawers[m_uCurDrawer]->LoadBackgroundImage(psFileName))
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

	BOOL StampWindowController::PostDrawStampMessage(HWND hWnd)
	{
		return PostMessage(hWnd, WM_DRAW_STAMP, NULL, NULL);
	}

	BOOL StampWindowController::PostSizeMessage(HWND hWnd)
	{
		return PostMessage(hWnd, WM_SIZE, NULL, NULL);
	}

	StampWindowController::StampWindowController() : m_asStampDrawers(NULL),
		m_uDrawersCount(0)
	{ }

	StampWindowController::~StampWindowController()
	{
		ClearDrawers();
	}
}
