#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <filesystem>
#include <iostream>
#include <fstream>

namespace CandleStock
{
	struct Date
	{
		typedef enum e_Month
		{
			em_none,
			January,
			February,
			March,
			April,
			May,
			June,
			July,
			August,
			September,
			October,
			November,
			December
		}Month;

		typedef enum e_ParseOrder
		{
			ep_none,
			MDY,
			DMY,
			YMD,
			YDM
		}ParseOrder;

		public:
			uint16_t m_year;
		public:
			Date();
			Date(System::DateTime date);
			Date(uint8_t month, uint8_t day, uint16_t year);
			Date(std::string date, ParseOrder order, char seperator);
			~Date();

			uint8_t getMonth();
			uint8_t getDay();
			void setMonth(uint8_t& month);
			void setDay(uint8_t& day);
			
			System::DateTime toDateTime();

			bool operator!=(Date& rhs)
			{
				return (m_day != rhs.getDay() ||
					m_month != rhs.getMonth() ||
					m_year != rhs.m_year);
			}

		private:
			uint8_t m_day, m_month;
	};


	typedef enum e_ParseType
	{
		none,
		CSV
		/* JSON */

	}ParseType;

	class Candlestick
	{
		public:
			std::string m_name;
			Date m_date;
			double m_open, m_high, m_low, m_close;
			uint32_t m_volume;
		public:
			Candlestick();
			Candlestick(std::string name, Date date, double open, double high, double low, double close, uint32_t volume);
			Candlestick(std::string data, ParseType dataType, std::string header, std::string name);
			~Candlestick();

			bool operator!=(Candlestick& rhs)
			{
				return (m_name != rhs.m_name ||
					m_date != rhs.m_date ||
					m_open != rhs.m_open ||
					m_high != rhs.m_high ||
					m_low != rhs.m_low ||
					m_close != rhs.m_close ||
					m_volume != rhs.m_volume);
			}
	};

	std::vector<Candlestick> loadStock(std::string filename, ParseType dataType);

	template <typename T>
	int findValue(const std::vector<T>& vec, const T& value) {
		auto it = std::find(vec.begin(), vec.end(), value);
		if (it != vec.end()) {
			return std::distance(vec.begin(), it);
		}
		else {
			return -1;
		}
	}
}

