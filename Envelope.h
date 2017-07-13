// Envelope.h: interface for the Envelope class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENVELOPE_H__0287EE3D_DFB5_4F43_93CC_F9263D15CAB9__INCLUDED_)
#define AFX_ENVELOPE_H__0287EE3D_DFB5_4F43_93CC_F9263D15CAB9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// C/C++ includes files
#include <iostream>
#include <vector>
#include <string>

using namespace std ;

struct xy {
		float x  ;
		float y  ;
	} ;

class Envelope  
{
public:

	Envelope();
	~Envelope();
	Envelope(vector<xy> input);
	Envelope(xy* input, int length);

	int EnterEnvelopeData( void ) ;
	
	vector<xy> _xyData ;

};

#endif // !defined(AFX_ENVELOPE_H__0287EE3D_DFB5_4F43_93CC_F9263D15CAB9__INCLUDED_)
