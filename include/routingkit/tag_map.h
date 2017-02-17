#ifndef ROUTING_KIT_TAG_MAP_H
#define ROUTING_KIT_TAG_MAP_H

#include <routingkit/sort.h>
#include <routingkit/permutation.h>
#include <routingkit/inverse_vector.h>

#include <vector>
#include <string.h>

namespace RoutingKit{

class TagMap{
private:
	static unsigned compute_char_hash(char c){
		switch(c){
			case 'e': case 'E': return 7;
			case 't': case 'T': return 8;
			case 'a': case 'A': return 9;
			case 'o': case 'O': return 10;
			case 'i': case 'I': return 11;
			case 'n': case 'N': return 12;
			case 's': case 'S': return 13;
			case 'h': case 'H': return 14;
			case 'r': case 'R': return 15;
			default:return static_cast<unsigned char>(c)%7;
		}
	}

	static const unsigned char_hash_count = 16;

	static unsigned compute_hash(const char*str){
		unsigned last_char = strlen(str);
		if(last_char == 0)
			return 0;
		else
			--last_char;

		return
			(
				(
					compute_char_hash(str[0])
				)*char_hash_count + compute_char_hash(str[last_char])
			)*char_hash_count + compute_char_hash(str[last_char/2])
		;
	}

	static const unsigned hash_count = char_hash_count*char_hash_count*char_hash_count;

public:
	TagMap():entry_begin(hash_count, 0), entry_end(hash_count, 0){}

	void clear(){
		for(auto x:hash_element_list){
			entry_begin[x>>16] = 0;
			entry_end[x>>16] = 0;
		}
	}

	template<class GetKey, class GetValue>
	void build(unsigned key_value_count, const GetKey&get_key, const GetValue&get_value){

		clear();

		hash_element_list.resize(key_value_count);
		for(unsigned i=0; i<key_value_count; ++i)
			hash_element_list[i] = (compute_hash(get_key(i))<<16) | i;
		
		std::sort(hash_element_list.begin(), hash_element_list.end());

		entry.resize(key_value_count);
		unsigned prev_hash = hash_count;
		for(unsigned i=0; i<key_value_count; ++i){
			unsigned hash = hash_element_list[i] >> 16;
			unsigned j = hash_element_list[i] & 0xFFFFu;
			entry[i].key = get_key(j);
			entry[i].value = get_value(j);

			unsigned current_hash = hash;
			if(current_hash != prev_hash)
				entry_begin[current_hash] = i;
			entry_end[current_hash] = i+1;
			prev_hash = current_hash;
		}
	}

	const char*operator[](const char*key) const {
		unsigned hash = compute_hash(key);

		for(unsigned i=entry_begin[hash]; i<entry_end[hash]; ++i)
			if(!strcmp(key, entry[i].key)){
				return entry[i].value;
			}
		return nullptr;
	}

	bool empty() const {
		return entry.empty();
	}

	unsigned size() const {
		return entry.size();
	}

	struct Entry{
		const char*key;
		const char*value;
	};

	std::vector<Entry>::const_iterator begin()const{
		return entry.begin();
	}

	std::vector<Entry>::const_iterator end()const{
		return entry.end();
	}

private:
	std::vector<unsigned>hash_element_list;
	std::vector<Entry>entry;
	std::vector<unsigned>entry_begin, entry_end;
};

} // RoutingKit

#endif

