// SPDX-FileCopyrightText: Silicon Laboratories Inc. <https://www.silabs.com/>
//
// SPDX-License-Identifier: BSD-3-Clause

/**
 * @file
 *
 * Definitions of events for Door Lock Certified App.
 *
 * @copyright 2019 Silicon Laboratories Inc.
 */
#ifndef APPS_DOORLOCKKEYPAD_EVENTS_H_
#define APPS_DOORLOCKKEYPAD_EVENTS_H_

#include <ev_man.h>

/**
 * Defines events for the application.
 *
 * These events are not referred to anywhere else than in the application. Hence, they can be
 * altered to suit the application flow.
 *
 * The events are located in a separate file to make it possible to include them in other
 * application files. An example could be a peripheral driver that enqueues an event when something
 * specific happens, e.g. door lock state is changed.
 */
typedef enum EVENT_APP_DOOR_LOCK
{
  EVENT_EMPTY = DEFINE_EVENT_APP_NBR,//!< EVENT_EMPTY
  EVENT_APP_START_USER_CODE_EVENT,   //!< EVENT_APP_START_USER_CODE_EVENT
  EVENT_APP_START_KEYPAD_ACTIVE,     //!< EVENT_APP_START_KEYPAD_ACTIVE
  EVENT_APP_FINISH_KEYPAD_ACTIVE,    //!< EVENT_APP_FINISH_KEYPAD_ACTIVE
  EVENT_APP_PERIODIC_BATTERY_CHECK_TRIGGER, //!< EVENT_APP_PERIODIC_BATTERY_CHECK_TRIGGER
  EVENT_APP_BATTERY_REPORT,  //!< EVENT_APP_BATTERY_REPORT
  EVENT_APP_DOORHANDLE_ACTIVATED,   //!< EVENT_APP_DOORHANDLE_ACTIVATED
  EVENT_APP_DOORHANDLE_DEACTIVATED, //!< EVENT_APP_DOORHANDLE_DEACTIVATED
  EVENT_APP_CREDENTIAL_LEARN_START, //!< EVENT_APP_CREDENTIAL_LEARN_START
}
EVENT_APP;

#endif /* APPS_DOORLOCKKEYPAD_EVENTS_H_ */
