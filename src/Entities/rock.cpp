#include "Entities/rock.hpp"
#include "world.hpp"

Rock::Rock(World_Ptr world) : Rock(world, world->world_plot.getRandomCoord()) {}

Rock::Rock(World_Ptr world, const coord& location): Entity(Entity_Types::ROCK), location(world->world_plot[location], location) {
#ifdef DEBUG
	std::cout << "Rock placed at location: " << this->location.point_coord << "\n";
#endif

	assert(this->location.graph_box != nullptr);	// ie. we received valid coordinate, AND that coordinate is still in matrix

	this->location.graph_box->getDataRef().addEntity(this);
}
