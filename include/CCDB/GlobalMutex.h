#ifndef DCCDBGlobalMutex_h__
#define DCCDBGlobalMutex_h__


#include <string>
#include <stdlib.h>
#include <map>
#include <new>

#include "CCDB/CCDBGlobals.h"

using namespace std;

#ifdef WIN32
#include "winpthreads.h"
#else //posix
#include <pthread.h>
#endif



namespace ccdb
{

/** 
 * This class is obsolete concept of constants reading syncronization
 * Don't use it. It will be deleted. 
 * With time...
 *                                                                     
 */
class CCDBGlobalMutex
{
public:
	/** @brief Singleton instance for global mutex-es
	 *
	 * @return   DConsoleContext* instance
	 */
	static CCDBGlobalMutex* Instance();
	
	void ReadConstantsLock();
	void ReadConstantsRelease();

	void LogLock();
	void LogRelease();

	static unsigned int GetCurrentThreadId();
	~CCDBGlobalMutex();
protected:
private:
	CCDBGlobalMutex();										/// Private so that it can  not be called
	CCDBGlobalMutex(CCDBGlobalMutex const&){};			/// copy constructor is private
	CCDBGlobalMutex& operator=(CCDBGlobalMutex const&);	/// assignment operator is private
		
	static CCDBGlobalMutex* mInstance;					    ///Main and only singleton instance

	pthread_mutex_t mReadConstsMutex;	    ///read constants mutex posix

	void Lock(pthread_mutex_t * mutex);     ///locks mutex by handle
	void Release(pthread_mutex_t * mutex);  ///releases mutex by handle


};
}
#endif // DCCDBGlobalMutex_h__