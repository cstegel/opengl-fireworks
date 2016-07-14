#pragma once

#include <memory>
#include <vector>
#include <array>
#include <cstdint>
#include <string>
#include <unordered_map>

using std::shared_ptr;
using std::unique_ptr;

using std::make_shared;

using std::vector;
using std::array;
using std::unordered_map;

using int8 = std::int8_t;
using int16 = std::int16_t;
using int32 = std::int32_t;
using int64 = std::int64_t;

using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;

using std::to_string;
using std::string;

namespace fw
{
	class NonCopyable
	{
	public:
		NonCopyable &operator=(const NonCopyable &) = delete;
		NonCopyable(const NonCopyable &) = delete;
		NonCopyable() = default;
	};

	/**
	 * Functor that can be instantiated as a hash function for an enum class.
	 * Useful for when you want to use an enum class as an unordered_map key
	 */
	struct EnumHash
	{
		template <typename T>
		std::size_t operator()(T t) const
		{
			return static_cast<std::size_t>(t);
		}
	};
}

