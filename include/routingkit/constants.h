#ifndef ROUTING_KIT_CONSTANTS_H
#define ROUTING_KIT_CONSTANTS_H

/**
* RoutingKit is the namespace for the entire project
*/
namespace RoutingKit{

const unsigned invalid_id = 4294967295u; //field that keeps the invalid id
const unsigned inf_weight = 2147483647u; //field that keeps the inf weight

static_assert(inf_weight + inf_weight > inf_weight, "inf_weight is too large");
static_assert(sizeof(unsigned)==4, "code base assumes that unsigned is 32 bit wide");

} // namespace RoutingKit

#endif
