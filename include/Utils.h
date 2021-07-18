#pragma once

#ifndef Utils_H
#define Utils_H

#include <string>
#include <ctime>
#include <string>
#include <sstream>
#include <fstream>

namespace Utils
{
	/**
	 * Has to be defined in header because
	 * template functions have to be defined
	 * where they are declared
	 **/
	template <class T>
	std::string ToString(const T& s)
	{
		std::ostringstream out;
		out << s;

		return out.str();
	}

	struct DateTime
	{
		int D, m, y, H, M, S;

		/**
		 * Creates new Datetime object using current
		 * date and time
		 **/
		DateTime();

		/**
		 * Creates new Datetime object with given
		 * date, and with a time of 0:0:0
		 **/
		DateTime(int D, int m, int y);

		/**
		 * Creates new Datetime object with given
		 * date and time
		 **/
		DateTime(int D, int m, int y, int H, int M, int S);

		/**
		 * Returns string representation of date
		 * in format "day.month.year"
		 **/
		std::string GetDateString() const;

		/**
		 * Returns string representation of time
		 * in format "hour:minute:second"
		 **/
		std::string GetTimeString(const std::string& sep = ":") const;

		/**
		 * Returns string representation of date
		 * and time in format "day.month.year hour:minute:second"
		 **/
		std::string GetDateTimeString(const std::string& sep = ":") const;

		/**
		 * Returns string representation of current time
		 * and date in format "day.month.year.hour.minute.second"
		 */
		std::string GetTimestamp() const;

		/**
		 * Returns DateTime object that represents now
		 **/
		static DateTime Now();

	};
}

#endif
