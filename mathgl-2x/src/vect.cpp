/***************************************************************************
 * vect.cpp is part of Math Graphic Library
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
#include "mgl/vect.h"
#include "mgl/eval.h"
#include "mgl/data.h"
#include <stdlib.h>
//-----------------------------------------------------------------------------
//
//	Traj series
//
//-----------------------------------------------------------------------------
void mgl_traj_xyz(HMGL gr, HCDT x, HCDT y, HCDT z, HCDT ax, HCDT ay, HCDT az, const char *sch, const char *opt)
{
	long m,mx,my,mz,nx,ny,nz,n=ax->GetNx(),pal;
	if(n<2)	{	gr->SetWarn(mglWarnLow,"Traj");	return;	}
	if(n!=x->GetNx() || z->GetNx()!=n || y->GetNx()!=n || ay->GetNx()!=n || az->GetNx()!=n)
	{	gr->SetWarn(mglWarnDim,"Traj");	return;	}
	float len=gr->SaveState(opt);
	if(isnan(len))	len = isnan(gr->PrevValue()) ? gr->PrevValue():0;
	static int cgid=1;	gr->StartGroup("Traj",cgid++);

	register long i, j;
	// find maximum
	i = ax->GetNy()>ay->GetNy() ? ax->GetNy():ay->GetNy();	j = z->GetNy()>az->GetNy() ? z->GetNy():az->GetNy();
	m = x->GetNy()>y->GetNy() ? x->GetNy():y->GetNy();		if(i>m)	m=i;	if(j>m)	m=j;
	gr->SetPenPal(sch,&pal);	gr->Reserve(4*n*m);

	float dx,dy,dz,dd,da,xm=0;
	mglPoint p1,p2;
	for(j=0;j<m;j++)	for(i=0;i<n;i++)	// find maximal amplitude of vector field
	{
		mx = j<ax->GetNy() ? j:0;	my = j<ay->GetNy() ? j:0;	mz = j<az->GetNy() ? j:0;
		da = sqrt(ax->v(i,mx)*ax->v(i,mx)+ay->v(i,my)*ay->v(i,my)+az->v(i,mz)*az->v(i,mz));
		xm = xm>da ? xm : da;
	}
	xm = 1./(xm ? sqrt(xm):1);
	for(j=0;j<m;j++) // start prepare arrows
	{
		gr->NextColor(pal);
		for(i=0;i<n;i++)
		{
			nx = j<x->GetNy() ? j:0;	ny = j<y->GetNy() ? j:0;	nz = j<z->GetNy() ? j:0;
			mx = j<ax->GetNy() ? j:0;	my = j<ay->GetNy() ? j:0;	mz = j<az->GetNy() ? j:0;
			da = sqrt(ax->v(i,mx)*ax->v(i,mx)+ay->v(i,my)*ay->v(i,my)+az->v(i,mz)*az->v(i,mz));
			if(len==0)
			{
				if(i<n-1)
				{	dx=x->v(i+1,nx)-x->v(i,nx);	dy=y->v(i+1,ny)-y->v(i,ny);	dz=z->v(i+1,nz)-z->v(i,nz);	}
				else
				{	dx=x->v(i,nx)-x->v(i-1,nx);	dy=y->v(i,ny)-y->v(i-1,ny);	dz=z->v(i,nz)-z->v(i-1,nz);	}
				dd = da ? 1/da : 0;		dd *= sqrt(dx*dx+dy*dy+dz*dz);
			}
			else dd = len;

			p1 = mglPoint(x->v(i,nx), y->v(i,ny), z->v(i,nz));
			p2 = mglPoint(x->v(i,nx)+dd*ax->v(i,mx), y->v(i,ny)+dd*ay->v(i,my), z->v(i,nz)+dd*az->v(i,mz));
			nx = gr->AddPnt(p1,gr->CDef);	ny = gr->AddPnt(p2,gr->CDef);
			gr->vect_plot(nx,ny);
		}
	}
	gr->EndGroup();
}
//-----------------------------------------------------------------------------
void mgl_traj_xy(HMGL gr, HCDT x, HCDT y, HCDT ax, HCDT ay, const char *sch, const char *opt)
{
	if(ax->GetNx()<2)	{	gr->SetWarn(mglWarnLow,"Traj");	return;	}
	if(x->GetNx()!=ax->GetNx() || y->GetNx()!=ax->GetNx() || ay->GetNx()!=ax->GetNx())
	{	gr->SetWarn(mglWarnDim,"Traj");	return;	}
	gr->SaveState(opt);
	mglData z(x->GetNx()), az(x->GetNx());	z.Fill(gr->Min.z,gr->Min.z);
	mgl_traj_xyz(gr,x,y,&z,ax,ay,&az,sch,0);
}
//-----------------------------------------------------------------------------
void mgl_traj_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *ax, uintptr_t *ay, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_traj_xy(_GR_, _DA_(x), _DA_(y), _DA_(ax), _DA_(ay), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void mgl_traj_xyz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_traj_xyz(_GR_, _DA_(x), _DA_(y), _DA_(z), _DA_(ax), _DA_(ay), _DA_(az), s, o);
	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
//
//	Vect series
//
//-----------------------------------------------------------------------------
void mgl_vect_xy(HMGL gr, HCDT x, HCDT y, HCDT ax, HCDT ay, const char *sch, const char *opt)
{
	long i,j,n=ax->GetNx(),m=ax->GetNy(),k;
	if(n*m*ax->GetNz()!=ay->GetNx()*ay->GetNy()*ay->GetNz())	{	gr->SetWarn(mglWarnDim,"Vect");	return;	}
	if(n<2 || m<2)						{	gr->SetWarn(mglWarnLow,"Vect");	return;	}
	bool both = x->GetNx()==n && y->GetNx()==n && x->GetNy()==m && y->GetNy()==m;
	if(!(both || (x->GetNx()==n && y->GetNx()==m)))	{	gr->SetWarn(mglWarnDim,"Vect");	return;	}
	float r = gr->SaveState(opt);
	long flag = isnan(r) ? 0:long(r+0.5);
	static int cgid=1;	gr->StartGroup("Vect",cgid++);

	long ss = gr->AddTexture(sch);
	gr->Reserve(4*n*m);
	float	zVal = gr->Min.z;

	long tx=1,ty=1;
	if(gr->MeshNum>1)	{	tx=(n-1)/(gr->MeshNum-1);	ty=(m-1)/(gr->MeshNum-1);	}
	if(tx<1)	tx=1;	if(ty<1)	ty=1;
	float xm=0,ym,dx,dy;
	float dd,dm=(fabs(gr->Max.c)+fabs(gr->Min.c))*1e-5;

	for(k=0;k<ax->GetNz();k++)	for(j=0;j<m;j++)	for(i=0;i<n;i++)
	{
		ym = ax->v(i,j,k)*ax->v(i,j,k)+ay->v(i,j,k)*ay->v(i,j,k);
		xm = xm>ym ? xm : ym;
	}
	xm = 1./(xm==0 ? 1:sqrt(xm));
	long n1,n2;
	mglPoint p1,p2;
	float c1,c2, xx,yy;

	for(k=0;k<ax->GetNz();k++)
	{
		if(ax->GetNz()>1)	zVal = gr->Min.z+(gr->Max.z-gr->Min.z)*float(k)/(ax->GetNz()-1);
		for(i=0;i<n;i+=tx)	for(j=0;j<m;j+=ty)
		{
			xx = GetX(x,i,j,k).x;	yy = GetY(y,i,j,k).x;
			dx = i<n-1 ? (GetX(x,i+1,j,k).x-xx) : (xx-GetX(x,i-1,j,k).x);
			dy = j<m-1 ? (GetY(y,i,j+1,k).x-yy) : (yy-GetY(y,i,j-1,k).x);
			dx *= tx;	dy *= ty;	dd = hypot(ax->v(i,j,k),ay->v(i,j,k));
			dx *= (flag&MGL_VEC_LEN) ? (dd>dm ? ax->v(i,j,k)/dd : 0) : ax->v(i,j,k)*xm;
			dy *= (flag&MGL_VEC_LEN) ? (dd>dm ? ay->v(i,j,k)/dd : 0) : ay->v(i,j,k)*xm;

			if(flag & MGL_VEC_END)
			{	p1 = mglPoint(xx-dx,yy-dy,zVal);	p2 = mglPoint(xx,yy,zVal);	}
			else if(flag & MGL_VEC_MID)
			{	p1=mglPoint(xx-dx/2,yy-dy/2,zVal);	p2=mglPoint(xx+dx/2,yy+dy/2,zVal);	}
			else
			{	p1 = mglPoint(xx,yy,zVal);	p2 = mglPoint(xx+dx,yy+dy,zVal);	}
			if(flag&MGL_VEC_COL)	{	c1 = c2 = ss;	}	else
			{	c1=gr->GetC(ss,dd*xm*1.5-1,false);	c2=gr->GetC(ss,dd*xm*1.5-0.5,false);}
			n1=gr->AddPnt(p1,c1);	n2=gr->AddPnt(p2,c2);
			if(flag & MGL_VEC_DOT)	gr->line_plot(n1,n2);
			else	gr->vect_plot(n1,n2);
		}
	}
	gr->EndGroup();
}
//-----------------------------------------------------------------------------
void mgl_vect_2d(HMGL gr, HCDT ax, HCDT ay, const char *sch, const char *opt)
{
	gr->SaveState(opt);
	mglData x(ax->GetNx()), y(ax->GetNy());
	x.Fill(gr->Min.x,gr->Max.x);
	y.Fill(gr->Min.y,gr->Max.y);
	mgl_vect_xy(gr,&x,&y,ax,ay,sch,0);
}
//-----------------------------------------------------------------------------
void mgl_vect_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *ax, uintptr_t *ay, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_vect_xy(_GR_, _DA_(x), _DA_(y), _DA_(ax), _DA_(ay), s, o);
	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void mgl_vect_2d_(uintptr_t *gr, uintptr_t *ax, uintptr_t *ay, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_vect_2d(_GR_, _DA_(ax), _DA_(ay), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
//
//	Vect3 series
//
//-----------------------------------------------------------------------------
void mgl_vect_xyz(HMGL gr, HCDT x, HCDT y, HCDT z, HCDT ax, HCDT ay, HCDT az, const char *sch, const char *opt)
{
	register long i,j,n=ax->GetNx(),m=ax->GetNy(),l=ax->GetNz(),k;
	if(n*m*l!=ay->GetNx()*ay->GetNy()*ay->GetNz() || ax->GetNx()*ax->GetNy()*ax->GetNz()!=az->GetNx()*az->GetNy()*az->GetNz())
	{	gr->SetWarn(mglWarnDim,"Vect");	return;	}
	if(n<2 || m<2 || l<2)	{	gr->SetWarn(mglWarnLow,"Vect");	return;	}
	bool both = x->GetNx()*x->GetNy()*x->GetNz()==n*m*l && y->GetNx()*y->GetNy()*y->GetNz()==n*m*l && z->GetNx()*z->GetNy()*z->GetNz()==n*m*l;
	if(!(both || (x->GetNx()==n && y->GetNx()==m && z->GetNx()==l)))
	{	gr->SetWarn(mglWarnDim,"Vect");	return;	}
	float r = gr->SaveState(opt);
	long flag = isnan(r) ? 0:long(r+0.5);
	static int cgid=1;	gr->StartGroup("Vect3",cgid++);

	float xm=0,ym,dx,dy,dz,dd,dm=(fabs(gr->Max.c)+fabs(gr->Min.c))*1e-5;
	long ss = gr->AddTexture(sch);
	gr->Reserve(2*n*m*l);
	long tx=1,ty=1,tz=1;
	if(gr->MeshNum>1)
	{	tx=(n-1)/(gr->MeshNum-1);	ty=(m-1)/(gr->MeshNum-1);	tz=(l-1)/(gr->MeshNum-1);}
	if(tx<1)	tx=1;	if(ty<1)	ty=1;	if(tz<1)	tz=1;

	for(k=0;k<l;k++)	for(j=0;j<m;j++)	for(i=0;i<n;i++)
	{
		ym = ax->v(i,j,k)*ax->v(i,j,k)+ay->v(i,j,k)*ay->v(i,j,k)+az->v(i,j,k)*az->v(i,j,k);
		xm = xm>ym ? xm : ym;
	}
	xm = 1./(xm==0 ? 1:sqrt(xm));

	long n1,n2;
	mglPoint p1,p2;
	float c1,c2, xx,yy,zz;

	for(k=0;k<l;k+=tz)	for(i=0;i<n;i+=tx)	for(j=0;j<m;j+=ty)
	{
		xx = GetX(x,i,j,k).x;	yy = GetY(y,i,j,k).x;	zz = GetZ(z,i,j,k).x;
		dx = i<n-1 ? (GetX(x,i+1,j,k).x-xx) : (xx-GetX(x,i-1,j,k).x);
		dy = j<m-1 ? (GetY(y,i,j+1,k).x-yy) : (yy-GetY(y,i,j-1,k).x);
		dz = k<l-1 ? (GetZ(z,i,j,k+1).x-zz) : (zz-GetZ(z,i,j,k-1).x);
		dx *= tx;	dy *= ty;	dz *= tz;
		dd = sqrt(ax->v(i,j,k)*ax->v(i,j,k)+ay->v(i,j,k)*ay->v(i,j,k)+az->v(i,j,k)*az->v(i,j,k));
		dx *= (flag&MGL_VEC_LEN) ? (dd>dm ? ax->v(i,j,k)/dd : 0) : ax->v(i,j,k)*xm;
		dy *= (flag&MGL_VEC_LEN) ? (dd>dm ? ay->v(i,j,k)/dd : 0) : ay->v(i,j,k)*xm;
		dz *= (flag&MGL_VEC_LEN) ? (dd>dm ? az->v(i,j,k)/dd : 0) : az->v(i,j,k)*xm;

		if(flag & MGL_VEC_END)
		{	p1 = mglPoint(xx-dx,yy-dy,zz-dz);	p2 = mglPoint(xx,yy,zz);	}
		else if(flag & MGL_VEC_MID)
		{	p1=mglPoint(xx-dx/2,yy-dy/2,zz-dz/2);	p2=mglPoint(xx+dx/2,yy+dy/2,zz+dz/2);	}
		else
		{	p1 = mglPoint(xx,yy,zz);	p2 = mglPoint(xx+dx,yy+dy,zz+dz);	}
		if(flag&MGL_VEC_COL)	{	c1 = c2 = ss;	}	else
		{	c1=gr->GetC(ss,dd*xm*1.5-1,false);	c2=gr->GetC(ss,dd*xm*1.5-0.5,false);	}
		n1=gr->AddPnt(p1,c1);	n2=gr->AddPnt(p2,c2);
		if(flag & MGL_VEC_DOT)	gr->line_plot(n1,n2);
		else	gr->vect_plot(n1,n2);
	}
	gr->EndGroup();
}
//-----------------------------------------------------------------------------
void mgl_vect_3d(HMGL gr, HCDT ax, HCDT ay, HCDT az, const char *sch, const char *opt)
{
	gr->SaveState(opt);
	mglData x(ax->GetNx()), y(ax->GetNy()), z(ax->GetNz());
	x.Fill(gr->Min.x,gr->Max.x);
	y.Fill(gr->Min.y,gr->Max.y);
	z.Fill(gr->Min.z,gr->Max.z);
	mgl_vect_xyz(gr,&x,&y,&z,ax,ay,az,sch,0);
}
//-----------------------------------------------------------------------------
void mgl_vect_xyz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_vect_xyz(_GR_, _DA_(x), _DA_(y), _DA_(z), _DA_(ax), _DA_(ay), _DA_(az), s, o);
	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void mgl_vect_3d_(uintptr_t *gr, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch, const char *opt, int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_vect_3d(_GR_, _DA_(ax), _DA_(ay), _DA_(az), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
//
//	Flow 2d series
//
//-----------------------------------------------------------------------------
void flow(mglBase *gr, float zVal, float u, float v, const mglData &x, const mglData &y, const mglData &ax, const mglData &ay,long ss)
{
	long n=10*(ax.nx+ax.ny);
	bool both = x.nx==ax.nx && y.nx==ax.nx && x.ny==ax.ny && y.ny==ax.ny;

	mglPoint *pp = new mglPoint[n], dp;
	float *cc = new float[n];
	mglPoint dx(1/fabs(gr->Max.x-gr->Min.x),1/fabs(gr->Max.y-gr->Min.y),1/fabs(gr->Max.z-gr->Min.z));

	float dt = 0.5/(ax.nx > ax.ny ? ax.nx : ax.ny),e,f,g,ff[4],gg[4],h,s=1;
	if(u<0 || v<0)	{	dt = -dt;	u = -u;		v = -v;		s = -1;}
	register long k=0,m;
	bool end = false;
	do{
		pp[k].x = both ? x.Spline1(u,v,0):x.Spline1(u,0,0);
		pp[k].y = both ? y.Spline1(u,v,0):y.Spline1(v,0,0);
		pp[k].z = zVal;
		for(m=0;m<k-1;m++)	// determines encircle
			if(Norm((pp[k]-pp[m])/dx)<dt/10.)	{	end = true;	break;	}
		f = ax.Linear1(u,v,0);	g = ay.Linear1(u,v,0);
		h = hypot(f,g);	cc[k] = gr->GetC(ss,s*h);
		if(h<1e-5)	break;	// stationary point
		k++;
		// find next point by midpoint method
		h+=1;	ff[0]=f*dt/h;	gg[0]=g*dt/h;
		e = u+ff[0]/2;	h = v+gg[0]/2;
		f = ax.Linear1(e,h,0);	g = ay.Linear1(e,h,0);
		h = 1+hypot(f,g);	ff[1]=f*dt/h;	gg[1]=g*dt/h;
		e = u+ff[1]/2;	h = v+gg[1]/2;
		f = ax.Linear1(e,h,0);	g = ay.Linear1(e,h,0);
		h = 1+hypot(f,g);	ff[2]=f*dt/h;	gg[2]=g*dt/h;
		e = u+ff[2];	h = v+gg[2];
		f = ax.Linear1(e,h,0);	g = ay.Linear1(e,h,0);
		h = 1+hypot(f,g);	ff[3]=f*dt/h;	gg[3]=g*dt/h;
		u += ff[0]/6+ff[1]/3+ff[2]/3+ff[3]/6;
		v += gg[0]/6+gg[1]/3+gg[2]/3+gg[3]/6;
		// condition of end
		end = end || k>=n || u<0 || v<0 || u>1 || v>1;
	} while(!end);
	if(k>1)
	{
		long i,j,jj;
		gr->Reserve(k);		j = gr->AddPnt(pp[0],cc[0]);
		for(i=1;i<k;i++)
		{	jj=j;	j = gr->AddPnt(pp[i],cc[i]);	gr->line_plot(jj,j);	}
	}
	delete []pp;	delete []cc;
}
//-----------------------------------------------------------------------------
void mgl_flow_xy(HMGL gr, HCDT x, HCDT y, HCDT ax, HCDT ay, const char *sch, const char *opt)
{
	float u,v;
	long n=ax->GetNx(), m=ax->GetNy();
	if(n*m*ax->GetNz()!=ay->GetNx()*ay->GetNy()*ay->GetNz())	{	gr->SetWarn(mglWarnDim,"Flow");	return;	}
	if(n<2 || m<2)						{	gr->SetWarn(mglWarnLow,"Flow");	return;	}
	bool both = x->GetNx()==n && y->GetNx()==n && x->GetNy()==m && y->GetNy()==m;
	if(!(both || (x->GetNx()==n && y->GetNx()==m)))	{	gr->SetWarn(mglWarnDim,"Flow");	return;	}
	float r = gr->SaveState(opt);
	if(isnan(r))	r = gr->PrevValue();
	long num = isnan(r)?5:long(r+0.5);
	static int cgid=1;	gr->StartGroup("Flow",cgid++);

	long ss = gr->AddTexture(sch);
	// allocate memory
	float zVal = gr->Min.z;
	bool cnt=(num>0);
	num = abs(num);
	mglData xx(x), yy(y), bx(ax), by(ay);

	for(long k=0;k<ax->GetNz();k++)
	{
		if(ax->GetNz()>1)	zVal = gr->Min.z+(gr->Max.z-gr->Min.z)*float(k)/(ax->GetNz()-1);
		for(long i=0;i<num;i++)
		{
			u = 0;	v = (i+1.)/(num+1.);
			flow(gr, zVal, u, v, xx, yy, bx, by,ss);
			flow(gr, zVal, -u, -v, xx, yy, bx, by,ss);
			u = 1;	v = (i+1.)/(num+1.);
			flow(gr, zVal, u, v, xx, yy, bx, by,ss);
			flow(gr, zVal, -u, -v, xx, yy, bx, by,ss);
			u = (i+1.)/(num+1.);	v = 0;
			flow(gr, zVal, u, v, xx, yy, bx, by,ss);
			flow(gr, zVal, -u, -v, xx, yy, bx, by,ss);
			u = (i+1.)/(num+1.);	v = 1;
			flow(gr, zVal, u, v, xx, yy, bx, by,ss);
			flow(gr, zVal, -u, -v, xx, yy, bx, by,ss);
			if(cnt)
			{
				u = 0.5;	v = (i+1.)/(num+1.);
				flow(gr, zVal, u, v, xx, yy, bx, by,ss);
				flow(gr, zVal, -u, -v, xx, yy, bx, by,ss);
				u = (i+1.)/(num+1.);	v = 0.5;
				flow(gr, zVal, u, v, xx, yy, bx, by,ss);
				flow(gr, zVal, -u, -v, xx, yy, bx, by,ss);
			}
		}
	}
	gr->EndGroup();
}
//-----------------------------------------------------------------------------
void mgl_flow_2d(HMGL gr, HCDT ax, HCDT ay, const char *sch, const char *opt)
{
	gr->SaveState(opt);
	mglData x(ax->GetNx()), y(ax->GetNy());
	x.Fill(gr->Min.x,gr->Max.x);
	y.Fill(gr->Min.y,gr->Max.y);
	mgl_flow_xy(gr,&x,&y,ax,ay,sch,0);
}
//-----------------------------------------------------------------------------
void mgl_flow_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *ax, uintptr_t *ay, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_flow_xy(_GR_, _DA_(x), _DA_(y), _DA_(ax), _DA_(ay), s, o);	delete []o;	delete []s;	}
void mgl_flow_2d_(uintptr_t *gr, uintptr_t *ax, uintptr_t *ay, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_flow_2d(_GR_, _DA_(ax), _DA_(ay), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void mgl_flowp_xy(HMGL gr, float x0, float y0, float z0, HCDT x, HCDT y, HCDT ax, HCDT ay, const char *sch, const char *opt)
{
	mglPoint p(x0,y0,z0);
	float u,v;
	long n=ax->GetNx(), m=ax->GetNy();
	if(n*m*ax->GetNz()!=ay->GetNx()*ay->GetNy()*ay->GetNz())	{	gr->SetWarn(mglWarnDim,"Flow");	return;	}
	if(n<2 || m<2)						{	gr->SetWarn(mglWarnLow,"Flow");	return;	}
	bool both = x->GetNx()==n && y->GetNx()==n && x->GetNy()==m && y->GetNy()==m;
	if(!(both || (x->GetNx()==n && y->GetNx()==m)))	{	gr->SetWarn(mglWarnDim,"Flow");	return;	}
	gr->SaveState(opt);
	static int cgid=1;	gr->StartGroup("FlowP",cgid++);

	long ss = gr->AddTexture(sch);
	// find coordinates u, v
	register long i,j;
	register float d, dm=1e7;
	long i0=0,j0=0;
	for(i=0;i<n;i++)	for(j=0;j<m;j++)	// first find closest
	{
		d = both ? hypot(x->v(i,j)-p.x,y->v(i,j)-p.y) : hypot(x->v(i)-p.x,y->v(j)-p.y);
		if(d<dm)	{	i0=i;	j0=j;	dm=d;	}
	}
	if(dm==0)	{	u = i0/float(n);	v = j0/float(m);	}	// we find it
	else
	{
		float dxu,dxv,dyu,dyv, dx, dy;
		if(both)
		{
			dx = x->v(i0,j0)-p.x;	dy = y->v(i0,j0)-p.y;
			dxu= x->dvx(i0,j0);		dyu= y->dvx(i0,j0);
			dxv= x->dvy(i0,j0);		dyv= y->dvy(i0,j0);
			d = dxv*dyu-dxu*dyv;
			u = (i0+(dxv*dy-dx*dyv)/d)/n;
			v = (j0-(dxu*dy-dx*dyu)/d)/m;
		}
		else
		{
			dx = x->v(i0)-p.x;	dy = y->v(j0)-p.y;
			dxu= x->dvx(i0);	dyv= y->dvx(j0);
			u = (i0+dx/dxu)/n;	v = (j0+dy/dyv)/m;
		}
	}
	mglData xx(x), yy(y), bx(ax), by(ay);
	flow(gr, p.z, u, v, xx, yy, bx, by,ss);
	gr->EndGroup();
}
//-----------------------------------------------------------------------------
void mgl_flowp_2d(HMGL gr, float x0, float y0, float z0, HCDT ax, HCDT ay, const char *sch, const char *opt)
{
	gr->SaveState(opt);
	mglData x(ax->GetNx()), y(ax->GetNy());
	x.Fill(gr->Min.x,gr->Max.x);
	y.Fill(gr->Min.y,gr->Max.y);
	mgl_flowp_xy(gr,x0,y0,z0,&x,&y,ax,ay,sch,0);
}
//-----------------------------------------------------------------------------
void mgl_flowp_xy_(uintptr_t *gr, float *x0, float *y0, float *z0, uintptr_t *x, uintptr_t *y, uintptr_t *ax, uintptr_t *ay, const char *sch, const char *opt, int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_flowp_xy(_GR_, *x0,*y0,*z0, _DA_(x), _DA_(y), _DA_(ax), _DA_(ay), s, o);	delete []o;	delete []s;
}
void mgl_flowp_2d_(uintptr_t *gr, float *x0, float *y0, float *z0, uintptr_t *ax, uintptr_t *ay, const char *sch, const char *opt, int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_flowp_2d(_GR_, *x0,*y0,*z0, _DA_(ax), _DA_(ay), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
//
//	Flow 3d series
//
//-----------------------------------------------------------------------------
void flow(mglBase *gr, float u, float v, float w, const mglData &x, const mglData &y, const mglData &z, const mglData &ax, const mglData &ay, const mglData &az,long ss)
{
	static long n=10*(ax.nx+ax.ny);
	long nn = ax.nx*ax.ny*ax.nz;
	bool both = x.nx*x.ny*x.nz==nn && y.nx*y.ny*y.nz==nn && z.nx*z.ny*z.nz==nn;
	mglPoint *pp = new mglPoint[n], dp;
	float *cc = new float[n];
	mglPoint dx(1/fabs(gr->Max.x-gr->Min.x),1/fabs(gr->Max.y-gr->Min.y),1/fabs(gr->Max.z-gr->Min.z));

	nn = (ax.nx > ax.ny ? ax.nx : ax.ny);
	nn = (nn > ax.nz ? nn : ax.nz);
	float dt = 0.2/nn, e,f,g,ee[4],ff[4],gg[4],h,s=1,u1,v1,w1;
	if(u<0 || v<0 || w<0)
	{	dt = -dt;	u = -u;		v = -v;		w = -w;		s = -1;}
	register long k=0,m;
	bool end = false;
	do{
		pp[k].x = both ? x.Spline1(u,v,w):x.Spline1(u,0,0);
		pp[k].y = both ? y.Spline1(u,v,w):y.Spline1(v,0,0);
		pp[k].z = both ? z.Spline1(u,v,w):z.Spline1(w,0,0);
		for(m=0;m<k-1;m++)	// determines encircle
			if(Norm((pp[k]-pp[m])/dx)<dt/10.)	{	end = true;	break;	}
		e = ax.Linear1(u,v,w);	f = ay.Linear1(u,v,w);	g = az.Linear1(u,v,w);
		h = sqrt(e*e+f*f+g*g);	cc[k] = gr->GetC(ss,s*h);
		if(h<1e-5)	break;	// stationary point
		k++;
		// find next point by midpoint method
		h+=1;	ee[0]=e*dt/h;	ff[0]=f*dt/h;	gg[0]=g*dt/h;
		u1 = u+ee[0]/2;	v1 = v+ff[0]/2;	w1 = w+gg[0]/2;
		e = ax.Linear1(u1,v1,w1);	f = ay.Linear1(u1,v1,w1);
		g = az.Linear1(u1,v1,w1);	h = 1+sqrt(e*e+f*f+g*g);
		ee[1]=e*dt/h;	ff[1]=f*dt/h;	gg[1]=g*dt/h;
		u1 = u+ee[1]/2;	v1 = v+ff[1]/2;	w1 = w+gg[1]/2;
		e = ax.Linear1(u1,v1,w1);	f = ay.Linear1(u1,v1,w1);
		g = az.Linear1(u1,v1,w1);	h = 1+sqrt(e*e+f*f+g*g);
		ee[2]=e*dt/h;	ff[2]=f*dt/h;	gg[2]=g*dt/h;
		u1 = u+ee[2];	v1 = v+ff[2];	w1 = w+gg[2];
		e = ax.Linear1(u1,v1,w1);	f = ay.Linear1(u1,v1,w1);
		g = az.Linear1(u1,v1,w1);	h = 1+sqrt(e*e+f*f+g*g);
		ee[3]=e*dt/h;	ff[3]=f*dt/h;	gg[3]=g*dt/h;
		u += ee[0]/6+ee[1]/3+ee[2]/3+ee[3]/6;
		v += ff[0]/6+ff[1]/3+ff[2]/3+ff[3]/6;
		w += gg[0]/6+gg[1]/3+gg[2]/3+gg[3]/6;
		// condition of end
		end = end || k>=n || u<0 || v<0 || u>1 || v>1 || w<0 || w>1;
	} while(!end);
	if(k>1)
	{
		long i,j,jj;
		gr->Reserve(k);		j = gr->AddPnt(pp[0],cc[0]);
		for(i=1;i<k;i++)
		{	jj=j;	j = gr->AddPnt(pp[i],cc[i]);	gr->line_plot(jj,j);	}
	}
	delete []pp;
	delete []cc;
}
//-----------------------------------------------------------------------------
void mgl_flow_xyz(HMGL gr, HCDT x, HCDT y, HCDT z, HCDT ax, HCDT ay, HCDT az, const char *sch, const char *opt)
{
	float u,v,w;
	long i,j,n=ax->GetNx(),m=ax->GetNy(),l=ax->GetNz();
	if(ax->GetNx()*ax->GetNy()*ax->GetNz()!=ay->GetNx()*ay->GetNy()*ay->GetNz() || ax->GetNx()*ax->GetNy()*ax->GetNz()!=az->GetNx()*az->GetNy()*az->GetNz())
	{	gr->SetWarn(mglWarnDim,"Flow");	return;	}
	if(ax->GetNx()<2 || ax->GetNy()<2 || ax->GetNz()<2)
	{	gr->SetWarn(mglWarnLow,"Flow");	return;	}
	bool both = x->GetNx()*x->GetNy()*x->GetNz()==n*m*l && y->GetNx()*y->GetNy()*y->GetNz()==n*m*l && z->GetNx()*z->GetNy()*z->GetNz()==n*m*l;
	if(!(both || (x->GetNx()==n && y->GetNx()==m && z->GetNx()==l)))
	{	gr->SetWarn(mglWarnDim,"Flow");	return;	}
	float r = gr->SaveState(opt);
	if(isnan(r))	r = gr->PrevValue();
	long num = isnan(r)?3:long(r+0.5);
	static int cgid=1;	gr->StartGroup("Flow3",cgid++);
	bool cnt=(num>0);	num = abs(num);	// redefine central parater
	long ss = gr->AddTexture(sch);

	mglData xx(x), yy(y), zz(z), bx(ax), by(ay), bz(az);
	for(i=0;i<num;i++)	for(j=0;j<num;j++)
	{
		u = (i+1.)/(num+1.);	v = (j+1.)/(num+1.);	w = 0;
		flow(gr, u, v, w, xx, yy, zz, bx, by, bz,ss);
		flow(gr,-u,-v,-w, xx, yy, zz, bx, by, bz,ss);
		u = (i+1.)/(num+1.);	v = (j+1.)/(num+1.);	w = 1;
		flow(gr, u, v, w, xx, yy, zz, bx, by, bz,ss);
		flow(gr,-u,-v,-w, xx, yy, zz, bx, by, bz,ss);
		u = 0;	v = (j+1.)/(num+1.);	w = (i+1.)/(num+1.);
		flow(gr, u, v, w, xx, yy, zz, bx, by, bz,ss);
		flow(gr,-u,-v,-w, xx, yy, zz, bx, by, bz,ss);
		u = 1;	v = (j+1.)/(num+1.);	w = (i+1.)/(num+1.);
		flow(gr, u, v, w, xx, yy, zz, bx, by, bz,ss);
		flow(gr,-u,-v,-w, xx, yy, zz, bx, by, bz,ss);
		u = (i+1.)/(num+1.);	v = 0;	w = (j+1.)/(num+1.);
		flow(gr, u, v, w, xx, yy, zz, bx, by, bz,ss);
		flow(gr,-u,-v,-w, xx, yy, zz, bx, by, bz,ss);
		u = (i+1.)/(num+1.);	v = 1;	w = (j+1.)/(num+1.);
		flow(gr, u, v, w, xx, yy, zz, bx, by, bz,ss);
		flow(gr,-u,-v,-w, xx, yy, zz, bx, by, bz,ss);
		if(cnt)
		{
			u = (i+1.)/(num+1.);	v = (j+1.)/(num+1.);	w = 0.5;
			flow(gr, u, v, w, xx, yy, zz, bx, by, bz,ss);
			flow(gr,-u,-v,-w, xx, yy, zz, bx, by, bz,ss);
			u = 0.5;	v = (j+1.)/(num+1.);	w = (i+1.)/(num+1.);
			flow(gr, u, v, w, xx, yy, zz, bx, by, bz,ss);
			flow(gr,-u,-v,-w, xx, yy, zz, bx, by, bz,ss);
			u = (i+1.)/(num+1.);	v = 0.5;	w = (j+1.)/(num+1.);
			flow(gr, u, v, w, xx, yy, zz, bx, by, bz,ss);
			flow(gr,-u,-v,-w, xx, yy, zz, bx, by, bz,ss);
		}
	}
	gr->EndGroup();
}
//-----------------------------------------------------------------------------
void mgl_flow_3d(HMGL gr, HCDT ax, HCDT ay, HCDT az, const char *sch, const char *opt)
{
	gr->SaveState(opt);
	mglData x(ax->GetNx()), y(ax->GetNy()), z(ax->GetNz());
	x.Fill(gr->Min.x,gr->Max.x);
	y.Fill(gr->Min.y,gr->Max.y);
	z.Fill(gr->Min.z,gr->Max.z);
	mgl_flow_xyz(gr,&x,&y,&z,ax,ay,az,sch,0);
}
//-----------------------------------------------------------------------------
void mgl_flow_xyz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_flow_xyz(_GR_, _DA_(x), _DA_(y), _DA_(z), _DA_(ax), _DA_(ay), _DA_(az), s, o);	delete []o;	delete []s;	}
void mgl_flow_3d_(uintptr_t *gr, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_flow_3d(_GR_, _DA_(ax), _DA_(ay), _DA_(az), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void mgl_flowp_xyz(HMGL gr, float x0, float y0, float z0, HCDT x, HCDT y, HCDT z, HCDT ax, HCDT ay, HCDT az, const char *sch, const char *opt)
{
	mglPoint p(x0,y0,z0);
	float u,v,w;
	long n=ax->GetNx(),m=ax->GetNy(),l=ax->GetNz();
	if(ax->GetNx()*ax->GetNy()*ax->GetNz()!=ay->GetNx()*ay->GetNy()*ay->GetNz() || ax->GetNx()*ax->GetNy()*ax->GetNz()!=az->GetNx()*az->GetNy()*az->GetNz())
	{	gr->SetWarn(mglWarnDim,"Flow");	return;	}
	if(ax->GetNx()<2 || ax->GetNy()<2 || ax->GetNz()<2)
	{	gr->SetWarn(mglWarnLow,"Flow");	return;	}
	bool both = x->GetNx()*x->GetNy()*x->GetNz()==n*m*l && y->GetNx()*y->GetNy()*y->GetNz()==n*m*l && z->GetNx()*z->GetNy()*z->GetNz()==n*m*l;
	if(!(both || (x->GetNx()==n && y->GetNx()==m && z->GetNx()==l)))
	{	gr->SetWarn(mglWarnDim,"Flow");	return;	}
	gr->SaveState(opt);
	static int cgid=1;	gr->StartGroup("FlowP3",cgid++);
	long ss = gr->AddTexture(sch);

	// find coordinates u, v, w
	register long i,j,k,ii;
	register float d, dm=1e7;
	long i0=0,j0=0,k0=0;
	float dx,dy,dz;
	for(i=0;i<n;i++)	for(j=0;j<m;j++)	for(k=0;k<l;k++)	// first find closest
	{
		ii = i+n*(j+m*k);
		if(both)
		{	dx = x->v(i,j,k)-p.x;	dy = y->v(i,j,k)-p.y;	dz = x->v(i,j,k)-p.z;	}
		else
		{	dx = x->v(i)-p.x;	dy = y->v(j)-p.y;	dz = x->v(k)-p.z;	}
		d = sqrt(dx*dx+dy*dy+dz*dz);
		if(d<dm)	{	i0=i;	j0=j;	k0=k;	dm=d;	}
	}
	if(dm==0)	// we find it
	{	u=i0/float(n);	v=j0/float(m);	w=k0/float(l);	}
	else
	{
		float dxu,dxv,dxw,dyu,dyv,dyw,dzu,dzv,dzw;
		if(both)
		{
			ii = i0+n*j0;
			dx = x->v(i0,j0,k0)-p.x;	dy = y->v(i0,j0,k0)-p.y;	dz = z->v(i0,j0,k0)-p.z;
			dxu= x->dvx(i0,j0,k0);		dyu= y->dvx(i0,j0,k0);		dzu= z->dvx(i0,j0,k0);
			dxv= x->dvy(i0,j0,k0);		dyv= y->dvy(i0,j0,k0);		dzv= z->dvy(i0,j0,k0);
			dxw= x->dvz(i0,j0,k0);		dyw= y->dvz(i0,j0,k0);		dzw= z->dvz(i0,j0,k0);
			d = dxu*(dyw*dzv-dyv*dzw)+dxv*(dyu*dzw-dyw*dzu)+dxw*(dyv*dzu-dyu*dzv);
			u = (i0+(dx*(dyw*dzv-dyv*dzw)+dxv*(dy*dzw-dyw*dz)+dxw*(dyv*dz-dy*dzv))/d)/n;
			v = (j0-(dx*(dyw*dzu-dyu*dzw)+dxu*(dy*dzw-dyw*dz)+dxw*(dyu*dz-dy*dzu))/d)/m;
			w = (i0+(dx*(dyv*dzu-dyu*dzv)+dxu*(dy*dzv-dyv*dz)+dxv*(dyu*dz-dy*dzu))/d)/l;
		}
		else
		{
			dx = x->v(i0)-p.x;	dy = y->v(j0)-p.y;	dz = z->v(k0)-p.z;
			dxu= x->dvx(i0);	dyv= y->dvx(j0);	dzw= z->dvx(k0);
			u = (i0+dx/dxu)/n;	v = (j0+dy/dyv)/m;	w = (k0+dz/dzw)/m;
		}
	}
	mglData xx(x), yy(y), zz(z), bx(ax), by(ay), bz(az);
	flow(gr, u, v, w, xx, yy, zz, bx, by, bz,ss);
	gr->EndGroup();
}
//-----------------------------------------------------------------------------
void mgl_flowp_3d(HMGL gr, float x0, float y0, float z0, HCDT ax, HCDT ay, HCDT az, const char *sch, const char *opt)
{
	gr->SaveState(opt);
	mglData x(ax->GetNx()), y(ax->GetNy()), z(ax->GetNz());
	x.Fill(gr->Min.x,gr->Max.x);
	y.Fill(gr->Min.y,gr->Max.y);
	z.Fill(gr->Min.z,gr->Max.z);
	mgl_flowp_xyz(gr, x0,y0,z0, &x,&y,&z,ax,ay,az,sch,0);
}
//-----------------------------------------------------------------------------
void mgl_flowp_xyz_(uintptr_t *gr, float *x0, float *y0, float *z0, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch, const char *opt, int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_flowp_xyz(_GR_, *x0,*y0,*z0, _DA_(x), _DA_(y), _DA_(z), _DA_(ax), _DA_(ay), _DA_(az), s, o);
	delete []o;	delete []s;	}
void mgl_flowp_3d_(uintptr_t *gr, float *x0, float *y0, float *z0, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch, const char *opt, int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_flowp_3d(_GR_, *x0,*y0,*z0, _DA_(ax), _DA_(ay), _DA_(az), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
//
//	Grad series
//
//-----------------------------------------------------------------------------
void mgl_grad_xyz(HMGL gr, HCDT x, HCDT y, HCDT z, HCDT phi, const char *sch, const char *opt)
{
	mglData ax(phi), ay(phi),az(phi),xx(phi),yy(phi),zz(phi);
	long n=xx.nx, m=xx.ny, l=xx.nz, nn = n*m*l;
	if(x->GetNx()*x->GetNy()*x->GetNz()==nn && y->GetNx()*y->GetNy()*y->GetNz()==nn && x->GetNx()*x->GetNy()*x->GetNz()==nn)
	{	xx.Set(x);	yy.Set(y);	zz.Set(z);	}	// nothing to do
	else if(x->GetNx()==n && y->GetNx()==m && z->GetNx()==l)
	{	// prepare data
		register long i,j,k,i0;
		for(i=0;i<n;i++)	for(j=0;j<m;j++)	for(k=0;k<l;k++)
		{	i0 = i+n*(j+m*k);	xx.a[i0] = x->v(i);
			yy.a[i0] = y->v(j);	zz.a[i0] = z->v(k);	}
	}
	else	{	gr->SetWarn(mglWarnDim,"Grad");	return;	}
	ax.Diff(xx,yy,zz);	ay.Diff(yy,xx,zz);	az.Diff(zz,xx,yy);
	mgl_flow_xyz(gr,&xx,&yy,&zz,&ax,&ay,&az,sch,opt);
}
//-----------------------------------------------------------------------------
void mgl_grad_xy(HMGL gr, HCDT x, HCDT y, HCDT phi, const char *sch, const char *opt)
{
	mglData ax(phi), ay(phi),xx(phi),yy(phi);
	long n = phi->GetNx(), m=phi->GetNy(), nn=n*m;
	if(x->GetNx()*x->GetNy()==nn && y->GetNx()*y->GetNy()==nn)	{	xx.Set(x);	yy.Set(y);	}
	else if(x->GetNx()==n && y->GetNx()==m)
	{
		register long i,j,i0;
		for(i=0;i<n;i++)	for(j=0;j<m;j++)
		{	i0 = i+n*j;	xx.a[i0] = x->v(i);	yy.a[i0] = y->v(j);	}
	}
	else	{	gr->SetWarn(mglWarnDim,"Grad");	return;	}
	ax.Diff(xx,yy);	ay.Diff(yy,xx);
	mgl_flow_xy(gr,&xx,&yy,&ax,&ay,sch,opt);
}
//-----------------------------------------------------------------------------
void mgl_grad(HMGL gr, HCDT phi, const char *sch, const char *opt)
{
	mglData x(phi->GetNx()), y(phi->GetNy()), z(phi->GetNz());
	gr->SaveState(opt);
	x.Fill(gr->Min.x,gr->Max.x);	y.Fill(gr->Min.y,gr->Max.y);	z.Fill(gr->Min.z,gr->Max.z);
	if(phi->GetNz()==1)	mgl_grad_xy(gr,&x,&y,phi,sch,0);
	else				mgl_grad_xyz(gr,&x,&y,&z,phi,sch,0);
}
//-----------------------------------------------------------------------------
void mgl_grad_xyz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *ph, const char *sch, const char *opt, int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_grad_xyz(_GR_, _DA_(x), _DA_(y), _DA_(z), _DA_(ph), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void mgl_grad_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *ph, const char *sch, const char *opt, int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_grad_xy(_GR_, _DA_(x), _DA_(y), _DA_(ph), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void mgl_grad_(uintptr_t *gr, uintptr_t *ph, const char *sch, const char *opt, int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_grad(_GR_, _DA_(ph), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
//
//	Pipe 2d series
//
//-----------------------------------------------------------------------------
void flowr(mglBase *gr, float zVal, float u, float v, const mglData &x, const mglData &y, const mglData &ax, const mglData &ay, float r0,long sc)
{
	long n=10*(ax.nx+ax.ny);
	bool both = x.nx==ax.nx && y.nx==ax.nx && x.ny==ax.ny && y.ny==ax.ny;

	mglPoint *pp = new mglPoint[n], dp;
	float *cc = new float[n];
	mglPoint dx(1/fabs(gr->Max.x-gr->Min.x),1/fabs(gr->Max.y-gr->Min.y),1/fabs(gr->Max.z-gr->Min.z));

	float dt = 0.5/(ax.nx > ax.ny ? ax.nx : ax.ny),e,f,g,ff[4],gg[4],h,s=1;
	float ss = 	4/mgl_ipow(gr->Max.c - gr->Min.c,2);
	if(u<0 || v<0)	{	dt = -dt;	u = -u;		v = -v;		s = -1;}
	register long k=0,m;
	bool end = false;
	do{
		pp[k].x = both ? x.Spline1(u,v,0):x.Spline1(u,0,0);
		pp[k].y = both ? y.Spline1(u,v,0):y.Spline1(v,0,0);
		pp[k].z = zVal;
		for(m=0;m<k-1;m++)	// determines encircle
			if(Norm((pp[k]-pp[m])/dx)<dt/10.)	{	end = true;	break;	}
		f = ax.Linear1(u,v,0);	g = ay.Linear1(u,v,0);
		h = hypot(f,g);	cc[k] = gr->GetC(sc,s*h);
		pp[k].c = r0>0 ? r0*sqrt(1e-2+ss*h*h)/2 : -r0/sqrt(1e-2+ss*h*h)/5;
		if(h<1e-5)	break;	// stationary point
		k++;
		// find next point by midpoint method
		h+=1;	ff[0]=f*dt/h;	gg[0]=g*dt/h;
		e = u+ff[0]/2;	h = v+gg[0]/2;
		f = ax.Linear1(e,h,0);	g = ay.Linear1(e,h,0);	h = 1+hypot(f,g);
		ff[1]=f*dt/h;	gg[1]=g*dt/h;
		e = u+ff[1]/2;	h = v+gg[1]/2;
		f = ax.Linear1(e,h,0);	g = ay.Linear1(e,h,0);	h = 1+hypot(f,g);
		ff[2]=f*dt/h;	gg[2]=g*dt/h;
		e = u+ff[2];	h = v+gg[2];
		f = ax.Linear1(e,h,0);	g = ay.Linear1(e,h,0);	h = 1+hypot(f,g);
		ff[3]=f*dt/h;	gg[3]=g*dt/h;
		u += ff[0]/6+ff[1]/3+ff[2]/3+ff[3]/6;
		v += gg[0]/6+gg[1]/3+gg[2]/3+gg[3]/6;
		// condition of end
		end = end || k>=n || u<0 || v<0 || u>1 || v>1;
	} while(!end);
	if(k>1)
	{
		const int num=41;
		long i,j,*id=new long[2*num];
		mglPoint p,l=pp[1]-pp[0],t,q,d;
		t = !l;	t/=Norm(t);		q = t^l;	q/=Norm(q);
		float si,co,fi, rr=pp[0].c,dr=l.c;
		gr->Reserve(num*k);

		for(j=0;j<num;j++)
		{
			fi = j*2*M_PI/(num-1);	co = cos(fi);	si = sin(fi);
			p = pp[0] + t*(rr*co) + q*(rr*si);
			d = (t*si - q*co)^(l + t*(dr*co) + q*(dr*si));
			id[j] = gr->AddPnt(p,cc[0],d);
		}
		for(i=1;i<k;i++)
		{
			if(i<k-1)	l = pp[i+1]-pp[i-1];
			else	l = pp[i]-pp[i-1];
			t = !l;	t/=Norm(t);		q = t^l;	q/=Norm(q);
			rr=pp[i].c;	dr=l.c;
			memcpy(id+num,id,num*sizeof(long));
			for(j=0;j<num;j++)
			{
				fi = j*2*M_PI/(num-1);	co = cos(fi);	si = sin(fi);
				p = pp[i] + t*(rr*co) + q*(rr*si);
				d = (t*si - q*co)^(l + t*(dr*co) + q*(dr*si));
				id[j] = gr->AddPnt(p,cc[i],d);
				if(j>0)	gr->quad_plot(id[j-1],id[j],id[j+num-1],id[j+num]);
			}
		}
		delete []id;
	}
	delete []pp;	delete []cc;
}
//-----------------------------------------------------------------------------
void mgl_pipe_xy(HMGL gr, HCDT x, HCDT y, HCDT ax, HCDT ay, const char *sch, float r0, const char *opt)
{
	float u,v;
	long n=ax->GetNx(), m=ax->GetNy();
	if(n*m*ax->GetNz()!=ay->GetNx()*ay->GetNy()*ay->GetNz())	{	gr->SetWarn(mglWarnDim,"Pipe");	return;	}
	if(n<2 || m<2)						{	gr->SetWarn(mglWarnLow,"Pipe");	return;	}
	bool both = x->GetNx()==n && y->GetNx()==n && x->GetNy()==m && y->GetNy()==m;
	if(!(both || (x->GetNx()==n && y->GetNx()==m)))	{	gr->SetWarn(mglWarnDim,"Pipe");	return;	}
	float r = gr->SaveState(opt);
	if(isnan(r))	r = gr->PrevValue();
	long num = isnan(r)?5:long(r+0.5);
	static int cgid=1;	gr->StartGroup("Pipe",cgid++);

	long ss = gr->AddTexture(sch);
	// allocate memory
	float zVal = gr->Min.z;
	bool cnt=(num>0);	num = abs(num);

	mglData xx(x), yy(y), bx(ax), by(ay);
	for(long k=0;k<ax->GetNz();k++)
	{
		if(ax->GetNz()>1)	zVal = gr->Min.z+(gr->Max.z-gr->Min.z)*float(k)/(ax->GetNz()-1);
		for(long i=0;i<num;i++)
		{
			u = 0;	v = (i+1.)/(num+1.);
			flowr(gr, zVal, u, v, xx, yy, bx, by,r0,ss);
			flowr(gr, zVal, -u, -v, xx, yy, bx, by,r0,ss);
			u = 1;	v = (i+1.)/(num+1.);
			flowr(gr, zVal, u, v, xx, yy, bx, by,r0,ss);
			flowr(gr, zVal, -u, -v, xx, yy, bx, by,r0,ss);
			u = (i+1.)/(num+1.);	v = 0;
			flowr(gr, zVal, u, v, xx, yy, bx, by,r0,ss);
			flowr(gr, zVal, -u, -v, xx, yy, bx, by,r0,ss);
			u = (i+1.)/(num+1.);	v = 1;
			flowr(gr, zVal, u, v, xx, yy, bx, by,r0,ss);
			flowr(gr, zVal, -u, -v, xx, yy, bx, by,r0,ss);
			if(cnt)
			{
				u = 0.5;	v = (i+1.)/(num+1.);
				flowr(gr, zVal, u, v, xx, yy, bx, by,r0,ss);
				flowr(gr, zVal, -u, -v, xx, yy, bx, by,r0,ss);
				u = (i+1.)/(num+1.);	v = 0.5;
				flowr(gr, zVal, u, v, xx, yy, bx, by,r0,ss);
				flowr(gr, zVal, -u, -v, xx, yy, bx, by,r0,ss);
			}
		}
	}
	gr->EndGroup();
}
//-----------------------------------------------------------------------------
void mgl_pipe_2d(HMGL gr, HCDT ax, HCDT ay, const char *sch, float r0, const char *opt)
{
	gr->SaveState(opt);
	mglData x(ax->GetNx()), y(ax->GetNy());
	x.Fill(gr->Min.x,gr->Max.x);
	y.Fill(gr->Min.y,gr->Max.y);
	mgl_pipe_xy(gr,&x,&y,ax,ay,sch,r0,0);
}
//-----------------------------------------------------------------------------
void mgl_pipe_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *ax, uintptr_t *ay, const char *sch, float *r0, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_pipe_xy(_GR_, _DA_(x), _DA_(y), _DA_(ax), _DA_(ay), s, *r0, o);	delete []o;	delete []s;	}
void mgl_pipe_2d_(uintptr_t *gr, uintptr_t *ax, uintptr_t *ay, const char *sch, float *r0, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_pipe_2d(_GR_, _DA_(ax), _DA_(ay), s, *r0, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
//
//	Pipe 3d series
//
//-----------------------------------------------------------------------------
void flowr(mglBase *gr, float u, float v, float w, const mglData &x, const mglData &y, const mglData &z, const mglData &ax, const mglData &ay, const mglData &az, float r0,long sc)
{
	static long n=10*(ax.nx+ax.ny);
	long nn = ax.nx*ax.ny*ax.nz;
	bool both = x.nx*x.ny*x.nz==nn && y.nx*y.ny*y.nz==nn && z.nx*z.ny*z.nz==nn;
	mglPoint *pp = new mglPoint[n], dp;
	float *cc = new float[n];
	mglPoint dx(1/fabs(gr->Max.x-gr->Min.x),1/fabs(gr->Max.y-gr->Min.y),1/fabs(gr->Max.z-gr->Min.z));

	nn = (ax.nx > ax.ny ? ax.nx : ax.ny);
	nn = (nn > ax.nz ? nn : ax.nz);
	float dt = 0.2/nn, e,f,g,ee[4],ff[4],gg[4],h,s=1,u1,v1,w1;
	if(u<0 || v<0 || w<0)
	{	dt = -dt;	u = -u;		v = -v;		w = -w;		s = -1;}
	register long k=0,m;
	bool end = false;
	do{
		pp[k].x = both ? x.Spline1(u,v,w):x.Spline1(u,0,0);
		pp[k].y = both ? y.Spline1(u,v,w):y.Spline1(v,0,0);
		pp[k].z = both ? z.Spline1(u,v,w):z.Spline1(w,0,0);
		for(m=0;m<k-1;m++)	// determines encircle
			if(Norm((pp[k]-pp[m])/dx)<dt/10.)	{	end = true;	break;	}
		e = ax.Linear1(u,v,w);	f = ay.Linear1(u,v,w);	g = az.Linear1(u,v,w);
		h = sqrt(e*e+f*f+g*g);	cc[k] = gr->GetC(sc,s*h);
		if(h<1e-5)	break;	// stationary point
		k++;
		// find next point by midpoint method
		h+=1;	ee[0]=e*dt/h;	ff[0]=f*dt/h;	gg[0]=g*dt/h;
		u1 = u+ee[0]/2;	v1 = v+ff[0]/2;	w1 = w+gg[0]/2;
		e = ax.Linear1(u1,v1,w1);	f = ay.Linear1(u1,v1,w1);
		g = az.Linear1(u1,v1,w1);	h = 1+sqrt(e*e+f*f+g*g);
		ee[1]=e*dt/h;	ff[1]=f*dt/h;	gg[1]=g*dt/h;
		u1 = u+ee[1]/2;	v1 = v+ff[1]/2;	w1 = w+gg[1]/2;
		e = ax.Linear1(u1,v1,w1);	f = ay.Linear1(u1,v1,w1);
		g = az.Linear1(u1,v1,w1);	h = 1+sqrt(e*e+f*f+g*g);
		ee[2]=e*dt/h;	ff[2]=f*dt/h;	gg[2]=g*dt/h;
		u1 = u+ee[2];	v1 = v+ff[2];	w1 = w+gg[2];
		e = ax.Linear1(u1,v1,w1);	f = ay.Linear1(u1,v1,w1);
		g = az.Linear1(u1,v1,w1);	h = 1+sqrt(e*e+f*f+g*g);
		ee[3]=e*dt/h;	ff[3]=f*dt/h;	gg[3]=g*dt/h;
		u += ee[0]/6+ee[1]/3+ee[2]/3+ee[3]/6;
		v += ff[0]/6+ff[1]/3+ff[2]/3+ff[3]/6;
		w += gg[0]/6+gg[1]/3+gg[2]/3+gg[3]/6;
		// condition of end
		end = end || k>=n || u<0 || v<0 || u>1 || v>1 || w<0 || w>1;
	} while(!end);
	if(k>1)
	{
		const int num=41;
		long i,j,*id=new long[2*num];
		mglPoint p,l=pp[1]-pp[0],t,q,d;
		t = !l;	t/=Norm(t);		q = t^l;	q/=Norm(q);
		float si,co,fi, rr=pp[0].c,dr=l.c;
		gr->Reserve(num*k);

		for(j=0;j<num;j++)
		{
			fi = j*2*M_PI/(num-1);	co = cos(fi);	si = sin(fi);
			p = pp[0] + t*(rr*co) + q*(rr*si);
			d = (t*si - q*co)^(l + t*(dr*co) + q*(dr*si));
			id[j] = gr->AddPnt(p,cc[0],d);
		}
		for(i=1;i<k;i++)
		{
			if(i<k-1)	l = pp[i+1]-pp[i-1];
			else	l = pp[i]-pp[i-1];
			t = !l;	t/=Norm(t);		q = t^l;	q/=Norm(q);
			rr=pp[i].c;	dr=l.c;
			memcpy(id+num,id,num*sizeof(long));
			for(j=0;j<num;j++)
			{
				fi = j*2*M_PI/(num-1);	co = cos(fi);	si = sin(fi);
				p = pp[i] + t*(rr*co) + q*(rr*si);
				d = (t*si - q*co)^(l + t*(dr*co) + q*(dr*si));
				id[j] = gr->AddPnt(p,cc[i],d);
				if(j>0)	gr->quad_plot(id[j-1],id[j],id[j+num-1],id[j+num]);
			}
		}
		delete []id;
	}
	delete []pp;	delete []cc;
}
//-----------------------------------------------------------------------------
void mgl_pipe_xyz(HMGL gr, HCDT x, HCDT y, HCDT z, HCDT ax, HCDT ay, HCDT az, const char *sch, float r0, const char *opt)
{
	float u,v,w;
	long i,j,n=ax->GetNx(),m=ax->GetNy(),l=ax->GetNz();
	if(ax->GetNx()*ax->GetNy()*ax->GetNz()!=ay->GetNx()*ay->GetNy()*ay->GetNz() || ax->GetNx()*ax->GetNy()*ax->GetNz()!=az->GetNx()*az->GetNy()*az->GetNz())
	{	gr->SetWarn(mglWarnDim,"Pipe");	return;	}
	if(ax->GetNx()<2 || ax->GetNy()<2 || ax->GetNz()<2)
	{	gr->SetWarn(mglWarnLow,"Pipe");	return;	}
	bool both = x->GetNx()*x->GetNy()*x->GetNz()==n*m*l && y->GetNx()*y->GetNy()*y->GetNz()==n*m*l && z->GetNx()*z->GetNy()*z->GetNz()==n*m*l;
	if(!(both || (x->GetNx()==n && y->GetNx()==m && z->GetNx()==l)))
	{	gr->SetWarn(mglWarnDim,"Pipe");	return;	}
	float r = gr->SaveState(opt);
	if(isnan(r))	r = gr->PrevValue();
	long num = isnan(r)?3:long(r+0.5);
	static int cgid=1;	gr->StartGroup("Pipe3",cgid++);

	long ss = gr->AddTexture(sch);
	bool cnt=(num>0);	num = abs(num);

	mglData xx(x), yy(y), zz(z), bx(ax), by(ay), bz(az);
	for(i=0;i<num;i++)	for(j=0;j<num;j++)
	{
		u = (i+1.)/(num+1.);	v = (j+1.)/(num+1.);	w = 0;
		flowr(gr, u, v, w, xx, yy, zz, bx, by, bz,r0,ss);
		flowr(gr,-u,-v,-w, xx, yy, zz, bx, by, bz,r0,ss);
		u = (i+1.)/(num+1.);	v = (j+1.)/(num+1.);	w = 1;
		flowr(gr, u, v, w, xx, yy, zz, bx, by, bz,r0,ss);
		flowr(gr,-u,-v,-w, xx, yy, zz, bx, by, bz,r0,ss);
		u = 0;	v = (j+1.)/(num+1.);	w = (i+1.)/(num+1.);
		flowr(gr, u, v, w, xx, yy, zz, bx, by, bz,r0,ss);
		flowr(gr,-u,-v,-w, xx, yy, zz, bx, by, bz,r0,ss);
		u = 1;	v = (j+1.)/(num+1.);	w = (i+1.)/(num+1.);
		flowr(gr, u, v, w, xx, yy, zz, bx, by, bz,r0,ss);
		flowr(gr,-u,-v,-w, xx, yy, zz, bx, by, bz,r0,ss);
		u = (i+1.)/(num+1.);	v = 0;	w = (j+1.)/(num+1.);
		flowr(gr, u, v, w, xx, yy, zz, bx, by, bz,r0,ss);
		flowr(gr,-u,-v,-w, xx, yy, zz, bx, by, bz,r0,ss);
		u = (i+1.)/(num+1.);	v = 1;	w = (j+1.)/(num+1.);
		flowr(gr, u, v, w, xx, yy, zz, bx, by, bz,r0,ss);
		flowr(gr,-u,-v,-w, xx, yy, zz, bx, by, bz,r0,ss);
		if(cnt)
		{
			u = (i+1.)/(num+1.);	v = (j+1.)/(num+1.);	w = 0.5;
			flowr(gr, u, v, w, xx, yy, zz, bx, by, bz,r0,ss);
			flowr(gr,-u,-v,-w, xx, yy, zz, bx, by, bz,r0,ss);
			u = 0.5;	v = (j+1.)/(num+1.);	w = (i+1.)/(num+1.);
			flowr(gr, u, v, w, xx, yy, zz, bx, by, bz,r0,ss);
			flowr(gr,-u,-v,-w, xx, yy, zz, bx, by, bz,r0,ss);
			u = (i+1.)/(num+1.);	v = 0.5;	w = (j+1.)/(num+1.);
			flowr(gr, u, v, w, xx, yy, zz, bx, by, bz,r0,ss);
			flowr(gr,-u,-v,-w, xx, yy, zz, bx, by, bz,r0,ss);
		}
	}
	gr->EndGroup();
}
//-----------------------------------------------------------------------------
void mgl_pipe_3d(HMGL gr, HCDT ax, HCDT ay, HCDT az, const char *sch, float r0, const char *opt)
{
	gr->SaveState(opt);
	mglData x(ax->GetNx()), y(ax->GetNy()), z(ax->GetNz());
	x.Fill(gr->Min.x,gr->Max.x);
	y.Fill(gr->Min.y,gr->Max.y);
	z.Fill(gr->Min.z,gr->Max.z);
	mgl_pipe_xyz(gr,&x,&y,&z,ax,ay,az,sch,r0,0);
}
//-----------------------------------------------------------------------------
void mgl_pipe_xyz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch, float *r0, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];		memcpy(o,opt,lo);	o[lo]=0;
	mgl_pipe_xyz(_GR_, _DA_(x), _DA_(y), _DA_(z), _DA_(ax), _DA_(ay), _DA_(az), s, *r0, o);
	delete []o;	delete []s;	}
void mgl_pipe_3d_(uintptr_t *gr, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch, float *r0, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];		memcpy(o,opt,lo);	o[lo]=0;
	mgl_pipe_3d(_GR_, _DA_(ax), _DA_(ay), _DA_(az), s, *r0, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------