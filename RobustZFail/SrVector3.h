/************************************************************************		
\link	www.twinklingstar.cn
\author Twinkling Star
\date	2013/11/22
****************************************************************************/
#ifndef	SR_FOUNDATION_SRVECTOR3_H_
#define SR_FOUNDATION_SRVECTOR3_H_
#include <assert.h>
#include "SrSimpleTypes.h"
#include "SrMath.h"
/** \addtogroup foundation
  @{
*/

#ifndef SR_ASSERT
#ifdef _DEBUG
#define SR_ASSERT(x) assert(x)
#else
#define SR_ASSERT(x) {}
#endif
#endif


/**
\brief 3 Element vector class.

This is a vector class with public data members.
This is not nice but it has become such a standard that hiding the xyz data members
makes it difficult to reuse external code that assumes that these are public in the library.
The vector class can be made to use float or double precision by appropriately defining float.
This has been chosen as a cleaner alternative to a template class.
*/
class SrVector3
	{
	public:
	//!Constructors

	/**
	\brief default constructor leaves data uninitialized.
	*/
	SR_INLINE SrVector3();

	/**
	\brief Assigns scalar parameter to all elements.
	
	Useful to initialize to zero or one.

	\param[in] a Value to assign to elements.
	*/
	SR_INLINE explicit SrVector3(float a);

	/**
	\brief Initializes from 3 scalar parameters.

	\param[in] nx Value to initialize X component.
	\param[in] ny Value to initialize Y component.
	\param[in] nz Value to initialize Z component.
	*/
	SR_INLINE SrVector3(float nx, float ny, float nz);
	
	/**
	\brief Initializes from an array of scalar parameters.

	\param[in] v Value to initialize with.
	*/
	SR_INLINE SrVector3(const float v[]);

	/**
	\brief Copy constructor.
	*/
	SR_INLINE SrVector3(const SrVector3& v);

	/**
	\brief Assignment operator.
	*/
	SR_INLINE const SrVector3& operator=(const SrVector3&);

	/**
	\brief Access the data as an array.

	\return Array of 3 floats.
	*/
	SR_INLINE const float *get() const;
	/**
	\brief writes out the 3 values to dest.

	\param[out] dest Array to write elements to.
	*/
	SR_INLINE void get(SrF32 * dest) const;

	/**
	\brief writes out the 3 values to dest.
	*/
	SR_INLINE void get(SrF64 * dest) const;

	/**
	\brief Access the data as an array.

	\return Array of 3 floats.
	*/
	SR_INLINE float* get();

	SR_INLINE float& operator[](int index);
	SR_INLINE float  operator[](int index) const;

	//Operators
	/**
	\brief true if all the members are smaller.
	*/
	SR_INLINE bool operator< (const SrVector3&) const;
	/**
	\brief returns true if the two vectors are exactly equal.

	use equal() to test with a tolerance.
	*/
	SR_INLINE bool operator==(const SrVector3&) const;
	/**
	\brief returns true if the two vectors are exactly unequal.

	use !equal() to test with a tolerance.
	*/
	SR_INLINE bool operator!=(const SrVector3&) const;

//Methods
	
	/**
	\brief reads 3 consecutive values from the ptr passed
	*/
	SR_INLINE void  set(const SrF32 *);

	/**
	\brief reads 3 consecutive values from the ptr passed
	*/
	SR_INLINE void  set(const SrF64 *);
	SR_INLINE void  set(const SrVector3 &);

//legacy methods:
	SR_INLINE void setx(const float & d);
	SR_INLINE void sety(const float & d);
	SR_INLINE void setz(const float & d);

	/**
	\brief this = -a
	*/
	SR_INLINE void  setNegative(const SrVector3 &a);

	/**
	\brief this = -this
	*/
	SR_INLINE void  setNegative();

	/**
	\brief reads 3 consecutive values from the ptr passed
	*/
	SR_INLINE void  set(float, float, float);
	SR_INLINE void  set(float);

	SR_INLINE void  zero();
	
	/**
	\brief tests for exact zero vector
	*/
	SR_INLINE bool isZero()	const
		{
		if((x != 0.0f) || (y != 0.0f) || (z != 0.0f))	return false;
		return true;
		}

	SR_INLINE void  setPlusInfinity();
	SR_INLINE void  setMinusInfinity();

	/**
	\brief this = element wise min(this,other)
	*/
	SR_INLINE void min(const SrVector3 &);
	/**
	\brief this = element wise max(this,other)
	*/
	SR_INLINE void max(const SrVector3 &);

	/**
	\brief this = a + b
	*/
	SR_INLINE void  add(const SrVector3 & a, const SrVector3 & b);
	/**
	\brief this = a - b
	*/
	SR_INLINE void  subtract(const SrVector3 &a, const SrVector3 &b);
	/**
	\brief this = s * a;
	*/
	SR_INLINE void  multiply(float s,  const SrVector3 & a);

	/**
	\brief this[i] = a[i] * b[i], for all i.
	*/
	SR_INLINE void  arrayMultiply(const SrVector3 &a, const SrVector3 &b);


	/**
	\brief this = s * a + b;
	*/
	SR_INLINE void  multiplyAdd(float s, const SrVector3 & a, const SrVector3 & b);

	/**
	\brief normalizes the vector
	*/
	SR_INLINE float normalize();

	/**
	\brief sets the vector's magnitude
	*/
	SR_INLINE void	setMagnitude(float);

	/**
	\brief snaps to closest axis
	*/
	SR_INLINE SrU32	closestAxis()	const;

	/**
	\brief returns true if all 3 elems of the vector are finite (not NAN or INF, etc.)
	*/
	SR_INLINE bool isFinite() const;

	/**
	\brief returns the scalar product of this and other.
	*/
	SR_INLINE float dot(const SrVector3 &other) const;

	/**
	\brief compares orientations (more readable, user-friendly function)
	*/
	SR_INLINE bool sameDirection(const SrVector3 &) const;

	/**
	\brief returns the magnitude
	*/
	SR_INLINE float magnitude() const;

	/**
	\brief returns the squared magnitude
	
	Avoids calling sqrt()!
	*/
	SR_INLINE float magnitudeSquared() const;

	/**
	\brief returns (this - other).magnitude();
	*/
	SR_INLINE float distance(const SrVector3 &) const;

	/**
	\brief returns (this - other).magnitudeSquared();
	*/
	SR_INLINE float distanceSquared(const SrVector3 &v) const;

	/**
	\brief this = left x right
	*/
	SR_INLINE void cross(const SrVector3 &left, const SrVector3 & right);

	/**
	\brief Stuff magic values in the point, marking it as explicitly not used.
	*/
	SR_INLINE void setNotUsed();

	/**
	\brief Checks the point is marked as not used
	*/
	SR_INLINE bool isNotUsed() const;

	/**
	\brief returns true if this and arg's elems are within epsilon of each other.
	*/
	SR_INLINE bool equals(const SrVector3 &, float epsilon) const;

	/**
	\brief negation
	*/
	SR_INLINE SrVector3 operator -() const;
	/**
	\brief vector addition
	*/
	SR_INLINE SrVector3 operator +(const SrVector3 & v) const;
	/**
	\brief vector difference
	*/
	SR_INLINE SrVector3 operator -(const SrVector3 & v) const;
	/**
	\brief scalar post-multiplication
	*/
	SR_INLINE SrVector3 operator *(float f) const;
	/**
	\brief scalar division
	*/
	SR_INLINE SrVector3 operator /(float f) const;
	/**
	\brief vector addition
	*/
	SR_INLINE SrVector3&operator +=(const SrVector3& v);
	/**
	\brief vector difference
	*/
	SR_INLINE SrVector3&operator -=(const SrVector3& v);
	/**
	\brief scalar multiplication
	*/
	SR_INLINE SrVector3&operator *=(float f);
	/**
	\brief scalar division
	*/
	SR_INLINE SrVector3&operator /=(float f);
	/**
	\brief cross product
	*/
	SR_INLINE SrVector3 cross(const SrVector3& v) const;

	/**
	\brief cross product
	*/
	SR_INLINE SrVector3 operator^(const SrVector3& v) const;
	/**
	\brief dot product
	*/
	SR_INLINE float      operator|(const SrVector3& v) const;

	float x,y,z;
	};


/** \endcond */

//implementations:
SrVector3::SrVector3(float v) : x(v), y(v), z(v)
	{
	}

SR_INLINE SrVector3::SrVector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z)
	{
	}


SR_INLINE SrVector3::SrVector3(const float v[]) : x(v[0]), y(v[1]), z(v[2])
	{
	}


SR_INLINE SrVector3::SrVector3(const SrVector3 &v) : x(v.x), y(v.y), z(v.z)
	{
	}


SR_INLINE SrVector3::SrVector3()
	{
	//default constructor leaves data uninitialized.
	}


SR_INLINE const SrVector3& SrVector3::operator=(const SrVector3& v)
	{
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
	}


// Access the data as an array.

SR_INLINE const float* SrVector3::get() const
	{
	return &x;
	}


SR_INLINE float* SrVector3::get()
	{
	return &x;
	}

 
SR_INLINE void  SrVector3::get(SrF32 * v) const
	{
	v[0] = (SrF32)x;
	v[1] = (SrF32)y;
	v[2] = (SrF32)z;
	}

 
SR_INLINE void  SrVector3::get(SrF64 * v) const
	{
	v[0] = (SrF64)x;
	v[1] = (SrF64)y;
	v[2] = (SrF64)z;
	}


SR_INLINE float& SrVector3::operator[](int index)
	{
	SR_ASSERT(index>=0 && index<=2);
	return (&x)[index];
	}


SR_INLINE float  SrVector3::operator[](int index) const
	{
	SR_ASSERT(index>=0 && index<=2);
	return (&x)[index];
	}

 
SR_INLINE void SrVector3::setx(const float & d) 
	{ 
	x = d; 
	}

 
SR_INLINE void SrVector3::sety(const float & d) 
	{ 
	y = d; 
	}

 
SR_INLINE void SrVector3::setz(const float & d) 
	{ 
	z = d; 
	}

//Operators
 
SR_INLINE bool SrVector3::operator< (const SrVector3&v) const
	{
	return ((x < v.x)&&(y < v.y)&&(z < v.z));
	}

 
SR_INLINE bool SrVector3::operator==(const SrVector3& v) const
	{
	return ((x == v.x)&&(y == v.y)&&(z == v.z));
	}

 
SR_INLINE bool SrVector3::operator!=(const SrVector3& v) const
	{
	return ((x != v.x)||(y != v.y)||(z != v.z));
	}

//Methods
 
SR_INLINE void  SrVector3::set(const SrVector3 & v)
	{
	x = v.x;
	y = v.y;
	z = v.z;
	}

 
SR_INLINE void  SrVector3::setNegative(const SrVector3 & v)
	{
	x = -v.x;
	y = -v.y;
	z = -v.z;
	}

 
SR_INLINE void  SrVector3::setNegative()
	{
	x = -x;
	y = -y;
	z = -z;
	}


 
SR_INLINE void  SrVector3::set(const SrF32 * v)
	{
	x = (float)v[0];
	y = (float)v[1];
	z = (float)v[2];
	}

 
SR_INLINE void  SrVector3::set(const SrF64 * v)
	{
	x = (float)v[0];
	y = (float)v[1];
	z = (float)v[2];
	}


 
SR_INLINE void  SrVector3::set(float _x, float _y, float _z)
	{
	this->x = _x;
	this->y = _y;
	this->z = _z;
	}

 
SR_INLINE void SrVector3::set(float v)
	{
	x = v;
	y = v;
	z = v;
	}

 
SR_INLINE void  SrVector3::zero()
	{
	x = y = z = 0.0;
	}

 
SR_INLINE void  SrVector3::setPlusInfinity()
	{
	x = y = z = SR_MAX_F32; //TODO: this may be double too, but here we can't tell!
	}

 
SR_INLINE void  SrVector3::setMinusInfinity()
	{
	x = y = z = SR_MIN_F32; //TODO: this may be double too, but here we can't tell!
	}

 
SR_INLINE void SrVector3::max(const SrVector3 & v)
	{
		x = x > v.x?x:v.x;
		y = y > v.y?y:v.y;
		z = z > v.z?z:v.z;
	}

 
SR_INLINE void SrVector3::min(const SrVector3 & v)
	{
		x = x > v.x?v.x:x;
		y = y > v.y?v.y:y;
		z = z > v.z?v.z:z;
	}




SR_INLINE void  SrVector3::add(const SrVector3 & a, const SrVector3 & b)
	{
	x = a.x + b.x;
	y = a.y + b.y;
	z = a.z + b.z;
	}


SR_INLINE void  SrVector3::subtract(const SrVector3 &a, const SrVector3 &b)
	{
	x = a.x - b.x;
	y = a.y - b.y;
	z = a.z - b.z;
	}


SR_INLINE void  SrVector3::arrayMultiply(const SrVector3 &a, const SrVector3 &b)
	{
	x = a.x * b.x;
	y = a.y * b.y;
	z = a.z * b.z;
	}


SR_INLINE void  SrVector3::multiply(float s,  const SrVector3 & a)
	{
	x = a.x * s;
	y = a.y * s;
	z = a.z * s;
	}


SR_INLINE void  SrVector3::multiplyAdd(float s, const SrVector3 & a, const SrVector3 & b)
	{
	x = s * a.x + b.x;
	y = s * a.y + b.y;
	z = s * a.z + b.z;
	}

 
SR_INLINE float SrVector3::normalize()
	{
	float m = magnitude();
	if (m)
		{
		const float il =  float(1.0) / m;
		x *= il;
		y *= il;
		z *= il;
		}
	return m;
	}

 
SR_INLINE void SrVector3::setMagnitude(float length)
	{
	float m = magnitude();
	if(m)
		{
		float newLength = length / m;
		x *= newLength;
		y *= newLength;
		z *= newLength;
		}
	}
SR_INLINE bool SrVector3::isFinite() const
	{
		return SrMath::isFinite(x) && SrMath::isFinite(y) && SrMath::isFinite(z);
	}
SR_INLINE float SrVector3::dot(const SrVector3 &v) const
	{
	return x * v.x + y * v.y + z * v.z;
	}

 
SR_INLINE bool SrVector3::sameDirection(const SrVector3 &v) const
	{
	return x*v.x + y*v.y + z*v.z >= 0.0f;
	}

 
SR_INLINE float SrVector3::magnitude() const
	{
	return SrMath::sqrt(x * x + y * y + z * z);
	}

 
SR_INLINE float SrVector3::magnitudeSquared() const
	{
	return x * x + y * y + z * z;
	}

 
SR_INLINE float SrVector3::distance(const SrVector3 & v) const
	{
	float dx = x - v.x;
	float dy = y - v.y;
	float dz = z - v.z;
	return SrMath::sqrt(dx * dx + dy * dy + dz * dz);
	}

 
SR_INLINE float SrVector3::distanceSquared(const SrVector3 &v) const
	{
	float dx = x - v.x;
	float dy = y - v.y;
	float dz = z - v.z;
	return dx * dx + dy * dy + dz * dz;
	}

 
SR_INLINE void SrVector3::cross(const SrVector3 &left, const SrVector3 & right)	//prefered version, w/o temp object.
	{
	// temps needed in case left or right is this.
	float a = (left.y * right.z) - (left.z * right.y);
	float b = (left.z * right.x) - (left.x * right.z);
	float c = (left.x * right.y) - (left.y * right.x);

	x = a;
	y = b;
	z = c;
	}

 
SR_INLINE bool SrVector3::equals(const SrVector3 & v, float epsilon) const
	{
	return 
		SrMath::equals(x, v.x, epsilon) &&
		SrMath::equals(y, v.y, epsilon) &&
		SrMath::equals(z, v.z, epsilon);
	}


 
SR_INLINE SrVector3 SrVector3::operator -() const
	{
	return SrVector3(-x, -y, -z);
	}

 
SR_INLINE SrVector3 SrVector3::operator +(const SrVector3 & v) const
	{
	return SrVector3(x + v.x, y + v.y, z + v.z);	// RVO version
	}

 
SR_INLINE SrVector3 SrVector3::operator -(const SrVector3 & v) const
	{
	return SrVector3(x - v.x, y - v.y, z - v.z);	// RVO version
	}



SR_INLINE SrVector3 SrVector3::operator *(float f) const
	{
	return SrVector3(x * f, y * f, z * f);	// RVO version
	}


SR_INLINE SrVector3 SrVector3::operator /(float f) const
	{
		f = float(1.0) / f; return SrVector3(x * f, y * f, z * f);
	}


SR_INLINE SrVector3& SrVector3::operator +=(const SrVector3& v)
	{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
	}


SR_INLINE SrVector3& SrVector3::operator -=(const SrVector3& v)
	{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
	}


SR_INLINE SrVector3& SrVector3::operator *=(float f)
	{
	x *= f;
	y *= f;
	z *= f;
	
	return *this;
	}


SR_INLINE SrVector3& SrVector3::operator /=(float f)
	{
	f = 1.0f/f;
	x *= f;
	y *= f;
	z *= f;
	
	return *this;
	}


SR_INLINE SrVector3 SrVector3::cross(const SrVector3& v) const
	{
	SrVector3 temp;
	temp.cross(*this,v);
	return temp;
	}


SR_INLINE SrVector3 SrVector3::operator^(const SrVector3& v) const
	{
	SrVector3 temp;
	temp.cross(*this,v);
	return temp;
	}


SR_INLINE float SrVector3::operator|(const SrVector3& v) const
	{
	return x * v.x + y * v.y + z * v.z;
	}

/**
scalar pre-multiplication
*/

SR_INLINE SrVector3 operator *(float f, const SrVector3& v)
	{
	return SrVector3(f * v.x, f * v.y, f * v.z);
	}


/** @} */
#endif
