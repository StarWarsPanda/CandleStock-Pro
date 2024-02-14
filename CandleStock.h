#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>

namespace CandleStock
{
	int importSetting(const std::string& filename, std::string& title, char& windowState, uint16_t& width, uint16_t height, std::string& icon);

	public ref class MainForm : public System::Windows::Forms::Form
	{
		public:
			MainForm();
		protected:
			~MainForm();
	};
}