#ifndef _FQF_H_
#define _FQF_H_

class FQF
{
public:
	static FQF * getObject()
	{
		static FQF f;
		return &f;
	}
	virtual ~FQF();

protected:
	FQF();
};

#endif