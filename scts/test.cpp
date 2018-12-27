#include <iostream>
#include <vector>
#include <array>
#include <map>

#include "scts.h"

struct Vector2 {
	int X = 0, Y = 0;
};

template <> struct scts::register_type<Vector2> : scts::allow_serialization {
	static constexpr scts::object_descriptor<Vector2,
		scts::members<
		scts::member<&Vector2::X>,
		scts::member<&Vector2::Y>>> descriptor{ "X", "Y" };
};

struct Collider {
	Vector2 Position{ 10, -5 };
	Vector2 Size{ 12, 30000 };
};

template <> struct scts::register_type<Collider> : scts::allow_serialization {
	static constexpr scts::object_descriptor<Collider,
		scts::members<
		scts::member<&Collider::Position>,
		scts::member<&Collider::Size>>> descriptor{ "Position", "Size" };
};

struct Entity {
	Collider Box;
	int Health = 100;
};

template <> struct scts::register_type<Entity> : scts::allow_serialization {
	static constexpr scts::object_descriptor<Entity,
		scts::members<
		scts::member<&Entity::Box>,
		scts::member<&Entity::Health>>> descriptor{ "Box", "Health" };
};

struct Player : Entity {
	float Damage = 10.23f;
	std::string Name = "Dave";
	std::vector<Vector2> LastPositions{ 5 };
	std::array<bool, 3> CoolStates{ false, true, true };
	std::map<std::string, float> SpeedSettings{
		std::pair{"Fast", 2.5f},
		std::pair{"Normal", 1.0f},
		std::pair{"Sloow", 1.0f / 3.0f}
	};
};

template <> struct scts::register_type<Player> : scts::allow_serialization {
	static constexpr scts::object_descriptor<Player,
		scts::members<
		scts::member<&Player::Damage>,
		scts::member<&Player::Name>,
		scts::member<&Player::LastPositions>,
		scts::member<&Player::CoolStates>,
		scts::member<&Player::SpeedSettings>>,
		scts::inherits_from<Entity>> descriptor{ "Damage", "Name", "LastPositions", "CoolStates", "SpeedSettings" };
};

int main()
{
	// TODO: Loading assumes order!

	scts::out_stream s;
	{
		Player e{
			Collider{ Vector2{17, 40}, Vector2{1405, 70043} },
			4634,
			69350.0f,
			"Jeff",
			{ Vector2{325, 2350}, Vector2{-1000, -100403200} }
		};
		e.CoolStates[0] = e.CoolStates[1] = e.CoolStates[2] = false;
		e.SpeedSettings.clear();
		e.SpeedSettings["Stuff"] = 15.0f;
		scts::serialize(e, s);
		std::cout << "Initial Data:\n";
		std::cout << s.str() << "\n";
	}
	Player n;
	{
		auto serialized = s.get_in_stream();
		scts::deserialize(n, serialized);
	}
	scts::out_stream another;
	std::cout << "After Load:\n";
	std::cout << scts::serialize(n, another).str();
	int i;
	std::cin >> i;
}
