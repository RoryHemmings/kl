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
	Screenshot(const std::string& path);

	/**
	 * Compresses, encrypts, and saves screenshot to
	 * outPath
	 */
	bool Process() const;

private:
	std::string path;

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