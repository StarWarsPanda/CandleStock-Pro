#include "CandleStock.h"

CandleStock::MainForm::MainForm()
{
	std::string title = "Candlestock Pro", iconPath = "mainLogo.ico";
	char windowState = 0;
	uint16_t width = 1280, height = 720;

	padding = 2.0;

	//int complete = loadSetting("settings", title, windowState, width, height, iconPath);

	this->StockFile = gcnew System::Windows::Forms::OpenFileDialog();

	wchar_t exePath[MAX_PATH];
	GetModuleFileName(nullptr, exePath, MAX_PATH);

	// Extract directory path
	std::wstring directoryPath(exePath);
	size_t lastSlashPos = directoryPath.find_last_of(L"\\");
	directoryPath = directoryPath.substr(0, lastSlashPos);

	this->StockFile->InitialDirectory = gcnew System::String(directoryPath.c_str());
	this->StockFile->InitialDirectory = this->StockFile->InitialDirectory->Substring(0, this->StockFile->InitialDirectory->LastIndexOf(L"\\"));
	this->StockFile->InitialDirectory = this->StockFile->InitialDirectory->Substring(0, this->StockFile->InitialDirectory->LastIndexOf(L"\\")) + L"\\Stock Data";

	if (!System::IO::Directory::Exists(this->StockFile->InitialDirectory))
	{
		System::IO::Directory::CreateDirectory(this->StockFile->InitialDirectory);
	}

	this->StockFile->FileName = this->StockFile->InitialDirectory + "\\" + L"AAPL-Day.csv";

	this->Name = gcnew System::String(title.c_str());
	this->Text = gcnew System::String(title.c_str());
	this->WindowState = static_cast<System::Windows::Forms::FormWindowState>(windowState);
	this->ClientSize = System::Drawing::Size(width, height);
	this->Icon = gcnew System::Drawing::Icon(gcnew System::String(iconPath.c_str()));
	this->MaximizeBox = false;
	this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;

	std::vector<Candlestick> Stock = loadStock(msclr::interop::marshal_as<std::string>(this->StockFile->FileName), ParseType::CSV);

	/* objects */
	this->stockFileOpener = gcnew System::Windows::Forms::Button();
	this->stockFileOpener->Name = L"Stock File Opener";
	this->stockFileOpener->Text = gcnew System::String(Stock[0].m_name.c_str());

	this->stockFileOpener->Location = System::Drawing::Point(590, 637);
	this->stockFileOpener->Size = System::Drawing::Size(100, 35);

	this->stockPeriodSelector = gcnew System::Windows::Forms::ComboBox();
	
	this->stockPeriodSelector->Name = L"Stock Period Selector";
	
	this->stockPeriodSelector->Location = System::Drawing::Point(590, 610);
	this->stockPeriodSelector->Size = System::Drawing::Size(100, 35);

	array<System::String^>^ PickerItems = gcnew array<System::String^>(3) { L"Daily", L"Weekly", L"Monthly" };

	this->stockPeriodSelector->AutoCompleteCustomSource->AddRange(PickerItems);
	this->stockPeriodSelector->Items->AddRange(PickerItems);
	this->stockPeriodSelector->AutoCompleteMode = System::Windows::Forms::AutoCompleteMode::Suggest;
	this->stockPeriodSelector->AutoCompleteSource = System::Windows::Forms::AutoCompleteSource::CustomSource;

	System::String^ selectedItem = this->StockFile->FileName->Substring(this->StockFile->FileName->LastIndexOf('-') + 1, this->StockFile->FileName->IndexOf('.') - this->StockFile->FileName->LastIndexOf('-') - 1);

	if (selectedItem == "Day")
	{
		this->stockPeriodSelector->SelectedItem = L"Daily";
	}
	else if (selectedItem == "Week")
	{
		this->stockPeriodSelector->SelectedItem = L"Weekly";
	}
	else if (selectedItem == "Month")
	{
		this->stockPeriodSelector->SelectedItem = L"Monthly";
	}

	this->updateButton = gcnew System::Windows::Forms::Button();

	this->updateButton->Name = L"Update Button";

	this->updateButton->Text = L"Update";
	this->updateButton->Location = System::Drawing::Point(590, 570);
	this->updateButton->Size = System::Drawing::Size(100, 35);

	this->startDateSelector = gcnew System::Windows::Forms::MonthCalendar();
	this->endDateSelector = gcnew System::Windows::Forms::MonthCalendar();

	this->startDateSelector->Name = L"Start DateTimePicker";
	this->endDateSelector->Name = L"End DateTimePicker";

	this->startDateSelector->Location = System::Drawing::Point(220, 545);
	this->endDateSelector->Location = System::Drawing::Point(835, 545);

	this->startDateSelector->SelectionStart = Stock[1].m_date.toDateTime().AddDays(1);
	this->startDateSelector->SelectionEnd = Stock[1].m_date.toDateTime().AddDays(1);
	this->endDateSelector->SelectionStart = Stock[Stock.size() - 1].m_date.toDateTime().AddDays(-1);
	this->endDateSelector->SelectionEnd = Stock[Stock.size() - 1].m_date.toDateTime().AddDays(-1);

	this->startDateSelector->MinDate = Stock[0].m_date.toDateTime().AddDays(1);
	this->startDateSelector->MaxDate = Stock[Stock.size() - 1].m_date.toDateTime();
	this->endDateSelector->MinDate = Stock[0].m_date.toDateTime().AddDays(1);
	this->endDateSelector->MaxDate = Stock[Stock.size() - 1].m_date.toDateTime();

	this->StockChart = gcnew System::Windows::Forms::DataVisualization::Charting::Chart();

	System::Windows::Forms::DataVisualization::Charting::ChartArea^ StockChartArea = gcnew System::Windows::Forms::DataVisualization::Charting::ChartArea(L"Stock Chart");
	System::Windows::Forms::DataVisualization::Charting::ChartArea^ VolumeChartArea = gcnew System::Windows::Forms::DataVisualization::Charting::ChartArea(L"Volume Chart");

	StockChartArea->AxisX->Minimum = this->startDateSelector->SelectionStart.ToOADate();
	StockChartArea->AxisX->Maximum = this->endDateSelector->SelectionStart.ToOADate();

	VolumeChartArea->AxisX->Minimum = StockChartArea->AxisX->Minimum;
	VolumeChartArea->AxisX->Maximum = StockChartArea->AxisX->Maximum;

	StockChartArea->AxisX->ScrollBar->Enabled = true;
	StockChartArea->AxisX->ScaleView->Zoomable = true;
	StockChartArea->AxisY->ScrollBar->Enabled = true;
	StockChartArea->AxisY->ScaleView->Zoomable = true;

	this->StockChart->Location = System::Drawing::Point(220, 0);
	this->StockChart->Text = "Stock Chart";
	this->StockChart->Size = System::Drawing::Size(840, 545);

	System::Windows::Forms::DataVisualization::Charting::Series^ candleStickSeries = gcnew System::Windows::Forms::DataVisualization::Charting::Series(L"Candlestick Series");
	candleStickSeries->ChartArea = L"Stock Chart";
	candleStickSeries->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::Candlestick;

	candleStickSeries->XValueType = System::Windows::Forms::DataVisualization::Charting::ChartValueType::Date;

	candleStickSeries->Color = System::Drawing::Color::Black;
	candleStickSeries->CustomProperties = "PriceDownColor=Red, PriceUpColor=Lime";

	for each (Candlestick candlestick in Stock)
	{
		candleStickSeries->Points->Add(gcnew System::Windows::Forms::DataVisualization::Charting::DataPoint(candlestick.m_date.toDateTime().ToOADate(), candlestick.m_low + L", " + candlestick.m_high + L", " + candlestick.m_open + L", " + candlestick.m_close));
	}

	int minIndex = PointsMinIndex(candleStickSeries, StockChartArea->AxisX->Minimum);
	int maxIndex = PointsMaxIndex(candleStickSeries, minIndex, StockChartArea->AxisX->Maximum);

	StockChartArea->AxisY->Minimum = PointsMinHeight(candleStickSeries, minIndex - 1, maxIndex + 1) - padding;
	StockChartArea->AxisY->Maximum = PointsMaxHeight(candleStickSeries, minIndex - 1, maxIndex + 1) + padding;

	System::Windows::Forms::DataVisualization::Charting::Series^ volumeSeries = gcnew System::Windows::Forms::DataVisualization::Charting::Series(L"Volume Series");
	volumeSeries->ChartArea = L"Volume Chart";
	volumeSeries->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::Column;

	volumeSeries->XValueType = System::Windows::Forms::DataVisualization::Charting::ChartValueType::Date;

	for each (Candlestick candlestick in Stock)
	{
		volumeSeries->Points->Add(gcnew System::Windows::Forms::DataVisualization::Charting::DataPoint(candlestick.m_date.toDateTime().ToOADate(), candlestick.m_volume));
	}
	
	this->StockChart->ChartAreas->Add(StockChartArea);
	this->StockChart->Series->Add(candleStickSeries);
	this->StockChart->ChartAreas->Add(VolumeChartArea);
	this->StockChart->Series->Add(volumeSeries);

	this->Controls->Add(stockFileOpener);
	this->Controls->Add(stockPeriodSelector);
	this->Controls->Add(updateButton);

	this->Controls->Add(startDateSelector);
	this->Controls->Add(endDateSelector);
	
	this->Controls->Add(StockChart);

	/* events */
	this->Resize += gcnew System::EventHandler(this, &MainForm::whenWindowResized);
	this->updateButton->Click += gcnew System::EventHandler(this, &MainForm::whenUpdateButtonClicked);
	this->stockFileOpener->Click += gcnew System::EventHandler(this, &MainForm::whenStockFileOpenerButtonClicked);
	this->StockFile->FileOk += gcnew System::ComponentModel::CancelEventHandler(this, &MainForm::whenStockFileOK);
	this->stockPeriodSelector->SelectedValueChanged += gcnew System::EventHandler(this, &MainForm::whenPeriodItemChanged);
}

CandleStock::MainForm::~MainForm() 
{
	saveSetting("settings", (uint16_t)(this->Width), (uint16_t)(this->Height));
}

System::Void CandleStock::MainForm::whenWindowResized(System::Object^ sender, System::EventArgs^ e)
{
	
}

System::Void CandleStock::MainForm::whenUpdateButtonClicked(System::Object^ sender, System::EventArgs^ e)
{
	if (this->startDateSelector->SelectionStart.ToOADate() > this->StockChart->Series[0]->Points[1]->XValue && this->endDateSelector->SelectionStart.ToOADate() < this->StockChart->Series[0]->Points[this->StockChart->Series[0]->Points->Count - 1]->XValue)
	{
		this->StockChart->ChartAreas[0]->AxisX->Minimum = this->startDateSelector->SelectionStart.ToOADate();
		this->StockChart->ChartAreas[0]->AxisX->Maximum = this->endDateSelector->SelectionStart.ToOADate();

		this->StockChart->ChartAreas[1]->AxisX->Minimum = this->StockChart->ChartAreas[0]->AxisX->Minimum;
		this->StockChart->ChartAreas[1]->AxisX->Maximum = this->StockChart->ChartAreas[0]->AxisX->Maximum;

		int minIndex = PointsMinIndex(this->StockChart->Series[0], this->StockChart->ChartAreas[0]->AxisX->Minimum);
		int maxIndex = PointsMaxIndex(this->StockChart->Series[0], minIndex, this->StockChart->ChartAreas[0]->AxisX->Maximum);

		this->StockChart->ChartAreas[0]->AxisY->Minimum = PointsMinHeight(this->StockChart->Series[0], minIndex - 1, maxIndex + 1) - padding;
		this->StockChart->ChartAreas[0]->AxisY->Maximum = PointsMaxHeight(this->StockChart->Series[0], minIndex - 1, maxIndex + 1) + padding;
	}
}

System::Void CandleStock::MainForm::whenStockFileOpenerButtonClicked(System::Object^ sender, System::EventArgs^ e)
{
	this->StockFile->ShowDialog();
}

System::Void CandleStock::MainForm::whenStockFileOK(System::Object^ sender, System::ComponentModel::CancelEventArgs^ e)
{
	System::String^ selectedItem = this->StockFile->FileName->Substring(this->StockFile->FileName->LastIndexOf('-') + 1, this->StockFile->FileName->IndexOf('.') - this->StockFile->FileName->LastIndexOf('-') - 1);
	this->StockFile->FileName = this->StockFile->FileName->Substring(0, this->StockFile->FileName->LastIndexOf('-') + 1);

	if (selectedItem == "Day")
	{
		this->stockPeriodSelector->SelectedItem = L"Daily";
	}
	else if (selectedItem == "Week")
	{
		this->stockPeriodSelector->SelectedItem = L"Weekly";
	}
	else if (selectedItem == "Month")
	{
		this->stockPeriodSelector->SelectedItem = L"Monthly";
	}
}

System::Void CandleStock::MainForm::whenPeriodItemChanged(System::Object^ sender, System::EventArgs^ e)
{
	System::String^ period = this->stockPeriodSelector->SelectedItem->ToString();

	if (this->stockPeriodSelector->SelectedItem == "Daily")
	{
		period = "Day";
	}
	else if (this->stockPeriodSelector->SelectedItem == "Weekly")
	{
		period = "Week";
	}
	else if (this->stockPeriodSelector->SelectedItem == "Monthly")
	{
		period = "Month";
	}

	if (this->StockFile->FileName->EndsWith(".csv"))
	{
		this->StockFile->FileName = this->StockFile->FileName->Substring(0, this->StockFile->FileName->LastIndexOf('-') + 1);
	}

	System::String^ Filename = (this->StockFile->FileName + period + ".csv");

	std::vector<Candlestick> Stock = loadStock(msclr::interop::marshal_as<std::string>(Filename), ParseType::CSV);

	this->startDateSelector->MinDate = Stock[0].m_date.toDateTime().AddDays(1);
	this->startDateSelector->MaxDate = Stock[Stock.size() - 1].m_date.toDateTime();
	this->endDateSelector->MinDate = Stock[0].m_date.toDateTime().AddDays(1);
	this->endDateSelector->MaxDate = Stock[Stock.size() - 1].m_date.toDateTime();

	this->startDateSelector->SelectionStart = Stock[1].m_date.toDateTime().AddDays(1);
	this->startDateSelector->SelectionEnd = Stock[1].m_date.toDateTime().AddDays(1);
	this->endDateSelector->SelectionStart = Stock[Stock.size() - 1].m_date.toDateTime().AddDays(-1);
	this->endDateSelector->SelectionEnd = Stock[Stock.size() - 1].m_date.toDateTime().AddDays(-1);


	this->StockChart->ChartAreas[0]->AxisX->Minimum = this->startDateSelector->SelectionStart.ToOADate();
	this->StockChart->ChartAreas[0]->AxisX->Maximum = this->endDateSelector->SelectionStart.ToOADate();

	this->StockChart->ChartAreas[1]->AxisX->Minimum = this->StockChart->ChartAreas[0]->AxisX->Minimum;
	this->StockChart->ChartAreas[1]->AxisX->Maximum = this->StockChart->ChartAreas[0]->AxisX->Maximum;

	this->StockChart->Series[0]->Points->Clear();

	for each (Candlestick candlestick in Stock)
	{
		this->StockChart->Series[0]->Points->Add(gcnew System::Windows::Forms::DataVisualization::Charting::DataPoint(candlestick.m_date.toDateTime().ToOADate(), candlestick.m_low + L", " + candlestick.m_high + L", " + candlestick.m_open + L", " + candlestick.m_close));
	}

	int minIndex = PointsMinIndex(this->StockChart->Series[0], this->StockChart->ChartAreas[0]->AxisX->Minimum);
	int maxIndex = PointsMaxIndex(this->StockChart->Series[0], minIndex, this->StockChart->ChartAreas[0]->AxisX->Maximum);

	this->StockChart->ChartAreas[0]->AxisY->Minimum = PointsMinHeight(this->StockChart->Series[0], minIndex - 1, maxIndex + 1) - padding;
	this->StockChart->ChartAreas[0]->AxisY->Maximum = PointsMaxHeight(this->StockChart->Series[0], minIndex - 1, maxIndex + 1) + padding;

	this->StockChart->Series[1]->Points->Clear();

	for each (Candlestick candlestick in Stock)
	{
		this->StockChart->Series[1]->Points->Add(gcnew System::Windows::Forms::DataVisualization::Charting::DataPoint(candlestick.m_date.toDateTime().ToOADate(), candlestick.m_volume));
	}

	this->StockChart->Refresh();
}

int CandleStock::loadSetting(const std::string& filename, std::string& title, char windowState, uint16_t width, uint16_t height, std::string& icon)
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

int CandleStock::saveSetting(const std::string& filename, std::string& title, char windowState, uint16_t width, uint16_t height, std::string& icon)
{
	if (!std::filesystem::exists(filename)) return 1;

	std::ofstream file(filename, std::ios::binary);

	if (!file.is_open()) return 2;

	file << title << 0x7c << windowState << 0x7c << width << 0x7c << height << icon;

	return 0;
}

int CandleStock::saveSetting(const std::string& filename, std::string& title)
{
	if (!std::filesystem::exists(filename)) return 1;

	std::fstream outfile(filename, std::ios::binary);
	std::ifstream infile(filename, std::ios::binary);

	

	return 0;
}

int CandleStock::PointsMinIndex(System::Windows::Forms::DataVisualization::Charting::Series^ series, double minOADate)
{
	if (series->Points->Count == 0) return -1;

	if (series->Points[0]->XValue >= minOADate)
	{
		return 0;
	}

	if (series->Points[series->Points->Count - 1]->XValue <= minOADate)
	{
		return series->Points->Count - 1;
	}

	int minIndex = -1;

	int low = 0, high = series->Points->Count - 1;

	while (low <= high)
	{
		int mid = (low + high) / 2;

		if (series->Points[mid]->XValue >= minOADate)
		{
			minIndex = mid;
			high = mid - 1;
		}
		else
		{
			low = mid + 1;
		}
	}

	return minIndex;
}

int CandleStock::PointsMaxIndex(System::Windows::Forms::DataVisualization::Charting::Series^ series, int minIndex, double maxOADate)
{
	int maxIndex = -1;

	int low = (minIndex < 0) ? 0 : minIndex, high = series->Points->Count - 1;

	while (low <= high)
	{
		int mid = (low + high) / 2;

		if (series->Points[mid]->XValue <= maxOADate)
		{
			maxIndex = mid;
			low = mid + 1;
		}
		else
		{
			high = mid - 1;
		}
	}

	return maxIndex;
}

double CandleStock::PointsMinHeight(System::Windows::Forms::DataVisualization::Charting::Series^ series, int minIndex, int maxIndex)
{
	double minHeight = double::MaxValue;

	for (size_t i = minIndex; i < maxIndex && i < series->Points->Count; i++)
	{
		for each (double value in series->Points[i]->YValues)
		{
			if (value < minHeight)
			{
				minHeight = value;
			}
		}
	}

	return minHeight;
}

double CandleStock::PointsMaxHeight(System::Windows::Forms::DataVisualization::Charting::Series^ series, int minIndex, int maxIndex)
{
	double maxHeight = double::MinValue;

	for (size_t i = minIndex; i < maxIndex && i < series->Points->Count; i++)
	{
		for each (double value in series->Points[i]->YValues)
		{
			if (value > maxHeight)
			{
				maxHeight = value;
			}
		}
	}

	return maxHeight;
}

int CandleStock::saveSetting(const std::string& filename, char windowState)
{
	if (!std::filesystem::exists(filename)) return 1;

	std::fstream outfile(filename, std::ios::binary);
	std::ifstream infile(filename, std::ios::binary);

	while (infile.get() != 0x7c);

	outfile.seekp(infile.tellg());
	outfile << windowState;

	return 0;
}

int CandleStock::saveSetting(const std::string& filename, uint16_t width, uint16_t height)
{
	if (!std::filesystem::exists(filename)) return 1;

	std::fstream outfile(filename, std::ios::binary);
	std::ifstream infile(filename, std::ios::binary);

	while (infile.get() != 0x7c);

	outfile.seekp(infile.tellg());
	outfile << width << 0x7c << height;

	return 0;
}
