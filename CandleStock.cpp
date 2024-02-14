#include "CandleStock.h"

CandleStock::MainForm::MainForm()
{
	std::string title = "Title", iconPath = "";
	char windowState = 0;
	uint16_t width = 960, height = 540;

	int complete = importSetting("settings", title, windowState, width, height, iconPath);

	this->Name = gcnew System::String(title.c_str());
	this->Text = gcnew System::String(title.c_str());
	this->WindowState = static_cast<System::Windows::Forms::FormWindowState>(windowState);
	this->ClientSize = System::Drawing::Size(width, height);
	this->Icon = gcnew System::Drawing::Icon(gcnew System::String(iconPath.c_str()));
}

CandleStock::MainForm::~MainForm() {}

int CandleStock::importSetting(const std::string& filename, std::string& title, char& windowState, uint16_t& width, uint16_t height, std::string& icon)
{
	if (!std::filesystem::exists(filename)) return 1;

	std::ifstream file(filename, std::ios::binary);

	if (!file.is_open()) return 2;

	title = "";
	windowState = 0;
	width = 0;
	height = 0;
	icon = "";

	char data;
	while (file.get(data))
	{
		if (data == 0x7c) break;

		title.push_back(data);
	}

	while (file.get(data))
	{
		if (data == 0x7c) break;

		windowState = (windowState << 8) + static_cast<uint8_t>(data);
	}

	while (file.get(data))
	{
		if (data == 0x7c) break;

		width = (width << 8) + static_cast<uint8_t>(data);
	}

	while (file.get(data))
	{
		if (data == 0x7c) break;

		height = (height << 8) + static_cast<uint8_t>(data);
	}

	while (file.get(data))
	{
		if (data == 0x7c) break;

		icon.push_back(data);
	}

	return 0;
}
