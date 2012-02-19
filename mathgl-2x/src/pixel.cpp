/***************************************************************************
 * pixel.cpp is part of Math Graphic Library
 * Copyright (C) 2007 Alexey Balakin <balakin@appl.sci-nnov.ru>            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <stdlib.h>
#include <math.h>
#include <algorithm>
#include "mgl/canvas.h"
//-----------------------------------------------------------------------------
void mglCanvas::SetSize(int w,int h)
{
	if(w<=0 || h<=0)	{	SetWarn(mglWarnSize,"SetSize");	return;	}
	Width = w;	Height = h;	Depth = long(sqrt(w*h));
	if(G)	{	delete []G;	delete []C;	delete []Z;	delete []G4;delete []OI;	}
	G = new unsigned char[w*h*3];
	G4= new unsigned char[w*h*4];
	C = new unsigned char[w*h*12];
	Z = new float[w*h*3];	// only 3 planes
	OI= new int[w*h];
	InPlot(0,1,0,1,false);	Clf();
}
//-----------------------------------------------------------------------------
void mglDrawReg::set(mglCanvas *gr, int nx, int ny, int m)
{
	int mx = m%nx, my = m/nx;
	x1 = gr->GetWidth()*mx/nx;		y1 = gr->GetHeight()-gr->GetHeight()*(my+1)/ny;
	x2 = gr->GetWidth()*(mx+1)/nx;	y2 = gr->GetHeight()-gr->GetHeight()*my/ny;
}
//-----------------------------------------------------------------------------
void mglCanvas::PutDrawReg(mglDrawReg *d, const mglCanvas *gr)
{
	if(!gr)	return;
	int dd = d->x2 - d->x1;
	register long i,j;
	for(j=d->y1;j<d->y2;j++)
	{
		i = d->x1+Width*(Height-1-j);
		memcpy(OI+i,gr->OI+i,dd*sizeof(int));
		memcpy(Z+3*i,gr->Z+3*i,3*dd*sizeof(float));
		memcpy(C+12*i,gr->C+12*i,12*dd);
	}
}
//-----------------------------------------------------------------------------
void mglCanvas::PostScale(mglPoint &p)
{
	mglPoint q=p/(2*B.pf);
	p.x = B.x + q.x*B.b[0] + q.y*B.b[1] + q.z*B.b[2];
	p.y = B.y+ q.x*B.b[3] + q.y*B.b[4] + q.z*B.b[5];
	p.z = B.z+ q.x*B.b[6] + q.y*B.b[7] + q.z*B.b[8];
/*	if(Persp)
	{
		register float d = (1-Persp*Depth/2)/(1-Persp*p.z);
		p.x = Width/2 + d*(p.x-Width/2);
		p.y = Height/2 + d*(p.y-Height/2);
	}*/
}
//-----------------------------------------------------------------------------
bool mglCanvas::ScalePoint(mglPoint &p, mglPoint &n, bool use_nan)
{
	bool res = get(MGL_DISABLE_SCALE) || mglBase::ScalePoint(p,n,use_nan);
	if(TernAxis&4)	return res;
	PostScale(p);

	mglPoint y=n/(2*B.pf);
	n.x = y.x*B.b[0] + y.y*B.b[1] + y.z*B.b[2];
	n.y = y.x*B.b[3] + y.y*B.b[4] + y.z*B.b[5];
	n.z = y.x*B.b[6] + y.y*B.b[7] + y.z*B.b[8];
/*	if(Persp)
	{
		register float d = (1-Persp*Depth/2)/(1-Persp*p.z);
		// NOTE: No d* since I use transformed p here.
		register float dd = Persp*n.z/(1-Persp*p.z);
		n.x = d*n.x + dd*(p.x-Width/2);
		n.y = d*n.y + dd*(p.y-Height/2);
	}*/
	return res;
}
//-----------------------------------------------------------------------------
long mglCanvas::ProjScale(int nf, long id)
{
	const mglPnt &p0=Pnt[id];
	mglPoint pp(p0.x,p0.y,p0.z), nn(p0.u,p0.v,p0.w);
	if(isnan(pp.x))	return -1;
	mglPoint q=pp/(2*B.pf), p, n=nn;
	register float w=B1.b[0]/2, h=B1.b[4]/2, d=B1.b[8]/2, xx=B1.x-w/2, yy=B1.y-h/2;
	if(TernAxis&1)	// usual ternary axis
	{
		if(nf==0)
		{	p.x = xx+w/2 + (q.x+(q.y+1)/2)*w/2;
			n.x = (nn.x+nn.y/2)*w/2;
			p.y = yy+h + q.y*h/2;	n.y = nn.y*h/2;	}
	}
	else if(TernAxis&2)	// quaternary axis
	{
		if(nf==0)
		{	p.x = xx+w/2 + (q.x+(q.y+1)/2)*w/2;
			n.x = (nn.x+nn.y/2)*w/2;
			p.y = yy+h + q.y*h/2;
			n.y = nn.y*h/2;	}
		else if(nf==1)
		{	p.x = xx+w/2 + (q.x+(1-q.z)/2)*w/2;
			n.x = (nn.x-nn.z/2)*w/2;
			p.y = yy+h - q.z*h/2;
			n.y = -nn.z*h/2;	}
		else if(nf==2)
		{	p.x = xx+w/2 + (q.y-q.z)/2*w/2;
			n.x = (nn.y-nn.z/2)*w/2;
			p.y = yy+h + (q.y+q.z)*h/2;
			n.y = (nn.y+nn.z)/2*h/2;	}
		else
		{	p.x = xx+w/2 + (q.x+1+(q.y+q.z)/2)*w/2;
			n.x = (nn.x+(nn.y+nn.z)/2)*w/2;
			p.y = yy+h + (q.y+(q.z+1)/3)*h/2;
			n.y = (nn.y+nn.z/3)*h/2;	}
	}
	else
	{
		if(nf==0)
		{	p.x = xx + q.x*w;		n.x = nn.x*w/2;
			p.y = yy + q.y*h;		n.y = nn.y*h/2;
			p.z = B1.z + q.z*d;	n.z = nn.z*d/2;	}
		else if(nf==1)
		{	p.x = xx + q.x*w;		n.x = nn.x*w/2;
			p.y = yy+h + q.z*h;		n.y = nn.z*h/2;
			p.z = B1.z + q.y*d;	n.z = nn.y*d/2;	}
		else if(nf==2)
		{	p.x = xx+w + q.z*w;		n.x = nn.z*w/2;
			p.y = yy + q.y*h;		n.y = nn.y*h/2;
			p.z = B1.z+ q.x*d;	n.z = nn.x*d/2;	}
		else
		{	p.x = xx+w + q.x*B.b[0]/2 + q.y*B.b[1]/2 + q.z*B.b[2]/2;
			p.y = yy+h + q.x*B.b[3]/2 + q.y*B.b[4]/2 + q.z*B.b[5]/2;
			p.z = B.z + q.x*B.b[6]/2 + q.y*B.b[7]/2 + q.z*B.b[8]/2;
			n.x = (nn.x*B.b[0] + nn.y*B.b[1] + nn.z*B.b[2])/2;
			n.y = (nn.x*B.b[3] + nn.y*B.b[4] + nn.z*B.b[5])/2;
			n.z = (nn.x*B.b[6] + nn.y*B.b[7] + nn.z*B.b[8])/2;	}
	}
	return CopyProj(id,p,n);
}
//-----------------------------------------------------------------------------
void mglCanvas::LightScale()
{
	register float xx,yy,zz;
	register long i;
	for(i=0;i<10;i++)
	{
		if(!light[i].n)	continue;
		light[i].p=light[i].d;	light[i].q=light[i].r;
		ScalePoint(light[i].q,light[i].p,false);
		light[i].p /= light[i].p.norm();
	}
}
//-----------------------------------------------------------------------------
// NOTE: Perspective, transformation formulas and lists are not support just now !!! Also it use LAST InPlot parameters!!!
mglPoint mglCanvas::CalcXYZ(int xs, int ys)
{
	float s3 = 2*B.pf, x, y, z;	// TODO: Take into account z-value of z-buffer
	ys = Height - ys;
	float xx = xs-B.x, yy = ys-B.y;
	float d1=B.b[0]*B.b[4]-B.b[1]*B.b[3], d2=B.b[1]*B.b[5]-B.b[2]*B.b[4], d3=B.b[0]*B.b[5]-B.b[2]*B.b[3];
	if(fabs(d1) > fabs(d2) && fabs(d1) > fabs(d3))	// x-y plane
	{
		z = 0;
		x = s3*(B.b[4]*xx-B.b[1]*yy)/d1;
		y = s3*(B.b[0]*yy-B.b[3]*xx)/d1;
	}
	else if(fabs(d2) > fabs(d3))	// y-z
	{
		x = 0;
		y = s3*(B.b[5]*xx-B.b[2]*yy)/d2;
		z = s3*(B.b[1]*yy-B.b[4]*xx)/d2;
	}
	else	// x-z
	{
		y = 0;
		x = s3*(B.b[5]*xx-B.b[2]*yy)/d3;
		z = s3*(B.b[0]*yy-B.b[3]*xx)/d3;
	}
	LastMousePos = mglPoint(Min.x + (Max.x-Min.x)*(x+1)/2,
					Min.y + (Max.y-Min.y)*(y+1)/2,
					Min.z + (Max.z-Min.z)*(z+1)/2);
	return LastMousePos;
}
//-----------------------------------------------------------------------------
void mglCanvas::CalcScr(mglPoint p, int *xs, int *ys)
{
	mglPoint n;
	ScalePoint(p,n);
	if(xs)	*xs=int(p.x);
	if(ys)	*ys=int(p.y);
}
//-----------------------------------------------------------------------------
mglPoint mglCanvas::CalcScr(mglPoint p)
{	int x,y;	CalcScr(p,&x,&y);	return mglPoint(x,y);	}
//-----------------------------------------------------------------------------
//mglCanvas *mgl_tmp_gr;
bool operator<(const mglPrim &a, const mglPrim &b)
{
	if( a.z < b.z-0.5 )	return true;
	if( a.z > b.z+0.5 )	return false;
	if( a.w < b.w )		return true;
	if( a.w > b.w )		return false;
	if( a.n3 < b.n3 )	return true;
	if( a.n3 > b.n3 )	return false;
	return a.type < b.type;
//	if( type != a.type )	return type < a.type;
//	if( a1->type==1 && (a1->xx(mgl_tmp_gr)!=a2->xx(mgl_tmp_gr)) )
//		return (a2->xx(mgl_tmp_gr)<a1->xx(mgl_tmp_gr)) ? 1 : -1;
//	if( a1->type==1 )	return (a2->yy(mgl_tmp_gr)<a1->yy(mgl_tmp_gr)) ? 1 : -1;
//	return 0;
}
//-----------------------------------------------------------------------------
bool operator>(const mglPrim &a, const mglPrim &b)
{
	if( a.z < b.z-0.5 )	return false;
	if( a.z > b.z+0.5 )	return true;
	if( a.w < b.w )		return false;
	if( a.w > b.w )		return true;
	if( a.n3 < b.n3 )	return false;
	if( a.n3 > b.n3 )	return true;
	return a.type > b.type;
}
//-----------------------------------------------------------------------------
void *mgl_canvas_thr(void *par)
{	mglThreadG *t=(mglThreadG *)par;	(t->gr->*(t->f))(t->id, t->n, t->p);	return NULL;	}
void mglStartThread(void (mglCanvas::*func)(unsigned long i, unsigned long n, const void *p), mglCanvas *gr, unsigned long n, const void *p=NULL)
{
	if(!func || !gr)	return;
#ifdef HAVE_PTHREAD
	if(mglNumThr<1)	mglSetNumThr(0);
	if(mglNumThr>1)
	{
		pthread_t *tmp=new pthread_t[mglNumThr];
		mglThreadG *par=new mglThreadG[mglNumThr];
		register int i;
		for(i=0;i<mglNumThr;i++)	// put parameters into the structure
		{	par[i].gr=gr;	par[i].f=func;	par[i].n=n;	par[i].p=p;	par[i].id=i;	}
		for(i=0;i<mglNumThr;i++)	pthread_create(tmp+i, 0, mgl_canvas_thr, par+i);
		for(i=0;i<mglNumThr;i++)	pthread_join(tmp[i], 0);
		delete []tmp;	delete []par;
	}
	else
#endif
	{	mglNumThr = 1;	(gr->*func)(0,n,p);	}
}
//-----------------------------------------------------------------------------
void mglCanvas::pxl_primdr(unsigned long id, unsigned long n, const void *)
{
	int nx=1,ny=1,pdef=PDef;
	register unsigned long i;
	if(id<unsigned(mglNumThr))
	{
		for(i=1;i<=unsigned(sqrt(mglNumThr)+0.5);i++)
			if(mglNumThr%i==0)	ny=i;
		nx = mglNumThr/ny;
	}
	else	{	nx=ny=1;	id=0;	}
	mglDrawReg d;	d.set(this,nx,ny,id);
	float ss=pPos, ww=PenWidth;
	mglPrim p;
	for(i=0;i<n;i++)
	{
		p=Prm[i];	PDef=p.n3;	pPos=p.s;	PenWidth=p.w;
		switch(p.type)
		{
		case 0:	mark_draw(p.n1,p.n4,p.s,&d);	break;
		case 1:	line_draw(p.n1,p.n2,&d);		break;
		case 2:	trig_draw(p.n1,p.n2,p.n3,true,&d);	break;
		case 3:	quad_draw(p.n1,p.n2,p.n3,p.n4,&d);	break;
		case 4:	glyph_draw(&p,&d);	break;
		}
	}
	PDef=pdef;	pPos=ss;	PenWidth=ww;
}
//-----------------------------------------------------------------------------
void mglCanvas::pxl_combine(unsigned long id, unsigned long n, const void *)
{
	unsigned char c[4],*cc;
	for(unsigned long i=id;i<n;i+=mglNumThr)
	{	cc = C+12*i;		memcpy(c,BDef,4);
		combine(c,cc+8);	combine(c,cc+4);
		combine(c,cc);		memcpy(G4+4*i,c,4);	}
}
//-----------------------------------------------------------------------------
void mglCanvas::pxl_memcpy(unsigned long id, unsigned long n, const void *)
{	for(unsigned long i=id;i<n;i+=mglNumThr)	memcpy(G4+4*i,C+12*i,4);	}
//-----------------------------------------------------------------------------
void mglCanvas::pxl_backgr(unsigned long id, unsigned long n, const void *)
{
	unsigned char c[4];
	for(unsigned long i=id;i<n;i+=mglNumThr)
	{	memcpy(c,BDef,4);	combine(c,G4+4*i);	memcpy(G+3*i,c,3);	}
}
//-----------------------------------------------------------------------------
void mglCanvas::pxl_transform(unsigned long id, unsigned long n, const void *)
{
	register float x,y,z;
	for(unsigned long i=id;i<n;i+=mglNumThr)
	{
		mglPnt &p=Pnt[i];
		x = p.xx-Width/2.;	y = p.yy-Height/2.;	z = p.zz-Depth/2.;
		p.x = Bp.x*Width + Width/2 + Bp.b[0]*x + Bp.b[1]*y + Bp.b[2]*z;
		p.y = Bp.y*Height+ Height/2+ Bp.b[3]*x + Bp.b[4]*y + Bp.b[5]*z;
		p.z = Depth/2. + Bp.b[6]*x + Bp.b[7]*y + Bp.b[8]*z;
		if(Bp.pf)
		{
			register float d = (1-Bp.pf*Depth/2)/(1-Bp.pf*p.z);
			p.x = Width/2 + d*(p.x-Width/2);
			p.y = Height/2 + d*(p.y-Height/2);
		}
	}
}
//-----------------------------------------------------------------------------
void mglCanvas::pxl_setz(unsigned long id, unsigned long n, const void *)
{
	for(unsigned long i=id;i<n;i+=mglNumThr)
	{	mglPrim &q=Prm[i];	q.z = Pnt[q.n1].z;	}
}
//-----------------------------------------------------------------------------
void mglCanvas::Finish()
{
	static mglMatrix bp;
	if(memcmp(&Bp,&bp,sizeof(mglMatrix)) && !(Quality&4) && Prm.size()>0)
		clr(MGL_FINISHED);
	if(get(MGL_FINISHED))	return;	// nothing to do
	if(!(Quality&4) && Prm.size()>0)
	{
		mglStartThread(&mglCanvas::pxl_transform,this,Pnt.size());
		mglStartThread(&mglCanvas::pxl_setz,this,Prm.size());
		std::sort(Prm.begin(), Prm.end());	bp=Bp;
//		mglStartThread(&mglCanvas::pxl_primdr,this,Prm.size());	// TODO: check conflicts in pthreads
		pxl_primdr(-1,Prm.size(),NULL);
	}
	unsigned long n=Width*Height;
	BDef[3] = (Flag&3)!=2 ? 0:255;
	if(Quality&2)	mglStartThread(&mglCanvas::pxl_combine,this,n);
	else	mglStartThread(&mglCanvas::pxl_memcpy,this,n);
	BDef[3] = 255;
	mglStartThread(&mglCanvas::pxl_backgr,this,n);
	set(MGL_FINISHED);
}
//-----------------------------------------------------------------------------
void mglCanvas::ClfZB()
{
	register long i,n=Width*Height;
	memset(C,0,12*n);	memset(OI,0,n*sizeof(int));
	for(i=0;i<3*n;i++)	Z[i] = -1e20f;	// TODO: Parallelization ?!?
}
//-----------------------------------------------------------------------------
void mglCanvas::Clf(mglColor Back)
{
	Fog(0);			PDef = 0xffff;	pPos = 0;	StartAutoGroup(NULL);
	Pnt.clear();	Prm.clear();	Ptx.clear();
	Sub.clear();	Leg.clear();	Grp.clear();
	if(Back==0)			Back = 'w';
	if((Flag&3)==2)	Back = 'k';
	BDef[0]=Back.r*255;	BDef[1]=Back.g*255;BDef[2]=Back.b*255;	BDef[3]=0;
	ClfZB();		clr(MGL_FINISHED);
}
//-----------------------------------------------------------------------------
void mglCanvas::pxl_other(unsigned long id, unsigned long n, const void *p)
{
	unsigned long i,j,k;
	const mglCanvas *gr = (const mglCanvas *)p;
	if(!gr)	return;
	for(k=id;k<n;k+=mglNumThr)
	{
		i = k%Width;	j = Height-1-(k/Width);
		if(Quality&2)
		{
			pnt_plot(i,j,gr->Z[3*k+2],gr->C+12*k+8);
			pnt_plot(i,j,gr->Z[3*k+1],gr->C+12*k+4);
		}
		pnt_plot(i,j,gr->Z[3*k],gr->C+12*k);
	}
}
//-----------------------------------------------------------------------------
void mglCanvas::Combine(const mglCanvas *gr)
{
	if(Width!=gr->Width || Height!=gr->Height)	return;	// wrong sizes
	mglStartThread(&mglCanvas::pxl_other,this,Width*Height,gr);
}
//-----------------------------------------------------------------------------
void mglCanvas::pnt_plot(long x,long y,float z,const unsigned char ci[4])
{
	long i0=x+Width*(Height-1-y);
	if(ci[3]==0)	return;
	unsigned char *cc = C+12*i0, c[4];
	memcpy(c,ci,4);
	float *zz = Z+3*i0, zf = FogDist*(z/Depth-0.5-FogDz);
	if(zf<0)
	{
		int d = int(255.f-255.f*exp(5.f*zf));
		unsigned char cb[4] = {BDef[0], BDef[1], BDef[2], d};
		if(d==255)	return;
		combine(c,cb);
	}
	if(Quality&2)
	{
		if(z>zz[1])	// shift point on slice down and paste new point
		{
			zz[2] = zz[1];	combine(cc+8,cc+4);
			if(z>zz[0])
			{	zz[1] = zz[0];	zz[0] = z;	OI[i0]=ObjId;
				memcpy(cc+4,cc,4);	memcpy(cc,c,4);		}
			else	{	zz[1] = z;	memcpy(cc+4,c,4);	}
		}
		else
		{
			if(z>zz[2])	// shift point on slice down and paste new point
			{	zz[2] = z;	combine(cc+8,c);	}
			else		// point below the background
			{	combine(c,cc+8);	memcpy(cc+8,c,4);	}
		}
	}
	else
	{
		if(z>zz[0])	// point upper the background
		{	zz[0]=z;	memcpy(cc,c,4);		OI[i0]=ObjId;	}
	}
}
//-----------------------------------------------------------------------------
unsigned char* mglCanvas::col2int(const mglPnt &p,unsigned char *r)
{
	if(!r)	return r;
	if(p.a<=0)	{	memset(r,0,4*sizeof(unsigned char));	return r;	}
	register float b0=0,b1=0,b2=0, ar,ag,ab;
	ar = ag = ab = AmbBr;

	if(get(MGL_ENABLE_LIGHT) && !isnan(p.u))
	{
		float d0,d1,d2,nn;
		register long i;
		for(i=0;i<10;i++)
		{
			if(!light[i].n)	continue;
			if(isnan(light[i].q.x))		// source at infinity
			{
				nn = 2*(p.u*light[i].p.x+p.v*light[i].p.y+p.w*light[i].p.z) /
				(p.u*p.u+p.v*p.v+p.w*p.w+1e-6);
				d0 = light[i].p.x - p.u*nn;
				d1 = light[i].p.y - p.v*nn;
				d2 = light[i].p.z - p.w*nn;
				nn = 1 + d2/sqrt(d0*d0+d1*d1+d2*d2+1e-6);

				nn = exp(-light[i].a*nn)*light[i].b*2;
				b0 += nn*light[i].c.r;
				b1 += nn*light[i].c.g;
				b2 += nn*light[i].c.b;
			}
			else if(get(MGL_DIFFUSIVE))		// diffuse light
			{
				d0 = light[i].q.x-p.x;	// direction to light source
				d1 = light[i].q.y-p.y;
				d2 = light[i].q.z-p.z;
				nn = 2*(d0*light[i].p.x+d1*light[i].p.y+d2*light[i].p.z)/(d0*d0+d1*d1+d2*d2+1e-6);
				nn = exp(-light[i].a*nn)*light[i].b*2;
				ar += nn*light[i].c.r;
				ag += nn*light[i].c.g;
				ab += nn*light[i].c.b;
			}
			else						// specular light
			{
				d0 = light[i].q.x-p.x;	// direction to light source
				d1 = light[i].q.y-p.y;
				d2 = light[i].q.z-p.z;
				nn = d0*d0 + d1*d1 + d2*d2 + 1e-6;
				float bb = 2*(d0*light[i].p.x+d1*light[i].p.y+d2*light[i].p.z)/nn;
				bb = exp(-light[i].a*nn)*light[i].b*2;
				// now difference for angles between normale and direction to light
				nn = 2*(p.u*d0+p.v*d1+p.w*d2)/(p.u*p.u+p.v*p.v+p.w*p.w+1e-6)/nn;
				d0 -= p.u*nn;	d1 -= p.v*nn;	d2 -= p.w*nn;
				nn = 1 + d2/sqrt(d0*d0+d1*d1+d2*d2+1e-6);
				// NOTE: here should be another aperture, but for simplicity I use the same
				nn = exp(-light[i].a*nn)*bb;
				b0 += nn*light[i].c.r;
				b1 += nn*light[i].c.g;
				b2 += nn*light[i].c.b;
			}
		}
		b0 += (ar>1 ? 1:ar)*p.r;	// diffuse light
		b1 += (ar>1 ? 1:ar)*p.g;
		b2 += (ar>1 ? 1:ar)*p.b;
		b0 = b0<1 ? b0 : 1;			// normalize components
		b1 = b1<1 ? b1 : 1;
		b2 = b2<1 ? b2 : 1;
	}
	else	{	b0=p.r;	b1=p.g;	b2=p.b;	}
	// try to highlight faces
	if(get(MGL_HIGHLIGHT))	{	b0*=0.7;	b1*=0.7;	b2*=0.7;	}
	r[0] = (unsigned char)(255*b0);
	r[1] = (unsigned char)(255*b1);
	r[2] = (unsigned char)(255*b2);
	// p.a should be <1
	r[3] = get(MGL_ENABLE_ALPHA) ? (unsigned char)(256*p.a) : 255;
	return r;
}
//-----------------------------------------------------------------------------
/// color mixing: color c1 is under color c2 !!!
void mglCanvas::combine(unsigned char *c1,unsigned char *c2)
{
	if(!c2[3])	return;
	register unsigned int a1=c1[3], a2=c2[3],b1=255-a2;
	if(a1==0 || a2==255)	{	memcpy(c1,c2,4);	return; }
	if((Flag&3)==0)
	{
		c1[0] = (c1[0]*b1 + c2[0]*a2)/256;
		c1[1] = (c1[1]*b1 + c2[1]*a2)/256;
		c1[2] = (c1[2]*b1 + c2[2]*a2)/256;
		c1[3] = (unsigned char)(a2+a1*b1/255);
	}
	else if((Flag&3)==1)
	{
		c1[0] = (unsigned char)((255-a1*(255-c1[0])/256)*(255-a2*(255-c2[0])/256)/256);
		c1[1] = (unsigned char)((255-a1*(255-c1[1])/256)*(255-a2*(255-c2[1])/256)/256);
		c1[2] = (unsigned char)((255-a1*(255-c1[2])/256)*(255-a2*(255-c2[2])/256)/256);
		c1[3] = 255;
	}
	else if((Flag&3)==2)
	{
		unsigned int b2,b3;
		b1 = (c1[0]*a1 + c2[0]*a2)/256;		c1[0] = b1<255 ? b1 : 255;
		b2 = (c1[1]*a1 + c2[1]*a2)/256;		c1[1] = b2<255 ? b2 : 255;
		b3 = (c1[2]*a1 + c2[2]*a2)/256;		c1[2] = b3<255 ? b3 : 255;
		c1[3] = a1+a2>255? 255 : a1+a2;
	}
}
//-----------------------------------------------------------------------------
unsigned char **mglCanvas::GetRGBLines(long &w, long &h, unsigned char *&f, bool alpha)
{
	long d = alpha ? 4:3;
	unsigned char **p;
	Finish();
	p = (unsigned char **)malloc(Height * sizeof(unsigned char *));
	for(long i=0;i<Height;i++)	p[i] = (alpha?G4:G)+d*Width*i;
	w = Width;	h = Height;		f = 0;
	return p;
}
//-----------------------------------------------------------------------------
/* Bilinear interpolation r(u,v) = r0 + (r1-r0)*u + (r2-r0)*v + (r3+r0-r1-r2)*u*v
	is used (where r is one of {x,y,z,R,G,B,A}. Variables u,v are determined
	for each point (x,y) and selected one pair which 0<u<1 and 0<v<1.*/
void mglCanvas::quad_draw(long k1, long k2, long k3, long k4, mglDrawReg *d)
{
	if(!(Quality&3))
	{
		fast_draw(k1,k2,d);	fast_draw(k1,k3,d);
		fast_draw(k4,k2,d);	fast_draw(k4,k3,d);	return;
	}
	clr(MGL_FINISHED);
	unsigned char r[4];
	long y1,x1,y2,x2;
	float dd,dsx,dsy;
	const mglPnt &p1=Pnt[k1], &p2=Pnt[k2], &p3=Pnt[k3], &p4=Pnt[k4];
	mglPnt d1=p2-p1, d2=p3-p1, d3=p4+p1-p2-p3, p;

	x1 = long(fmin(fmin(p1.x,p2.x),fmin(p3.x,p4.x)));	// bounding box
	y1 = long(fmin(fmin(p1.y,p2.y),fmin(p3.y,p4.y)));
	x2 = long(fmax(fmax(p1.x,p2.x),fmax(p3.x,p4.x)));
	y2 = long(fmax(fmax(p1.y,p2.y),fmax(p3.y,p4.y)));
	x1=x1>d->x1?x1:d->x1;	x2=x2<d->x2?x2:d->x2-1;
	y1=y1>d->y1?y1:d->y1;	y2=y2<d->y2?y2:d->y2-1;
	if(x1>x2 || y1>y2)	return;

	dd = d1.x*d2.y-d1.y*d2.x;
	dsx =-4*(d2.y*d3.x - d2.x*d3.y)*d1.y;
	dsy = 4*(d2.y*d3.x - d2.x*d3.y)*d1.x;

	if((d1.x==0 && d1.y==0) || (d2.x==0 && d2.y==0) || !(Quality&2))
	{	trig_draw(k1,k2,k4,true,d);	trig_draw(k1,k3,k4,true,d);	return;	}

	mglPoint n1 = mglPoint(p2.x-p1.x,p2.y-p1.y,p2.z-p1.z)^mglPoint(p3.x-p1.x,p3.y-p1.y,p3.z-p1.z);
	mglPoint n2 = mglPoint(p2.x-p4.x,p2.y-p4.y,p2.z-p4.z)^mglPoint(p3.x-p4.x,p3.y-p4.y,p3.z-p4.z);
	mglPoint nr = (n1+n2)*0.5;

	register long i,j,g;
	register float u,v,s,xx,yy,q;
	float x0 = p1.x, y0 = p1.y;
	for(i=x1;i<=x2;i++)	for(j=y1;j<=y2;j++)
	{
		xx = (i-x0);	yy = (j-y0);
		s = dsx*xx + dsy*yy + (dd+d3.y*xx-d3.x*yy)*(dd+d3.y*xx-d3.x*yy);
		if(s<0)	continue;	// no solution
		s = sqrt(s);
		q = d3.x*yy - d3.y*xx + dd + s;
		u = q ? 2.f*(d2.y*xx - d2.x*yy)/q : -1.f;
		q = d3.y*xx - d3.x*yy + dd + s;
		v = q ? 2.f*(d1.x*yy - d1.y*xx)/q : -1.f;
		g = u<0.f || u>1.f || v<0.f || v>1.f;
		if(g)	// first root bad
		{
			q = d3.x*yy - d3.y*xx + dd - s;
			u = q ? 2.f*(d2.y*xx - d2.x*yy)/q : -1.f;
			q = d3.y*xx - d3.x*yy + dd - s;
			v = q ? 2.f*(d1.x*yy - d1.y*xx)/q : -1.f;
			g = u<0.f || u>1.f || v<0.f || v>1.f;
			if(g)	continue;	// second root bad
		}
		p = p1+d1*u+d2*v+d3*(u*v);
		if(isnan(p.u))
		{	p.u = nr.x;	p.v = nr.y;	p.w = nr.z;	}
		pnt_plot(i,j,p.z,col2int(p,r));
	}
}
//-----------------------------------------------------------------------------
/* Linear interpolation r(u,v) = r0 + (r1-r0)*u + (r2-r0)*v is used, where r is
	one of {x,y,z,R,G,B,A}. Variables u,v are determined for each point (x,y).
	Point plotted is u>0 and v>0 and u+v<1.*/
void mglCanvas::trig_draw(long k1, long k2, long k3, bool anorm, mglDrawReg *d)
{
	if(!(Quality&3))
	{
		fast_draw(k1,k2,d);	fast_draw(k1,k3,d);
		fast_draw(k2,k3,d);	return;
	}
	clr(MGL_FINISHED);
	unsigned char r[4];
	long y1,x1,y2,x2;
	float dxu,dxv,dyu,dyv;
	const mglPnt &p1=Pnt[k1], &p2=Pnt[k2], &p3=Pnt[k3];
	mglPnt d1=p2-p1, d2=p3-p1, p;

	dxu = d2.x*d1.y - d1.x*d2.y;
	if(fabs(dxu)<1e-5)	return;		// points lies on the same line
	dyv =-d1.x/dxu;	dxv = d1.y/dxu;
	dyu = d2.x/dxu;	dxu =-d2.y/dxu;

	x1 = long(fmin(fmin(p1.x,p2.x),p3.x));	// bounding box
	y1 = long(fmin(fmin(p1.y,p2.y),p3.y));
	x2 = long(fmax(fmax(p1.x,p2.x),p3.x));
	y2 = long(fmax(fmax(p1.y,p2.y),p3.y));
	x1=x1>d->x1?x1:d->x1;	x2=x2<d->x2?x2:d->x2-1;
	y1=y1>d->y1?y1:d->y1;	y2=y2<d->y2?y2:d->y2-1;
	if(x1>x2 || y1>y2)	return;
	// default normale
	mglPoint nr = mglPoint(p2.x-p1.x,p2.y-p1.y,p2.z-p1.z)^mglPoint(p3.x-p1.x,p3.y-p1.y,p3.z-p1.z);

	register float u,v,xx,yy;
	register long i,j,g;
	float x0 = p1.x, y0 = p1.y;
	for(i=x1;i<=x2;i++)	for(j=y1;j<=y2;j++)
	{
		xx = (i-x0);	yy = (j-y0);
		u = dxu*xx+dyu*yy;	v = dxv*xx+dyv*yy;
		g = u<0 || v<0 || u+v>1;
		if(g)	continue;
		if(Quality&2)	// slow but accurate
		{
			p = p1+d1*u+d2*v;
			if(isnan(p.u) && anorm)
			{	p.u = nr.x;	p.v = nr.y;	p.w = nr.z;	}
			pnt_plot(i,j,p.z,col2int(p,r));
		}
		else	pnt_plot(i,j,p1.z,col2int(p1,r));
	}
}
//-----------------------------------------------------------------------------
void mglCanvas::line_draw(long k1, long k2, mglDrawReg *dr)
{
	if(!(Quality&3))	{	fast_draw(k1,k2,dr);	return;	}
	clr(MGL_FINISHED);
	unsigned char r[4];
	long y1,x1,y2,x2;

	float pw=PenWidth*sqrt(font_factor/400), dxu,dxv,dyu,dyv,dd;
	if(get(MGL_HIGHLIGHT))	pw *= 2;
	const mglPnt &p1=Pnt[k1], &p2=Pnt[k2];
	mglPnt d=p2-p1, p;
	bool hor = fabs(d.x)>fabs(d.y);

	x1 = long(fmin(p1.x,p2.x));	y1 = long(fmin(p1.y,p2.y));	// bounding box
	x2 = long(fmax(p1.x,p2.x));	y2 = long(fmax(p1.y,p2.y));
	x1=x1>dr->x1?x1:dr->x1;	x2=x2<dr->x2?x2:dr->x2-1;
	y1=y1>dr->y1?y1:dr->y1;	y2=y2<dr->y2?y2:dr->y2-1;
	dd = sqrt(d.x*d.x + d.y*d.y);
	if(x1>x2 || y1>y2 || dd<1e-5)	return;

	dxv = d.y/dd;	dyv =-d.x/dd;
	dxu = d.x/dd;	dyu = d.y/dd;

	bool aa=get(MGL_ENABLE_ALPHA);
	register float u,v,xx,yy;
	register long i,j;
	set(MGL_ENABLE_ALPHA);
	if(hor)	for(i=x1;i<=x2;i++)
	{
		y1 = int(p1.y+d.y*(i-p1.x)/d.x - pw - 3.5);
		y2 = int(p1.y+d.y*(i-p1.x)/d.x + pw + 3.5);
		y1=y1>dr->y1?y1:dr->y1;	y2=y2<dr->y2?y2:dr->y2-1;
		if(y1>y2)	continue;
		for(j=y1;j<=y2;j++)
		{
			xx = (i-p1.x);	yy = (j-p1.y);
			u = dxu*xx+dyu*yy;	v = dxv*xx+dyv*yy;	v = v*v;
			if(u<0)			{	v += u*u;			u = 0;	}
			else if(u>dd)	{	v += (u-dd)*(u-dd);	u = dd;	}
			if(v>pw*pw)		continue;
			if(!( PDef & ( 1<<long(fmod(pPos+u/pw/1.5, 16)) ) ))	continue;
			p = p1+d*(u/dd);	col2int(p,r);
//			r[3] = (unsigned char)(255/cosh(3.f*sqrt(v/pw/pw)));
			r[3] = v<(pw-1)*(pw-1)/4 ? 255 : (unsigned char)(255/cosh(3.f*(sqrt(v)+(1-pw)/2)));
			pnt_plot(i,j,p.z+pw,r);
		}
	}
	else	for(j=y1;j<=y2;j++)
	{
		x1 = int(p1.x+d.x*(j-p1.y)/d.y - pw - 3.5);
		x2 = int(p1.x+d.x*(j-p1.y)/d.y + pw + 3.5);
		x1=x1>dr->x1?x1:dr->x1;	x2=x2<dr->x2?x2:dr->x2-1;
		if(x1>x2)	continue;

		for(i=x1;i<=x2;i++)
		{
			xx = (i-p1.x);	yy = (j-p1.y);
			u = dxu*xx+dyu*yy;	v = dxv*xx+dyv*yy;	v = v*v;
			if(u<0)			{	v += u*u;			u = 0;	}
			else if(u>dd)	{	v += (u-dd)*(u-dd);	u = dd;	}
			if(v>pw*pw)		continue;
			if(!(PDef & (1<<long(fmod(pPos+u/pw/1.5, 16)))))		continue;
			p = p1+d*(u/dd);	col2int(p,r);
//			r[3] = (unsigned char)(255/cosh(3.f*sqrt(v/pw/pw)));
//			r[3] = v<(pw-1)*(pw-1) ? 255 : (unsigned char)(255/cosh(3.f*(sqrt(v)+1-pw)));
//			r[3] = v<pw*pw/4 ? 255 : (unsigned char)(255/cosh(3.f*(sqrt(v)-pw/2)));
			r[3] = v<(pw-1)*(pw-1)/4 ? 255 : (unsigned char)(255/cosh(3.f*(sqrt(v)+(1-pw)/2)));
			pnt_plot(i,j,p.z+pw,r);
		}
	}
	set(aa,MGL_ENABLE_ALPHA);
}
//-----------------------------------------------------------------------------
void mglCanvas::fast_draw(long k1, long k2, mglDrawReg *dr)
{
	clr(MGL_FINISHED);
	const mglPnt &p1=Pnt[k1], &p2=Pnt[k2];
	mglPnt d=p2-p1;
	unsigned char r[4];	col2int(p1,r);
	long y1,x1,y2,x2;

	float pw = PenWidth*sqrt(font_factor/400);
	bool hor = fabs(d.x)>fabs(d.y);

	x1 = long(fmin(p1.x,p2.x));	y1 = long(fmin(p1.y,p2.y));	// bounding box
	x2 = long(fmax(p1.x,p2.x));	y2 = long(fmax(p1.y,p2.y));
	x1=x1>dr->x1?x1:dr->x1;	x2=x2<dr->x2?x2:dr->x2-1;
	y1=y1>dr->y1?y1:dr->y1;	y2=y2<dr->y2?y2:dr->y2-1;
	if(x1>x2 || y1>y2)	return;

	register long i;
	if(hor)	for(i=x1;i<=x2;i++)
		pnt_plot(i, p1.y+d.y*(i-p1.x)/d.x, p1.z+d.z*(i-p1.x)/d.x+pw, r);
	else	for(i=y1;i<=y2;i++)
		pnt_plot(p1.x+d.x*(i-p1.y)/d.y, i, p1.z+d.z*(i-p1.y)/d.y+pw, r);
}
//-----------------------------------------------------------------------------
void mglCanvas::pnt_draw(long k, mglDrawReg *dr)
{
	bool aa=get(MGL_ENABLE_ALPHA);	set(MGL_ENABLE_ALPHA);
	register long i,j,s,x,y;
	register float v,pw=PenWidth*sqrt(font_factor/400);
	const mglPnt &p=Pnt[k];
	unsigned char cs[4], cc;	col2int(p,cs);	cc = cs[3];
	s = long(5.5+fabs(pw));
	for(j=-s;j<=s;j++)	for(i=-s;i<=s;i++)
	{
		v = (i*i+j*j)/(9*pw*pw);
		cs[3] = (unsigned char)(cc*exp(-6*v));
		if(cs[3]==0)	continue;
		x=p.x+i;	y=p.y+j;
		if(x>=dr->x1 && x<=dr->x2 && y>=dr->y1 && y<=dr->y2)
			pnt_plot(p.x+i,p.y+j,p.z,cs);
	}
	set(aa,MGL_ENABLE_ALPHA);
}
//-----------------------------------------------------------------------------
void mglCanvas::mark_draw(long k, char type, float size, mglDrawReg *d)
{
	const mglPnt &q=Pnt[k];
	unsigned char cs[4];	col2int(q,cs);	cs[3] = size>0 ? 255 : 255*q.t;
	unsigned long pos = Pnt.size(), qos=pos;
	mglPnt p=q;
	float ss=fabs(size)*0.35*font_factor;
	register long i,j;
#ifdef HAVE_PTHREAD
	pthread_mutex_lock(&mutexPnt);
#endif
	if(type=='.' || ss==0)	pnt_draw(k,d);
	else
	{
		float pw = PenWidth;	PenWidth = 1;
		int pd = PDef;	PDef = 0xffff;
		if(!strchr("xsSoO",type))	ss *= 1.1;
		switch(type)
		{
		case 'P':
			p.x = q.x-ss;	p.y = q.y-ss;	Pnt.push_back(p);
			p.x = q.x+ss;	p.y = q.y-ss;	Pnt.push_back(p);	line_draw(pos,pos+1,d);
			p.x = q.x+ss;	p.y = q.y+ss;	Pnt.push_back(p);	line_draw(pos+1,pos+2,d);
			p.x = q.x-ss;	p.y = q.y+ss;	Pnt.push_back(p);	line_draw(pos+2,pos+3,d);
			line_draw(pos+3,pos,d);	qos+=4;
		case '+':
			p.x = q.x-ss;	p.y = q.y;		Pnt.push_back(p);
			p.x = q.x+ss;	p.y = q.y;		Pnt.push_back(p);	line_draw(qos,qos+1,d);
			p.x = q.x;	p.y = q.y-ss;		Pnt.push_back(p);
			p.x = q.x;	p.y = q.y+ss;		Pnt.push_back(p);	line_draw(qos+2,qos+3,d);
			break;
		case 'X':
			p.x = q.x-ss;	p.y = q.y-ss;	Pnt.push_back(p);
			p.x = q.x+ss;	p.y = q.y-ss;	Pnt.push_back(p);	line_draw(pos,pos+1,d);
			p.x = q.x+ss;	p.y = q.y+ss;	Pnt.push_back(p);	line_draw(pos+1,pos+2,d);
			p.x = q.x-ss;	p.y = q.y+ss;	Pnt.push_back(p);	line_draw(pos+2,pos+3,d);
			line_draw(pos+3,pos,d);	qos+=4;
		case 'x':
			p.x = q.x-ss;	p.y = q.y-ss;	Pnt.push_back(p);
			p.x = q.x+ss;	p.y = q.y+ss;	Pnt.push_back(p);	line_draw(qos,qos+1,d);
			p.x = q.x+ss;	p.y = q.y-ss;	Pnt.push_back(p);
			p.x = q.x-ss;	p.y = q.y+ss;	Pnt.push_back(p);	line_draw(qos+2,qos+3,d);
			break;
		case 'S':
			p.x = q.x-ss;	p.y = q.y-ss;	Pnt.push_back(p);
			p.x = q.x-ss;	p.y = q.y+ss;	Pnt.push_back(p);
			p.x= q.x+ss;	p.y= q.y+ss;	Pnt.push_back(p);
			p.x = q.x+ss;	p.y = q.y-ss;	Pnt.push_back(p);
			quad_draw(pos,pos+1,pos+3,pos+2,d);	qos+=4;
		case 's':
			p.x = q.x-ss;	p.y = q.y-ss;	Pnt.push_back(p);
			p.x = q.x+ss;	p.y = q.y-ss;	Pnt.push_back(p);	line_draw(qos,qos+1,d);
			p.x = q.x+ss;	p.y = q.y+ss;	Pnt.push_back(p);	line_draw(qos+1,qos+2,d);
			p.x = q.x-ss;	p.y = q.y+ss;	Pnt.push_back(p);	line_draw(qos+2,qos+3,d);
			line_draw(qos+3,qos,d);	break;
		case 'D':
			p.x = q.x;	p.y = q.y-ss;		Pnt.push_back(p);
			p.x = q.x+ss;	p.y = q.y;		Pnt.push_back(p);
			p.x= q.x;	p.y= q.y+ss;		Pnt.push_back(p);
			p.x = q.x-ss;	p.y = q.y;		Pnt.push_back(p);
			quad_draw(pos,pos+1,pos+3,pos+2,d);	qos+=4;
		case 'd':
			p.x = q.x;	p.y = q.y-ss;		Pnt.push_back(p);
			p.x = q.x+ss;	p.y = q.y;		Pnt.push_back(p);	line_draw(qos,qos+1,d);
			p.x = q.x;	p.y = q.y+ss;		Pnt.push_back(p);	line_draw(qos+1,qos+2,d);
			p.x = q.x-ss;	p.y = q.y;		Pnt.push_back(p);	line_draw(qos+2,qos+3,d);
			line_draw(qos+3,qos,d);	break;
		case 'Y':
			p.x = q.x;	p.y = q.y;			Pnt.push_back(p);
			p.x = q.x;	p.y = q.y-ss;		Pnt.push_back(p);	line_draw(pos,pos+1,d);
			p.x = q.x-0.8*ss;	p.y = q.y+0.6*ss;	Pnt.push_back(p);	line_draw(pos,pos+2,d);
			p.x = q.x+0.8*ss;	p.y = q.y+0.6*ss;	Pnt.push_back(p);	line_draw(pos,pos+3,d);
			break;
		case '*':
			p.x = q.x-ss;		p.y = q.y;	Pnt.push_back(p);
			p.x = q.x+ss;		p.y = q.y;	Pnt.push_back(p);	line_draw(pos,pos+1,d);
			p.x = q.x-0.6*ss;	p.y = q.y-0.8*ss;	Pnt.push_back(p);
			p.x = q.x+0.6*ss;	p.y = q.y+0.8*ss;	Pnt.push_back(p);	line_draw(pos+2,pos+3,d);
			p.x = q.x-0.6*ss;	p.y = q.y+0.8*ss;	Pnt.push_back(p);
			p.x = q.x+0.6*ss;	p.y = q.y-0.8*ss;	Pnt.push_back(p);	line_draw(pos+4,pos+5,d);
			break;
		case 'T':
			p.x = q.x-ss;	p.y = q.y-ss/2;	Pnt.push_back(p);
			p.x = q.x+ss;	p.y = q.y-ss/2;	Pnt.push_back(p);
			p.x= q.x;		p.y= q.y+ss;	Pnt.push_back(p);
			trig_draw(pos,pos+1,pos+2,false,d);	qos+=3;
		case '^':
			p.x = q.x-ss;	p.y = q.y-ss/2;	Pnt.push_back(p);
			p.x = q.x+ss;	p.y = q.y-ss/2;	Pnt.push_back(p);	line_draw(qos,qos+1,d);
			p.x= q.x;		p.y= q.y+ss;	Pnt.push_back(p);	line_draw(qos+1,qos+2,d);
			line_draw(qos+2,qos,d);		break;
		case 'V':
			p.x = q.x-ss;	p.y = q.y+ss/2;	Pnt.push_back(p);
			p.x = q.x+ss;	p.y = q.y+ss/2;	Pnt.push_back(p);
			p.x= q.x;		p.y= q.y-ss;	Pnt.push_back(p);
			trig_draw(pos,pos+1,pos+2,false,d);	qos+=3;
		case 'v':
			p.x = q.x-ss;	p.y = q.y+ss/2;	Pnt.push_back(p);
			p.x = q.x+ss;	p.y = q.y+ss/2;	Pnt.push_back(p);	line_draw(qos,qos+1,d);
			p.x= q.x;		p.y= q.y-ss;	Pnt.push_back(p);	line_draw(qos+1,qos+2,d);
			line_draw(qos+2,qos,d);		break;
		case 'L':
			p.x = q.x+ss/2;	p.y = q.y+ss;	Pnt.push_back(p);
			p.x = q.x+ss/2;	p.y = q.y-ss;	Pnt.push_back(p);
			p.x= q.x-ss;	p.y= q.y;		Pnt.push_back(p);
			trig_draw(pos,pos+1,pos+2,false,d);	qos+=3;
		case '<':
			p.x = q.x+ss/2;	p.y = q.y+ss;	Pnt.push_back(p);
			p.x = q.x+ss/2;	p.y = q.y-ss;	Pnt.push_back(p);	line_draw(qos,qos+1,d);
			p.x= q.x-ss;	p.y= q.y;		Pnt.push_back(p);	line_draw(qos+1,qos+2,d);
			line_draw(qos+2,qos,d);		break;
		case 'R':
			p.x = q.x-ss/2;	p.y = q.y+ss;	Pnt.push_back(p);
			p.x = q.x-ss/2;	p.y = q.y-ss;	Pnt.push_back(p);
			p.x= q.x+ss;	p.y= q.y;		Pnt.push_back(p);
			trig_draw(pos,pos+1,pos+2,false,d);	qos+=3;
		case '>':
			p.x = q.x-ss/2;	p.y = q.y+ss;	Pnt.push_back(p);
			p.x = q.x-ss/2;	p.y = q.y-ss;	Pnt.push_back(p);	line_draw(qos,qos+1,d);
			p.x= q.x+ss;	p.y= q.y;		Pnt.push_back(p);	line_draw(qos+1,qos+2,d);
			line_draw(qos+2,qos,d);		break;
		case 'O':
			for(j=long(-ss);j<=long(ss);j++)	for(i=long(-ss);i<=long(ss);i++)
			{
				register long x=long(q.x)+i, y=long(q.y)+j;
				if(i*i+j*j>=ss*ss || x<d->x1 || x>d->x2 || y<d->y1 || y>d->y2)	continue;
				pnt_plot(x,y,q.z+1,cs);
			}
		case 'o':
			for(i=0;i<=20;i++)
			{
				p.x = q.x+ss*cos(i*M_PI/10);	p.y = q.y+ss*sin(i*M_PI/10);	Pnt.push_back(p);
				if(i>0)	line_draw(pos+i-1,pos+i,d);
			}
			break;
		case 'C':
			pnt_draw(k,d);
			for(i=0;i<=20;i++)
			{
				p.x = q.x+ss*cos(i*M_PI/10);	p.y = q.y+ss*sin(i*M_PI/10);	Pnt.push_back(p);
				if(i>0)	line_draw(pos+i-1,pos+i,d);
			}
			break;
		}
		PDef = pd;	PenWidth = pw;
		Pnt.erase(Pnt.begin()+pos,Pnt.end());
	}
#ifdef HAVE_PTHREAD
	pthread_mutex_unlock(&mutexPnt);
#endif
}
//-----------------------------------------------------------------------------
void mglCanvas::glyph_draw(const mglPrim *P, mglDrawReg *d)
{
	mglPnt p=Pnt[P->n1];
	float f = p.w;
#ifdef HAVE_PTHREAD
	pthread_mutex_lock(&mutexPnt);
#endif
	Push();		B.clear();
	B.b[0] = B.b[4] = B.b[8] = P->s*P->p;
	RotateN(P->w,0,0,1);	B.pf = P->p;
	B.x=p.x;	B.y=p.y;	B.z=p.z;

	int ss=P->n3&3;
	if(P->n3&8)
	{
		if(!(P->n3&4))	glyph_line(p,f,true, d);
		glyph_line(p,f,false, d);
	}
	else
	{
		if(!(P->n3&4))	glyph_fill(p,f,fnt->GetNt(ss,P->n4),fnt->GetTr(ss,P->n4), d);
		glyph_wire(p,f,fnt->GetNl(ss,P->n4),fnt->GetLn(ss,P->n4), d);
	}
	Pop();
#ifdef HAVE_PTHREAD
	pthread_mutex_unlock(&mutexPnt);
#endif
}
//-----------------------------------------------------------------------------
void mglCanvas::glyph_fill(const mglPnt &pp, float f, int nt, const short *trig, mglDrawReg *d)
{
	if(!trig || nt<=0)	return;
	long ik,ii,pos=Pnt.size();
	mglPnt p=pp;	p.u=NAN;
	float pw = Width>2 ? fabs(PenWidth) : 1e-5*Width;

	mglPoint p1,p2,p3;
	for(ik=0;ik<nt;ik++)
	{
		ii = 6*ik;	p1 = mglPoint(f*trig[ii]+pp.u,f*trig[ii+1]+pp.v,0);	PostScale(p1);
		ii+=2;		p2 = mglPoint(f*trig[ii]+pp.u,f*trig[ii+1]+pp.v,0);	PostScale(p2);
		ii+=2;		p3 = mglPoint(f*trig[ii]+pp.u,f*trig[ii+1]+pp.v,0);	PostScale(p3);
		p.x = p1.x;	p.y = p1.y;	p.z = p1.z+pw;	Pnt.push_back(p);
		p.x = p2.x;	p.y = p2.y;	p.z = p2.z+pw;	Pnt.push_back(p);
		p.x = p3.x;	p.y = p3.y;	p.z = p3.z+pw;	Pnt.push_back(p);
		ii = Pnt.size()-3;	trig_draw(ii,ii+1,ii+2,false,d);
	}
	Pnt.erase(Pnt.begin()+pos,Pnt.end());
}
//-----------------------------------------------------------------------------
void mglCanvas::glyph_wire(const mglPnt &pp, float f, int nl, const short *line, mglDrawReg *d)
{
	if(!line || nl<=0)	return;
	long ik,ii,il=0,pos=Pnt.size();
	mglPnt p=pp;	p.u=NAN;
	unsigned pdef=PDef;	PDef = 0xffff;
	float opw=PenWidth;	PenWidth=0.75;
	mglPoint p1,p2;
	for(ik=0;ik<nl;ik++)
	{
		ii = 2*ik;
		if(line[ii]==0x3fff && line[ii+1]==0x3fff)	// line breakthrough
		{	il = ik+1;	continue;	}
		else if(ik==nl-1 || (line[ii+2]==0x3fff && line[ii+3]==0x3fff))
		{	// enclose the circle. May be in future this block should be commented
			p1 = mglPoint(f*line[ii]+pp.u,f*line[ii+1]+pp.v,0);	ii=2*il;
			p2 = mglPoint(f*line[ii]+pp.u,f*line[ii+1]+pp.v,0);
		}
		else
		{	// normal line
			p1 = mglPoint(f*line[ii]+pp.u,f*line[ii+1]+pp.v,0);	ii+=2;
			p2 = mglPoint(f*line[ii]+pp.u,f*line[ii+1]+pp.v,0);
		}
		PostScale(p1);	PostScale(p2);
		p.x = p1.x;	p.y = p1.y;	p.z = p1.z;	Pnt.push_back(p);
		p.x = p2.x;	p.y = p2.y;	p.z = p2.z;	Pnt.push_back(p);
		ii = Pnt.size()-2;	line_draw(ii,ii+1,d);
	}
	PDef = pdef;	PenWidth = opw;
	Pnt.erase(Pnt.begin()+pos,Pnt.end());
}
//-----------------------------------------------------------------------------
void mglCanvas::glyph_line(const mglPnt &pp, float f, bool solid, mglDrawReg *d)
{
	mglPnt p=pp;	p.u=NAN;
	float pw = Width>2 ? fabs(PenWidth) : 1e-5*Width;
	unsigned pdef=PDef;	PDef = 0xffff;
	float opw=PenWidth;	PenWidth=1;
	mglPoint p1,p2,p3,p4;
	long pos=Pnt.size();

	float dy = 0.004;
	p1 = mglPoint(pp.u,pp.v-dy,0);	PostScale(p1);
	p2 = mglPoint(pp.u,pp.v+dy,0);	PostScale(p2);
	p3 = mglPoint(fabs(f)+pp.u,pp.v+dy,0);	PostScale(p3);
	p4 = mglPoint(fabs(f)+pp.u,pp.v-dy,0);	PostScale(p4);

	p.x = p1.x;	p.y = p1.y;	p.z = p1.z+(solid?pw:0);	Pnt.push_back(p);
	p.x = p2.x;	p.y = p2.y;	p.z = p2.z+(solid?pw:0);	Pnt.push_back(p);
	p.x = p3.x;	p.y = p3.y;	p.z = p3.z+(solid?pw:0);	Pnt.push_back(p);
	p.x = p4.x;	p.y = p4.y;	p.z = p4.z+(solid?pw:0);	Pnt.push_back(p);

	if(solid)	quad_draw(pos,pos+1,pos+3,pos+2,d);
	else
	{
		line_draw(pos,pos+1,d);	line_draw(pos+2,pos+1,d);
		line_draw(pos,pos+3,d);	line_draw(pos+2,pos+3,d);
	}
	PDef = pdef;	PenWidth=opw;
	Pnt.erase(Pnt.begin()+pos,Pnt.end());
}
//-----------------------------------------------------------------------------
