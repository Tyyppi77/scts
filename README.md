# scts

scts (Simple C++ Template Serializer) is a C++17 library that allows easy serialization of C++ objects.

Currently the library supports serialization to and from JSON.

## Usage Example

The following is a basic example of the features currently in scts. You can define objects to be serialized. Nested object types and inheritance is supported. Serialization takes in a type parameter that defines the format you want to serialize to.

```cpp
#include "scts.h"

struct Vector3 {
	float x, y, z;
};

template <> struct scts::register_type<Vector3> : scts::allow_serialization {
	static constexpr scts::object_descriptor<Vector3,
		scts::members<
		scts::member<&Vector3::x>,
		scts::member<&Vector3::y>,
		scts::member<&Vector3::z>>> descriptor{ "x", "y", "z" };
};

struct Entity {
	Vector3 position;
};

template <> struct scts::register_type<Entity> : scts::allow_serialization {
	static constexpr scts::object_descriptor<Entity,
		scts::members<scts::member<&Entity::position>>> descriptor{ "position" };
};

struct Player : Entity {
	float health;
};

template <> struct scts::register_type<Player> : scts::allow_serialization {
	static constexpr scts::object_descriptor<Player,
		scts::members<
		scts::member<&Player::health>>,
		scts::inherits_from<Entity>> descriptor{ "health" };
};

int main() {
	Player p;
	auto serialized = scts::serialize<Player, scts::json_formatter>(p);  // Serialize to JSON.
}
```
