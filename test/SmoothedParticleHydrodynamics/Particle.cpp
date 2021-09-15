#include "Particle.h"







/*########################################################################################################
########################################### Public Functions  ############################################
########################################################################################################*/



Particle *newParticle()
{
	Particle *p = (Particle *) malloc (sizeof(Particle));
	
	InitZero( p->Position );
	InitZero( p->Uvec );
	p->Pressure = 1.0;
	p->Rho = 1000.0;
	p->m = 1;
	p->viscosity = 0.0;
	
	return p;
}


Particle *newParticle( const Vec3d& pos )
{
	Particle *p = (Particle *) malloc (sizeof(Particle));
	
	p->Position	= pos;
	InitZero( p->Uvec );
	p->Pressure = 1.0;
	p->Rho = 1000.0;
	p->m = 1.0;
	p->viscosity = 0.0;

	return p;
}



void Particle_Init( Particle *p, const Vec3d& pos )
{
	p->Position	= pos;
	InitZero( p->Uvec );
	p->Pressure = 1.0;
	p->Rho = 1000.0;
	p->m = 1.0;
	p->viscosity = 0.0;
}


void Particle_Init( Particle *p, const Vec3d& pos, const Vec3d& u )
{
	p->Position	= pos;
	p->Uvec	= u;
	p->Pressure = 1.0;
	p->Rho = 1000.0;
	p->m = 1.0;
	p->viscosity = 0.0;
}


void Particle_Info(Particle *p)
{
	cout << "Mass:	" << p->m << endl;
	cout << "Press:	" << p->Rho << endl;

}
/*########################################################################################################
######################################### Private Functions  #############################################
########################################################################################################*/