#ifndef ENCODER_ABSTRACT_H
#define ENCODER_ABSTRACT_H

class encoderAbstract
{
public:
	virtual void computeSubEvents()=0;
	virtual ~encoderAbstract(){};
};

#endif // ENCODER_H