#pragma once

#include <vector>

#include "EventSystem.hpp"

namespace shadow
{
	class Clock : public EventRecipient<Clock>
	{
	public:
		Clock( EventRecipient )
	};


}