	//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"

#include "PBInitialize.h"

#pragma GCC diagnostic pop

int main(int argc, char* argv[]) {

	PB_Initialize();

	// Should never get here
	while (1);
}

// ----------------------------------------------------------------------------
