#pragma once

/*
* 
* ONLY EXTEND THIS TO CLASSES THAT DERIVE FROM UOBJECT
* 
*/

#include "CoreMinimal.h"
#include "ClassType.h"

class Indexable
{
public:

	//Here we store the class ID of this variable
	Indexable( TypeID TI )
	{
		TID = TI;
	}

	virtual ~Indexable() {};

	//Use this to get the class ID of a variable
	inline TypeID GetClassID()
	{
		return TID;
	}

	//For use with object pooling.
	virtual void KillMe() {};

	//For use with object pooling.
	virtual void RessurrectMeAt( FVector position ) {};

private:
	TypeID TID;
};