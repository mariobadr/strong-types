#ifndef STRONG_STRONG_HPP
#define STRONG_STRONG_HPP

#include <type_traits>
#include <utility>

#ifdef STRONG_USE_STL_STREAMS
#include <iosfwd>
#endif

/**
 * Namespace for creating strong typedefs.
 */
namespace strong {

/**
 * A strong typedef wrapper around some Type.
 *
 * Allows the programmer to define a strong type to avoid comparing values that have the same type
 * but are logically different. For example, IDs in a geographic map application for intersections
 * and streets may both be integers, but one should not logically compare an intersection ID with
 * a street ID.
 *
 * Strong typedefs that inherit from this class can be explicitly converted to their base Type,
 * but not implicitly converted. For more information please see:
 * http://en.cppreference.com/w/cpp/language/cast_operator
 *
 * @tparam TypeName A unique identifier for this type
 * @tparam Type The underlying type (e.g. int) to use
 */
template<class TypeName, typename Type>
class type {
public:
  /**
   * The default constructor of type will attempt to initialize the underlying value with its own
   * default constructor.
   */
  constexpr type() : value()
  {
  }

  /**
   * Initialize the underlying value via a copy or move.
   *
   * @param v The value to copy.
   */
  explicit constexpr type(Type const &v) : value(v)
  {
  }

  /**
   * Move constructor.
   *
   * If Type's move constructor does not throw, then this move constructor will have noexcept
   * enabled, which will allow for certain optimizations. For more information, please refer
   * to https://akrzemi1.wordpress.com/2014/04/24/noexcept-what-for/
   *
   * @param v The value to move.
   */
  explicit constexpr type(Type && v) noexcept(std::is_nothrow_move_constructible<Type>::value)
    : value(static_cast<Type &&>(v))
  {
  }

  /**
   * Enables explicit conversion of the type.
   *
   * @return The underlying value.
   */
  explicit operator Type &() noexcept
  {
    return value;
  }

  /**
   * Enables const-correct explicit conversion of the type.
   *
   * @return The underlying value.
   */
  explicit constexpr operator Type const &() const noexcept
  {
    return value;
  }
private:
  Type value;
};

/**
 * Access the underlying value of a mutable strong type.
 *
 * @tparam TypeName The name of the strong typedef
 * @tparam Type The underlying type of the strong typedef
 * @param object The instance of the strong type
 * @return A reference to the underlying value
 */
template<class TypeName, typename Type>
constexpr Type & get(type<TypeName, Type> &object) noexcept
{
  return static_cast<Type &>(object);
};

/**
 * Access the underlying value of an immutable strong type.
 *
 * @tparam TypeName The name of the strong typedef
 * @tparam Type The underlying type of the strong typedef
 * @param object The instance of the strong type
 * @return A reference to the underlying value
 */
template<class TypeName, typename Type>
constexpr Type const & get(type<TypeName, Type> const &object) noexcept
{
  return static_cast<Type const &>(object);
};

/**
 * Operations to enable on strong typedefs.
 */
namespace op {

/**
 * Enables comparisons for equality or inequality between identical strong types.
 *
 * @tparam TypeName The strong typedef to compare.
 * @tparam Result The return type of the comparison
 */
template<class TypeName, typename Result = bool>
class equals {
public:
  /**
   * Test for equality.
   *
   * @param lhs The left-hand side of the relational expression.
   * @param rhs The right-hand side of the relational expression.
   * @return The result of the comparison.
   */
  friend constexpr Result operator==(TypeName const &lhs, TypeName const &rhs)
  {
    return get(lhs) == get(rhs);
  }

  /**
   * Test for inequality.
   *
   * @param lhs The left-hand side of the relational expression.
   * @param rhs The right-hand side of the relational expression.
   * @return The result of the comparison.
   */
  friend constexpr Result operator!=(TypeName const &lhs, TypeName const &rhs)
  {
    return !(lhs == rhs);
  }
};

/**
 * Enables comparisons to be made using <, <=, >, and >= operators between identical strong types.
 *
 * @tparam TypeName The strong typedef to compare.
 * @tparam Result The return type of the comparison
 */
template<class TypeName, typename Result = bool>
class orders {
public:
  /**
   * Test that the left-hand side is less than the right-hand side.
   *
   * @param lhs The left-hand side of the relational expression.
   * @param rhs The right-hand side of the relational expression.
   * @return The result of the comparison.
   */
  friend constexpr Result operator<(TypeName const &lhs, TypeName const &rhs)
  {
    return get(lhs) < get(rhs);
  }

  /**
   * Test that the left-hand side is less than or equal to the right-hand side.
   *
   * @param lhs The left-hand side of the relational expression.
   * @param rhs The right-hand side of the relational expression.
   * @return The result of the comparison.
   */
  friend constexpr Result operator<=(TypeName const &lhs, TypeName const &rhs)
  {
    return !(rhs < lhs);
  }

  /**
   * Test that the left-hand side is greater than the right-hand side.
   *
   * @param lhs The left-hand side of the relational expression.
   * @param rhs The right-hand side of the relational expression.
   * @return The result of the comparison.
   */
  friend constexpr Result operator>(TypeName const &lhs, TypeName const &rhs)
  {
    return get(lhs) > get(rhs);
  }

  /**
   * Test that the left-hand side is greater than or equal to the right-hand side.
   *
   * @param lhs The left-hand side of the relational expression.
   * @param rhs The right-hand side of the relational expression.
   * @return The result of the comparison.
   */
  friend constexpr Result operator>=(TypeName const &lhs, TypeName const &rhs)
  {
    return !(rhs > lhs);
  }
};

/**
 * Enables the addition of identical strong types.
 *
 * @tparam TypeName The strong typedef to add.
 */
template<class TypeName>
class adds {
public:
  /**
   * Add two strong types and return the result.
   *
   * @param lhs The left-hand side of the expression
   * @param rhs The right-hand side of the expression
   * @return The sum of the left- and right-hand side
   */
  friend constexpr TypeName operator+(TypeName const &lhs, TypeName const &rhs)
  {
    return TypeName(get(lhs) + get(rhs));
  }

  /**
   * Add the right-hand side to the left-hand side and store the result in the left-hand side.
   *
   * @param lhs The left-hand side of the expression
   * @param rhs The right-hand side of the expression
   * @return A reference to the summation, which is the left-hand side
   */
  friend TypeName & operator+=(TypeName &lhs, TypeName const &rhs)
  {
    get(lhs) += get(rhs);
    return lhs;
  }
};

/**
 * Enables the subtraction of identical strong types.
 *
 * @tparam TypeName The strong typedef to subtract.
 */
template<class TypeName>
class subtracts {
public:
  /**
   * Subtract two strong types and return the result.
   *
   * @param lhs The left-hand side of the expression
   * @param rhs The right-hand side of the expression
   * @return The difference of the left- and right-hand side
   */
  friend constexpr TypeName operator-(TypeName const &lhs, TypeName const &rhs)
  {
    return TypeName(get(lhs) - get(rhs));
  }

  /**
   * Subtract the right-hand side to the left-hand side and store the result in the left-hand side.
   *
   * @param lhs The left-hand side of the expression
   * @param rhs The right-hand side of the expression
   * @return A reference to the difference, which is the left-hand side
   */
  friend TypeName & operator-=(TypeName &lhs, TypeName const &rhs)
  {
    get(lhs) -= get(rhs);
    return lhs;
  }
};

/**
 * Enables the multiplication of identical strong types.
 *
 * @tparam TypeName The strong typedef to multiply.
 */
template<class TypeName>
class multiplies {
public:
  /**
   * Multiply two strong types and return the result.
   *
   * @param lhs The left-hand side of the expression
   * @param rhs The right-hand side of the expression
   * @return The product of the left- and right-hand side
   */
  friend constexpr TypeName operator*(TypeName const &lhs, TypeName const &rhs)
  {
    return TypeName(get(lhs) * get(rhs));
  }

  /**
   * Multiply the right- and left-hand side and store the result in the left-hand side.
   *
   * @param lhs The left-hand side of the expression
   * @param rhs The right-hand side of the expression
   * @return A reference to the product, which is the left-hand side
   */
  friend TypeName & operator*=(TypeName &lhs, TypeName const &rhs)
  {
    get(lhs) *= get(rhs);
    return lhs;
  }
};

/**
 * Enables the division of identical strong types.
 *
 * @tparam TypeName The strong typedef to divide.
 */
template<class TypeName>
class divides {
public:
  /**
   * Divide two strong types and return the result.
   *
   * @param lhs The left-hand side of the expression
   * @param rhs The right-hand side of the expression
   * @return The division of the left- and right-hand side
   */
  friend constexpr TypeName operator/(TypeName const &lhs, TypeName const &rhs)
  {
    return TypeName(get(lhs) / get(rhs));
  }

  /**
   * Divide the right- and left-hand side and store the result in the left-hand side.
   *
   * @param lhs The left-hand side of the expression
   * @param rhs The right-hand side of the expression
   * @return A reference to the division, which is the left-hand side
   */
  friend TypeName & operator/=(TypeName &lhs, TypeName const &rhs)
  {
    get(lhs) /= get(rhs);
    return lhs;
  }
};

/**
 * Enables the modulo of identical strong types.
 *
 * @tparam TypeName The strong typedef to apply the modulo operator to.
 */
template<class TypeName>
class modulo {
public:
  /**
   * Find the remainder after division of two strong types and return the result.
   *
   * @param lhs The left-hand side of the expression
   * @param rhs The right-hand side of the expression
   * @return The remainder after division of the left- with the right-hand side
   */
  friend constexpr TypeName operator%(TypeName const &lhs, TypeName const &rhs)
  {
    return TypeName(get(lhs) % get(rhs));
  }
};

/**
 * Enables the pre- and post-fix increment operators for TypeName.
 *
 * @tparam TypeName The strong typedef to increment
 */
template<class TypeName>
class increments {
public:
  /**
   * Increment the object (pre-fix).
   *
   * @return The object incremented by one.
   */
  TypeName & operator++()
  {
    // this is of type increment, which should also be of type TypeName (so we cast it)
    auto & object = static_cast<TypeName &>(*this);
    ++get(object);
    return object;
  }

  /**
   * Increment the object (post-fix).
   *
   * @return The object incremented by one.
   */
  TypeName operator++(int)
  {
    // reuse the pre-fix increment implementation
    ++(*this);

    // this is of type increment, which should also be of type TypeName (so we cast it)
    auto & object = static_cast<TypeName &>(*this);
    return object;
  }
};

/**
 * Enables the pre- and post-fix decrement operators for TypeName.
 *
 * @tparam TypeName The strong typedef to decrement
 */
template<class TypeName>
class decrements {
public:
  /**
   * Decrement the object (pre-fix).
   *
   * @return The object decremented by one.
   */
  TypeName & operator--()
  {
    // this is of type decrement, which should also be of type TypeName (so we cast it)
    auto & object = static_cast<TypeName &>(*this);
    --get(object);
    return object;
  }

  /**
   * Decrement the object (post-fix).
   *
   * @return The object decremented by one.
   */
  TypeName operator--(int)
  {
    // reuse the pre-fix increment implementation
    --(*this);

    // this is of type decrement, which should also be of type TypeName (so we cast it)
    auto & object = static_cast<TypeName &>(*this);
    return object;
  }
};

#ifdef STRONG_USE_STL_STREAMS

/**
 * Enables the output stream operator for the strong type.
 *
 * @tparam TypeName The name of the strong type.
 */
template<class TypeName>
class outputs {
public:
  /**
   * Output the value to the stream.
   *
   * @param stream The stream to modify.
   * @param value The value to output.
   * @return The modified stream.
   */
  friend std::ostream & operator<<(std::ostream & stream, TypeName const &value)
  {
    stream << get(value);
    return stream;
  }
};

/**
 * Enables the input stream operator for the strong type.
 *
 * @tparam TypeName The name of the strong type.
 */
template<class TypeName>
class inputs {
public:
  /**
   * Extract a value from the stream and store it in the variable.
   *
   * @param stream The stream to extract from.
   * @param value Stores the extracted value.
   * @return The modified stream post-extraction.
   */
  friend std::istream & operator>>(std::istream & stream, TypeName &value)
  {
    return stream >> get(value);
  }
};

#endif
}

}

#endif //STRONG_STRONG_HPP
