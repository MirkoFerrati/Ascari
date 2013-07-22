//written by Simone Nardi
#ifndef NOSTOP_THREAD_H
#define NOSTOP_THREAD_H

#ifdef ISAGENT

#include <mutex>

namespace NoStop
{
		/**
		*	A lock is a clever object that you construct on the stack
		*	and for the duration of its lifetime your object is protected
		*	from any other threads.
		*	You have to put Locks inside all the methods of your object that
		*	access data shared with the captive thread.
		*/
		class Lock
		{
		public:
			///	Acquire the state of the semaphore
			Lock( std::shared_ptr<std::mutex> _mutex ) : m_mutex(_mutex)
			{
				m_mutex->lock();
			}

			///	Release the state of the semaphore
			~Lock()
			{
				m_mutex->unlock();
			}

		private:
			std::shared_ptr<std::mutex> m_mutex;

			// Prevent copying
			Lock(const Lock&);
			Lock& operator=(const Lock&);
		};
}
#endif

#endif // NOSTOP_THREAD_H
