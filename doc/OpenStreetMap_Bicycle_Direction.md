# Bicycle direction by OpenStreetMap Examples

This implementation tries to follow the guidelines at http://wiki.openstreetmap.org/wiki/Bicycle#Cycle_features it is overly complex and was used to understand the tagging and make a simple implementation. It might benefit the reader when other applications such as road visualisations are written.


```
OSMWayDirectionCategory get_osm_bicycle_direction_category(uint64_t osm_way_id, const TagMap&tags, std::function<void(const std::string&)>log_message){
	const char
		*oneway = tags["oneway"],
		*oneway_bicycle = tags["oneway:bicycle"],
		*cycleway = tags["cycleway"],
		*cycleway_left  = tags["cycleway:left"],
		*cycleway_right = tags["cycleway:right"],
		*cycleway_both = tags["cycleway:both"],
		*junction = tags["junction"],
		*highway = tags["highway"],
		*segregated = tags["segregated"]
	;

	if(junction != nullptr && str_eq(junction, "roundabout"))
		return OSMWayDirectionCategory::only_open_forwards;


	/* L1a */
	if (
		(cycleway      != nullptr && str_eq(cycleway,      "lane")) ||
		(cycleway_both != nullptr && str_eq(cycleway_both, "lane")) ||
		(cycleway_left != nullptr && str_eq(cycleway_left, "lane") && cycleway_right != nullptr && str_eq(cycleway_right, "lane") )) {

		return OSMWayDirectionCategory::open_in_both;
	}

	/* L1b */
	if (
        (cycleway_left  != nullptr && str_eq(cycleway_left,  "lane") && cycleway_right == nullptr) ||
        (cycleway_right != nullptr && str_eq(cycleway_right, "lane") && cycleway_left  == nullptr)
       ) {

		if (oneway_bicycle) {
			if (tag_cmp_no(oneway_bicycle)) {
				return OSMWayDirectionCategory::open_in_both;
			}
		} else {
			/* L2 */

			/* The suggestion here is that the highway inherits bidirectional behavior by default
             * thus the directionality is depedent on the direction limitations applied on the highway.
             */
		}
	}


	/* M1 */
	if (
		(oneway != nullptr && tag_cmp_yes(oneway) && cycleway      != nullptr && str_eq(cycleway,      "lane")          && oneway_bicycle != nullptr && tag_cmp_no(oneway_bicycle)) ||
		(oneway != nullptr && tag_cmp_yes(oneway) && cycleway_left != nullptr && str_eq(cycleway_left, "opposite_lane") && cycleway_right != nullptr && str_eq(cycleway_right, "lane")) ||
		(oneway != nullptr && tag_cmp_yes(oneway) && cycleway_left != nullptr && str_eq(cycleway_left, "lane") && cycleway_right != nullptr && str_eq(cycleway_right, "opposite_lane"))
       ) {
		return OSMWayDirectionCategory::open_in_both;
	}

	if (
		(oneway != nullptr && tag_cmp_yes(oneway) && cycleway_left  != nullptr && str_eq(cycleway_left,  "lane")) || /* M2a */
		(oneway != nullptr && tag_cmp_yes(oneway) && cycleway_right != nullptr && str_eq(cycleway_right, "lane")) || /* M2b */
		(oneway != nullptr && tag_cmp_yes(oneway) && cycleway != nullptr && str_eq(cycleway, "lane")) ||             /* M2a, M2b, M2c */
		(oneway != nullptr && tag_cmp_yes(oneway) && cycleway_left  != nullptr && str_eq(cycleway_left,  "lane") && oneway_bicycle != nullptr && tag_cmp_no(oneway_bicycle)) || /* M2d */
		(oneway != nullptr && tag_cmp_yes(oneway) && cycleway_right != nullptr && str_eq(cycleway_right, "lane") && oneway_bicycle != nullptr && tag_cmp_no(oneway_bicycle))
       ) {
		return OSMWayDirectionCategory::only_open_forwards;
	}

	if (
		(oneway != nullptr && tag_cmp_yes(oneway) && cycleway       != nullptr && str_eq(cycleway,       "opposite_lane") && oneway_bicycle != nullptr && tag_cmp_no(oneway_bicycle)) || /* M3a + M3b */
		(oneway != nullptr && tag_cmp_yes(oneway) && cycleway_left  != nullptr && str_eq(cycleway_left,  "opposite_lane") && oneway_bicycle != nullptr && tag_cmp_no(oneway_bicycle)) || /* M3a */
		(oneway != nullptr && tag_cmp_yes(oneway) && cycleway_right != nullptr && str_eq(cycleway_right, "opposite_lane") && oneway_bicycle != nullptr && tag_cmp_no(oneway_bicycle))    /* M3a, M3b */
       ) {
		return OSMWayDirectionCategory::open_in_both;
	}

	if (highway != nullptr && str_eq(highway, "cycleway") && oneway != nullptr) {
		if (tag_cmp_yes(oneway)) {
			return OSMWayDirectionCategory::only_open_forwards; /* T1, T4, S2 */
		} else if (tag_cmp_no(oneway)) {
			return OSMWayDirectionCategory::open_in_both; /* T2, T3 */
		}
	}

	if (cycleway != nullptr && (str_eq(cycleway, "track") || str_eq(cycleway, "share_busway")) && oneway_bicycle == nullptr) {
		return OSMWayDirectionCategory::only_open_forwards; /* T1 */
	}

	if (
		(cycleway_left  != nullptr && (str_eq(cycleway_left, "track")  || str_eq(cycleway_left, "share_busway")) ) ||
        (cycleway_right != nullptr && (str_eq(cycleway_right, "track") || str_eq(cycleway_left, "share_busway")) )
       ) {
		if (oneway_bicycle != nullptr && tag_cmp_no(oneway_bicycle)) {
			return OSMWayDirectionCategory::open_in_both; /* T2, T3 */
		} else {
			return OSMWayDirectionCategory::only_open_forwards; /* T4 */
		}
	}

	if (
		(oneway_bicycle != nullptr && tag_cmp_no(oneway_bicycle)) ||
		(cycleway != nullptr && str_eq(cycleway, "opposite"))
       ) {
		return OSMWayDirectionCategory::open_in_both; /* S1 */
	}


	if (
		(cycleway_left != nullptr && str_eq(cycleway_left, "track") && cycleway_right != nullptr && str_eq(cycleway_right, "lane")) ||
		(cycleway_left != nullptr && str_eq(cycleway_left, "lane")  && cycleway_right != nullptr && str_eq(cycleway_right, "right"))
       ) {
		return OSMWayDirectionCategory::open_in_both; /* S2 */
	}


	if (cycleway != nullptr && str_eq(cycleway, "track") && segregated != nullptr && tag_cmp_yes(segregated)) {
		return OSMWayDirectionCategory::open_in_both; /* S3 */
	}

	if(cycleway != nullptr) {
		if (str_eq(cycleway, "lane") || str_eq(cycleway_left, "track")) {
			return OSMWayDirectionCategory::open_in_both;
		}
	}

	if (cycleway_left != nullptr) {
		if (str_eq(cycleway_left, "lane") || str_eq(cycleway_left, "track")) {
			return OSMWayDirectionCategory::open_in_both;
		}
	}

	if (cycleway_right != nullptr) {
		if (str_eq(cycleway_right, "lane") || str_eq(cycleway_right, "track")) {
			return OSMWayDirectionCategory::open_in_both;
		}
	}

	if (cycleway_both != nullptr) {
		if (str_eq(cycleway_both, "lane") || str_eq(cycleway_both, "track")) {
			return OSMWayDirectionCategory::open_in_both;
		}
	}

	if (oneway_bicycle != nullptr) {
		oneway = oneway_bicycle;
	}

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
			log_message("Warning: OSM way "+std::to_string(osm_way_id)+" has unknown oneway tag value \""+oneway+"\" for \"oneway\". Way is closed.");
		}
	}

	return OSMWayDirectionCategory::open_in_both;
}
```
