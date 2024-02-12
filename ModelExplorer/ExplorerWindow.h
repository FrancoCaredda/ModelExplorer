#ifndef _EXPLORER_WINDOW_H
#define _EXPLORER_WINDOW_H

#include "Core/AWindow.h"

class ExplorerWindow : public AWindow
{
public:
	ExplorerWindow(const std::string& title, int width, int height, bool fullscreen = false) 
		: AWindow(title, width, height) {};
	virtual ~ExplorerWindow() = default;
protected:
	// Inherited via AWindow
	virtual void Update(float deltaTime) override;

};

#endif // !_EXPLORER_WINDOW_H
