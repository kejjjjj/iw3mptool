#pragma once

#include <cmath>
#include <ostream>
#include <type_traits>
#include <functional>

struct ImVec2;

template <class T>
struct vec3;

template <class T>
struct vec2
{
	T x, y;

	[[nodiscard]] constexpr vec2() { x = 0; y = 0; }
	[[nodiscard]] constexpr vec2(const T& v) { x = v; y = v; }
	[[nodiscard]] constexpr vec2(const T& a, const T& b) { x = a; y = b; }
	[[nodiscard]] constexpr vec2(const T* a) { x = a[0]; y = a[1]; }
	[[nodiscard]] constexpr vec2(const vec3<T>& v);

	//[[nodiscard]] constexpr vec2(const float(*a)[2]) { x = a[0]; y = a[1];  }

	[[nodiscard]] constexpr vec2 operator+(const vec2& v) const { return { x + v.x, y + v.y }; }
	[[nodiscard]] constexpr vec2 operator-(const vec2& v) const { return { x - v.x, y - v.y }; }
	[[nodiscard]] constexpr vec2 operator*(const vec2& v) const { return { x * v.x, y * v.y }; }
	[[nodiscard]] constexpr vec2 operator/(const vec2& v) const { return { x / v.x, y / v.y }; }
	[[nodiscard]] constexpr void operator=(const vec2& v) { x = v.x; y = v.y; }
	[[nodiscard]] constexpr void operator=(const vec3<T>& v);

	[[nodiscard]] constexpr vec2 operator+(const T& v) const { return { x + v, y + v }; }
	[[nodiscard]] constexpr vec2 operator-(const T& v) const { return { x - v, y - v }; }
	[[nodiscard]] constexpr vec2 operator*(const T& v) const { return { x * v, y * v }; }
	[[nodiscard]] constexpr vec2 operator/(const T& v) const { return { x / v, y / v }; }

	[[nodiscard]] constexpr vec2 operator+=(const vec2& v) { return { x += v.x, y += v.y }; }
	[[nodiscard]] constexpr vec2 operator-=(const vec2& v) { return { x -= v.x, y -= v.y }; }
	[[nodiscard]] constexpr vec2 operator*=(const vec2& v) { return { x *= v.x, y *= v.y }; }
	[[nodiscard]] constexpr vec2 operator/=(const vec2& v) { return { x /= v.x, y /= v.y }; }
	[[nodiscard]] constexpr bool operator==(const vec2& v) const { return  x == v.x && y == v.y; }
	[[nodiscard]] constexpr bool operator!=(const vec2& v) const { return  x != v.x || y != v.y; }
	[[nodiscard]] constexpr bool operator>(const vec2& v) const { return  x > v.x && y > v.y; }
	[[nodiscard]] constexpr bool operator<(const vec2& v) const { return  x < v.x && y < v.y; }
	[[nodiscard]] constexpr bool operator>=(const vec2& v) const { return  x >= v.x && y >= v.y; }
	[[nodiscard]] constexpr bool operator<=(const vec2& v) const { return  x <= v.x && y <= v.y; }

	[[nodiscard]] constexpr vec2 operator+=(const T& v) { return { x += v, y += v }; }
	[[nodiscard]] constexpr vec2 operator-=(const T& v) { return { x -= v, y -= v }; }
	[[nodiscard]] constexpr vec2 operator*=(const T& v) { return { x *= v, y *= v }; }
	[[nodiscard]] constexpr vec2 operator/=(const T& v) { return { x /= v, y /= v }; }


	[[nodiscard]] constexpr T& operator[](const int index) const {
		return ((T*)&x)[index];
	}
	[[nodiscard]] constexpr operator vec2<int>() { return { (int)x, (int)y }; }
	[[nodiscard]] constexpr operator vec2<float>() { return { (float)x, (float)y }; }


	[[nodiscard]] T mag() const {
		return std::sqrtf(x * x + y * y);
	}
	[[nodiscard]] constexpr T dot(const vec2& vec) const
	{
		return x * vec.x + y * vec.y;
	}
	[[nodiscard]] vec2<T> normalize() const  noexcept {
		vec2<T> r = *this;
		float len = this->mag();
		float ilength;

		if (len) {
			ilength = 1 / len;
			r.x *= ilength;
			r.y *= ilength;
		}

		return r;
	}
	[[maybe_unused]] vec2<T> normalize(float& len) const noexcept {
		vec2<T> r = *this;
		len = this->mag();
		float ilength;

		if (len) {
			ilength = 1 / len;
			r.x *= ilength;
			r.y *= ilength;
		}

		return r;
	}
	[[nodiscard]] T dist(const vec2& vec) const
	{
		const vec2 sub = *this - vec;
		return sub.mag();
	}
	[[nodiscard]] constexpr vec2 inverse() const
	{
		return { -x, -y };
	}
	[[nodiscard]] constexpr float MagSq() const { //magnitude squared
		return (x * x + y * y);
	}
	[[nodiscard]] constexpr vec2 clamp(const T min, const T max) {
		vec2<T> r = *this;
		if (x < min)		r.x = min;
		else if (x > max)	r.x = max;

		if (y < min)		r.y = min;
		else if (y > max)	r.y = max;

		return r;
	}
	[[nodiscard]] constexpr vec2 normalize360() const noexcept
	{
		return vec2
		{
			(360.0f / 65536) * ((int)(x * (65536 / 360.0f)) & 65535),
			(360.0f / 65536) * ((int)(y * (65536 / 360.0f)) & 65535),
		};
	}
	[[nodiscard]] constexpr vec2 normalize180() const noexcept
	{
		vec3 angle = normalize360();
		for (int i = 0; i < 2; i++) {
			if (angle[i] > 180.0f) {
				angle[i] -= 360.0f;
			}
		}
		return angle;
	}
	//expects radians
	[[nodiscard]] vec2 fromAngle(T angle) {
		return { std::cosf(static_cast<float>(angle)), std::cosf(static_cast<float>(angle)) };
	}
	[[nodiscard]] constexpr vec2 smooth(const vec2& dst, float smoothingFactor) const noexcept {
		smoothingFactor = std::max(0.f, std::min(1.f, smoothingFactor));
		return *this + smoothingFactor * (dst - *this);
	}
	[[nodiscard]] constexpr vec2 angular_distance(const vec2& other)
	{
		[[nodiscard]] constexpr const auto ce_fmodf = [](float x, float y) {
			return (y != 0.0f) ? x - static_cast<int>(x / y) * y : 0.0f;
			};

		[[nodiscard]] constexpr auto x_diff = ce_fmodf(other.x - x + 180, 360) - 180;
		[[nodiscard]] constexpr auto y_diff = ce_fmodf(other.y - y + 180, 360) - 180;

		return vec2(x_diff, y_diff).normalize180();
	}

	template<typename Func, typename ... Args>
	[[nodiscard]] constexpr vec2 for_each(Func func, Args... args) 
	{
		return vec2
		{
			x = func(x, std::forward<Args>(args)...),
			y = func(y, std::forward<Args>(args)...),
		};
	}

};

using ivec2 = vec2<int>;
using fvec2 = vec2<float>;

template <class T>
struct vec3
{

	T x, y, z;

	[[nodiscard]] constexpr vec3() { x = 0; y = 0, z = 0; }
	[[nodiscard]] constexpr vec3(const T& v) { x = v; y = v, z = v; }
	[[nodiscard]] constexpr vec3(const T& a, const T& b, const T& c) { x = a; y = b, z = c; }
	[[nodiscard]] constexpr vec3(const T(*a)[3]) { x = a[0]; y = a[1], z = a[2]; }
	[[nodiscard]] constexpr vec3(const T* a) { x = a[0]; y = a[1], z = a[2]; }
	[[nodiscard]] constexpr vec3(const vec2<T>& vec) { x = vec.x; y = vec.y; z = 0.f; }
	//[[nodiscard]] constexpr explicit vec3(const vec3<int>& v) { x = (int)v.x, y = (int)v.y; }

	[[nodiscard]] constexpr vec3 operator+(const vec3& v) const { return { x + v.x, y + v.y, z + v.z }; }
	[[nodiscard]] constexpr vec3 operator-(const vec3& v) const { return { x - v.x, y - v.y, z - v.z }; }
	[[nodiscard]] constexpr vec3 operator*(const vec3& v) const { return { x * v.x, y * v.y, z * v.z }; }
	[[nodiscard]] constexpr vec3 operator/(const vec3& v) const { return { x / v.x, y / v.y, z / v.z }; }
	[[nodiscard]] constexpr void operator=(const vec3& v) { x = v.x; y = v.y, z = v.z; }
	[[nodiscard]] constexpr void operator=(const T* v) { x = v[0]; y = v[1], z = v[2]; }
	std::basic_ostream<char, std::char_traits<char>> operator<< (const vec3& v) {};

	[[nodiscard]] constexpr vec3 operator+(T v) const { return { x + v, y + v, z + v }; }
	[[nodiscard]] constexpr vec3 operator-(T v) const { return { x - v, y - v, z - v }; }
	[[nodiscard]] constexpr vec3 operator*(T v) const { return { x * v, y * v, z * v }; }
	[[nodiscard]] constexpr vec3 operator/(T v) const { return { x / v, y / v, z / v }; }

	[[nodiscard]] constexpr vec3 operator+=(const vec3& v) { return { x += v.x, y += v.y, z += v.z }; }
	[[nodiscard]] constexpr vec3 operator-=(const vec3& v) { return { x -= v.x, y -= v.y, z -= v.z }; }
	[[nodiscard]] constexpr vec3 operator*=(const vec3& v) { return { x *= v.x, y *= v.y, z *= v.z }; }
	[[nodiscard]] constexpr vec3 operator/=(const vec3& v) { return { x /= v.x, y /= v.y, z /= v.z }; }
	[[nodiscard]] constexpr bool operator==(const vec3& v) const { return  x == v.x && y == v.y && z == v.z; }
	[[nodiscard]] constexpr bool operator!=(const vec3& v) const { return  x != v.x || y != v.y || z != v.z; }
	[[nodiscard]] constexpr bool operator>(const vec3& v) const { return  x > v.x && y > v.y && z > v.z; }
	[[nodiscard]] constexpr bool operator<(const vec3& v) const { return  x < v.x && y < v.y && z < v.z; }
	[[nodiscard]] constexpr bool operator>=(const vec3& v) const { return  x >= v.x && y >= v.y && z >= v.z; }
	[[nodiscard]] constexpr bool operator<=(const vec3& v) const { return  x <= v.x && y <= v.y && z <= v.z; }

	[[nodiscard]] constexpr vec3 operator+=(T v) { return { x += v, y += v, z += v }; }
	[[nodiscard]] constexpr vec3 operator-=(T v) { return { x -= v, y -= v, z -= v }; }
	[[nodiscard]] constexpr vec3 operator*=(T v) { return { x *= v, y *= v, z *= v }; }
	[[nodiscard]] constexpr vec3 operator/=(T v) { return { x /= v, y /= v, z /= v }; }

	[[nodiscard]] constexpr operator vec3<int>() { return { (int)x, (int)y, (int)z }; }
	[[nodiscard]] constexpr operator vec3<float>() { return { (float)x, (float)y, (float)z }; }
	[[nodiscard]] constexpr operator vec2<int>() { return { (int)x, (int)y }; }
	[[nodiscard]] constexpr operator vec2<float>() { return { (float)x, (float)y }; }
	[[nodiscard]] constexpr operator T* () { return &x; }
	//[[nodiscard]] constexpr operator const T* () const { return &x; }

	[[nodiscard]] constexpr T& operator[](const int index) const {
		return ((T*)&x)[index];
	}

	[[nodiscard]] constexpr vec2<T> xy() const noexcept
	{
		return { x, y };
	}

	[[nodiscard]] T mag() const noexcept {
		return static_cast<T>(std::sqrtf(static_cast<float>(x * x + y * y + z * z)));
	}
	[[nodiscard]] constexpr T mag_sq() const noexcept {
		return (x * x + y * y + z * z);
	}
	[[nodiscard]] constexpr T dot(const vec3& vec) const noexcept
	{
		return x * vec.x + y * vec.y + z * vec.z;
	}
	[[nodiscard]] vec3<T> normalize() const  noexcept {
		vec3<T> r = *this;
		const float length = this->mag();
		float ilength;

		if (length) {
			ilength = 1 / length;
			r.x *= ilength;
			r.y *= ilength;
			r.z *= ilength;
		}

		return r;
	}
	[[maybe_unused]] vec3<T> normalize(float& len) const  noexcept {
		vec3<T> r = *this;
		len = this->mag();
		float ilength;

		if (len) {
			ilength = 1 / len;
			r.x *= ilength;
			r.y *= ilength;
			r.z *= ilength;
		}

		return r;
	}
	[[nodiscard]] T dist(const vec3& vec) const noexcept
	{
		const vec3 sub = *this - vec;
		return sub.mag();
	}
	[[nodiscard]] constexpr T dist_sq(const vec3& vec) const noexcept
	{
		const vec3 sub = *this - vec;
		return sub.mag_sq();
	}
	[[nodiscard]] constexpr vec3<T> inverse() const noexcept
	{
		return { -x, -y, -z };
	}
	[[nodiscard]] vec3<T> abs() const noexcept {

		[[nodiscard]] constexpr auto cexpr_abs = [](const T v) {
			return v < static_cast<T>(0) ? -v : v;
			};

		return { cexpr_abs(x), cexpr_abs(y), cexpr_abs(z) };
	}
	[[nodiscard]] constexpr vec3<T> clamp(const T min, const T max) const noexcept {
		vec3<T> r = *this;

		if (x < min)		r.x = min;
		else if (x > max)	r.x = max;

		if (y < min)		r.y = min;
		else if (y > max)	r.y = max;

		if (z < min)		r.z = min;
		else if (z > max)	r.z = max;

		return r;
	}
	[[nodiscard]] vec3<T> cross(const vec3<T>& other) const noexcept {

		return {
			this->y * other.z - this->z * other.y,
			this->z * other.x - this->x * other.z,
			this->x * other.y - this->y * other.x,
		};

	}
	[[nodiscard]] vec3<T> toangles() const noexcept
	{
		float forward;
		float yaw, pitch;

		if (y == 0 && x == 0) {
			yaw = 0;
			if (z > 0) {
				pitch = 90;
			}
			else {
				pitch = 270;
			}
		}
		else {
			if (x) {
				yaw = (std::atan2f(y, x) * 180 / 3.14159265358979323846f);
			}
			else if (y > 0) {
				yaw = 90;
			}
			else {
				yaw = 270;
			}
			//if (yaw < 0) {
			//	yaw += 360;
			//}

			forward = sqrt(x * x + y * y);
			pitch = (atan2(z, forward) * 180 / 3.14159265358979323846f);
			if (pitch < 0) {
				pitch += 360;
			}
		}

		return vec3(-pitch, yaw, 0);
	}
	[[nodiscard]] vec3<T> toforward() const noexcept {
		float angle;
		static float sp, sy, cp, cy;
		// static to help MS compiler fp bugs

		angle = y * (3.14159265358979323846f * 2 / 360);
		sy = sin(angle);
		cy = cos(angle);

		angle = x * (3.14159265358979323846f * 2 / 360);
		sp = sin(angle);
		cp = cos(angle);

		return vec3<T>(cp * cy, cp * sy, -sp);

	}
	[[nodiscard]] vec3<T> toright() const noexcept {
		float angle;
		static float sr, cr, sp, sy, cp, cy;
		// static to help MS compiler fp bugs

		angle = y * (3.14159265358979323846f * 2 / 360);
		sy = sin(angle);
		cy = cos(angle);

		angle = x * (3.14159265358979323846f * 2 / 360);
		sp = sin(angle);
		cp = cos(angle);

		angle = z * (3.14159265358979323846f * 2 / 360);
		sr = sin(angle);
		cr = cos(angle);

		return vec3<T>((-1 * sr * sp * cy + -1 * cr * -sy), (-1 * sr * sp * sy + -1 * cr * cy), -1 * sr * cp);

	}
	[[nodiscard]] vec3<T> toup() const noexcept {
		float angle;
		static float sr, cr, sp, sy, cp, cy;
		// static to help MS compiler fp bugs

		angle = y * (3.14159265358979323846f * 2 / 360);
		sy = sin(angle);
		cy = cos(angle);

		angle = x * (3.14159265358979323846f * 2 / 360);
		sp = sin(angle);
		cp = cos(angle);

		angle = z * (3.14159265358979323846f * 2 / 360);
		sr = sin(angle);
		cr = cos(angle);

		return vec3<T>((cr * sp * cy + -sr * -sy), (cr * sp * sy + -sr * cy), cr * cp);

	}
	[[nodiscard]] constexpr vec3<T> to_degrees()
	{
		[[nodiscard]] constexpr float pi = 3.14159265358979323846f;
		return { x * (180.f / pi), y * (180.f / pi), z * (180.f / pi) };
	}
	[[nodiscard]] constexpr vec3<T> normalize360() const noexcept
	{
		return vec3<T>
		{
			(360.0f / 65536) * ((int)(x * (65536 / 360.0f)) & 65535),
			(360.0f / 65536) * ((int)(y * (65536 / 360.0f)) & 65535),
			(360.0f / 65536) * ((int)(z * (65536 / 360.0f)) & 65535),

		};
	}
	[[nodiscard]] constexpr vec3<T> normalize180() const noexcept
	{
		vec3<T> angle = normalize360();
		for (int i = 0; i < 3; i++) {
			if (angle[i] > 180) {
				angle[i] -= 360;
			}
		}
		return angle;
	}
	[[nodiscard]] constexpr vec3<T> angle_delta(const vec3<T>& other) const noexcept
	{
		return (*this - other).normalize180();
	}
	[[nodiscard]] constexpr vec3 smooth(const vec3& dst, float smoothingFactor) const noexcept {
		smoothingFactor = std::max(0.f, std::min(1.f, smoothingFactor));
		return *this + (dst - *this) * smoothingFactor;
	}
	[[nodiscard]] constexpr vec3<T> angular_distance(const vec3<T>& other)
	{
		[[nodiscard]] constexpr const auto ce_fmodf = [](float x, float y) {
			return (y != 0.0f) ? x - static_cast<int>(x / y) * y : 0.0f;
			};

		[[nodiscard]] constexpr auto x_diff = ce_fmodf(other.x - x + 180, 360) - 180;
		[[nodiscard]] constexpr auto y_diff = ce_fmodf(other.y - y + 180, 360) - 180;
		[[nodiscard]] constexpr auto z_diff = ce_fmodf(other.z - z + 180, 360) - 180;

		return vec3(x_diff, y_diff, z_diff).normalize180();
	}

	template<typename Func, typename ... Args>
	[[nodiscard]] constexpr vec3<T> for_each(Func func, Args... args) {

		return vec3<T>
		{
			func(x, std::forward<Args>(args)...),
			func(y, std::forward<Args>(args)...),
			func(z, std::forward<Args>(args)...)
		};
	}
	template<typename Func, typename ... Args>
	[[nodiscard]] constexpr bool every(Func&& func, Args&&... args) const {

		static_assert(std::is_same_v<std::invoke_result_t<Func, decltype(x), Args...>, bool>,
			"Callable must return bool");

		return 
			func(x, std::forward<Args>(args)...) &&
			func(y, std::forward<Args>(args)...) &&
			func(z, std::forward<Args>(args)...);
		
	}
	[[nodiscard]] constexpr vec3<int> to_short() const noexcept
	{
		static_assert(std::is_floating_point_v<T>);
		[[nodiscard]] constexpr const auto a2s = [](float x) {
			return (static_cast<int>((x) * 65536 / 360) & 65535);
			};

		return vec3<int>{a2s(x), a2s(y), a2s(z) };
	}
	[[nodiscard]] constexpr vec3<float> from_short() const noexcept
	{
		static_assert(std::is_integral_v<T>);
		[[nodiscard]] constexpr const auto s2a = [](int x) {
			return static_cast<float>((x) * (360.0f / 65536));
			};

		return vec3<float>{s2a(x), s2a(y), s2a(z) };
	}
	template<typename Type>
	[[nodiscard]] constexpr Type As() noexcept {
		static_assert(std::is_pointer<Type>::value);
		return reinterpret_cast<Type>(&x);
	}
	template<typename Type>
	[[nodiscard]] constexpr Type As() const noexcept {
		static_assert(std::is_pointer<Type>::value);
		return (const Type)(&x);
	}
};

using ivec3 = vec3<int>;
using fvec3 = vec3<float>;
inline std::ostream& operator<<(std::ostream& os, fvec3 const& v) {
	return os << "{ " << v.x << ", " << v.y << ", " << v.z << " }";
}
inline std::ostream& operator<<(std::ostream& os, ivec3 const& v) {
	return os << "{ " << v.x << ", " << v.y << ", " << v.z << " }";
}
inline std::ostream& operator<<(std::ostream& os, fvec2 const& v) {
	return os << "{ " << v.x << ", " << v.y << " }";
}
inline std::ostream& operator<<(std::ostream& os, ivec2 const& v) {
	return os << "{ " << v.x << ", " << v.y << " }";
}
struct Quaternion {
	float w;
	float x;
	float y;
	float z;
};

struct Pixel
{
	uint8_t r, g, b, a;

	[[nodiscard]] constexpr Pixel() { r = 0, g = 0, b = 0, a = 255; }
	[[nodiscard]] constexpr Pixel(const uint8_t val) { r = val, g = val, b = val, a = val; };
	[[nodiscard]] constexpr Pixel(const uint8_t _r, const uint8_t _g, const uint8_t _b, const uint8_t _a) { r = _r, g = _g, b = _b, a = _a; }
	[[nodiscard]] constexpr Pixel(const Pixel& p) { r = p.r, g = p.g, b = p.b, a = p.a; }

	Pixel operator=(const Pixel& px) {
		return { px.r, px.g, px.b, px.a };
	}

#ifdef IMGUI_API
	operator ImColor() { return ImColor(r, g, b, a); };
	operator unsigned int() { return IM_COL32(r, g, b, a); };

#endif
	unsigned int packed() const { return *(unsigned int*)&r; }

};

template<typename T>
[[nodiscard]] constexpr vec2<T>::vec2(const vec3<T>& v)
{
	x = v.x;
	y = v.y;
}

template<typename T>
[[nodiscard]] constexpr void vec2<T>::operator=(const vec3<T>& v)
{
	x = v.x;
	y = v.y;
}

