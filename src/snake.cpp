#include "snake.hpp"
#include "world.hpp"
#include "declarations.hpp"
#include "util/random.hpp"

#include <map>
#include <thread>
#include <chrono>

void Snake::_Action1(){
    this->eatFood();    // @note - Ignoring returned boolean from eatFood(), if that's required use eatFood() directly, this method is just for the sake for generalisation using Entity
}

void Snake::_Action2(){
    this->moveForward();
}

const std::optional<coord&> Snake::getPrimaryPos() const{
    return this->getHead();
}

void Snake::simulateExistence(){
    // @future -> Can add more logic here, when more interaction options between entities are added

    // a one time check, before an entity starts existing
    if( std::this_thread::get_id() == this->parent_world->_shared_concurrent_data.get_world_thread_id() ){
        throw std::logic_error( "Entities should be on a different thread, than the world. In case, you don't want this behaviour, then pass -DNO_THREAD_ENTITY (To define NO_THREAD_ENTITY)" );
    }

    while ( this->parent_world->_shared_concurrent_data.is_world_running() ){   //while the parent world continues to exist keep the entity moving
        this->moveForward();

        std::this_thread::sleep_for( std::chrono::milliseconds( (int)statics::UNIT_TIME * 1000 ) );
    }
}

bool Snake::eatFood(){  //can only eat, if AT the position
    if( this->head->getCoords() == this->parent_world->get_food_coords()){
        this->parent_world->ateFood(this);
        ++ this->length;
        return true;
    }
    return false;
}

bool Snake::hasRoundTrips() const{
        // since array calls the initializer for its object type, so it is `value initialised IN THIS CASE`, ie 0
    std::array<int, 5>& temp_bucket{ this->__temp.bucket };
    std::fill(temp_bucket.begin(), temp_bucket.end(), 0); // in case bucket has been used earlier

    for each (auto dir in this->body) {
        temp_bucket[static_cast<uint8_t>(dir)] += static_cast<uint8_t>(dir) > 4 ? -1 : 1;
        // @warning - `dir` should be static cast only to the type given as the type of values of enum class Direction
    }

    return std::all_of(temp_bucket.begin(), temp_bucket.end(), [](int i) {return i == 0;});
}

bool Snake::isPathValid() const{    // checks that `no two adjacent` directions should be opposites
    auto& p = this->body;

    return std::adjacent_find(p.begin(), p.end(), &util::areDirectionsOpposite) == p.end();
}

bool Snake::isSnakeBodyOK() const{
    return isPathValid() && !hasRoundTrips();
}

//Returns if `this` snake has ate the food or not
bool Snake::moveForward(){  // this will also be on the snake's thread, and not the world_thread

    if( this->curr_Path.empty() ){
        if( this->eatFood() ){
            return true;
        }
    }

    this->parent_world->getShortestPathToFood(this->head, this->curr_Path);

    this->head.graph_box = this->head.graph_box->get_adj_box(curr_Path.front());
    this->_add_dir_to_coord( this->head.point_coord, curr_Path.front());

//    this->body.push_back( curr_Path.insert )
    // move with path.front()
    // @todo

}

void Snake::_add_dir_to_coord(coord& c, Direction dir) const
{
//    static const std::map<Direction, coord> dir_to_incrementing_coord{ {
//        {Direction::ADHARASTHA, {0,0,-1}},
//        {Direction::AGNEYA, {1,-1,0}},
//        {Direction::DAKSHIN, {}},
//        {},
//        {},
//        {},
//        {},
//        {},
//
//    }};

    switch (dir)
    {
    case Direction::UTTAR:
        c += std::array{0, 1, 0};
        break;
    case Direction::PURVA:
        c += std::array{1, 0, 0};
        break;
    case Direction::PASHCHIM:
        c += std::array{-1, 0, 0};
        break;
    case Direction::DAKSHIN:
        c += std::array{0, -1, 0};
        break;
    case Direction::ISHANYA:
        c += std::array{1, 1, 0};
        break;
    case Direction::AGNEYA:
        c += std::array{1, -1, 0};
        break;
    case Direction::NAIRUTYA:
        c += std::array{-1, -1, 0};
        break;
    case Direction::VAYAVYA:
        c += std::array{-1, 1, 0};
        break;
    case Direction::URDHWA:
        c += std::array{0, 0, 1};
        break;
    case Direction::ADHARASTHA:
        c += std::array{0, 0, -1};
        break;
    }
}

const Entity_Point& Snake::getHead() const{
    return this->head;
}

int Snake::getUniqProp() const{
    return this->length;
}

Snake::Snake(const World_Ptr world) : Snake(world, world->_init_SnakeLength){}

/*
@bug/enhancement alert - 
    As per the current code, let's consider a situation,
    Let one snake already on board, eg. the line x=4;

    BUT, what if the snake formed is such that, it is y = 3 (or anything);
    THOUGH, isCellEmpty() will return false, but the actual problem is
    "The next move this new snake moves, will cause the new snake to `collide` with the older one, thereby ending its life in starting itself"
*/

Snake::Snake(const World_Ptr world, uint16_t init_len) : Entity(entity_Types::SNAKE), parent_world(world){
    if (!world) {
        throw std::invalid_argument("World Pointer to Snake Type must be not null");
    }

    coord head_pos = {
        util::Random::random<dimen_t>(parent_world->get_curr_bound()),
        util::Random::random<dimen_t>(parent_world->get_curr_bound()),
        util::Random::random<dimen_t>(parent_world->get_curr_bound())
    };

    this->head = { parent_world->get_box(head_pos), head_pos };

    Direction rand_direction;
    while( !this->parent_world->isCellEmpty(this->head.graph_box) ){  // loop until you chose an empty box as the head
        rand_direction = statics::directions[util::Random::random(statics::directions.size())];
        this->_add_dir_to_coord(this->head.point_coord, rand_direction);
        this->head.reset(
            this->head.graph_box->get_adj_box(rand_direction),
            head.point_coord    /*modified above using _add_dir_to_coord() */
        );    // @warning- randomly assigning any number from 0 to 3
    }

    do {
        auto* prev_box = this->head.graph_box;
        for (uint16_t i = 0; i < init_len - 1; i++) {
            rand_direction = statics::directions[util::Random::random(statics::directions.size())];

            while (!prev_box->getData().hasEntities()) {
                prev_box = prev_box->get_adj_box(rand_direction);
                rand_direction = statics::directions[util::Random::random(4)];
            }

            prev_box = prev_box->get_adj_box(rand_direction);
            this->body.push_back(rand_direction);
        }

        this->length = init_len;
    } while ( !isSnakeBodyOK() );
}
