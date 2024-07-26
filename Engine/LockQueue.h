#pragma once

template<typename T>
class LockQueue
{
public:
	LockQueue() {}

	LockQueue(const LockQueue&) = delete;
	LockQueue& operator=(const LockQueue&) = delete;

	void Push(T value)
	{
		lock_guard<mutex> lock(_mutex);
		_queue.push(std::move(value));
		_condVar.notify_one();
	}

	bool TryPop(T& value)
	{
		lock_guard<mutex> lock(_mutex);
		if(_queue.empty())
			return false;

		value = std::move(_queue.front());
		_queue.pop();
		return true;
	}

	void WaitPop(T& value)
	{
		unique_lock<mutex> lock(_mutex);
		_condVar.wait(lock, [this] {return _queue.empty() == false; });

		value = std::move(_queue.front());
		_queue.pop();
	}

	size_t Size()
	{
		lock_guard<mutex> lock(_mutex);
		return _queue.size();
	}

	void Clear()
	{
		lock_guard<mutex> lock(_mutex);
		while(_queue.empty() == false)
			_queue.pop();
	}

private:
	queue<T> _queue;
	mutex _mutex;
	condition_variable _condVar;
};
