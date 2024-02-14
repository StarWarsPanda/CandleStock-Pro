#include "CandleStock.h"

int main()
{
	System::Windows::Forms::Application::EnableVisualStyles();
	System::Windows::Forms::Application::Run(gcnew CandleStock::MainForm());
	return 0;
}