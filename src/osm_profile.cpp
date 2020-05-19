#include <routingkit/osm_profile.h>

namespace RoutingKit{

namespace{
	bool str_eq(const char*l, const char*r){
		return !strcmp(l, r);
	}

	bool str_wild_char_eq(const char*l, const char*r){
		while(*l != '\0' && *r != '\0'){
			if(*l != '?' && *r != '?' && *l != *r)
				return false;
			++l;
			++r;
		}
		return *l == '\0' && *r == '\0';
	}

	bool starts_with(const char*prefix, const char*str){
		while(*prefix != '\0' && *str == *prefix){
			++prefix;
			++str;
		}
		return *prefix == '\0';
	}


	void copy_str_and_make_lower_case(const char*in, char*out, unsigned out_size){
		char*out_end = out + out_size-1;
		while(*in && out != out_end){
			if('A' <= *in && *in <= 'Z')
				*out = *in - 'A' + 'a';
			else
				*out = *in;
			++in;
			++out;
		}
		*out = '\0';
	}

	// Splits the string at some separators such as ; and calls f(str) for each part.
	// The ; is replaced by a '\0'. Leading spaces are removed
	template<class F>
	void split_str_at_osm_value_separators(char*in, const F&f){
		while(*in == ' ')
			++in;
		const char*value_begin = in;
		for(;;){
			while(*in != '\0' && *in != ';')
				++in;
			if(*in == '\0'){
				f(value_begin);
				return;
			}else{
				*in = '\0';
				f(value_begin);
				++in;
				while(*in == ' ')
					++in;
				value_begin = in;
			}
		}
	}
}

bool is_osm_way_used_by_pedestrians(uint64_t osm_way_id, const TagMap&tags, std::function<void(const std::string&)>log_message){
	const char* junction = tags["junction"];
	if(junction != nullptr)
		return true;

	const char* route = tags["route"];
	if(route && str_eq(route, "ferry"))
		return true;

	const char* ferry = tags["ferry"];
	if(ferry && str_eq(ferry, "ferry"))
		return true;

	const char* public_transport = tags["public_transport"];
	if(public_transport != nullptr &&
	   (str_eq(public_transport, "stop_position") ||
	    str_eq(public_transport, "platform") ||
	    str_eq(public_transport, "stop_area") ||
	    str_eq(public_transport, "station")
	   )
	  ) {
		return true;
	}

	const char* railway = tags["railway"];
	if(railway != nullptr &&
	   (str_eq(railway, "halt") ||
	    str_eq(railway, "platform") ||
	    str_eq(railway, "subway_entrance") ||
	    str_eq(railway, "station") ||
	    str_eq(railway, "tram_stop")
	   )
	  ) {
		return true;
	}

	const char* highway = tags["highway"];
	if(highway == nullptr)
		return false;

	const char*access = tags["access"];
	if(access){
		if(
			!str_eq(access, "yes") &&
			!str_eq(access, "permissive") &&
			!str_eq(access, "delivery") &&
			!str_eq(access, "designated") &&
			!str_eq(access, "destination") &&
			!str_eq(access, "agricultural") &&
			!str_eq(access, "forestry") &&
			!str_eq(access, "public")
		){
			return false;
		}
	}

	const char* crossing = tags["crossing"];
	if(crossing != nullptr && str_eq(crossing, "no"))
		return false;

	if(
		str_eq(highway, "secondary") ||
		str_eq(highway, "tertiary") ||
		str_eq(highway, "unclassified") ||
		str_eq(highway, "residential") ||
		str_eq(highway, "service") ||
		str_eq(highway, "secondary_link") ||
		str_eq(highway, "tertiary_link") ||
		str_eq(highway, "living_street") ||
		str_eq(highway, "residential") ||
		str_eq(highway, "track") ||
		str_eq(highway, "bicycle_road") ||
		str_eq(highway, "path") ||
		str_eq(highway, "footway") ||
		str_eq(highway, "cycleway") ||
		str_eq(highway, "bridleway") ||
		str_eq(highway, "pedestrian") ||
		str_eq(highway, "escape") ||
		str_eq(highway, "steps") ||
		str_eq(highway, "crossing") ||
		str_eq(highway, "escalator") ||
		str_eq(highway, "elevator") ||
		str_eq(highway, "platform") ||
		str_eq(highway, "ferry")
	)
		return true;


	if(
		str_eq(highway, "motorway") ||
		str_eq(highway, "motorway_link") ||
		str_eq(highway, "motorway_junction") ||
		str_eq(highway, "trunk") ||
		str_eq(highway, "trunk_link") ||
		str_eq(highway, "primary") ||
		str_eq(highway, "primary_link") ||
		str_eq(highway, "construction") ||
		str_eq(highway, "bus_guideway") ||
		str_eq(highway, "raceway") ||
		str_eq(highway, "proposed") ||
		str_eq(highway, "conveying")
	)
		return false;

	return false;
}

bool is_osm_way_used_by_cars(uint64_t osm_way_id, const TagMap&tags, std::function<void(const std::string&)>log_message){
	const char* junction = tags["junction"];
	if(junction != nullptr)
		return true;

	const char* route = tags["route"];
	if(route && str_eq(route, "ferry"))
		return true;

	const char* ferry = tags["ferry"];
	if(ferry && str_eq(ferry, "yes"))
		return true;

	const char* highway = tags["highway"];
	if(highway == nullptr)
		return false;

	const char*motorcar = tags["motorcar"];
	if(motorcar && str_eq(motorcar, "no"))
		return false;

	const char*motor_vehicle = tags["motor_vehicle"];
	if(motor_vehicle && str_eq(motor_vehicle, "no"))
		return false;

	const char*access = tags["access"];
	if(access){
		if(!(str_eq(access, "yes") || str_eq(access, "permissive") || str_eq(access, "delivery")|| str_eq(access, "designated") || str_eq(access, "destination")))
			return false;
	}

	if(
		str_eq(highway, "motorway") ||
		str_eq(highway, "trunk") ||
		str_eq(highway, "primary") ||
		str_eq(highway, "secondary") ||
		str_eq(highway, "tertiary") ||
		str_eq(highway, "unclassified") ||
		str_eq(highway, "residential") ||
		str_eq(highway, "service") ||
		str_eq(highway, "motorway_link") ||
		str_eq(highway, "trunk_link") ||
		str_eq(highway, "primary_link") ||
		str_eq(highway, "secondary_link") ||
		str_eq(highway, "tertiary_link") ||
		str_eq(highway, "motorway_junction") ||
		str_eq(highway, "living_street") ||
		str_eq(highway, "residential") ||
		str_eq(highway, "track") ||
		str_eq(highway, "ferry")
	)
		return true;

	if(str_eq(highway, "bicycle_road")){
		auto motorcar = tags["motorcar"];
		if(motorcar != nullptr)
			if(str_eq(motorcar, "yes"))
				return true;
		return false;
	}

	if(
		str_eq(highway, "construction") ||
		str_eq(highway, "path") ||
		str_eq(highway, "footway") ||
		str_eq(highway, "cycleway") ||
		str_eq(highway, "bridleway") ||
		str_eq(highway, "pedestrian") ||
		str_eq(highway, "bus_guideway") ||
		str_eq(highway, "raceway") ||
		str_eq(highway, "escape") ||
		str_eq(highway, "steps") ||
		str_eq(highway, "proposed") ||
		str_eq(highway, "conveying")
	)
		return false;

	const char* oneway = tags["oneway"];
	if(oneway != nullptr){
		if(str_eq(oneway, "reversible") || str_eq(oneway, "alternating")) {
			return false;
		}
	}

	const char* maxspeed = tags["maxspeed"];
	if(maxspeed != nullptr)
		return true;

	return false;
}


OSMWayDirectionCategory get_osm_car_direction_category(uint64_t osm_way_id, const TagMap&tags, std::function<void(const std::string&)>log_message){
	const char
		*oneway = tags["oneway"],
		*junction = tags["junction"],
		*highway = tags["highway"]
	;
	if(oneway != nullptr){
		if(str_eq(oneway, "-1") || str_eq(oneway, "reverse") || str_eq(oneway, "backward")) {
			return OSMWayDirectionCategory::only_open_backwards;
		} else if(str_eq(oneway, "yes") || str_eq(oneway, "true") || str_eq(oneway, "1")) {
			return OSMWayDirectionCategory::only_open_forwards;
		} else if(str_eq(oneway, "no") || str_eq(oneway, "false") || str_eq(oneway, "0")) {
			return OSMWayDirectionCategory::open_in_both;
		} else if(str_eq(oneway, "reversible") || str_eq(oneway, "alternating")) {
			return OSMWayDirectionCategory::closed;
		} else {
			if(log_message)
				log_message("Warning: OSM way "+std::to_string(osm_way_id)+" has unknown oneway tag value \""+oneway+"\" for \"oneway\". Way is closed.");
		}
	} else if(junction != nullptr && str_eq(junction, "roundabout")) {
		return OSMWayDirectionCategory::only_open_forwards;
	} else if(highway != nullptr && (str_eq(highway, "motorway") || str_eq(highway, "motorway_link"))) {
		return OSMWayDirectionCategory::only_open_forwards;
	}
	return OSMWayDirectionCategory::open_in_both;
}

namespace{
	unsigned parse_maxspeed_value(uint64_t osm_way_id, const char*maxspeed, std::function<void(const std::string&)>log_message){
		if(str_eq(maxspeed, "signals") || str_eq(maxspeed, "variable"))
			return inf_weight;

		if(str_eq(maxspeed, "none") || str_eq(maxspeed, "unlimited"))
			return 130;

		if(str_eq(maxspeed, "walk") || str_eq(maxspeed, "foot") || str_wild_char_eq(maxspeed, "??:walk"))
			return 5;

		if(str_wild_char_eq(maxspeed, "??:urban") || str_eq(maxspeed, "urban"))
			return 40;

		if(str_wild_char_eq(maxspeed, "??:living_street") || str_eq(maxspeed, "living_street"))
			return 10;

		if(str_eq(maxspeed, "de:rural") || str_eq(maxspeed, "at:rural") || str_eq(maxspeed, "ro:rural") || str_eq(maxspeed, "rural"))
			return 100;
		if(str_eq(maxspeed, "ru:rural") || str_eq(maxspeed, "ua:rural"))
			return 90;

		if(str_eq(maxspeed, "ru:motorway"))
			return 110;
		if(str_eq(maxspeed, "at:motorway") || str_eq(maxspeed, "ro:motorway"))
			return 130;

		if(str_eq(maxspeed, "national"))
			return 100;

		if(str_eq(maxspeed, "ro:trunk"))
			return 100;
		if(str_eq(maxspeed, "dk:rural") || str_eq(maxspeed, "ch:rural") || str_eq(maxspeed, "fr:rural"))
			return 80;
		if(str_eq(maxspeed, "it:rural") || str_eq(maxspeed, "hu:rural"))
			return 90;
		if(str_eq(maxspeed, "de:zone:30") || str_eq(maxspeed, "de:zone30"))
			return 30;


		if('0' <= *maxspeed && *maxspeed <= '9'){
			unsigned speed = 0;
			while('0' <= *maxspeed && *maxspeed <= '9'){
				speed *= 10;
				speed += *maxspeed - '0';
				++maxspeed;
			}
			while(*maxspeed == ' ')
				++maxspeed;
			if(*maxspeed == '\0' || str_eq(maxspeed, "km/h") || str_eq(maxspeed, "kmh") || str_eq(maxspeed, "kph")){
				return speed;
			}else if(str_eq(maxspeed, "mph")){
				return speed * 1609 / 1000;
			}else if(str_eq(maxspeed, "knots")){
				return speed * 1852 / 1000;
			}else{
				if(log_message)
					log_message("Warning: OSM way "+std::to_string(osm_way_id) +" has an unknown unit \""+maxspeed+"\" for its \"maxspeed\" tag -> assuming \"km/h\".");
				return speed;
			}
		}else{
			if(log_message)
				log_message("Warning: OSM way "+std::to_string(osm_way_id) +" has an unrecognized value of \""+maxspeed+"\" for its \"maxspeed\" tag.");
		}

		return inf_weight;
	}
}

unsigned get_osm_way_speed(uint64_t osm_way_id, const TagMap&tags, std::function<void(const std::string&)>log_message){
	auto maxspeed = tags["maxspeed"];
	if(maxspeed != nullptr && !str_eq(maxspeed, "unposted")){
		char lower_case_maxspeed[1024];
		copy_str_and_make_lower_case(maxspeed, lower_case_maxspeed, sizeof(lower_case_maxspeed)-1);

		unsigned speed = inf_weight;

		split_str_at_osm_value_separators(
			lower_case_maxspeed,
			 [&](const char*maxspeed){
				min_to(speed, parse_maxspeed_value(osm_way_id, maxspeed, log_message));
			}
		);

		if(speed == 0){
			speed = 1;
			if(log_message)
				log_message("Warning: OSM way "+std::to_string(osm_way_id)+" has speed 0 km/h, setting it to 1 km/h");
		}

		if(speed != inf_weight)
			return speed;

	}

	auto highway = tags["highway"];
	if(highway){
		if(str_eq(highway, "motorway"))
			return 90;
		if(str_eq(highway, "motorway_link"))
			return 45;
		if(str_eq(highway, "trunk"))
			return 85;
		if(str_eq(highway, "trunk_link"))
			return 40;
		if(str_eq(highway, "primary"))
			return 65;
		if(str_eq(highway, "primary_link"))
			return 30;
		if(str_eq(highway, "secondary"))
			return 55;
		if(str_eq(highway, "secondary_link"))
			return 25;
		if(str_eq(highway, "tertiary"))
			return 40;
		if(str_eq(highway, "tertiary_link"))
			return 20;
		if(str_eq(highway, "unclassified"))
			return 25;
		if(str_eq(highway, "residential"))
			return 25;
		if(str_eq(highway, "living_street"))
			return 10;
		if(str_eq(highway, "service"))
			return 8;
		if(str_eq(highway, "track"))
			return 8;
		if(str_eq(highway, "ferry"))
			return 5;
	}

	auto junction = tags["junction"];
	if(junction){
		return 20;
	}

	// TODO: a ferry may have a duration tag
	auto route = tags["route"];
	if(route && str_eq(route, "ferry")) {
		return 5;
	}

	auto ferry = tags["ferry"];
	if(ferry) {
		return 5;
	}

	if(maxspeed && highway && log_message)
		log_message("Warning: OSM way "+std::to_string(osm_way_id) +" has an unrecognized \"maxspeed\" tag of \""+maxspeed+"\" and an unrecognized \"highway\" tag of \""+highway+"\" and an no junction tag -> assuming 50km/h.");
	if(!maxspeed && highway && log_message)
		log_message("Warning: OSM way "+std::to_string(osm_way_id) +" has no \"maxspeed\" and an unrecognized \"highway\" tag of \""+highway+"\" and an no junction tag -> assuming 50km/h.");
	if(!maxspeed && !highway && log_message)
		log_message("Warning: OSM way "+std::to_string(osm_way_id) +" has no \"maxspeed\" and no \"highway\" tag and an no junction tag -> assuming 50km/h.");
	if(maxspeed && !highway && log_message)
		log_message("Warning: OSM way "+std::to_string(osm_way_id) +" has an unrecognized \"maxspeed\" tag of \""+maxspeed+"\" and no \"highway\" tag and an no junction tag -> assuming 50km/h.");
	return 50;
}

std::string get_osm_way_name(uint64_t osm_way_id, const TagMap&tags, std::function<void(const std::string&)>log_message){
	auto
		name = tags["name"],
		ref = tags["ref"];

	if(name != nullptr && ref != nullptr)
		return std::string(name) + ";"+ref;
	else if(name != nullptr)
		return std::string(name);
	else if(ref != nullptr)
		return std::string(ref);
	else
		return std::string();
}



bool is_osm_way_used_by_bicycles(uint64_t osm_way_id, const TagMap&tags, std::function<void(const std::string&)>log_message){
	const char* junction = tags["junction"];
	if(junction != nullptr)
		return true;

	const char* route = tags["route"];
	if(route != nullptr && str_eq(route, "ferry"))
		return true;

	const char* ferry = tags["ferry"];
	if(ferry != nullptr && str_eq(ferry, "ferry"))
		return true;

	const char* highway = tags["highway"];
	if(highway == nullptr)
		return false;


	if(str_eq(highway, "proposed"))
		return false;

	const char*access = tags["access"];
	if(access){
		if(
			!str_eq(access, "yes") &&
			!str_eq(access, "permissive") &&
			!str_eq(access, "delivery") &&
			!str_eq(access, "designated") &&
			!str_eq(access, "destination") &&
			!str_eq(access, "agricultural") &&
			!str_eq(access, "forestry") &&
			!str_eq(access, "public")
		){
			return false;
		}
	}

	const char*bicycle = tags["bicycle"];
	if(bicycle && (str_eq(bicycle, "no") || str_eq(bicycle, "use_sidepath")))
		return false;

	// if a cycleway is specified we can be sure
	// that the highway will be used in a direction
	const char* cycleway = tags["cycleway"];
	if(cycleway != nullptr)
		return true;
	const char* cycleway_left = tags["cycleway:left"];
	if(cycleway_left != nullptr)
		return true;
	const char* cycleway_right = tags["cycleway:right"];
	if(cycleway_right != nullptr)
		return true;
	const char* cycleway_both = tags["cycleway:both"];
	if(cycleway_both != nullptr)
		return true;

	if(
		str_eq(highway, "secondary") ||
		str_eq(highway, "tertiary") ||
		str_eq(highway, "unclassified") ||
		str_eq(highway, "residential") ||
		str_eq(highway, "service") ||
		str_eq(highway, "secondary_link") ||
		str_eq(highway, "tertiary_link") ||
		str_eq(highway, "living_street") ||
		str_eq(highway, "residential") ||
		str_eq(highway, "track") ||
		str_eq(highway, "bicycle_road") ||
		str_eq(highway, "primary") ||
		str_eq(highway, "primary_link") ||
		str_eq(highway, "path") ||
		str_eq(highway, "footway") ||
		str_eq(highway, "cycleway") ||
		str_eq(highway, "bridleway") ||
		str_eq(highway, "pedestrian") ||
		str_eq(highway, "crossing") ||
		str_eq(highway, "escape") ||
		str_eq(highway, "steps") ||
		str_eq(highway, "ferry")
	)
		return true;

	if(
		str_eq(highway, "motorway") ||
		str_eq(highway, "motorway_link") ||
		str_eq(highway, "motorway_junction") ||
		str_eq(highway, "trunk") ||
		str_eq(highway, "trunk_link") ||
		str_eq(highway, "construction") ||
		str_eq(highway, "bus_guideway") ||
		str_eq(highway, "raceway") ||
		str_eq(highway, "conveying")
	)
		return false;

	return false;
}

OSMWayDirectionCategory get_osm_bicycle_direction_category(uint64_t osm_way_id, const TagMap&tags, std::function<void(const std::string&)>log_message){
	const char*oneway_bicycle = tags["oneway:bicycle"];
	if(oneway_bicycle != nullptr){
		if(str_eq(oneway_bicycle, "-1") || str_eq(oneway_bicycle, "opposite"))
			return OSMWayDirectionCategory::only_open_backwards;

		if(str_eq(oneway_bicycle, "1") || str_eq(oneway_bicycle, "yes") || str_eq(oneway_bicycle, "true") || str_eq(oneway_bicycle, "no_planned"))
			return OSMWayDirectionCategory::only_open_forwards;

		if(str_eq(oneway_bicycle, "0") || str_eq(oneway_bicycle, "no") || str_eq(oneway_bicycle, "false") || str_eq(oneway_bicycle, "tolerated") || str_eq(oneway_bicycle, "permissive"))
			return OSMWayDirectionCategory::open_in_both;
		if(log_message)
			log_message("Warning: OSM way "+std::to_string(osm_way_id)+" has unknown oneway tag value \""+oneway_bicycle+"\" for \"oneway:bicycle\". Way is closed.");
		return OSMWayDirectionCategory::closed;
	}

	const char*oneway = tags["oneway"];
	if(oneway == nullptr){
		return OSMWayDirectionCategory::open_in_both;
	}else{
		if(str_eq(oneway, "no") || str_eq(oneway, "false") || str_eq(oneway, "0")) {
			return OSMWayDirectionCategory::open_in_both;
		}

		const char*cycleway = tags["cycleway"];
		if(cycleway != nullptr){
			// "opposite" is interpreted as the other direction than cars are allowed.
			// This is not necessarily opposite to the direction of the OSM way.
			//
			// A consequence is that "cycleway=opposite" combined "oneway=-1" does not imply that bicycles are only allowed to drive backwards
			//
			// (Yes, people actually do combine those two tags https://www.openstreetmap.org/way/88925376 )
			if(str_eq(cycleway, "opposite") || str_eq(cycleway, "opposite_track") || str_eq(cycleway, "opposite_lane") || str_eq(cycleway, "opposite_share_busway")){
				return OSMWayDirectionCategory::open_in_both;
			}
		}

		const char*cycleway_both = tags["cycleway:both"];
		if(cycleway_both != nullptr)
			return OSMWayDirectionCategory::open_in_both;

		const char*cycleway_left = tags["cycleway:left"];
		const char*cycleway_right = tags["cycleway:right"];
		if(cycleway_left != nullptr && cycleway_right != nullptr)
			return OSMWayDirectionCategory::open_in_both;


		if(str_eq(oneway, "-1") || str_eq(oneway, "reverse") || str_eq(oneway, "backward")) {
			return OSMWayDirectionCategory::only_open_backwards;
		} else if(str_eq(oneway, "yes") || str_eq(oneway, "true") || str_eq(oneway, "1")) {
			return OSMWayDirectionCategory::only_open_forwards;
		} else if(str_eq(oneway, "reversible") || str_eq(oneway, "alternating")) {
			return OSMWayDirectionCategory::closed;
		} else {
			if(log_message)
				log_message("Warning: OSM way "+std::to_string(osm_way_id)+" has unknown oneway tag value \""+oneway+"\" for \"oneway\". Way is closed.");
			return OSMWayDirectionCategory::closed;
		}
	}
}

unsigned char get_min_bicycle_comfort_level(){
	return 0;
}

unsigned char get_max_bicycle_comfort_level(){
	return 4;
}

unsigned char get_osm_way_bicycle_comfort_level(uint64_t osm_way_id, const TagMap&tags, std::function<void(const std::string&)>log_message){
	const char*highway = tags["highway"];
	if(highway != nullptr && str_eq(highway, "cycleway"))
		return 4;

	const char*cycleway = tags["cycleway"];
	if(cycleway != nullptr){
		if(str_eq(cycleway, "track"))
			return 3;
		else
			return 2;
	}

	if(highway != nullptr && (str_eq(highway, "primary") || str_eq(highway, "primary_link")))
		return 0;

	const char*bicycle = tags["bicycle"];
	if(bicycle != nullptr && str_eq(bicycle, "dismount"))
		return 0;

	return 1;
}

void decode_osm_car_turn_restrictions(
	uint64_t osm_relation_id, const std::vector<OSMRelationMember>&member_list,
	const TagMap&tags,
	std::function<void(OSMTurnRestriction)>on_new_turn_restriction,
	std::function<void(const std::string&)>log_message
){
	const char*restriction = tags["restriction"];
	if(restriction == nullptr)
		return;

	OSMTurnRestrictionCategory restriction_type;

	int direction_offset;

	if(starts_with("only_", restriction)) {
		restriction_type = OSMTurnRestrictionCategory::mandatory;
		direction_offset = 5;
	} else if(starts_with("no_", restriction)) {
		restriction_type = OSMTurnRestrictionCategory::prohibitive;
		direction_offset = 3;
	} else {
		if(log_message)
			log_message("Unknown OSM turn restriction with ID "+std::to_string(osm_relation_id)+" and value \""+restriction+"\", ignoring restriction");
		return;
	}

	OSMTurnDirection turn_direction;

	if(str_eq("left_turn", restriction+direction_offset)){
		turn_direction = OSMTurnDirection::left_turn;
	} else if(str_eq("right_turn", restriction+direction_offset)) {
		turn_direction = OSMTurnDirection::right_turn;
	} else if(str_eq("straight_on", restriction+direction_offset)) {
		turn_direction = OSMTurnDirection::straight_on;
	} else if(str_eq("u_turn", restriction+direction_offset)) {
		turn_direction = OSMTurnDirection::u_turn;
	}else{
		if(log_message)
			log_message("Unknown OSM turn restriction with ID "+std::to_string(osm_relation_id)+" and value \""+restriction+"\", ignoring restriction");
		return;
	}

	std::vector<unsigned>from_member_list;
	std::vector<unsigned>to_member_list;
	unsigned via_member = invalid_id;

	for(unsigned i=0; i<member_list.size(); ++i){
		if(str_eq(member_list[i].role, "via")) {
			if(via_member != invalid_id){
				if(log_message)
					log_message("OSM turn restriction with ID "+std::to_string(osm_relation_id)+" has several \"via\" roles, ignoring restriction");
				return;
			}
			via_member = i;
		} else if(str_eq(member_list[i].role, "from")) {
			from_member_list.push_back(i);
		} else if(str_eq(member_list[i].role, "to")) {
			to_member_list.push_back(i);
		} else if(str_eq(member_list[i].role, "location_hint")){
			// ignore
		} else {
			if(log_message)
				log_message("OSM turn restriction with ID "+std::to_string(osm_relation_id)+" and unknown role \""+member_list[i].role+"\", ignoring role");
		}
	}

	if(via_member != invalid_id && member_list[via_member].type == OSMIDType::relation){
		if(log_message)
			log_message("OSM turn restriction with ID "+std::to_string(osm_relation_id)+" has a relation as \"via\"-role, this is invalid, ignoring restriction");
		return;
	}

	if(via_member != invalid_id && member_list[via_member].type == OSMIDType::way){
		//log_message("OSM turn restriction with ID "+std::to_string(osm_relation_id)+" and name \""+restriction+"\" has a relation as \"way\"-role, this feature is not supported, ignoring restriction");
		return;
	}

	uint64_t via_node = (uint64_t)-1;
	if(via_member != invalid_id){
		via_node = member_list[via_member].id;
	}

	from_member_list.erase(
		std::remove_if(
			from_member_list.begin(), from_member_list.end(),
			[&](unsigned member){
				if(member_list[member].type != OSMIDType::way){
					if(log_message)
						log_message("OSM turn restriction with ID "+std::to_string(osm_relation_id)+" has \"from\"-role that is not a way, ignoring role");
					return true;
				} else {
					return false;
				}
			}
		),
		from_member_list.end()
	);

	to_member_list.erase(
		std::remove_if(
			to_member_list.begin(), to_member_list.end(),
			[&](unsigned member){
				if(member_list[member].type != OSMIDType::way){
					if(log_message)
						log_message("OSM turn restriction with ID "+std::to_string(osm_relation_id)+" has \"to\"-role that is not a way, ignoring role");
					return true;
				} else {
					return false;
				}
			}
		),
		to_member_list.end()
	);

	if(to_member_list.empty() ){
		if(log_message)
			log_message("OSM turn restriction with ID "+std::to_string(osm_relation_id)+" is missing \"to\" role, ignoring restriction");
		return;
	}

	if(from_member_list.empty() ){
		if(log_message)
			log_message("OSM turn restriction with ID "+std::to_string(osm_relation_id)+" is missing \"from\" role, ignoring restriction");
		return;
	}

	if(restriction_type == OSMTurnRestrictionCategory::mandatory && to_member_list.size() != 1){
		if(log_message)
			log_message("OSM turn restriction with ID "+std::to_string(osm_relation_id)+" is mandatory but has several \"to\" roles, ignoring restriction");
		return;
	}

	if(restriction_type == OSMTurnRestrictionCategory::mandatory && from_member_list.size() != 1){
		if(log_message)
			log_message("OSM turn restriction with ID "+std::to_string(osm_relation_id)+" is mandatory but has several \"to\" roles, ignoring restriction");
		return;
	}

	for(unsigned from_member:from_member_list)
		for(unsigned to_member:to_member_list)
			on_new_turn_restriction(OSMTurnRestriction{osm_relation_id, restriction_type, turn_direction, member_list[from_member].id, via_node, member_list[to_member].id});
}

} // RoutingKit
