#ifndef NOSTOP_ISLALGORITHM_PACKET_HPP
#define NOSTOP_ISLALGORITHM_PACKET_HPP

#include "../shared/types.h"

namespace NoStop
{  
	struct ISLAlgorithm_packet
	{
		std::vector<int> m_area;

		template <typename Archive>
		void serialize(Archive& ar, const unsigned int /*version*/)
		{
			ar& m_area;
		}
	};
}

#endif