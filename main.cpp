#include "CandleStock.h"

using namespace System;
using namespace System::Windows::Forms;

[STAThread]
#ifdef _DEBUG
int main()
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif // DEBUG
{
	System::Windows::Forms::Application::EnableVisualStyles();
	System::Windows::Forms::Application::Run(gcnew CandleStock::MainForm());
	return 0;
}