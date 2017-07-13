// Envelope.cpp: implementation of the Envelope class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include "Envelope.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Envelope::Envelope()
{

}

Envelope::~Envelope()
{

}


Envelope::Envelope(vector<xy> input)
{

}

Envelope::Envelope(xy* input, int length)
{
	int x = 0 ;
	xy tempxy ;

	for (x=0 ; x < length; x++)
	{
		tempxy.x = (float) (input->x) ;
		tempxy.y = (float) (input->y) ;
		input++ ;
		_xyData.push_back( tempxy ) ;
	}

}


int Envelope::EnterEnvelopeData( void ) 
{

	xy tempxy ;
	
	cout << endl ;
	cout << " This is single side of envelope - other is mirror image" << endl ;
	cout << "    |"									  	<< endl ;
	cout << " 100|   ___"						   	<< endl ;
	cout << "    |  /   \\ "						<< endl ;
	cout << "    | /     \\___ "				<< endl ;
	cout << "    |/           \\ "			<< endl ;
	cout << "   0|_____________\\___  " << endl ;
	cout << "     0             100    "<< endl ;

	
	char input = 'y' ;
	while ( input == 'y' )
	{
		cout << "Attack (xy range 0-100)" << endl << "x: " ;
		cin.clear(0) ; 
		cin >> tempxy.x ;
		cout << "y: " ;
		cin.clear(0) ; 
		cin >> tempxy.y ;


		cout << "Enter Another? (y/n)" ;
		cin.clear(0) ;
		cin >> input ;
		//input = cin.
		if ((input == 'n') || (input == 'N'))
		{
			cout << "true!" ;
		}
		else
			input = 'y' ;

		cin.clear(0) ;

		_xyData.push_back( tempxy ) ;
	}

	return _xyData.size() ;
 
}
