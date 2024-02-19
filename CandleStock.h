#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>

#include <msclr/marshal_cppstd.h>

#include "Candlestick.h"

namespace CandleStock
{
	int loadSetting(const std::string& filename, std::string& title, char windowState, uint16_t width, uint16_t height, std::string& icon);
	int saveSetting(const std::string& filename, std::string& title, char windowState, uint16_t width, uint16_t height, std::string& icon);
	int saveSetting(const std::string& filename, std::string& title);
	int saveSetting(const std::string& filename, char windowState);
	int saveSetting(const std::string& filename, uint16_t width, uint16_t height);
	int saveSetting(const std::string& filename, std::string& icon);

	public ref class MainForm : public System::Windows::Forms::Form
	{
		public:
			MainForm();
		public:
		protected:
			~MainForm();
		private:
			System::Void whenWindowResized(System::Object^ sender, System::EventArgs^ e);
            System::Void whenUpdateButtonClicked(System::Object^ sender, System::EventArgs^ e);
            System::Void whenStockFileOpenerButtonClicked(System::Object^ sender, System::EventArgs^ e);
            System::Void whenStockFileOK(System::Object^ sender, System::ComponentModel::CancelEventArgs^ e);
            System::Void whenPeriodItemChanged(System::Object^ sender, System::EventArgs^ e);
		private:
			System::Windows::Forms::OpenFileDialog^ StockFile;

			System::Windows::Forms::DataVisualization::Charting::Chart^ StockChart;
			System::Windows::Forms::DataGridView^ StockData;

			System::Windows::Forms::MonthCalendar^ startDateSelector;
			System::Windows::Forms::MonthCalendar^ endDateSelector;

			System::Windows::Forms::Button^ updateButton;
            System::Windows::Forms::Button^ stockFileOpener;
			System::Windows::Forms::ComboBox^ stockPeriodSelector;

            double padding;
	};

	int PointsMinIndex(System::Windows::Forms::DataVisualization::Charting::Series^ series, double minOADate);
	int PointsMaxIndex(System::Windows::Forms::DataVisualization::Charting::Series^ series, int minIndex, double maxOADate);
	double PointsMinHeight(System::Windows::Forms::DataVisualization::Charting::Series^ series, int minIndex, int maxIndex);
	double PointsMaxHeight(System::Windows::Forms::DataVisualization::Charting::Series^ series, int minIndex, int maxIndex);

    value struct ManagedCandlestick
    {
        public:
            property System::String^ Name {
                System::String^ get() { return m_name; }
                void set(System::String^ value) { m_name = value; }
            }

            property System::DateTime^ Date {
                System::DateTime^ get() { return m_date; }
                void set(System::DateTime^ value) { m_date = value; }
            }

            property double Open {
                double get() { return m_open; }
                void set(double value) { m_open = value; }
            }

            property double High {
                double get() { return m_high; }
                void set(double value) { m_high = value; }
            }

            property double Low {
                double get() { return m_low; }
                void set(double value) { m_low = value; }
            }

            property double Close {
                double get() { return m_close; }
                void set(double value) { m_close = value; }
            }

            property uint32_t Volume {
                uint32_t get() { return m_volume; }
                void set(uint32_t value) { m_volume = value; }
            }

            ManagedCandlestick(Candlestick candlestick)
                : m_open(candlestick.m_open), m_high(candlestick.m_high), m_low(candlestick.m_low), m_close(candlestick.m_close), m_volume(candlestick.m_volume)
            {
                m_name = gcnew System::String(candlestick.m_name.c_str());
                m_date = gcnew System::DateTime(candlestick.m_date.m_year, candlestick.m_date.getMonth(), candlestick.m_date.getDay());
            }

        private:
            System::String^ m_name;
            System::DateTime^ m_date;
            double m_open, m_high, m_low, m_close;
            uint32_t m_volume;
    };

}

