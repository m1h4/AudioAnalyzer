#pragma once

#define DECLARE_GLOBAL_INSTANCE(className)							\
	inline className* Get##className(bool destroy = false)			\
	{																\
		static className* instance = NULL;							\
																	\
		if(!instance && !destroy)									\
			instance = new className;								\
		else if(destroy)											\
			SAFEDELETE(instance);									\
																	\
		return instance;											\
	}