/*
 * Copyright (c) 2012 by Albert-Jan N. Yzelman
 *
 * This file is part of MulticoreBSP in C --
 *        a port of the original Java-based MulticoreBSP.
 *
 * MulticoreBSP for C is distributed as part of the original
 * MulticoreBSP and is free software: you can redistribute
 * it and/or modify it under the terms of the GNU Lesser
 * General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * MulticoreBSP is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with MulticoreBSP. If not, see
 * <http://www.gnu.org/licenses/>.
 */

/*! \mainpage MulticoreBSP for C
 *
 * This is the doxygen code of the MulticoreBSP for C library.
 * The latest version of the library, and of this documentation,
 * can always be found at:
 *
 *     http://www.multicorebsp.com
 *
 * For users of the library: the documentation of mcbsp.h is
 * all you require when coding in C.
 * A C++-wrapper is also available; see mcbsp.hpp.
 * For advanced run-time thread affinity control, see
 * mcbsp-affinity.h.
 *
 * The remainder of the documentation is meant to help those
 * who want to understand or adapt the MulticoreBSP library.
 *
 * MulticoreBSP, which includes<ol>
 *     <li>MulticoreBSP for C,</li>
 *     <li>MulticoreBSP for Java,</li>
 *     <li>Java MulticoreBSP utilities, and</li>
 *     <li>BSPedupack for Java</li>
 * </ol>
 *
 * are copyright by A. N. Yzelman (
 *     Dept. of Mathematics, Utrecht University, 2007-2011;
 *     Dept. of Computer Science, KU Leuven, 2011-2013;
 *     Flanders ExaScience Laboratory, Intel Labs Europe, 2011-2013.
 * )
 *
 * <p>
 * MulticoreBSP for C is distributed as part of the original
 * MulticoreBSP and is free software: you can redistribute
 * it and/or modify it under the terms of the GNU Lesser
 * General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or
 * (at your option) any later version.<br><br>
 * MulticoreBSP is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.
 * </p><p>
 * You should have received a copy of the GNU Lesser General
 * Public License along with MulticoreBSP. If not, see
 * <http://www.gnu.org/licenses/>.</p>
 */

/*! \file mcbsp.h
 * Defines the MulticoreBSP visible functions.
 *
 * Currently, high-performance variants of the bsp_get and
 * bsp_put are not implemented. For compatability, the HP
 * variants can map to the regular get and put function. To
 * enable this, define the ENABLE_FAKE_HP_DIRECTIVES flag.
 */

#ifndef _H_MCBSP
#define _H_MCBSP

/**
 * Uses spin-locks instead of standard PThreads mutexes
 * and conditions. Less energy-efficient, but faster.
 */
#define MCBSP_USE_SPINLOCK

/**
 * Allows multiple registrations of the same memory address.
 *
 * The asymptotic running times of bsp_push_reg, bsp_pop_reg,
 * all communication primitives, and the synchronisation
 * methods, are unaffected by this flag.
 * However, it does introduce run-time overhead as well as
 * memory overhead. If your application(s) do not use
 * multiple registrations of the same variable, you may
 * consider compiling MulticoreBSP for C with this flag
 * turned off for slightly enhanced performance.
 *
 * Note the original BSPlib standard does demand multiple
 * registrations be possible. Thus if
 * MCBSP_COMPATIBILITY_MODE is set, this flag is
 * automatically set as well.
 */
#define MCBSP_ALLOW_MULTIPLE_REGS

/**
 * Enables high-performance variants of bsp_put, bsp_get,
 * and bsp_send. The high-performance variant of bsp_move
 * is always enabled.
 */
#define MCBSP_ENABLE_HP_DIRECTIVES

/**
 * Sets bsp_put and bsp_get to substitute as implementations
 * for bsp_hpput and bsp_hpget, respectively. An actual high-
 * performance implementation is still forthcoming.
 * If this flag is not set, bsp_hpput and bsp_hpget will not
 * be defined. Note that according to the standard, the
 * regular bsp_put and bsp_get are indeed valid replacements
 * for bsp_hpput and bsp_hpget-- they just are not more
 * performant.
 *
 * If MCBSP_COMPATIBILITY_MODE is defined, this flag is
 * automatically set. Implies MCBSP_ENABLE_HP_DIRECTIVES.
 */
#define ENABLE_FAKE_HP_DIRECTIVES

//define MCBSP_NO_CHECKS to disable all run-time sanity
//checks, except those regarding memory allocation.

#ifdef MCBSP_COMPATIBILITY_MODE
 /** Data type used for thread IDs. */
 #define MCBSP_PROCESSOR_INDEX_DATATYPE int
 /** Data type used to count the number of BSMP messages waiting in queue. */
 #define MCBSP_NUMMSG_TYPE      int
 /** Data type used to refer to memory region sizes. */
 #define MCBSP_BYTESIZE_TYPE            int
#else
 /** Data type used for thread IDs. */
 #define MCBSP_PROCESSOR_INDEX_DATATYPE unsigned int
 /** Data type used to count the number of BSMP messages waiting in queue. */
 #define MCBSP_NUMMSG_TYPE      unsigned int
 /** Data type used to refer to memory region sizes. */
 #define MCBSP_BYTESIZE_TYPE            size_t
#endif

//set forced defines
#ifdef MCBSP_COMPATIBILITY_MODE
 //The BSPlib standard requires multiple registrations.
 #define MCBSP_ALLOW_MULTIPLE_REGS
 //The BSPlib standard prescribes the existance of
 //bsp_hpput and bsp_hpget, so they should be defined
 //even if no actual high-performant implementation is
 //actually available.
 #define MCBSP_ENABLE_HP_DIRECTIVES
 #define ENABLE_FAKE_HP_DIRECTIVES
#endif
#ifdef ENABLE_FAKE_HP_DIRECTIVES
 //if fake HP directives are defined,
 //HP directives are enabled.
 #define MCBSP_ENABLE_HP_DIRECTIVES
#endif


#include <stdarg.h>
#include <stddef.h>

/**
 * The first statement in an SPMD program. Spawns P threads.
 *
 * If the bsp_begin() is the first statement in a program (i.e.,
 * it is the first executable statement in main), then a call to
 * bsp_init prior to bsp_begin is not necessary.
 *
 * Normally main() intialises the parallel computation and
 * provides an entry-point for the SPMD part of the computation
 * using bsp_init. This entry-point is a function wherein the
 * first executable statement is this bsp_begin primitive.
 *
 * Note: with MCBSP_COMPATIBILITY_MODE defined, P is of type int.
 *
 * @param P The number of threads requested for the SPMD program.
 */
void bsp_begin( const MCBSP_PROCESSOR_INDEX_DATATYPE P );

/**
 * Terminates an SPMD program.
 *
 * This should be the last statement in a block of code opened
 * by bsp_begin. It will end the thread which calls this primitive,
 * unless the current thread is the originator of the parallel run.
 * Only that original thread will resume with any remaining
 * sequential code.
 */
void bsp_end( void );

/**
 * Prepares for parallel SPMD execution of a given program.
 *
 * Provides an entry-point for new threads when spawned by
 * a bsp_begin. To properly start a new thread on
 * distributed-memory machines, as was the goal of BSPlib,
 * the program arguments are also supplied.
 *
 * The function spmd points is assumed to have bsp_begin as
 * its first statement, and bsp_end as its final statement.
 *
 * @param spmd Entry point for new threads in the SPMD program.
 * @param argc Number of supplied arguments in argv.
 * @param argv List of the application arguments.
 */
void bsp_init( void (*spmd)(void), int argc, char **argv );

/**
 * Aborts an SPMD program.
 *
 * Will print an error message once and notifies other threads
 * in the SPMD program to abort. Other threads stop at the next
 * bsp_sync or bsp_end at latest.
 * The error message is in the regular C printf format, with a
 * variable number of arguments.
 *
 * @param error_message The error message to display.
 */
void bsp_abort( const char * const error_message, ... );

/**
 * Alternative bsp_abort call.
 *
 * Works with a va_list reference to pass a variable number of
 * arguments. Implemented primarily for if a user wants to
 * wrap bsp_abort in his or her own function with variable
 * arguments.
 *
 * @param error_message The error message to display.
 * @param args A list of arguments needed to display the
 *             above error_message.
 */
void bsp_vabort( const char * const error_message, va_list args );

/**
 * Returns the number of available processors.
 *
 * With `processor' a single unit of execution is meant,
 * which in a regular multicore setting means `core'.
 * Depending on the target machine architecture, it can also
 * refer to hardware threads, for instance.
 *
 * The number of such processors is defined as (1) the
 * number of threads corresponding to the currently running
 * SPMD program, or (2) the total number of available
 * processors on the current machine otherwise.
 *
 * Note: if MCBSP_COMPATIBILITY_MODE is defined, the return
 *       type of this function is `int'. Otherwise, it is
 *       of type `unsigned int'.
 *
 * @return The number of available processors.
 */
MCBSP_PROCESSOR_INDEX_DATATYPE bsp_nprocs( void );

/**
 * Returns a unique thread identifier.
 *
 * A thread identifier during an SPMD run is a unique
 * integer in-between 0 and P-1, where P is the value
 * returned by bsp_nprocs. This unique identifier is
 * constant during the entire SPMD program, that is,
 * from bsp_begin until bsp_end.
 *
 * Note: if MCBSP_COMPATIBILITY_MODE is defined, the return
 *       type of this function is `int'. Otherwise, it is
 *       of type `unsigned int'.
 *
 * @return The unique thread ID.
 */
MCBSP_PROCESSOR_INDEX_DATATYPE bsp_pid( void );

/**
 * Indicates the elapsed time in this SPMD run.
 *
 * Returns the time elapsed since the start of the
 * current thread. Time is in seconds, and the timers
 * across threads in the same SPMD program are not
 * necessarily synchronised.
 *
 * @return Elepsed time since bsp_begin in seconds.
 */
double bsp_time( void );

/**
 * Signals the end of a superstep and starts global
 * synchronisation.
 *
 * A BSP SPMD program is logically separated in
 * supersteps; during a superstep threads may perform
 * local computations and buffer communication
 * requests with other BSP threads.
 * These buffered communication requests are executed
 * during synchronisation. When synchronisation ends,
 * all communication is guaranteed to be finished and
 * threads start executing the next superstep.
 *
 * The available communication-queueing requests
 * during a superstep are: bsp_put, bsp_get, bsp_send.
 * Those primitives strictly adhere to the separation
 * of supersteps and communication. Other communication
 * primitives, bsp_hpput, bsp_hpget and bsp_direct_get
 * are less strict in this separation; refer to their
 * documentation for details.
 */
void bsp_sync( void );

/**
 * Registers a memory area for communication.
 *
 * If an SPMD program defines a local variable x,
 * each of the P threads actually has its own memory
 * areas associated with that variable.
 * Communication requires threads to be aware of the
 * memory location of a destination variable. This
 * function achieves this.
 * The order of variable registration must be the
 * same across all threads in the SPMD program. The
 * size of the registered memory block may differ
 * from thread to thread. Registration takes effect
 * only after a synchronisation.
 *
 * Note: if MCBSP_COMPATIBILITY_MODE is defined,
 *       size will be of type `int'. Otherwise, it
 *       is of type `size_t'.
 *
 * @param address Pointer to the memory area to register.
 * @param size    Size, in bytes, of the area to register.
 */
void bsp_push_reg( void * const address, const MCBSP_BYTESIZE_TYPE size );

/**
 * De-registers a pushed registration.
 *
 * Makes a memory region unavailable for communication.
 * The region should first have been registered using
 * bsp_push_reg, otherwise a run-time error will result.
 *
 * If the same memory address is registered multiple
 * times, only the latest registration is cancelled.
 *
 * The order of deregistrations must be the same across
 * all threads to ensure correct execution. Like with
 * bsp_push_reg, this is entirely the responsibility
 * of the programmer; MulticoreBSP does check for
 * correctness (it cannot efficiently do so).
 *
 * @param address Pointer to the memory region to deregister.
 */
void bsp_pop_reg( void * const address );

/**
 * Put data in a remote memory location.
 *
 * This is a non-blocking communication request.
 * Communication will be executed during the next
 * synchronisation step. The remote memory location must
 * be registered using bsp_push_reg in a previous
 * superstep.
 *
 * The data to be communicated to the remote area will
 * be buffered on request; i.e., the source memory
 * location is free to change after this communication
 * request; the communicated data will not reflect
 * those changes.
 *
 * Note: if MCBSP_COMPATIBILITY_MODE is defined, pid,
 *       offset and size are of type `int'. Otherwise,
 *       pid is of type `unsigned int', and offset and
 *       size of type `size_t'.
 *
 * @param pid         The ID number of the remote thread.
 * @param source      Pointer to the source data.
 * @param destination Pointer to the registered remote
 *                    memory area to send data to.
 * @param offset      Offset (in bytes) of the memory
 *                    area. Offset must be positive and
 *                    less than the remotely registered
 *                    memory size.
 * @param size        Size (in bytes) of the data to be
 *                    communicated; i.e., all the data
 *                    from address source up to address
 *                    (source + size) at the current
 *                    thread, is copied to
 *                    (destination+offset) up to
 *                    (destination+offset+size) at the
 *                    thread with ID pid.
 */
void bsp_put( const MCBSP_PROCESSOR_INDEX_DATATYPE pid, const void * const source,
    void * const destination, const MCBSP_BYTESIZE_TYPE offset, const MCBSP_BYTESIZE_TYPE size );

/**
 * Get data from a remote memory location.
 *
 * This is a non-blocking communication request.
 * Communication will be executed during the next
 * synchronisation step. The remote memory location must
 * be registered using bsp_push_reg in a previous
 * superstep.
 *
 * The data retrieved will be the data at the remote
 * memory location at the time of synchronisation. It
 * will not (and cannot) retrieve data at "this"
 * point in the SPMD program at the remote thread.
 * If other communication at the remote process
 * would change the data at the region of interest,
 * these changes are not included in the retrieved
 * data; in this sense, the get is buffered.
 *
 * Note: if MCBSP_COMPATIBILITY_MODE is defined, pid,
 *       offset and size are of type `int'. Otherwise,
 *       pid will be of type `unsigned int' and
 *       offset and size of type `size_t'.
 *
 * @param pid         The ID number of the remote thread.
 * @param source      Pointer to the registered remote
 *                    memory area where to get data from.
 * @param offset      Offset (in bytes) of the remote
 *                    memory area. Offset must be
 *                    positive and must be less than
 *                    the remotely registered  memory
 *                    size.
 * @param destination Pointer to the local destination
 *                    memory area.
 * @param size        Size (in bytes) of the data to be
 *                    communicated; i.e., all the data
 *                    from address (source+offset) up
 *                    to address (source+offset+size)
 *                    at the remote thread, is copied
 *                    to destination up to
 *                    (destination+size) at this thread.
 */
void bsp_get( const MCBSP_PROCESSOR_INDEX_DATATYPE pid, const void * const source,
    const MCBSP_BYTESIZE_TYPE offset, void * const destination, const MCBSP_BYTESIZE_TYPE size );

/**
 * Get data from a remote memory location.
 *
 * This is a blocking communication primitive:
 * communication is executed immediately and is not
 * queued until the next synchronisation step. The
 * remote memory location must be regustered using
 * bsp_push_reg in a previous superstep.
 *
 * The data retrieved will be the data at the remote
 * memory location at "this" time. There is no
 * guarantee that the remote thread is at the same
 * position in executing the SPMD program; it might
 * be anywhere in the current superstep. If the
 * remote thread writes to the source memory block
 * in this superstep, the retrieved data may
 * partially consist of old and new data; this
 * function does not buffer nor is it atomic in
 * any way.
 *
 * Note: if MCBSP_COMPATIBILITY_MODE is defined, pid,
 *       offset and size are of type `int'. Otherwise,
 *       pid is of type `unsigned int' and offset &
 *       size of type `size_t'.
 *
 * @param pid         The ID number of the remote thread.
 * @param source      Pointer to the registered remote
 *                    memory area where to get data from.
 * @param offset      Offset (in bytes) of the remote
 *                    memory area. Offset must be
 *                    positive and must be less than
 *                    the remotely registered  memory
 *                    size.
 * @param destination Pointer to the local destination
 *                    memory area.
 * @param size        Size (in bytes) of the data to be
 *                    communicated; i.e., all the data
 *                    from address source up to address
 *                    (source + size) at the remote
 *                    thread, is copied to
 *                    (destination+offset) up to
 *                    (destination+offset+size) at this
 *                    thread.
 */
void bsp_direct_get( const MCBSP_PROCESSOR_INDEX_DATATYPE pid, const void * const source,
        const MCBSP_BYTESIZE_TYPE offset, void * const destination, const MCBSP_BYTESIZE_TYPE size );

/**
 * Sets the tag size of inter-thread messages.
 *
 * bsp_send can be used to send message tuples
 * (tag,payload). Tag must be of a fixed size,
 * the payload size may differ per message.
 *
 * This function sets the tagsize to the given
 * size. This new size will be valid from the
 * next bsp_sync() on.
 * All processes must call bsp_set_tagsize with
 * the same new size, or MulticoreBSP for C
 * will return an error.
 * The given size will be overwritten with the
 * old tagsize upon function exit.
 *
 * Note: if MCBSP_COMPATIBILITY_MODE is defined,
 *       size is of type `int*'. Otherwise, it is
 *       of type `size_t*'.
 *
 * @param size Pointer to the new tag size
 *             (in bytes). On exit, the memory
 *             area pointed to is changed to
 *             reflect the old tag size.
 */
void bsp_set_tagsize( MCBSP_BYTESIZE_TYPE * const size );

/**
 * Sends a message to a remote thread.
 *
 * A message is actually a tuple (tag,payload). Tag is of a fixed size
 * (see bsp_set_tagsize), the payload size is set per message. Messages
 * will be available at the destination thread in the next superstep.
 *
 * Note: If MCBSP_COMPATIBILITY_MODE is defined, then pid and size are
 *       of type `int'. Otherwise, pid is of type `unsigned int' and
 *       size of type `size_t'.
 *
 * @param pid     ID of the remote thread to send this message to.
 * @param tag     Pointer to the tag data.
 * @param payload Pointer to the payload data.
 * @param size    Size of the payload data (in bytes).
 */
void bsp_send( const MCBSP_PROCESSOR_INDEX_DATATYPE pid, const void * const tag,
    const void * const payload, const MCBSP_BYTESIZE_TYPE size );

#ifdef MCBSP_ENABLE_HP_DIRECTIVES
/**
 * This is a non-buffering and non-blocking send request.
 *
 * The function differs from the regular bsp_send in two major ways:
 * (1) the actual send communication may occur between now and the
 *     end of the next synchronisation phase, and
 * (2) the tag and payload data is read and sent somewhere between
 *     now and the end of the next synchronisation phase.
 * If you change the contents of the memory area tag and payload
 * point to after calling this function, undefined communication
 * will occur.
 * The semantics of BSMP remain unchanged: the sent messages will
 * only become available at the remote processor when the next
 * computation superstep begins. The performance gain is two-fold:
 * (a) bsp_hpsend avoids buffering-on-send, and
 * (b) BSP may send messages during a computation phase, thus
 *     overlapping computation with communication.
 *
 * Ad (a): normally, BSMP in BSP copies tag and payload data at
 * least three times. It buffers on bsp_send (buffer-on-send),
 * it buffers at the receiving processes' incoming BSMP queue
 * (buffer-on-receive), and finally bsp_get_tag and bsp_move
 * copy the data in the target user-supplied memory areas. To
 * also eliminate the latter data movement, please consider
 * using bsp_hpmove.
 *
 * Ad (b): it is not guaranteed this overlap results in faster
 * execution time. You should think about if using these high-
 * performance primitives makes sense on a per-application
 * basis, and factor in the extra costs  of structuring your
 * algorithm to enable correct use of these primitives.
 *
 * See bsp_send for general remarks about using BSMP primitives
 * in BSP. See bsp_hpget and bsp_hpput for equivalent (non-
 * buffering and non-blocking) high-performance communication
 * requests.
 *
 * Note: If MCBSP_COMPATIBILITY_MODE is defined, then pid and
 *       size are of type `int'. Otherwise, pid is of type
 *       `unsigned int' and size of type `size_t'.
 *
 * @param pid     ID of the receiving thread.
 * @param tag     Pointer to the tag data
 *                (actual data is *not* buffered)
 * @param payload Pointer to the payload data.
 *                (actual data is *not* buffered)
 * @param size    Size of the payload data (in bytes).
 */
void bsp_hpsend( const MCBSP_PROCESSOR_INDEX_DATATYPE pid, const void * const tag,
    const void * const payload, const MCBSP_BYTESIZE_TYPE size );
#endif

/**
 * Queries the size of the incoming BSMP queue.
 *
 * @param packets Where to store the number of incoming messages.
 * @param accumulated_size Where to store the accumulated size
 *      of all incoming messages (optional, can be NULL).
 */
void bsp_qsize( MCBSP_NUMMSG_TYPE * const packets,
    MCBSP_BYTESIZE_TYPE * const accumulated_size );

/**
 * Retrieves the tag of the first message in the queue of
 * incoming messages.
 *
 * Also retrieves the size of the payload in that message.
 *
 * If there are no messages waiting in queue, status
 * will be set to the maximum possible value and no
 * tag data is retrieved.
 *
 * Note: if MCBSP_COMPATIBILITY_MODE is defined, status
 *       is of type `int*', and *status will be set to
 *       -1 if there are no incoming messages.
 *       Otherwise, status is of type `unsigned int'.
 *
 * @param status Pointer to where to store the payload
 *               size (in bytes) of the first incoming
 *               message.
 * @param tag    Pointer to where to store the tag of
 *               the first incoming message.
 */
void bsp_get_tag( MCBSP_BYTESIZE_TYPE * const status,
    void * const tag );

/**
 * Retrieves the payload from the first message in the
 * queue of incoming messages, and removes that message.
 *
 * If the incoming queue is empty, this function has
 * no effect. This function will copy a given maximum
 * of bytes from the message payload into a supplied
 * buffer. This maximum should equal or be larger
 * than the payload size (which can, e.g.,  be
 * retrieved via bsp_get_tag).
 * The maximum can be 0 bytes; the net effect is
 * the efficient removal of the first message from
 * the queue.
 *
 * Note that Bulk Synchronous Message Passing (BSMP)
 * is doubly buffered: bsp_send buffers on send and
 * this function buffers again on receives.
 *
 * See bsp_hpmove if buffer-on-receive is unwanted.
 *
 * Note: if MCBSP_COMPATIBILITY_MODE is defined,
 *       max_copy_size is of type `int'. Otherwise,
 *       it is of type `size_t'.
 *
 * @param payload       Where to copy the message
 *                      payload into.
 * @param max_copy_size The maximum number of
 *                      bytes to copy.
 */
void bsp_move( void * const payload, const MCBSP_BYTESIZE_TYPE max_copy_size );

/**
 * Unbuffered retrieval of the payload of the first
 * message in the incoming message queue.
 *
 * See bsp_move for general remarks. This function
 * returns a pointer to an area in the incoming
 * message buffer where the requested message tag
 * and message payload reside.
 * The function returns the size (in bytes) of the
 * message payload.
 *
 * Take care *not* to free the memory referred to
 * by these pointers; the data resides in a
 * MulticoreBSP-managed buffer area and will be
 * freed by the run-time system.
 *
 * Warning: use this high-performance function
 *          with care; out of bound access on tag
 *          or payload can cause corruption of
 *          other messages in queue.
 *
 * Note: if MCBSP_COMPATIBILITY_MODE is defined,
 *       the return type of this function is `int'.
 *       Otherwise, is it of type `size_t'.
 *
 * @param p_tag     Where to store the pointer to
 *                  the message tag.
 * @param p_payload Where to store the pointer to
 *                  the message payload.
 * @return Pointer to the area where the message
 * tag and payload reside, or -1 if the message
 * queue was empty.
 */
MCBSP_BYTESIZE_TYPE bsp_hpmove( void* * const p_tag, void* * const p_payload );

#ifdef ENABLE_FAKE_HP_DIRECTIVES
/**
 * Put data in a remote memory location.
 *
 * Note: current implementation does a normal bsp_get!
 *       An communication overlapping implementation
 *       is forthcoming.
 *
 * This is a non-blocking communication request.
 * Communication will be executed sometime between now
 * and during the next synchronisation step. Note that
 * this differs from bsp_put. Communication is guaranteed
 * to have finished before the next superstep. Note this
 * means that both source and destination memory areas
 * might be read and written to at any time after
 * issueing this communication request. This overlap of
 * communication and computation is the fundamental
 * difference with the standard bsp_put.
 *
 * It is not guaranteed this overlap results in faster
 * execution time. You should think about if using these
 * high-performance primitives makes sense on a
 * per-application basis, and factor in the extra costs
 * of structuring your algorithm to enable correct use
 * of these primitives.
 *
 * Otherwise usage is similar to that of bsp_put;
 * please refer to that function for further
 * documentation.
 *
 * Note: if MCBSP_COMPATIBILITY_MODE is defined, pid,
 *       offset and size are of type `int'. Otherwise,
 *       pid is of type `unsigned int', offset and
 *       size of type `size_t'.
 *
 * @param pid         The ID number of the remote thread.
 * @param source      Pointer to the source data.
 * @param destination Pointer to the registered remote
 *                    memory area to send data to.
 * @param offset      Offset (in bytes) of the memory
 *                    area. Offset must be positive and
 *                    less than the remotely registered
 *                    memory size.
 * @param size        Size (in bytes) of the data to be
 *                    communicated; i.e., all the data
 *                    from address source up to address
 *                    (source + size) at the current
 *                    thread, is copied to
 *                    (destination+offset) up to
 *                    (destination+offset+size) at the
 *                    thread with ID pid.
 */
void bsp_hpput( const MCBSP_PROCESSOR_INDEX_DATATYPE pid, const void * const source,
        void * const destination, const MCBSP_BYTESIZE_TYPE offset, const MCBSP_BYTESIZE_TYPE size );

/**
 * Get data from a remote memory location.
 *
 * Note: current implementation does a normal bsp_get!
 *       An communication overlapping implementation
 *       is forthcoming.
 *
 * This is a non-blocking communication request.
 * Communication will be executed between now and the
 * next synchronisation step. Note that this differs
 * from bsp_get. Communication is guaranteed to have
 * finished before the next superstep. Note this
 * means that both source and destination memory
 * areas might be read and written to at any time
 * after issueing this communication request. This
 * overlap of communication and computation is the
 * fundamental difference with the standard bsp_get.
 *
 * It is not guaranteed this overlap results in faster
 * execution time. You should think about if using these
 * high-performance primitives makes sense on a
 * per-application basis, and factor in the extra costs
 * of structuring your algorithm to enable correct use
 * of these primitives.
 *
 * Note the difference between this high-performance
 * get and bsp_direct_get is that the latter function
 * is blocking (performs the communication
 * immediately and waits for it to end).
 *
 * Otherwise usage is similar to that of bsp_get;
 * please refer to that function for further
 * documentation.
 *
 * Note: if MCBSP_COMPATIBILITY_MODE is defined, pid,
 *       offset and size are of type `int'. Otherwise,
 *       pid is of type `unsigned int' and offset &
 *       size of type `size_t'.
 *
 * @param pid         The ID number of the remote thread.
 * @param source      Pointer to the registered remote
 *                    memory area where to get data from.
 * @param offset      Offset (in bytes) of the remote
 *                    memory area. Offset must be
 *                    positive and must be less than
 *                    the remotely registered  memory
 *                    size.
 * @param destination Pointer to the local destination
 *                    memory area.
 * @param size        Size (in bytes) of the data to be
 *                    communicated; i.e., all the data
 *                    from address source up to address
 *                    (source + size) at the remote
 *                    thread, is copied to
 *                    (destination+offset) up to
 *                    (destination+offset+size) at this
 *                    thread.
 */
void bsp_hpget( const MCBSP_PROCESSOR_INDEX_DATATYPE pid, const void * const source,
        const MCBSP_BYTESIZE_TYPE offset, void * const destination, const MCBSP_BYTESIZE_TYPE size );
#endif

#endif

