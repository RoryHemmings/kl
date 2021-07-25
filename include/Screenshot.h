#pragma once

#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include <string>

class Screenshot
{

public:
	/**
	 * Automatically takes screenshot upon construction
	 */
	Screenshot();

	/**
	 * Compresses, encrypts, and saves screenshot to
	 * outPath
	 */
	void Save(const std::string& outPath) const;

private:
	// some way of storing the data

	/**
	 * Compresses internal data into a jpeg image
	 */
	void compress();

	/**
	 * Encrpyts data using cc public key
	 */
	void encrpyt();

};

#endif