// -*- c++ -*-
//
// $Id$

#include "ace/Service_Config.h"


ACE_INLINE int
TAO_Internal::open_services (int& argc, char** argv)
{
#if 0
  ACE_MT (ACE_GUARD_RETURN (ACE_Thread_Mutex, guard, service_lock_, -1));
#else
  ACE_MT (ACE_GUARD_RETURN (ACE_Thread_Mutex, guard, *ACE_Static_Object_Lock::instance (), -1));
#endif /* 0 */

  if (TAO_Internal::service_open_count_++ == 0)
    {
#if defined(TAO_PLATFORM_SVC_CONF_FILE_NOTSUP)
      return fake_service_entries_i ();
#else      
      return ACE_Service_Config::open (argc, argv);
#endif
    }
  else
    return 0;
}

ACE_INLINE int
TAO_Internal::close_services (void)
{
#if 0
  ACE_MT (ACE_GUARD_RETURN (ACE_Thread_Mutex, guard, service_lock_, -1));
#else
  ACE_MT (ACE_GUARD_RETURN (ACE_Thread_Mutex, guard, *ACE_Static_Object_Lock::instance (), -1));
#endif /* 0 */

  if (service_open_count_ == 0)
    return -1;
  
  if (--service_open_count_ == 0)
    return ACE_Service_Config::close ();

  return 0;
}


