// $Id$

// ============================================================================
//
// = LIBRARY
//    tests
//
// = FILENAME
//    Barrier_Test.cpp
//
// = DESCRIPTION
//     This program illustrates how the ACE barrier synchronization
//     mechanisms work.
//
// = AUTHOR
//    Prashant Jain and Doug Schmidt
//
// ============================================================================

#include "test_config.h"
#include "ace/Synch.h"
#include "ace/Thread_Manager.h"

#if defined (ACE_HAS_THREADS)

struct Tester_Args
  // = TITLE
  //     These arguments are passed into each test thread.
{
  Tester_Args (ACE_Barrier &tb, int i)
    : tester_barrier_ (tb),
    n_iterations_ (i) {}

  ACE_Barrier &tester_barrier_;
  // Reference to the tester barrier.  This controls each miteration
  // of the tester function running in every thread.

  int n_iterations_;
  // Number of iterations to run.
};

// Iterate <n_iterations> time printing off a message and "waiting"
// for all other threads to complete this iteration.

static void *
tester (Tester_Args *args)
{
#if defined (VXWORKS)
  ACE_DEBUG ((LM_DEBUG, "(%P|%t) %s stack size is %u\n",
              ACE_OS::thr_self (), ACE_OS::thr_min_stack ()));
#endif /* VXWORKS */

  for (int iterations = 1;
       iterations <= args->n_iterations_;
       iterations++)
    {
      ACE_DEBUG ((LM_DEBUG, "(%t) in iteration %d\n", iterations));

      // Block until all other threads have waited, then continue.
      args->tester_barrier_.wait ();
    }

  return 0;
}

#endif /* ACE_HAS_THREADS */

int
main (int, char *[])
{
  ACE_START_TEST ("Barrier_Test");

#if defined (ACE_HAS_THREADS)
  int n_threads = ACE_MAX_THREADS;
  int n_iterations = ACE_MAX_ITERATIONS;

  ACE_Barrier tester_barrier (n_threads);

  Tester_Args args (tester_barrier, n_iterations);

#if defined (VXWORKS)
  // Assign a thread (VxWorks task) name to test that feature.
  ACE_thread_t *thread_name;
  ACE_NEW_RETURN (thread_name, ACE_thread_t[n_threads], -1);

  // And test the ability to specify stack size.
  size_t *stack_size;
  ACE_NEW_RETURN (stack_size, size_t[n_threads], -1);

  int i;

  for (i = 0; i < n_threads; ++i)
    {
      ACE_NEW_RETURN (thread_name[i], char[32], -1);
      ACE_OS::sprintf (thread_name[i], "thread%u", i);

      stack_size[i] = 40000;
    }
#endif /* VXWORKS */

  for (size_t iteration_count = 0;
       iteration_count < ACE_MAX_ITERATIONS;
       iteration_count++)
    {
      ACE_DEBUG ((LM_DEBUG, "starting iteration %d\n",
                  iteration_count));

      if (ACE_Thread_Manager::instance ()->spawn_n
          (
#if defined (VXWORKS)
           thread_name,
#endif /* VXWORKS */
           n_threads,
           (ACE_THR_FUNC) tester,
           (void *) &args,
           THR_NEW_LWP | THR_JOINABLE
#if defined (VXWORKS)
           , ACE_DEFAULT_THREAD_PRIORITY
           , -1
           , 0
           , stack_size
#endif /* VXWORKS */
           ) == -1)

        ACE_ERROR_RETURN ((LM_ERROR, "%p\n", "spawn_n"), 1);

      ACE_Thread_Manager::instance ()->wait ();
    }

#if defined (VXWORKS)
  for (i = 0; i < n_threads; ++i)
    {
      delete [] thread_name[i];
    }
  delete [] thread_name;
  delete [] stack_size;
#endif /* VXWORKS */

  ACE_DEBUG ((LM_DEBUG, "test done\n"));
#else
  ACE_ERROR ((LM_ERROR, "threads not supported on this platform\n"));
#endif /* ACE_HAS_THREADS */
  ACE_END_TEST;
  return 0;
}
