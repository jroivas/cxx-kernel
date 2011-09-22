#ifndef LOCAL_H
#define LOCAL_H

namespace __cxxabiv1
{
	//_extension__ typedef int __guard __attribute__((mode(__DI__)));
	typedef int __guard __attribute__((mode(__DI__)));
	//typedef __int64_t __guard;

	extern "C" {
		int __cxa_guard_acquire(__guard *);
		void __cxa_guard_release(__guard *);
		void __cxa_guard_abort(__guard *);
	}
}
#endif
