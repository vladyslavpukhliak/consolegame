#include "Services.h"

namespace Services {

	Graphics& graphics() {
		static Graphics instance;
		return instance;
	}

	Message& message() {
		static Message instance;
		return instance;
	}

}