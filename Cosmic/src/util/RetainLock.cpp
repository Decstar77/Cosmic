#include "RetainLock.h"
namespace cm
{

	void RetainLock::Lock()
	{
		lk = std::unique_lock<std::mutex>(mx);
		cv.wait(lk, [this] { return !locked.load(); });
		locked = true;
	}

	void RetainLock::Unlock()
	{
		locked.store(false);
		lk.unlock();
		cv.notify_one();
	}

	bool RetainLock::IsLocked()
	{
		return locked.load();
	}

}