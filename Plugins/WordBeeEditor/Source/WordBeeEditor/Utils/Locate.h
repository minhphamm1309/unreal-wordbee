#pragma once

template <typename T>
class Locate {
private:
	inline static std::unique_ptr<T> instance=nullptr;

public:
	static T* Get() {
		return instance.get();
	}

	static void Set(T* newInstance) {
		instance.reset(newInstance);
	}

	// Xóa instance
	static void Clear() {
		instance.reset();
	}
};
