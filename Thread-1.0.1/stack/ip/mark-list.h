// File: mark-list.h
//
// Description: A collection of marking functions to call during buffer
//              reclamation
//
// Copyright 2015 by Silicon Laboratories. All rights reserved.             *80*

void emAddMarkingFunction(BufferMarker marker);
void emCallMarkFunctions(void);
