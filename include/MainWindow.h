#pragma once

#include "Window.h"
#include "HexView.h"
#include "HelpWindow.h"
#include "File.h"

#include <string>

class MainWindow : public Window
{
    public:
        MainWindow(File& file);
        virtual void OnWindowRefreshed();
        virtual void OnKeyEvent(KeyEvent& keyEvent);

    private:
        std::string m_filename;//   Max Length = [MAX_PATH]
        HexView m_hexView;
        HelpWindow m_helpWindow;
		File& m_file;
};
