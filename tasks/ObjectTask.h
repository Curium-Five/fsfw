
/**
 * @file ObjectTask.h
 *
 * @brief This file contains the definition for the ObjectTask class.
 *
 * @author Bastian Baetz
 *
 * @date 02/03/2012
 *
 */
#ifndef OBJECTTASK_H_
#define OBJECTTASK_H_

#include <framework/objectmanager/ObjectManagerIF.h>
#include <framework/tasks/ExecutableObjectIF.h>
#include <framework/tasks/TaskBase.h>

/**
 * @brief This class represents a specialized task for periodic activities of objects.
 *
 * @details For object-oriented embedded systems, simple periodic tasks executing a single function
 * 			are not very useful. An object method, representing some kind of activity to be performed,
 * 			would either be instantiated in each execution, thus loosing all attribute information, or
 * 			must be known globally. To address this issue, the ObjectTask class is used.
 * 			It implements the TaskBase prototype and provides additional means to make use of any
 * 			object that implements the ExecutableObjectIF interface. The required performOperation
 * 			method is then called periodically. Attributes of the executed object are persistent
 * 			during the task's lifetime.
 * 			Functionally, the class works similar to PeriodicTask.
 *
 * @author Bastian Baetz
 *
 * @date 02/03/2012
 *
 * @ingroup task_handling
 */
class ObjectTask: public TaskBase {
protected:
	/**
	 * @brief	This attribute holds a pointer to the object to be executed.
	 */
	ExecutableObjectIF* executingObject;
	/**
	 * @brief	The period of the task.
	 * @details	The period determines the frequency of the task's execution. It is expressed in clock ticks.
	 */
	Interval_t period;
	/**
	* @brief id of the associated OS period
	*/
	PeriodId_t periodId;
	/**
	 * @brief	The pointer to the deadline-missed function.
	 * @details	This pointer stores the function that is executed if the task's deadline is missed.
	 * 			So, each may react individually on a timing failure. The pointer may be NULL,
	 * 			then nothing happens on missing the deadline. The deadline is equal to the next execution
	 * 			of the periodic task.
	 */
	void ( *deadlineMissedFunc )( void );
	/**
	 * @brief	This is the function executed in the new task's context.
	 * @details	It converts the argument back to the thread object type and copies the class instance
	 * 			to the task context. The taskFunctionality method is called afterwards.
	 * @param	A pointer to the task object itself is passed as argument.
	 */
	static TaskReturn_t taskEntryPoint( TaskArgument_t argument );
	/**
	 * @brief	The function containing the actual functionality of the task.
	 * @details	The method sets and starts
	 * 			the task's period, then enters a loop that is repeated as long as the isRunning
	 * 			attribute is true. Within the loop, the object's performFunction method is called, and
	 * 			afterwards the checkAndRestartPeriod system call to block the task until the next
	 * 			period. On missing the deadline, the deadlineMissedFunction is executed.
	 */
	void taskFunctionality( void );

public:
	/**
	 * @brief	The first of the two standard constructors of the class.
	 * @details	This constructor initializes the object to be executed with the aid of an
	 * 			initialization function that returns the pointer to the object.
	 * 			In the underlying TaskBase class, a new operating system task is created.
	 * 			In addition to the TaskBase parameters, the period, the pointer to the
	 * 			aforementioned initialization function and an optional "deadline-missed"
	 * 			function pointer is passed.
	 * @param priority		Sets the priority of a task. Values range from a low 0 to a high 99.
	 * @param stack_size	The stack size reserved by the operating system for the task.
	 * @param setPeriod 	The length of the period with which the task's functionality will be
	 * 						executed. It is expressed in clock ticks.
	 * @param setDeadlineMissedFunc	The function pointer to the deadline missed function
	 * 								that shall be assigned.
	 * @param initFunction	A pointer to the initialization function that returns the object to be executed.
	 */
	ObjectTask( const char *name, TaskPriority_t setPriority, size_t setStack, Interval_t setPeriod, void (*setDeadlineMissedFunc)(), ExecutableObjectIF* (*initFunction)() );
	/**
	 * @brief	The second of the two standard constructors of the class.
	 * @details	This constructor initializes the object to be executed with the aid of an
	 * 			object id and the global ObjectManager class.
	 * 			In the underlying TaskBase class, a new operating system task is created.
	 * 			In addition to the TaskBase parameters, the period, the object_id of the
	 * 			object to be executed and an optional "deadline-missed" function pointer is passed.
	 * @param priority		Sets the priority of a task. Values range from a low 0 to a high 99.
	 * @param stack_size	The stack size reserved by the operating system for the task.
	 * @param setPeriod 	The length of the period with which the task's functionality will be
	 * 						executed. It is expressed in clock ticks.
	 * @param setDeadlineMissedFunc	The function pointer to the deadline missed function
	 * 								that shall be assigned.
	 * @param object_id	The object id of the object to be executed.
	 */
	ObjectTask( const char *name, TaskPriority_t setPriority, size_t setStack, Interval_t setPeriod, void (*setDeadlineMissedFunc)(), object_id_t object_id );
	/**
	 * @brief	Currently, the executed object's lifetime is not coupled with the task object's
	 * 			lifetime, so the destructor is empty.
	 */
	virtual ~ObjectTask( void );

	/**
	 * @brief	The method to start the task.
	 * @details	The method starts the task with the respective system call.
	 * 			Entry point is the taskEntryPoint method described below.
	 * 			The address of the task object is passed as an argument
	 * 			to the system call.
	 */
	ReturnValue_t startTask( void );

};

#endif /* OBJECTTASK_H_ */
