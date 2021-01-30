#pragma once
#include "src/Cosmic.h"
#include "src/core/World.h"
#include "src/platforms/Win32PlatformCallback.h"
#include "src/rendering/DirectX11Renderer.h"
#include "src/editor/GeneralWindow.h"
#include "src/Debug.h"

namespace cm
{
	class Editor : public Win32PlatformCallback
	{
	public:

		EditorContext context;
		std::vector<EditorWindow*> editor_windows;
		std::vector<std::shared_ptr<EditorChain>> editor_chain;

		GraphicsContext *gc;

	public:
		void Start();
		void Update(GameState *game_state);
		void Render();

		virtual LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) override;

	public:
		Editor(GraphicsContext *gc);
		Editor(const Editor &) = delete;
		Editor &operator=(const Editor &) = delete;
		virtual ~Editor() {};

	private:
		void StartImgui();
		void BeginFrame();
		void EndFrame();
	};


}