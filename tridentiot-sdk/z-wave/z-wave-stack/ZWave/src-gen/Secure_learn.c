// SPDX-FileCopyrightText: Silicon Laboratories Inc. <https://www.silabs.com/>
//
// SPDX-License-Identifier: BSD-3-Clause

/**
 * @file Secure_learn.c
 * @copyright 2022 Silicon Laboratories Inc.
 */
#include <stdlib.h>
#include <string.h>
#include "sc_types.h"
#include "Secure_learn.h"
#include "Secure_learnRequired.h"

/*! \file Implementation of the state machine 'secure_learn'
*/

// prototypes of all internal functions

static void secure_learn_entryaction(Secure_learn* handle);
static void secure_learn_exitaction(Secure_learn* handle);
static void secure_learn_react_main_region_LearnMode_r1_Start(Secure_learn* handle);
static void secure_learn_react_main_region_LearnMode_r1_Scheme_report(Secure_learn* handle);
static void secure_learn_react_main_region_LearnMode_r1_Fail(Secure_learn* handle);
static void secure_learn_react_main_region_LearnMode_r1_NewKey(Secure_learn* handle);
static void secure_learn_react_main_region_LearnMode_r1_Inherited(Secure_learn* handle);
static void secure_learn_react_main_region_LearnMode_r1_InsecureNet(Secure_learn* handle);
static void secure_learn_react_main_region_LearnMode_r1_end(Secure_learn* handle);
static void secure_learn_react_main_region_LearnMode_r1_EarlyStart(Secure_learn* handle);
static void secure_learn_react_main_region_Idle(Secure_learn* handle);
static void secure_learn_react_main_region_InclusionMode_r1_SchemeRequest(Secure_learn* handle);
static void secure_learn_react_main_region_InclusionMode_r1_SendKey(Secure_learn* handle);
static void secure_learn_react_main_region_InclusionMode_r1_SendInterit(Secure_learn* handle);
static void secure_learn_react_main_region_InclusionMode_r1_Fail(Secure_learn* handle);
static void secure_learn_react_main_region_InclusionMode_r1_end(Secure_learn* handle);
static void secure_learn_react_main_region_InclusionMode_r1_waitVerify(Secure_learn* handle);
static void secure_learn_react_main_region_Complete(Secure_learn* handle);
static void secure_learn_react_main_region_SendReport(Secure_learn* handle);
static void clearInEvents(Secure_learn* handle);
static void clearOutEvents(Secure_learn* handle);


void secure_learn_init(Secure_learn* handle)
{
	int i;

	for (i = 0; i < SECURE_LEARN_MAX_ORTHOGONAL_STATES; ++i)
		handle->stateConfVector[i] = Secure_learn_last_state;
	
	
	handle->stateConfVectorPosition = 0;

clearInEvents(handle);
clearOutEvents(handle);

	// TODO: initialize all events ...

	{
		/* Default init sequence for statechart secure_learn */
		handle->iface.supported_schemes = 0x1;
		handle->iface.txOptions = 0;
		handle->iface.isController = bool_false;
		handle->iface.node = 0;
		handle->iface.scheme = 0;
		handle->iface.net_scheme = 0;
	}

}

void secure_learn_enter(Secure_learn* handle)
{
	{
		/* Default enter sequence for statechart secure_learn */
		secure_learn_entryaction(handle);
		{
			/* Default enter sequence for region main region */
			{
				/* Default react sequence for initial entry  */
				{
					/* Default enter sequence for state Idle */
					handle->stateConfVector[0] = secure_learn_main_region_Idle;
					handle->stateConfVectorPosition = 0;
				}
			}
		}
	}
}

void secure_learn_exit(Secure_learn* handle)
{
	{
		/* Default exit sequence for statechart secure_learn */
		{
			/* Default exit sequence for region main region */
			/* Handle exit of all possible states (of main region) at position 0... */
			switch(handle->stateConfVector[ 0 ]) {
				case secure_learn_main_region_LearnMode_r1_Start : {
					{
						/* Default exit sequence for state Start */
						handle->stateConfVector[0] = Secure_learn_last_state;
						handle->stateConfVectorPosition = 0;
						{
							/* Exit action for state 'Start'. */
							secure_learn_unsetTimer( (sc_eventid) &(handle->timeEvents.secure_learn_main_region_LearnMode_r1_Start_time_event_0_raised) );		
						}
					}
					break;
				}
				case secure_learn_main_region_LearnMode_r1_Scheme_report : {
					{
						/* Default exit sequence for state Scheme_report */
						handle->stateConfVector[0] = Secure_learn_last_state;
						handle->stateConfVectorPosition = 0;
						{
							/* Exit action for state 'Scheme_report'. */
							secure_learn_unsetTimer( (sc_eventid) &(handle->timeEvents.secure_learn_main_region_LearnMode_r1_Scheme_report_time_event_0_raised) );		
						}
					}
					break;
				}
				case secure_learn_main_region_LearnMode_r1_Fail : {
					{
						/* Default exit sequence for state Fail */
						handle->stateConfVector[0] = Secure_learn_last_state;
						handle->stateConfVectorPosition = 0;
					}
					break;
				}
				case secure_learn_main_region_LearnMode_r1_NewKey : {
					{
						/* Default exit sequence for state NewKey */
						handle->stateConfVector[0] = Secure_learn_last_state;
						handle->stateConfVectorPosition = 0;
						{
							/* Exit action for state 'NewKey'. */
							secure_learn_unsetTimer( (sc_eventid) &(handle->timeEvents.secure_learn_main_region_LearnMode_r1_NewKey_time_event_0_raised) );		
						}
					}
					break;
				}
				case secure_learn_main_region_LearnMode_r1_Inherited : {
					{
						/* Default exit sequence for state Inherited */
						handle->stateConfVector[0] = Secure_learn_last_state;
						handle->stateConfVectorPosition = 0;
					}
					break;
				}
				case secure_learn_main_region_LearnMode_r1_InsecureNet : {
					{
						/* Default exit sequence for state InsecureNet */
						handle->stateConfVector[0] = Secure_learn_last_state;
						handle->stateConfVectorPosition = 0;
					}
					break;
				}
				case secure_learn_main_region_LearnMode_r1_end : {
					{
						/* Default exit sequence for state end */
						handle->stateConfVector[0] = Secure_learn_last_state;
						handle->stateConfVectorPosition = 0;
					}
					break;
				}
				case secure_learn_main_region_LearnMode_r1_EarlyStart : {
					{
						/* Default exit sequence for state EarlyStart */
						handle->stateConfVector[0] = Secure_learn_last_state;
						handle->stateConfVectorPosition = 0;
					}
					break;
				}
				case secure_learn_main_region_Idle : {
					{
						/* Default exit sequence for state Idle */
						handle->stateConfVector[0] = Secure_learn_last_state;
						handle->stateConfVectorPosition = 0;
					}
					break;
				}
				case secure_learn_main_region_InclusionMode_r1_SchemeRequest : {
					{
						/* Default exit sequence for state SchemeRequest */
						handle->stateConfVector[0] = Secure_learn_last_state;
						handle->stateConfVectorPosition = 0;
						{
							/* Exit action for state 'SchemeRequest'. */
							secure_learn_unsetTimer( (sc_eventid) &(handle->timeEvents.secure_learn_main_region_InclusionMode_r1_SchemeRequest_time_event_0_raised) );		
						}
					}
					break;
				}
				case secure_learn_main_region_InclusionMode_r1_SendKey : {
					{
						/* Default exit sequence for state SendKey */
						handle->stateConfVector[0] = Secure_learn_last_state;
						handle->stateConfVectorPosition = 0;
						{
							/* Exit action for state 'SendKey'. */
							secure_learn_unsetTimer( (sc_eventid) &(handle->timeEvents.secure_learn_main_region_InclusionMode_r1_SendKey_time_event_0_raised) );		
						}
					}
					break;
				}
				case secure_learn_main_region_InclusionMode_r1_SendInterit : {
					{
						/* Default exit sequence for state SendInterit */
						handle->stateConfVector[0] = Secure_learn_last_state;
						handle->stateConfVectorPosition = 0;
						{
							/* Exit action for state 'SendInterit'. */
							secure_learn_unsetTimer( (sc_eventid) &(handle->timeEvents.secure_learn_main_region_InclusionMode_r1_SendInterit_time_event_0_raised) );		
						}
					}
					break;
				}
				case secure_learn_main_region_InclusionMode_r1_Fail : {
					{
						/* Default exit sequence for state Fail */
						handle->stateConfVector[0] = Secure_learn_last_state;
						handle->stateConfVectorPosition = 0;
					}
					break;
				}
				case secure_learn_main_region_InclusionMode_r1_end : {
					{
						/* Default exit sequence for state end */
						handle->stateConfVector[0] = Secure_learn_last_state;
						handle->stateConfVectorPosition = 0;
					}
					break;
				}
				case secure_learn_main_region_InclusionMode_r1_waitVerify : {
					{
						/* Default exit sequence for state waitVerify */
						handle->stateConfVector[0] = Secure_learn_last_state;
						handle->stateConfVectorPosition = 0;
						{
							/* Exit action for state 'waitVerify'. */
							secure_learn_unsetTimer( (sc_eventid) &(handle->timeEvents.secure_learn_main_region_InclusionMode_r1_waitVerify_time_event_0_raised) );		
						}
					}
					break;
				}
				case secure_learn_main_region_Complete : {
					{
						/* Default exit sequence for state Complete */
						handle->stateConfVector[0] = Secure_learn_last_state;
						handle->stateConfVectorPosition = 0;
					}
					break;
				}
				case secure_learn_main_region_SendReport : {
					{
						/* Default exit sequence for state SendReport */
						handle->stateConfVector[0] = Secure_learn_last_state;
						handle->stateConfVectorPosition = 0;
					}
					break;
				}
				default: break;
			}
		}
		secure_learn_exitaction(handle);
	}
}

static void clearInEvents(Secure_learn* handle) {
	handle->iface.learnRequest_raised = bool_false;
	handle->iface.assignIdDone_raised = bool_false;
	handle->iface.inclusionRequest_raised = bool_false;
	handle->iface.commandsSupportedRequest_raised = bool_false;
	handle->iface.tx_done_raised = bool_false;
	handle->iface.tx_fail_raised = bool_false;
	handle->iface.scheme_get_raised = bool_false;
	handle->iface.scheme_inherit_raised = bool_false;
	handle->iface.key_set_raised = bool_false;
	handle->iface.scheme_report_raised = bool_false;
	handle->iface.key_verify_raised = bool_false;
	handle->timeEvents.secure_learn_main_region_LearnMode_r1_Start_time_event_0_raised = bool_false;
	handle->timeEvents.secure_learn_main_region_LearnMode_r1_Scheme_report_time_event_0_raised = bool_false;
	handle->timeEvents.secure_learn_main_region_LearnMode_r1_NewKey_time_event_0_raised = bool_false;
	handle->timeEvents.secure_learn_main_region_InclusionMode_r1_SchemeRequest_time_event_0_raised = bool_false;
	handle->timeEvents.secure_learn_main_region_InclusionMode_r1_SendKey_time_event_0_raised = bool_false;
	handle->timeEvents.secure_learn_main_region_InclusionMode_r1_SendInterit_time_event_0_raised = bool_false;
	handle->timeEvents.secure_learn_main_region_InclusionMode_r1_waitVerify_time_event_0_raised = bool_false;
}


static void clearOutEvents(__attribute__((unused)) Secure_learn* handle) {
  /* FIXME: Use a lower warning level for src-gen instead of UNUSED */
}

void secure_learn_runCycle(Secure_learn* handle) {

	clearOutEvents(handle);

	for (handle->stateConfVectorPosition = 0;
		handle->stateConfVectorPosition < SECURE_LEARN_MAX_ORTHOGONAL_STATES;
		handle->stateConfVectorPosition++) {

		switch (handle->stateConfVector[handle->stateConfVectorPosition]) {
		case secure_learn_main_region_LearnMode_r1_Start : {
			secure_learn_react_main_region_LearnMode_r1_Start(handle);
			break;
		}
		case secure_learn_main_region_LearnMode_r1_Scheme_report : {
			secure_learn_react_main_region_LearnMode_r1_Scheme_report(handle);
			break;
		}
		case secure_learn_main_region_LearnMode_r1_Fail : {
			secure_learn_react_main_region_LearnMode_r1_Fail(handle);
			break;
		}
		case secure_learn_main_region_LearnMode_r1_NewKey : {
			secure_learn_react_main_region_LearnMode_r1_NewKey(handle);
			break;
		}
		case secure_learn_main_region_LearnMode_r1_Inherited : {
			secure_learn_react_main_region_LearnMode_r1_Inherited(handle);
			break;
		}
		case secure_learn_main_region_LearnMode_r1_InsecureNet : {
			secure_learn_react_main_region_LearnMode_r1_InsecureNet(handle);
			break;
		}
		case secure_learn_main_region_LearnMode_r1_end : {
			secure_learn_react_main_region_LearnMode_r1_end(handle);
			break;
		}
		case secure_learn_main_region_LearnMode_r1_EarlyStart : {
			secure_learn_react_main_region_LearnMode_r1_EarlyStart(handle);
			break;
		}
		case secure_learn_main_region_Idle : {
			secure_learn_react_main_region_Idle(handle);
			break;
		}
		case secure_learn_main_region_InclusionMode_r1_SchemeRequest : {
			secure_learn_react_main_region_InclusionMode_r1_SchemeRequest(handle);
			break;
		}
		case secure_learn_main_region_InclusionMode_r1_SendKey : {
			secure_learn_react_main_region_InclusionMode_r1_SendKey(handle);
			break;
		}
		case secure_learn_main_region_InclusionMode_r1_SendInterit : {
			secure_learn_react_main_region_InclusionMode_r1_SendInterit(handle);
			break;
		}
		case secure_learn_main_region_InclusionMode_r1_Fail : {
			secure_learn_react_main_region_InclusionMode_r1_Fail(handle);
			break;
		}
		case secure_learn_main_region_InclusionMode_r1_end : {
			secure_learn_react_main_region_InclusionMode_r1_end(handle);
			break;
		}
		case secure_learn_main_region_InclusionMode_r1_waitVerify : {
			secure_learn_react_main_region_InclusionMode_r1_waitVerify(handle);
			break;
		}
		case secure_learn_main_region_Complete : {
			secure_learn_react_main_region_Complete(handle);
			break;
		}
		case secure_learn_main_region_SendReport : {
			secure_learn_react_main_region_SendReport(handle);
			break;
		}
		default:
			break;
		}
	}
	
	clearInEvents(handle);
}

void secure_learn_raiseTimeEvent(Secure_learn* handle, sc_eventid evid) {
	if ( ((intptr_t)evid) >= ((intptr_t)&(handle->timeEvents))
		&&  ((intptr_t)evid) < (intptr_t)(((intptr_t)&(handle->timeEvents)) + sizeof(Secure_learnTimeEvents))) {
		*(sc_boolean*)evid = bool_true;
	}		
}

sc_boolean secure_learn_isActive(Secure_learn* handle, Secure_learnStates state) {
	_Static_assert(secure_learn_main_region_LearnMode == 0, "secure_learn_main_region_LearnMode must zero");

	switch (state) {
		case secure_learn_main_region_LearnMode : 
			return (sc_boolean) (handle->stateConfVector[0] <= secure_learn_main_region_LearnMode_r1_EarlyStart);
		case secure_learn_main_region_LearnMode_r1_Start : 
			return (sc_boolean) (handle->stateConfVector[0] == secure_learn_main_region_LearnMode_r1_Start
			);
		case secure_learn_main_region_LearnMode_r1_Scheme_report : 
			return (sc_boolean) (handle->stateConfVector[0] == secure_learn_main_region_LearnMode_r1_Scheme_report
			);
		case secure_learn_main_region_LearnMode_r1_Fail : 
			return (sc_boolean) (handle->stateConfVector[0] == secure_learn_main_region_LearnMode_r1_Fail
			);
		case secure_learn_main_region_LearnMode_r1_NewKey : 
			return (sc_boolean) (handle->stateConfVector[0] == secure_learn_main_region_LearnMode_r1_NewKey
			);
		case secure_learn_main_region_LearnMode_r1_Inherited : 
			return (sc_boolean) (handle->stateConfVector[0] == secure_learn_main_region_LearnMode_r1_Inherited
			);
		case secure_learn_main_region_LearnMode_r1_InsecureNet : 
			return (sc_boolean) (handle->stateConfVector[0] == secure_learn_main_region_LearnMode_r1_InsecureNet
			);
		case secure_learn_main_region_LearnMode_r1_end : 
			return (sc_boolean) (handle->stateConfVector[0] == secure_learn_main_region_LearnMode_r1_end
			);
		case secure_learn_main_region_LearnMode_r1_EarlyStart : 
			return (sc_boolean) (handle->stateConfVector[0] == secure_learn_main_region_LearnMode_r1_EarlyStart
			);
		case secure_learn_main_region_Idle : 
			return (sc_boolean) (handle->stateConfVector[0] == secure_learn_main_region_Idle
			);
		case secure_learn_main_region_InclusionMode : 
			return (sc_boolean) (handle->stateConfVector[0] >= secure_learn_main_region_InclusionMode
				&& handle->stateConfVector[0] <= secure_learn_main_region_InclusionMode_r1_waitVerify);
		case secure_learn_main_region_InclusionMode_r1_SchemeRequest : 
			return (sc_boolean) (handle->stateConfVector[0] == secure_learn_main_region_InclusionMode_r1_SchemeRequest
			);
		case secure_learn_main_region_InclusionMode_r1_SendKey : 
			return (sc_boolean) (handle->stateConfVector[0] == secure_learn_main_region_InclusionMode_r1_SendKey
			);
		case secure_learn_main_region_InclusionMode_r1_SendInterit : 
			return (sc_boolean) (handle->stateConfVector[0] == secure_learn_main_region_InclusionMode_r1_SendInterit
			);
		case secure_learn_main_region_InclusionMode_r1_Fail : 
			return (sc_boolean) (handle->stateConfVector[0] == secure_learn_main_region_InclusionMode_r1_Fail
			);
		case secure_learn_main_region_InclusionMode_r1_end : 
			return (sc_boolean) (handle->stateConfVector[0] == secure_learn_main_region_InclusionMode_r1_end
			);
		case secure_learn_main_region_InclusionMode_r1_waitVerify : 
			return (sc_boolean) (handle->stateConfVector[0] == secure_learn_main_region_InclusionMode_r1_waitVerify
			);
		case secure_learn_main_region_Complete : 
			return (sc_boolean) (handle->stateConfVector[0] == secure_learn_main_region_Complete
			);
		case secure_learn_main_region_SendReport : 
			return (sc_boolean) (handle->stateConfVector[0] == secure_learn_main_region_SendReport
			);
		default: return bool_false;
	}
}

void secure_learnIface_raise_learnRequest(Secure_learn* handle) {
	handle->iface.learnRequest_raised = bool_true;
}
void secure_learnIface_raise_assignIdDone(Secure_learn* handle) {
	handle->iface.assignIdDone_raised = bool_true;
}
void secure_learnIface_raise_inclusionRequest(Secure_learn* handle, sc_integer value) {
	handle->iface.inclusionRequest_value = value;
	handle->iface.inclusionRequest_raised = bool_true;
}
void secure_learnIface_raise_commandsSupportedRequest(Secure_learn* handle, sc_string value) {
	handle->iface.commandsSupportedRequest_value = value;
	handle->iface.commandsSupportedRequest_raised = bool_true;
}
void secure_learnIface_raise_tx_done(Secure_learn* handle) {
	handle->iface.tx_done_raised = bool_true;
}
void secure_learnIface_raise_tx_fail(Secure_learn* handle) {
	handle->iface.tx_fail_raised = bool_true;
}
void secure_learnIface_raise_scheme_get(Secure_learn* handle, sc_integer value) {
	handle->iface.scheme_get_value = value;
	handle->iface.scheme_get_raised = bool_true;
}
void secure_learnIface_raise_scheme_inherit(Secure_learn* handle, sc_integer value) {
	handle->iface.scheme_inherit_value = value;
	handle->iface.scheme_inherit_raised = bool_true;
}
void secure_learnIface_raise_key_set(Secure_learn* handle, sc_integer value) {
	handle->iface.key_set_value = value;
	handle->iface.key_set_raised = bool_true;
}
void secure_learnIface_raise_scheme_report(Secure_learn* handle, sc_integer value) {
	handle->iface.scheme_report_value = value;
	handle->iface.scheme_report_raised = bool_true;
}
void secure_learnIface_raise_key_verify(Secure_learn* handle, sc_integer value) {
	handle->iface.key_verify_value = value;
	handle->iface.key_verify_raised = bool_true;
}


sc_integer secure_learnIface_get_supported_schemes(Secure_learn* handle) {
	return handle->iface.supported_schemes;
}
sc_integer secure_learnIface_get_txOptions(Secure_learn* handle) {
	return handle->iface.txOptions;
}
void secure_learnIface_set_txOptions(Secure_learn* handle, sc_integer value) {
	handle->iface.txOptions = value;
}
sc_boolean secure_learnIface_get_isController(Secure_learn* handle) {
	return handle->iface.isController;
}
void secure_learnIface_set_isController(Secure_learn* handle, sc_boolean value) {
	handle->iface.isController = value;
}
sc_integer secure_learnIface_get_node(Secure_learn* handle) {
	return handle->iface.node;
}
void secure_learnIface_set_node(Secure_learn* handle, sc_integer value) {
	handle->iface.node = value;
}
sc_integer secure_learnIface_get_scheme(Secure_learn* handle) {
	return handle->iface.scheme;
}
void secure_learnIface_set_scheme(Secure_learn* handle, sc_integer value) {
	handle->iface.scheme = value;
}
sc_integer secure_learnIface_get_net_scheme(Secure_learn* handle) {
	return handle->iface.net_scheme;
}
void secure_learnIface_set_net_scheme(Secure_learn* handle, sc_integer value) {
	handle->iface.net_scheme = value;
}





// implementations of all internal functions

/* Entry action for statechart 'secure_learn'. */
static void secure_learn_entryaction(__attribute__((unused)) Secure_learn* handle) {
}

/* Exit action for state 'secure_learn'. */
static void secure_learn_exitaction(__attribute__((unused)) Secure_learn* handle) {
}

/* The reactions of state Start. */
static void secure_learn_react_main_region_LearnMode_r1_Start(Secure_learn* handle) {
	{
		/* The reactions of state Start. */
		if (handle->iface.scheme_get_raised) {
			{
				/* Default exit sequence for state Start */
				handle->stateConfVector[0] = Secure_learn_last_state;
				handle->stateConfVectorPosition = 0;
				{
					/* Exit action for state 'Start'. */
					secure_learn_unsetTimer( (sc_eventid) &(handle->timeEvents.secure_learn_main_region_LearnMode_r1_Start_time_event_0_raised) );
				}
			}
			handle->iface.node = handle->iface.scheme_get_value;
			{
				/* Default enter sequence for state Scheme_report */
				{
					/* Entry action for state 'Scheme_report'. */
					secure_learn_setTimer( (sc_eventid) &(handle->timeEvents.secure_learn_main_region_LearnMode_r1_Scheme_report_time_event_0_raised) , 10 * 1000, bool_false);
					secure_learnIfaceL_send_scheme_report(handle->iface.node, handle->iface.txOptions, bool_false);
					secure_learnIfaceL_set_inclusion_key();
					secure_learnIfaceI_register_scheme(handle->iface.node, handle->iface.scheme);
				}
				handle->stateConfVector[0] = secure_learn_main_region_LearnMode_r1_Scheme_report;
				handle->stateConfVectorPosition = 0;
			}
		}
		else
		{
			if (handle->timeEvents.secure_learn_main_region_LearnMode_r1_Start_time_event_0_raised) { 
				{
					/* Default exit sequence for state Start */
					handle->stateConfVector[0] = Secure_learn_last_state;
					handle->stateConfVectorPosition = 0;
					{
						/* Exit action for state 'Start'. */
						secure_learn_unsetTimer( (sc_eventid) &(handle->timeEvents.secure_learn_main_region_LearnMode_r1_Start_time_event_0_raised) );		
					}
				}
				{
					/* The reactions of state null. */
					if (handle->iface.isController)
					{
            /* Default enter sequence for state InsecureNet */
                    	{
              /* Entry action for state 'InsecureNet'. */
                        	secure_learnIfaceL_new_keys();
                        	handle->iface.scheme = 1;
                        }
                    	handle->stateConfVector[0] = secure_learn_main_region_LearnMode_r1_InsecureNet;
                    	handle->stateConfVectorPosition = 0;
					}
					else
					{
						if (bool_true)
						{
              /* Timeout waiting for Scheme Get - exit S0 inclusion machine */
              				secure_learnIface_state_start_timeout();

              /* Default enter sequence for state end */
              				handle->stateConfVector[0] = secure_learn_main_region_LearnMode_r1_end;
              				handle->stateConfVectorPosition = 0;
						}
					}
				}
			} 
		}
	}
}

/* The reactions of state Scheme_report. */
static void secure_learn_react_main_region_LearnMode_r1_Scheme_report(Secure_learn* handle) {
	{
		/* The reactions of state Scheme_report. */
		if (handle->iface.key_set_value == handle->iface.node && handle->iface.key_set_raised) { 
			{
				/* Default exit sequence for state Scheme_report */
				handle->stateConfVector[0] = Secure_learn_last_state;
				handle->stateConfVectorPosition = 0;
				{
					/* Exit action for state 'Scheme_report'. */
					secure_learn_unsetTimer( (sc_eventid) &(handle->timeEvents.secure_learn_main_region_LearnMode_r1_Scheme_report_time_event_0_raised) );		
				}
			}
			{
				/* Default enter sequence for state NewKey */
				{
					/* Entry action for state 'NewKey'. */
					secure_learn_setTimer( (sc_eventid) &(handle->timeEvents.secure_learn_main_region_LearnMode_r1_NewKey_time_event_0_raised) , 10 * 1000, bool_false);
					secure_learnIfaceL_send_key_verify(handle->iface.node, handle->iface.txOptions);
					handle->iface.scheme = (handle->iface.scheme & handle->iface.supported_schemes);
				}
				handle->stateConfVector[0] = secure_learn_main_region_LearnMode_r1_NewKey;
				handle->stateConfVectorPosition = 0;
			}
		}  else {
			if (handle->iface.tx_fail_raised || handle->timeEvents.secure_learn_main_region_LearnMode_r1_Scheme_report_time_event_0_raised) { 
				{
					/* Default exit sequence for state Scheme_report */
					handle->stateConfVector[0] = Secure_learn_last_state;
					handle->stateConfVectorPosition = 0;
					{
						/* Exit action for state 'Scheme_report'. */
						secure_learn_unsetTimer( (sc_eventid) &(handle->timeEvents.secure_learn_main_region_LearnMode_r1_Scheme_report_time_event_0_raised) );		
					}
				}
				{
					/* Default enter sequence for state Fail */
					{
						/* Entry action for state 'Fail'. */
						handle->iface.scheme = 0;
					}
					handle->stateConfVector[0] = secure_learn_main_region_LearnMode_r1_Fail;
					handle->stateConfVectorPosition = 0;
				}
			}  else {
				if ((handle->iface.scheme & handle->iface.supported_schemes) == 0 && handle->iface.tx_done_raised) { 
					{
						/* Default exit sequence for state Scheme_report */
						handle->stateConfVector[0] = Secure_learn_last_state;
						handle->stateConfVectorPosition = 0;
						{
							/* Exit action for state 'Scheme_report'. */
							secure_learn_unsetTimer( (sc_eventid) &(handle->timeEvents.secure_learn_main_region_LearnMode_r1_Scheme_report_time_event_0_raised) );		
						}
					}
					{
						/* Default enter sequence for state Fail */
						{
							/* Entry action for state 'Fail'. */
							handle->iface.scheme = 0;
						}
						handle->stateConfVector[0] = secure_learn_main_region_LearnMode_r1_Fail;
						handle->stateConfVectorPosition = 0;
					}
				} 
			}
		}
	}
}

/* The reactions of state Fail. */
static void secure_learn_react_main_region_LearnMode_r1_Fail(Secure_learn* handle) {
	{
		/* The reactions of state Fail. */
		if (bool_true) { 
			{
				/* Default exit sequence for state Fail */
				handle->stateConfVector[0] = Secure_learn_last_state;
				handle->stateConfVectorPosition = 0;
			}
			{
				/* Default enter sequence for state end */
				handle->stateConfVector[0] = secure_learn_main_region_LearnMode_r1_end;
				handle->stateConfVectorPosition = 0;
			}
		} 
	}
}

/* The reactions of state NewKey. */
static void secure_learn_react_main_region_LearnMode_r1_NewKey(Secure_learn* handle) {
	{
		/* The reactions of state NewKey. */
		if (! handle->iface.isController && handle->iface.tx_done_raised) { 
			{
				/* Default exit sequence for state NewKey */
				handle->stateConfVector[0] = Secure_learn_last_state;
				handle->stateConfVectorPosition = 0;
				{
					/* Exit action for state 'NewKey'. */
					secure_learn_unsetTimer( (sc_eventid) &(handle->timeEvents.secure_learn_main_region_LearnMode_r1_NewKey_time_event_0_raised) );		
				}
			}
			{
				/* Default enter sequence for state end */
				handle->stateConfVector[0] = secure_learn_main_region_LearnMode_r1_end;
				handle->stateConfVectorPosition = 0;
			}
		}  else {
			if (handle->iface.tx_fail_raised || handle->timeEvents.secure_learn_main_region_LearnMode_r1_NewKey_time_event_0_raised) { 
				{
					/* Default exit sequence for state NewKey */
					handle->stateConfVector[0] = Secure_learn_last_state;
					handle->stateConfVectorPosition = 0;
					{
						/* Exit action for state 'NewKey'. */
						secure_learn_unsetTimer( (sc_eventid) &(handle->timeEvents.secure_learn_main_region_LearnMode_r1_NewKey_time_event_0_raised) );		
					}
				}
				{
					/* Default enter sequence for state Fail */
					{
						/* Entry action for state 'Fail'. */
						handle->iface.scheme = 0;
					}
					handle->stateConfVector[0] = secure_learn_main_region_LearnMode_r1_Fail;
					handle->stateConfVectorPosition = 0;
				}
			}  else {
				if (handle->iface.isController && (handle->iface.scheme_inherit_value == handle->iface.node) && handle->iface.scheme_inherit_raised) { 
					{
						/* Default exit sequence for state NewKey */
						handle->stateConfVector[0] = Secure_learn_last_state;
						handle->stateConfVectorPosition = 0;
						{
							/* Exit action for state 'NewKey'. */
							secure_learn_unsetTimer( (sc_eventid) &(handle->timeEvents.secure_learn_main_region_LearnMode_r1_NewKey_time_event_0_raised) );		
						}
					}
					{
						/* Default enter sequence for state Inherited */
						{
							/* Entry action for state 'Inherited'. */
							secure_learnIfaceL_send_scheme_report(handle->iface.node, handle->iface.txOptions, bool_true);
						}
						handle->stateConfVector[0] = secure_learn_main_region_LearnMode_r1_Inherited;
						handle->stateConfVectorPosition = 0;
					}
				} 
			}
		}
	}
}

/* The reactions of state Inherited. */
static void secure_learn_react_main_region_LearnMode_r1_Inherited(Secure_learn* handle) {
	{
		/* The reactions of state Inherited. */
		if (handle->iface.tx_done_raised) { 
			{
				/* Default exit sequence for state Inherited */
				handle->stateConfVector[0] = Secure_learn_last_state;
				handle->stateConfVectorPosition = 0;
			}
			{
				/* Default enter sequence for state end */
				handle->stateConfVector[0] = secure_learn_main_region_LearnMode_r1_end;
				handle->stateConfVectorPosition = 0;
			}
		}  else {
			if (handle->iface.tx_fail_raised) { 
				{
					/* Default exit sequence for state Inherited */
					handle->stateConfVector[0] = Secure_learn_last_state;
					handle->stateConfVectorPosition = 0;
				}
				{
					/* Default enter sequence for state Fail */
					{
						/* Entry action for state 'Fail'. */
						handle->iface.scheme = 0;
					}
					handle->stateConfVector[0] = secure_learn_main_region_LearnMode_r1_Fail;
					handle->stateConfVectorPosition = 0;
				}
			} 
		}
	}
}

/* The reactions of state InsecureNet. */
static void secure_learn_react_main_region_LearnMode_r1_InsecureNet(Secure_learn* handle) {
	{
		/* The reactions of state InsecureNet. */
		if (bool_true) { 
			{
				/* Default exit sequence for state InsecureNet */
				handle->stateConfVector[0] = Secure_learn_last_state;
				handle->stateConfVectorPosition = 0;
			}
			{
				/* Default enter sequence for state end */
				handle->stateConfVector[0] = secure_learn_main_region_LearnMode_r1_end;
				handle->stateConfVectorPosition = 0;
			}
		} 
	}
}

/* The reactions of state end. */
static void secure_learn_react_main_region_LearnMode_r1_end(Secure_learn* handle) {
	{
		/* The reactions of state end. */
		if (bool_true) { 
			{
				/* Default exit sequence for state LearnMode */
				{
					/* Default exit sequence for region r1 */
					/* Handle exit of all possible states (of r1) at position 0... */
					switch(handle->stateConfVector[ 0 ]) {
						case secure_learn_main_region_LearnMode_r1_Start : {
							{
								/* Default exit sequence for state Start */
								handle->stateConfVector[0] = Secure_learn_last_state;
								handle->stateConfVectorPosition = 0;
								{
									/* Exit action for state 'Start'. */
									secure_learn_unsetTimer( (sc_eventid) &(handle->timeEvents.secure_learn_main_region_LearnMode_r1_Start_time_event_0_raised) );		
								}
							}
							break;
						}
						case secure_learn_main_region_LearnMode_r1_Scheme_report : {
							{
								/* Default exit sequence for state Scheme_report */
								handle->stateConfVector[0] = Secure_learn_last_state;
								handle->stateConfVectorPosition = 0;
								{
									/* Exit action for state 'Scheme_report'. */
									secure_learn_unsetTimer( (sc_eventid) &(handle->timeEvents.secure_learn_main_region_LearnMode_r1_Scheme_report_time_event_0_raised) );		
								}
							}
							break;
						}
						case secure_learn_main_region_LearnMode_r1_Fail : {
							{
								/* Default exit sequence for state Fail */
								handle->stateConfVector[0] = Secure_learn_last_state;
								handle->stateConfVectorPosition = 0;
							}
							break;
						}
						case secure_learn_main_region_LearnMode_r1_NewKey : {
							{
								/* Default exit sequence for state NewKey */
								handle->stateConfVector[0] = Secure_learn_last_state;
								handle->stateConfVectorPosition = 0;
								{
									/* Exit action for state 'NewKey'. */
									secure_learn_unsetTimer( (sc_eventid) &(handle->timeEvents.secure_learn_main_region_LearnMode_r1_NewKey_time_event_0_raised) );		
								}
							}
							break;
						}
						case secure_learn_main_region_LearnMode_r1_Inherited : {
							{
								/* Default exit sequence for state Inherited */
								handle->stateConfVector[0] = Secure_learn_last_state;
								handle->stateConfVectorPosition = 0;
							}
							break;
						}
						case secure_learn_main_region_LearnMode_r1_InsecureNet : {
							{
								/* Default exit sequence for state InsecureNet */
								handle->stateConfVector[0] = Secure_learn_last_state;
								handle->stateConfVectorPosition = 0;
							}
							break;
						}
						case secure_learn_main_region_LearnMode_r1_end : {
							{
								/* Default exit sequence for state end */
								handle->stateConfVector[0] = Secure_learn_last_state;
								handle->stateConfVectorPosition = 0;
							}
							break;
						}
						case secure_learn_main_region_LearnMode_r1_EarlyStart : {
							{
								/* Default exit sequence for state EarlyStart */
								handle->stateConfVector[0] = Secure_learn_last_state;
								handle->stateConfVectorPosition = 0;
							}
							break;
						}
						default: break;
					}
				}
			}
			handle->iface.net_scheme = handle->iface.scheme;
			secure_learnIfaceL_save_state();
			{
				/* Default enter sequence for state Complete */
				{
					/* Entry action for state 'Complete'. */
					secure_learnIface_complete(handle->iface.scheme);
				}
				handle->stateConfVector[0] = secure_learn_main_region_Complete;
				handle->stateConfVectorPosition = 0;
			}
		} 
	}
}

/* The reactions of state EarlyStart. */
static void secure_learn_react_main_region_LearnMode_r1_EarlyStart(Secure_learn* handle) {
	{
		/* The reactions of state EarlyStart. */
		if (handle->iface.scheme_get_raised) { 
			{
				/* Default exit sequence for state EarlyStart */
				handle->stateConfVector[0] = Secure_learn_last_state;
				handle->stateConfVectorPosition = 0;
			}
			handle->iface.node = handle->iface.scheme_get_value;
			{
				/* Default enter sequence for state Scheme_report */
				{
					/* Entry action for state 'Scheme_report'. */
					secure_learn_setTimer( (sc_eventid) &(handle->timeEvents.secure_learn_main_region_LearnMode_r1_Scheme_report_time_event_0_raised) , 10 * 1000, bool_false);
					secure_learnIfaceL_send_scheme_report(handle->iface.node, handle->iface.txOptions, bool_false);
					secure_learnIfaceL_set_inclusion_key();
					secure_learnIfaceI_register_scheme(handle->iface.node, handle->iface.scheme);
				}
				handle->stateConfVector[0] = secure_learn_main_region_LearnMode_r1_Scheme_report;
				handle->stateConfVectorPosition = 0;
			}
		}  else {
			if (handle->iface.learnRequest_raised) { 
				{
					/* Default exit sequence for state EarlyStart */
					handle->stateConfVector[0] = Secure_learn_last_state;
					handle->stateConfVectorPosition = 0;
				}
				{
					/* Default enter sequence for state Start */
					{
						/* Entry action for state 'Start'. */
						secure_learn_setTimer( (sc_eventid) &(handle->timeEvents.secure_learn_main_region_LearnMode_r1_Start_time_event_0_raised) , 10 * 1000, bool_false);
						handle->iface.net_scheme = 0;
					}
					handle->stateConfVector[0] = secure_learn_main_region_LearnMode_r1_Start;
					handle->stateConfVectorPosition = 0;
				}
			} 
		}
	}
}

/* The reactions of state Idle. */
static void secure_learn_react_main_region_Idle(Secure_learn* handle) {
	{
		/* The reactions of state Idle. */
		if (handle->iface.learnRequest_raised) { 
			{
				/* Default exit sequence for state Idle */
				handle->stateConfVector[0] = Secure_learn_last_state;
				handle->stateConfVectorPosition = 0;
			}
			secure_learnIface_update_callback();
			{
				/* Default enter sequence for state LearnMode */
				{
					/* Default enter sequence for region r1 */
					{
						/* Default react sequence for initial entry  */
						{
							/* Default enter sequence for state Start */
							{
								/* Entry action for state 'Start'. */
								secure_learn_setTimer( (sc_eventid) &(handle->timeEvents.secure_learn_main_region_LearnMode_r1_Start_time_event_0_raised) , 10 * 1000, bool_false);
								handle->iface.net_scheme = 0;
							}
							handle->stateConfVector[0] = secure_learn_main_region_LearnMode_r1_Start;
							handle->stateConfVectorPosition = 0;
						}
					}
				}
			}
		}  else {
			if (handle->iface.inclusionRequest_raised) { 
				{
					/* Default exit sequence for state Idle */
					handle->stateConfVector[0] = Secure_learn_last_state;
					handle->stateConfVectorPosition = 0;
				}
				handle->iface.node = handle->iface.inclusionRequest_value;
				{
					/* Default enter sequence for state InclusionMode */
					{
						/* Default enter sequence for region r1 */
						{
							/* Default react sequence for initial entry  */
							{
								/* Default enter sequence for state SchemeRequest */
								{
									/* Entry action for state 'SchemeRequest'. */
									secure_learn_setTimer( (sc_eventid) &(handle->timeEvents.secure_learn_main_region_InclusionMode_r1_SchemeRequest_time_event_0_raised) , 10 * 1000, bool_false);
									secure_learnIfaceI_send_scheme_get(handle->iface.node, handle->iface.txOptions);
								}
								handle->stateConfVector[0] = secure_learn_main_region_InclusionMode_r1_SchemeRequest;
								handle->stateConfVectorPosition = 0;
							}
						}
					}
				}
			}  else {
				if (handle->iface.commandsSupportedRequest_raised) { 
					{
						/* Default exit sequence for state Idle */
						handle->stateConfVector[0] = Secure_learn_last_state;
						handle->stateConfVectorPosition = 0;
					}
					{
						/* Default enter sequence for state SendReport */
						{
							/* Entry action for state 'SendReport'. */
							secure_learnIface_send_commands_supported(handle->iface.node, handle->iface.txOptions);
						}
						handle->stateConfVector[0] = secure_learn_main_region_SendReport;
						handle->stateConfVectorPosition = 0;
					}
				}  else {
					if (handle->iface.assignIdDone_raised) { 
						{
							/* Default exit sequence for state Idle */
							handle->stateConfVector[0] = Secure_learn_last_state;
							handle->stateConfVectorPosition = 0;
						}
						secure_learnIface_update_callback();
						{
							/* Default enter sequence for state EarlyStart */
							handle->stateConfVector[0] = secure_learn_main_region_LearnMode_r1_EarlyStart;
							handle->stateConfVectorPosition = 0;
						}
					} 
				}
			}
		}
	}
}

/* The reactions of state SchemeRequest. */
static void secure_learn_react_main_region_InclusionMode_r1_SchemeRequest(Secure_learn* handle) {
	{
		/* The reactions of state SchemeRequest. */
		if (handle->iface.scheme_report_value == handle->iface.node && handle->iface.scheme_report_raised) { 
			{
				/* Default exit sequence for state SchemeRequest */
				handle->stateConfVector[0] = Secure_learn_last_state;
				handle->stateConfVectorPosition = 0;
				{
					/* Exit action for state 'SchemeRequest'. */
					secure_learn_unsetTimer( (sc_eventid) &(handle->timeEvents.secure_learn_main_region_InclusionMode_r1_SchemeRequest_time_event_0_raised) );		
				}
			}
			{
				/* The reactions of state null. */
				if ((handle->iface.scheme & 1) > 0) { 
					secure_learnIfaceI_register_scheme(handle->iface.node, handle->iface.scheme);
					{
						/* Default enter sequence for state SendKey */
						{
							/* Entry action for state 'SendKey'. */
							secure_learn_setTimer( (sc_eventid) &(handle->timeEvents.secure_learn_main_region_InclusionMode_r1_SendKey_time_event_0_raised) , 10 * 1000, bool_false);
							secure_learnIfaceI_send_key(handle->iface.node, handle->iface.txOptions);
						}
						handle->stateConfVector[0] = secure_learn_main_region_InclusionMode_r1_SendKey;
						handle->stateConfVectorPosition = 0;
					}
				}  else {
					if (bool_true) { 
						{
							/* Default enter sequence for state Fail */
							{
								/* Entry action for state 'Fail'. */
								handle->iface.scheme = 0;
								secure_learnIfaceI_register_scheme(handle->iface.node, 0);
							}
							handle->stateConfVector[0] = secure_learn_main_region_InclusionMode_r1_Fail;
							handle->stateConfVectorPosition = 0;
						}
					} 
				}
			}
		}  else {
			if (handle->iface.tx_fail_raised || handle->timeEvents.secure_learn_main_region_InclusionMode_r1_SchemeRequest_time_event_0_raised) { 
				{
					/* Default exit sequence for state SchemeRequest */
					handle->stateConfVector[0] = Secure_learn_last_state;
					handle->stateConfVectorPosition = 0;
					{
						/* Exit action for state 'SchemeRequest'. */
						secure_learn_unsetTimer( (sc_eventid) &(handle->timeEvents.secure_learn_main_region_InclusionMode_r1_SchemeRequest_time_event_0_raised) );		
					}
				}
				{
					/* Default enter sequence for state Fail */
					{
						/* Entry action for state 'Fail'. */
						handle->iface.scheme = 0;
						secure_learnIfaceI_register_scheme(handle->iface.node, 0);
					}
					handle->stateConfVector[0] = secure_learn_main_region_InclusionMode_r1_Fail;
					handle->stateConfVectorPosition = 0;
				}
			} 
		}
	}
}

/* The reactions of state SendKey. */
static void secure_learn_react_main_region_InclusionMode_r1_SendKey(Secure_learn* handle) {
	{
		/* The reactions of state SendKey. */
		if (handle->timeEvents.secure_learn_main_region_InclusionMode_r1_SendKey_time_event_0_raised || handle->iface.tx_fail_raised) { 
			{
				/* Default exit sequence for state SendKey */
				handle->stateConfVector[0] = Secure_learn_last_state;
				handle->stateConfVectorPosition = 0;
				{
					/* Exit action for state 'SendKey'. */
					secure_learn_unsetTimer( (sc_eventid) &(handle->timeEvents.secure_learn_main_region_InclusionMode_r1_SendKey_time_event_0_raised) );		
				}
			}
			{
				/* Default enter sequence for state Fail */
				{
					/* Entry action for state 'Fail'. */
					handle->iface.scheme = 0;
					secure_learnIfaceI_register_scheme(handle->iface.node, 0);
				}
				handle->stateConfVector[0] = secure_learn_main_region_InclusionMode_r1_Fail;
				handle->stateConfVectorPosition = 0;
			}
		}  else {
			if (handle->iface.tx_done_raised) { 
				{
					/* Default exit sequence for state SendKey */
					handle->stateConfVector[0] = Secure_learn_last_state;
					handle->stateConfVectorPosition = 0;
					{
						/* Exit action for state 'SendKey'. */
						secure_learn_unsetTimer( (sc_eventid) &(handle->timeEvents.secure_learn_main_region_InclusionMode_r1_SendKey_time_event_0_raised) );		
					}
				}
				{
					/* Default enter sequence for state waitVerify */
					{
						/* Entry action for state 'waitVerify'. */
						secure_learn_setTimer( (sc_eventid) &(handle->timeEvents.secure_learn_main_region_InclusionMode_r1_waitVerify_time_event_0_raised) , 10 * 1000, bool_false);
						secure_learnIfaceI_restore_key();
					}
					handle->stateConfVector[0] = secure_learn_main_region_InclusionMode_r1_waitVerify;
					handle->stateConfVectorPosition = 0;
				}
			} 
		}
	}
}

/* The reactions of state SendInterit. */
static void secure_learn_react_main_region_InclusionMode_r1_SendInterit(Secure_learn* handle) {
	{
		/* The reactions of state SendInterit. */
		if (handle->iface.tx_fail_raised || handle->timeEvents.secure_learn_main_region_InclusionMode_r1_SendInterit_time_event_0_raised) { 
			{
				/* Default exit sequence for state SendInterit */
				handle->stateConfVector[0] = Secure_learn_last_state;
				handle->stateConfVectorPosition = 0;
				{
					/* Exit action for state 'SendInterit'. */
					secure_learn_unsetTimer( (sc_eventid) &(handle->timeEvents.secure_learn_main_region_InclusionMode_r1_SendInterit_time_event_0_raised) );		
				}
			}
			{
				/* Default enter sequence for state Fail */
				{
					/* Entry action for state 'Fail'. */
					handle->iface.scheme = 0;
					secure_learnIfaceI_register_scheme(handle->iface.node, 0);
				}
				handle->stateConfVector[0] = secure_learn_main_region_InclusionMode_r1_Fail;
				handle->stateConfVectorPosition = 0;
			}
		}  else {
			if (handle->iface.scheme_report_value == handle->iface.node && handle->iface.scheme_report_raised) { 
				{
					/* Default exit sequence for state SendInterit */
					handle->stateConfVector[0] = Secure_learn_last_state;
					handle->stateConfVectorPosition = 0;
					{
						/* Exit action for state 'SendInterit'. */
						secure_learn_unsetTimer( (sc_eventid) &(handle->timeEvents.secure_learn_main_region_InclusionMode_r1_SendInterit_time_event_0_raised) );		
					}
				}
				secure_learnIfaceI_register_scheme(handle->iface.node, handle->iface.scheme);
				{
					/* The reactions of state null. */
					if ((handle->iface.scheme & 1) > 0) { 
						{
							/* Default enter sequence for state end */
							handle->stateConfVector[0] = secure_learn_main_region_InclusionMode_r1_end;
							handle->stateConfVectorPosition = 0;
						}
					}  else {
						if (bool_true) { 
							{
								/* Default enter sequence for state Fail */
								{
									/* Entry action for state 'Fail'. */
									handle->iface.scheme = 0;
									secure_learnIfaceI_register_scheme(handle->iface.node, 0);
								}
								handle->stateConfVector[0] = secure_learn_main_region_InclusionMode_r1_Fail;
								handle->stateConfVectorPosition = 0;
							}
						} 
					}
				}
			} 
		}
	}
}

/* The reactions of state Fail. */
static void secure_learn_react_main_region_InclusionMode_r1_Fail(Secure_learn* handle) {
	{
		/* The reactions of state Fail. */
		if (bool_true) { 
			{
				/* Default exit sequence for state Fail */
				handle->stateConfVector[0] = Secure_learn_last_state;
				handle->stateConfVectorPosition = 0;
			}
			{
				/* Default enter sequence for state end */
				handle->stateConfVector[0] = secure_learn_main_region_InclusionMode_r1_end;
				handle->stateConfVectorPosition = 0;
			}
		} 
	}
}

/* The reactions of state end. */
static void secure_learn_react_main_region_InclusionMode_r1_end(Secure_learn* handle) {
	{
		/* The reactions of state end. */
		if (bool_true) { 
			{
				/* Default exit sequence for state InclusionMode */
				{
					/* Default exit sequence for region r1 */
					/* Handle exit of all possible states (of r1) at position 0... */
					switch(handle->stateConfVector[ 0 ]) {
						case secure_learn_main_region_InclusionMode_r1_SchemeRequest : {
							{
								/* Default exit sequence for state SchemeRequest */
								handle->stateConfVector[0] = Secure_learn_last_state;
								handle->stateConfVectorPosition = 0;
								{
									/* Exit action for state 'SchemeRequest'. */
									secure_learn_unsetTimer( (sc_eventid) &(handle->timeEvents.secure_learn_main_region_InclusionMode_r1_SchemeRequest_time_event_0_raised) );		
								}
							}
							break;
						}
						case secure_learn_main_region_InclusionMode_r1_SendKey : {
							{
								/* Default exit sequence for state SendKey */
								handle->stateConfVector[0] = Secure_learn_last_state;
								handle->stateConfVectorPosition = 0;
								{
									/* Exit action for state 'SendKey'. */
									secure_learn_unsetTimer( (sc_eventid) &(handle->timeEvents.secure_learn_main_region_InclusionMode_r1_SendKey_time_event_0_raised) );		
								}
							}
							break;
						}
						case secure_learn_main_region_InclusionMode_r1_SendInterit : {
							{
								/* Default exit sequence for state SendInterit */
								handle->stateConfVector[0] = Secure_learn_last_state;
								handle->stateConfVectorPosition = 0;
								{
									/* Exit action for state 'SendInterit'. */
									secure_learn_unsetTimer( (sc_eventid) &(handle->timeEvents.secure_learn_main_region_InclusionMode_r1_SendInterit_time_event_0_raised) );		
								}
							}
							break;
						}
						case secure_learn_main_region_InclusionMode_r1_Fail : {
							{
								/* Default exit sequence for state Fail */
								handle->stateConfVector[0] = Secure_learn_last_state;
								handle->stateConfVectorPosition = 0;
							}
							break;
						}
						case secure_learn_main_region_InclusionMode_r1_end : {
							{
								/* Default exit sequence for state end */
								handle->stateConfVector[0] = Secure_learn_last_state;
								handle->stateConfVectorPosition = 0;
							}
							break;
						}
						case secure_learn_main_region_InclusionMode_r1_waitVerify : {
							{
								/* Default exit sequence for state waitVerify */
								handle->stateConfVector[0] = Secure_learn_last_state;
								handle->stateConfVectorPosition = 0;
								{
									/* Exit action for state 'waitVerify'. */
									secure_learn_unsetTimer( (sc_eventid) &(handle->timeEvents.secure_learn_main_region_InclusionMode_r1_waitVerify_time_event_0_raised) );		
								}
							}
							break;
						}
						default: break;
					}
				}
			}
			{
				/* Default enter sequence for state Complete */
				{
					/* Entry action for state 'Complete'. */
					secure_learnIface_complete(handle->iface.scheme);
				}
				handle->stateConfVector[0] = secure_learn_main_region_Complete;
				handle->stateConfVectorPosition = 0;
			}
		} 
	}
}

/* The reactions of state waitVerify. */
static void secure_learn_react_main_region_InclusionMode_r1_waitVerify(Secure_learn* handle) {
	{
		/* The reactions of state waitVerify. */
		if (handle->iface.node == handle->iface.key_verify_value && handle->iface.key_verify_raised) { 
			{
				/* Default exit sequence for state waitVerify */
				handle->stateConfVector[0] = Secure_learn_last_state;
				handle->stateConfVectorPosition = 0;
				{
					/* Exit action for state 'waitVerify'. */
					secure_learn_unsetTimer( (sc_eventid) &(handle->timeEvents.secure_learn_main_region_InclusionMode_r1_waitVerify_time_event_0_raised) );		
				}
			}
			{
				/* The reactions of state null. */
				if (handle->iface.isController) { 
					{
						/* Default enter sequence for state SendInterit */
						{
							/* Entry action for state 'SendInterit'. */
							secure_learn_setTimer( (sc_eventid) &(handle->timeEvents.secure_learn_main_region_InclusionMode_r1_SendInterit_time_event_0_raised) , 10 * 1000, bool_false);
							secure_learnIfaceI_send_scheme_inherit(handle->iface.node, handle->iface.txOptions);
						}
						handle->stateConfVector[0] = secure_learn_main_region_InclusionMode_r1_SendInterit;
						handle->stateConfVectorPosition = 0;
					}
				}  else {
					if (bool_true) { 
						{
							/* Default enter sequence for state end */
							handle->stateConfVector[0] = secure_learn_main_region_InclusionMode_r1_end;
							handle->stateConfVectorPosition = 0;
						}
					} 
				}
			}
		}  else {
			if (handle->timeEvents.secure_learn_main_region_InclusionMode_r1_waitVerify_time_event_0_raised) { 
				{
					/* Default exit sequence for state waitVerify */
					handle->stateConfVector[0] = Secure_learn_last_state;
					handle->stateConfVectorPosition = 0;
					{
						/* Exit action for state 'waitVerify'. */
						secure_learn_unsetTimer( (sc_eventid) &(handle->timeEvents.secure_learn_main_region_InclusionMode_r1_waitVerify_time_event_0_raised) );		
					}
				}
				{
					/* Default enter sequence for state Fail */
					{
						/* Entry action for state 'Fail'. */
						handle->iface.scheme = 0;
						secure_learnIfaceI_register_scheme(handle->iface.node, 0);
					}
					handle->stateConfVector[0] = secure_learn_main_region_InclusionMode_r1_Fail;
					handle->stateConfVectorPosition = 0;
				}
			} 
		}
	}
}

/* The reactions of state Complete. */
static void secure_learn_react_main_region_Complete(Secure_learn* handle) {
	{
		/* The reactions of state Complete. */
		if (bool_true) { 
			{
				/* Default exit sequence for state Complete */
				handle->stateConfVector[0] = Secure_learn_last_state;
				handle->stateConfVectorPosition = 0;
			}
			{
				/* Default enter sequence for state Idle */
				handle->stateConfVector[0] = secure_learn_main_region_Idle;
				handle->stateConfVectorPosition = 0;
			}
		} 
	}
}

/* The reactions of state SendReport. */
static void secure_learn_react_main_region_SendReport(Secure_learn* handle) {
	{
		/* The reactions of state SendReport. */
		if (handle->iface.tx_fail_raised || handle->iface.tx_done_raised) { 
			{
				/* Default exit sequence for state SendReport */
				handle->stateConfVector[0] = Secure_learn_last_state;
				handle->stateConfVectorPosition = 0;
			}
			{
				/* Default enter sequence for state Idle */
				handle->stateConfVector[0] = secure_learn_main_region_Idle;
				handle->stateConfVectorPosition = 0;
			}
		} 
	}
}


