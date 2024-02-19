#include "Candlestick.h"

CandleStock::Date::Date() :m_day(January), m_month(1), m_year(1970) {}
CandleStock::Date::Date(System::DateTime date) :m_day(date.Day), m_month(date.Month), m_year(date.Year) {}
CandleStock::Date::Date(uint8_t month, uint8_t day, uint16_t year) :m_day(day), m_month(month), m_year(year) {}

CandleStock::Date::Date(std::string date, ParseOrder order, char seperator)
{
	std::vector<std::string> Date;
	std::istringstream dateStream(date);

	std::string token;
	while (std::getline(dateStream, token, seperator))
	{
		Date.push_back(token);
	}

	switch (order)
	{
		case ParseOrder::MDY:
			m_month = std::stoi(Date[0]);
			m_day = std::stoi(Date[1]);
			m_year = std::stoi(Date[2]);
			break;
		case ParseOrder::DMY:
			m_day = std::stoi(Date[0]);
			m_month = std::stoi(Date[1]);
			m_year = std::stoi(Date[2]);
			break;
		case ParseOrder::YMD:
			m_year = std::stoi(Date[0]);
			m_month = std::stoi(Date[1]);
			m_day = std::stoi(Date[2]);
			break;
		case ParseOrder::YDM:
			m_year = std::stoi(Date[0]);
			m_day = std::stoi(Date[1]);
			m_month = std::stoi(Date[2]);
			break;
		default:
			m_month = std::stoi(Date[0]);
			m_day = std::stoi(Date[1]);
			m_year = std::stoi(Date[2]);
			break;
	}
}

CandleStock::Date::~Date() {}

uint8_t CandleStock::Date::getMonth()
{
	return m_month;
}

uint8_t CandleStock::Date::getDay()
{
	return m_day;
}

void CandleStock::Date::setMonth(uint8_t& month)
{
	m_month = std::max(month % 13, 1); /* 1 - 12 */
}

void CandleStock::Date::setDay(uint8_t& day)
{
	switch (m_month)
	{
		/* Months that have 31 days */
		case January:
		case March:
		case May:
		case July:
		case August:
		case October:
		case December:
			m_day = std::max(day % 32, 1); /* 1 - 31 */
			return;
		/* Months that have 30 days */
		case April:
		case June:
		case September:
		case November:
			m_day = std::max(day % 31, 1); /* 1 - 30 */
			return;
		case February:
			/* TODO: Check for leap year */
			m_day = std::max(day % 29, 1); /* 1 - 28 */
			return;
		default:
			m_day = 0;
			break;
	}
}

System::DateTime CandleStock::Date::toDateTime()
{
	return System::DateTime(m_year, m_month, m_day);
}

CandleStock::Candlestick::Candlestick() :m_name(""), m_date(Date()), m_open(0.0), m_high(0.0), m_low(0.0), m_close(0.0), m_volume(0) {}

CandleStock::Candlestick::Candlestick(std::string name, Date date, double open, double high, double low, double close, uint32_t volume) :m_name(name), m_date(date), m_open(open), m_high(high), m_low(low), m_close(close), m_volume(volume) {}

CandleStock::Candlestick::Candlestick(std::string data, ParseType dataType, std::string header, std::string name)
{
	if (dataType == ParseType::CSV)
	{
		std::vector<std::string> Header, Data;

		std::istringstream headerStream(header);
		std::istringstream dataStream(data);

		std::string token;
		while (std::getline(headerStream, token, ','))
		{
			Header.push_back(token);
		}

		while (std::getline(dataStream, token, ','))
		{
			if (token == "null")
			{
				Candlestick();
				return;
			}

			Data.push_back(token);
		}

		m_date = Date(Data[findValue<std::string>(Header, "Date")], Date::ParseOrder::YMD, '-');
		m_open = std::stod(Data[findValue<std::string>(Header, "Open")]);
		m_high = std::stod(Data[findValue<std::string>(Header, "High")]);
		m_low = std::stod(Data[findValue<std::string>(Header, "Low")]);
		m_close = std::stod(Data[findValue<std::string>(Header, "Close")]);
		m_volume = std::stoul(Data[findValue<std::string>(Header, "Volume")]);
		m_name = name;
	}
}

CandleStock::Candlestick::~Candlestick() {}

std::vector<CandleStock::Candlestick> CandleStock::loadStock(std::string filename, ParseType dataType)
{
	std::vector<CandleStock::Candlestick> Stock;

	if (!std::filesystem::exists(filename)) return Stock;

	std::ifstream file(filename, std::ios::binary);

	if (!file.is_open()) return Stock;

	std::string header;
	std::getline(file, header);

	std::string name = filename.substr(filename.find_last_of('\\') + 1, filename.find_last_of('-') - filename.find_last_of('\\') - 1);

	std::string line;
	while (std::getline(file, line))
	{
		Candlestick candlestick = Candlestick(line, ParseType::CSV, header, name);

		if (candlestick.m_date != Candlestick().m_date)
		{
			Stock.push_back(candlestick);
		}
	}

	return Stock;
}
