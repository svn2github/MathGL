/***************************************************************************
 * cont.cpp is part of Math Graphic Library
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
#include <wchar.h>
#include "mgl/surf.h"
#include "mgl/cont.h"
#include "mgl/data.h"
#include "mgl/eval.h"
//-----------------------------------------------------------------------------
//
//	Text printing along a curve
//
//-----------------------------------------------------------------------------
bool same_chain(long f,long i,long *nn)
{
	long j=f;
	while(1)
	{
		j = nn[j];
		if(j==f || j<0)	return false;
		if(j==i)	return true;
	}
}
//-----------------------------------------------------------------------------
void mgl_string_curve(mglBase *gr,long f,long n,long *ff,long *nn,const wchar_t *text, const char *font)
{
	int pos = font && strchr(font,'T');
	wchar_t L[2]=L"a";
	mglPoint p1,n1,p2;

	float w, r, ww, wg=gr->TextHeight()*gr->GetFontSize()/8.;//*font_factor;
	register long i,k,h,pp;

	h=f;	k=nn[f];	// print string symbol-by-symbol
	mglPoint p0=gr->GetPnt(ff[h]),n0=gr->GetPnt(ff[k])-p0, pa;
	float c=gr->GetClrC(ff[h]);

	for(unsigned j=0;j<wcslen(text);j++)
	{
		L[0] = text[j];	pa = pos>0 ? p0 : p0-wg*(!n0);
		pp = gr->AddPnt(pa,c,n0,0,false);
		ww = gr->text_plot(pp,text,font);
		p1 = p0+(ww/Norm(n0))*n0;
		// let find closest point
		for(r=1e10,i=0;i<n;i++)
		{
			n1 = gr->GetPnt(ff[i]);
			w = Norm(p1-n1);
			if(w<r)	{	r=w;	k=i;	}
		}
		i=k;	k=nn[i];
		for(h=0;h<n;h++)	if(nn[h]==i)	break;	// h is previous point
		// point not in the same chain (continue by stright line)
		if(k<0 || h>=n || !same_chain(f,i,nn))	{	p0=p1;	continue;	}
		// if last point let change it to previous one
		if(k<0)	{	k=i;	i=h;	}
		p2 = gr->GetPnt(ff[i]);	n1 = gr->GetPnt(ff[k])-p2;
		w = (p1-p2)*n1;
		// go in wrong direction, let turn back
		if((w<0 || w>n1*n1) && i!=h)
		{	p2 = gr->GetPnt(ff[h]);n1 = gr->GetPnt(ff[i])-p2;	k = i;	}
		// under maximum and should skip some points
		if(n1.x*n0.y>n1.y*n0.x && pos<0)
		{
			p1 = p0+((ww/Norm(n0))*n0) + ((wg/Norm(n0)/3)*mglPoint(n0.y,-n0.x,n0.z));
			for(r=1e10,i=0;i<n;i++)	// let find closest point
			{
				n1 = gr->GetPnt(ff[i]);
				w = Norm(p1-n1);
				if(w<r)	{	r=w;	k=i;	}
			}
			i=k;	k=nn[i];
			for(h=0;h<n;h++)	if(nn[h]==i)	break;
			if(k<0 || h>=n || !same_chain(f,i,nn))
			{	p0 = p0+(ww/Norm(n0))*n0;	continue;	}
			if(k<0)	{	k=i;	i=h;	}
			p2 = gr->GetPnt(ff[i]);	n1 = gr->GetPnt(ff[k])-p2;
			w = (p1-p2)*n1;
			if((w<0 || w>n1*n1) && i!=h)
			{	p2 = gr->GetPnt(ff[h]);n1 = gr->GetPnt(ff[i])-p2;	k = i;	}
		}
		// above minimum and should skip some points
		if(n1.x*n0.y<n1.y*n0.x && pos>0)
		{
			p1 = p0+((ww/Norm(n0))*n0) - ((wg/Norm(n0)/3)*mglPoint(n0.y,-n0.x,n0.z));
			for(r=1e10,i=0;i<n;i++)	// let find closest point
			{
				n1 = gr->GetPnt(ff[i]);
				w = Norm(p1-n1);
				if(w<r)	{	r=w;	k=i;	}
			}
			i=k;	k=nn[i];
			for(h=0;h<n;h++)	if(nn[h]==i)	break;
			if(k<0 || h>=n || !same_chain(f,i,nn))
			{	p0 = p0+(ww/Norm(n0))*n0;	continue;	}
			if(k<0)	{	k=i;	i=h;	}
			p2 = gr->GetPnt(ff[i]);	n1 = gr->GetPnt(ff[k])-p2;
			w = (p1-p2)*n1;
			if((w<0 || w>n1*n1) && i!=h)
			{	p2 = gr->GetPnt(ff[h]);n1 = gr->GetPnt(ff[i])-p2;	k = i;	}
		}
		// OK, next point will be
		p0 = p2 + n1*(((p1-p2)*n1)/(n1*n1));
		n0 = n1;
	}
}
//-----------------------------------------------------------------------------
void mgl_textw_xyz(HMGL gr, HCDT x, HCDT y, HCDT z,const wchar_t *text, const char *font, const char *opt)
{
	long n=y->GetNx();
	if(x->GetNx()!=n || z->GetNx()!=n)	{	gr->SetWarn(mglWarnDim,"Text");	return;	}
	if(n<2)	{	gr->SetWarn(mglWarnLow,"Text");	return;	}
	gr->SaveState(opt);
	static int cgid=1;	gr->StartGroup("TextC",cgid++);

	long *nn = new long[n], *ff = new long[n];
	mglPoint p;
	register long i;
	for(i=0;i<n;i++)
	{
		p = mglPoint(x->v(i),y->v(i),z->v(i));
		ff[i] = gr->AddPnt(p,-1);
	}
	for(i=1;i<n;i++)	nn[i-1] = i;
	nn[n-1]=-1;
	mgl_string_curve(gr,0,n,ff,nn,text,font);
	gr->EndGroup();
	delete []ff;	delete []nn;
}
//-----------------------------------------------------------------------------
void mgl_textw_xy(HMGL gr, HCDT x, HCDT y, const wchar_t *text, const char *font, const char *opt)
{
	if(y->GetNx()<2)	{	gr->SetWarn(mglWarnLow,"Text");	return;	}
	gr->SaveState(opt);
	mglData z(y->GetNx());
	z.Fill(gr->Min.z,gr->Min.z);
	mgl_textw_xyz(gr,x,y,&z,text,font,0);
}
//-----------------------------------------------------------------------------
void mgl_textw_y(HMGL gr, HCDT y, const wchar_t *text, const char *font, const char *opt)
{
	if(y->GetNx()<2)	{	gr->SetWarn(mglWarnLow,"Text");	return;	}
	gr->SaveState(opt);
	mglData x(y->GetNx()), z(y->GetNx());
	x.Fill(gr->Min.x,gr->Max.x);
	z.Fill(gr->Min.z,gr->Min.z);
	mgl_textw_xyz(gr,&x,y,&z,text,font,0);
}
//-----------------------------------------------------------------------------
void mgl_text_xyz(HMGL gr, HCDT x, HCDT y, HCDT z,const char *text, const char *font, const char *opt)
{
	unsigned s = strlen(text)+1;
	wchar_t *wcs = new wchar_t[s];
	mbstowcs(wcs,text,s);
	mgl_textw_xyz(gr,x,y,z, wcs, font, opt);
	delete []wcs;
}
//-----------------------------------------------------------------------------
void mgl_text_xy(HMGL gr, HCDT x, HCDT y, const char *text, const char *font, const char *opt)
{
	if(y->GetNx()<2)	{	gr->SetWarn(mglWarnLow,"Text");	return;	}
	mglData z(y->GetNx());
	z.Fill(gr->Min.z,gr->Min.z);
	mgl_text_xyz(gr,x,y,&z,text,font,opt);
}
//-----------------------------------------------------------------------------
void mgl_text_y(HMGL gr, HCDT y, const char *text, const char *font, const char *opt)
{
	if(y->GetNx()<2)	{	gr->SetWarn(mglWarnLow,"Text");	return;	}
	mglData x(y->GetNx()), z(y->GetNx());
	x.Fill(gr->Min.x,gr->Max.x);
	z.Fill(gr->Min.z,gr->Min.z);
	mgl_text_xyz(gr,&x,y,&z,text,font,opt);
}
//-----------------------------------------------------------------------------
void mgl_text_xyz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z,const char *text,const char *font, const char *opt,int l,int n,int lo)
{	char *s=new char[l+1];	memcpy(s,text,l);	s[l]=0;
	char *f=new char[n+1];	memcpy(f,font,n);	f[n]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_text_xyz(_GR_, _DA_(x),_DA_(y), _DA_(z), s, f, o);
	delete []o;	delete []s;	delete []f;	}
//-----------------------------------------------------------------------------
void mgl_text_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, const char *text, const char *font, const char *opt, int l,int n,int lo)
{	char *s=new char[l+1];	memcpy(s,text,l);	s[l]=0;
	char *f=new char[n+1];	memcpy(f,font,n);	f[n]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_text_xy(_GR_, _DA_(x),_DA_(y),s,f,o);
	delete []o;	delete []s;	delete []f;	}
//-----------------------------------------------------------------------------
void mgl_text_y_(uintptr_t *gr, uintptr_t *y, const char *text, const char *font, const char *opt, int l,int n,int lo)
{	char *s=new char[l+1];	memcpy(s,text,l);	s[l]=0;
	char *f=new char[n+1];	memcpy(f,font,n);	f[n]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_text_y(_GR_, _DA_(y),s,f,o);	delete []o;	delete []s;	delete []f;	}
//-----------------------------------------------------------------------------
//
//	Cont series
//
//-----------------------------------------------------------------------------
// NOTE! All data MUST have the same size! Only first slice is used!
void mgl_cont_gen(HMGL gr, float val, HCDT a, HCDT x, HCDT y, HCDT z, float c, int text,long ak)
{
	long n=a->GetNx(), m=a->GetNy();
	if(n<2 || m<2 || x->GetNx()*x->GetNy()!=n*m || y->GetNx()*y->GetNy()!=n*m || z->GetNx()*z->GetNy()!=n*m)
	{	gr->SetWarn(mglWarnDim,"ContGen");	return;	}

	mglPoint *kk = new mglPoint[2*n*m],p;
	float d, r, kx, ky;
	register long i,j,k, pc=0;
	// Usually number of points is much smaller. So, there is no reservation.
//	gr->Reserve(2*n*m);

	// add intersection point of isoline and Y axis
	for(i=0;i<n-1;i++)	for(j=0;j<m;j++)
	{
		d = mgl_d(val,a->v(i,j,ak),a->v(i+1,j,ak));
		if(d>=0 && d<1)
		{
			p = mglPoint(x->v(i,j)*(1-d)+x->v(i+1,j)*d,
						y->v(i,j)*(1-d)+y->v(i+1,j)*d,
						z->v(i,j)*(1-d)+z->v(i+1,j)*d);
			kk[pc] = mglPoint(i+d,j,gr->AddPnt(p,c));	pc++;
		}
	}
	// add intersection point of isoline and X axis
	for(i=0;i<n;i++)	for(j=0;j<m-1;j++)
	{
		d = mgl_d(val,a->v(i,j,ak),a->v(i,j+1,ak));
		if(d>=0 && d<1)
		{
			p = mglPoint(x->v(i,j)*(1-d)+x->v(i,j+1)*d,
						y->v(i,j)*(1-d)+y->v(i,j+1)*d,
						z->v(i,j)*(1-d)+z->v(i,j+1)*d);
			kk[pc] = mglPoint(i,j+d,gr->AddPnt(p,c));	pc++;
		}
	}
	// deallocate arrays and finish if no point
	if(pc==0)	{	free(kk);	return;	}
	// allocate arrays for curve (nn - next, ff - prev)
	long *nn = new long[pc], *ff = new long[pc];
	// -1 is not parsed, -2 starting
	for(i=0;i<pc;i++)	nn[i] = ff[i] = -1;
	// connect points to line
	long i11,i12,i21,i22,j11,j12,j21,j22;
	j=-1;	// current point
	do{
		if(j>=0)
		{
			kx = kk[j].x;	ky = kk[j].y;	i = -1;
			i11 = long(kx+1e-5);	i12 = long(kx-1e-5);
			j11 = long(ky+1e-5);	j12 = long(ky-1e-5);
			r=10;
			for(k=0;k<pc;k++)	// find closest point in grid
			{
				if(k==j || k==ff[j] || ff[k]!=-1)	continue;	// point is marked
				i21 = long(kk[k].x+1e-5);	i22 = long(kk[k].x-1e-5);
				j21 = long(kk[k].y+1e-5);	j22 = long(kk[k].y-1e-5);
				// check if in the same cell
				register bool cond = (i11==i21 || i11==i22 || i12==i21 || i12==i22) &&
						(j11==j21 || j11==j22 || j12==j21 || j12==j22);
				d = hypot(kk[k].x-kx,kk[k].y-ky);	// if several then select closest
				if(cond && d<r)	{	r=d;	i=k;	}
			}
			if(i<0)	j = -1;	// no free close points
			else			// mark the point
			{	nn[j] = i;	ff[i] = j;	j = nn[i]<0 ? i : -1;	}
		}
		if(j<0)
		{
			for(k=0;k<pc;k++)	if(nn[k]==-1)	// first check edges
			{
				if(kk[k].x==0 || fabs(kk[k].x-n+1)<1e-5 || kk[k].y==0 || fabs(kk[k].y-m+1)<1e-5)
				{	nn[k]=-2;	j = k;	break;	}
			}
			if(j<0)	for(k=0;k<pc;k++)	if(nn[k]==-1)	// or any points inside
			{	j = k;	nn[k]=-2;	break;	}
		}
	}while(j>=0);
	for(i=0;i<pc;i++)	{	ff[i] = long(0.5+kk[i].z);	}	// return to PntC numbering

	if(text)
	{
		wchar_t wcs[64];
		mglprintf(wcs,64,L"%4.3g",val);
		mglPoint p[25],t;
		float del = gr->TextWidth(wcs)*gr->GetFontSize()/16.;
		del = del>1 ? del:1;
		long k=1,i,j;
		bool less;
		p[0] =	gr->GetPnt(ff[0]);
		mgl_string_curve(gr,0,pc,ff,nn,wcs,"t");
		for(i=1;i<pc;i++)	// print it several times (for contours)
		{
			if(nn[i]<0)	continue;
			less = false;
			t = gr->GetPnt(ff[i]);
			for(j=0;j<k;j++)	if(Norm(t-p[j])<del)	{	less=true;	break;	}
			if(less)	continue;
			p[k] = t;	k++;
			mgl_string_curve(gr,i,pc,ff,nn,wcs,"t");
			if(k>=25)	break;
		}
	}
	for(i=0;i<pc;i++)	if(nn[i]>=0)	gr->line_plot(ff[i], ff[nn[i]]);
	delete []kk;	delete []nn;	delete []ff;
}
//-----------------------------------------------------------------------------
void mgl_cont_gen(HMGL gr, float val, HCDT a, HCDT x, HCDT y, HCDT z, const char *sch)
{
	bool text=(sch && strchr(sch,'t'));
	gr->SetPenPal(sch);
	mgl_cont_gen(gr,val,a,x,y,z,gr->CDef,text,0);
}
//-----------------------------------------------------------------------------
void mgl_cont_xy_val(HMGL gr, HCDT v, HCDT x, HCDT y, HCDT z, const char *sch, const char *opt)
{
	register long i,j,n=z->GetNx(),m=z->GetNy();
	if(x->GetNx()!=n)	{	gr->SetWarn(mglWarnDim,"Cont");	return;	}
	if(n<2 || m<2)		{	gr->SetWarn(mglWarnLow,"Cont");	return;	}
	bool both = x->GetNx()*x->GetNy()==m*n && y->GetNx()*y->GetNy()==m*n;
	if(y->GetNx()!=z->GetNy() && !both)	{	gr->SetWarn(mglWarnDim, "Cont");	return;	}
	gr->SaveState(opt);
	static int cgid=1;	gr->StartGroup("Cont",cgid++);

	bool text=(sch && strchr(sch,'t'));
	bool fixed=(sch && strchr(sch,'_'));
	long s=gr->AddTexture(sch);
	gr->SetPenPal(sch);

	mglData xx, yy, zz(z->GetNx(), z->GetNy());
	if(!both)	// make
	{
		xx.Create(z->GetNx(), z->GetNy());
		yy.Create(z->GetNx(), z->GetNy());
		for(i=0;i<n;i++)	for(j=0;j<m;j++)
		{	xx.a[i+n*j] = x->v(i);	yy.a[i+n*j] = y->v(j);	}
		x = &xx;	y = &yy;
	}
	// x, y -- have the same size z
	float z0, v0;
	for(j=0;j<z->GetNz();j++)	for(i=0;i<v->GetNx();i++)
	{
		v0 = v->v(i);		z0 = fixed ? gr->Min.z : v0;
		if(z->GetNz()>1)
			z0 = gr->Min.z+(gr->Max.z-gr->Min.z)*float(j)/(z->GetNz()-1);
		zz.Fill(z0,z0);
		mgl_cont_gen(gr,v0,z,x,y,&zz,gr->GetC(s,v0),text,j);
	}
	gr->EndGroup();
}
//-----------------------------------------------------------------------------
void mgl_cont_val(HMGL gr, HCDT v, HCDT z, const char *sch, const char *opt)
{
	if(z->GetNx()<2 || z->GetNy()<2)	{	gr->SetWarn(mglWarnLow,"Cont");	return;	}
	gr->SaveState(opt);
	mglData x(z->GetNx(), z->GetNy()), y(z->GetNx(), z->GetNy());
	x.Fill(gr->Min.x,gr->Max.x,'x');
	y.Fill(gr->Min.y,gr->Max.y,'y');
	mgl_cont_xy_val(gr,v,&x,&y,z,sch,0);
}
//-----------------------------------------------------------------------------
void mgl_cont_xy(HMGL gr, HCDT x, HCDT y, HCDT z, const char *sch, const char *opt)
{
	float r = gr->SaveState(opt);
	long Num = isnan(r)?7:long(r+0.5);
	if(Num<1)	{	gr->SetWarn(mglWarnCnt,"Cont");	return;	}
	mglData v(Num);
	for(long i=0;i<Num;i++)	v.a[i] = gr->Min.c + (gr->Max.c-gr->Min.c)*float(i+1)/(Num+1);
	mgl_cont_xy_val(gr,&v,x,y,z,sch,0);
}
//-----------------------------------------------------------------------------
void mgl_cont(HMGL gr, HCDT z, const char *sch, const char *opt)
{
	float r = gr->SaveState(opt);
	long Num = isnan(r)?7:long(r+0.5);
	if(Num<1)	{	gr->SetWarn(mglWarnCnt,"Cont");	return;	}
	mglData v(Num);
	for(long i=0;i<Num;i++)	v.a[i] = gr->Min.c + (gr->Max.c-gr->Min.c)*float(i+1)/(Num+1);
	mgl_cont_val(gr,&v,z,sch,0);
}
//-----------------------------------------------------------------------------
void mgl_cont_xy_val_(uintptr_t *gr, uintptr_t *v, uintptr_t *x, uintptr_t *y, uintptr_t *a, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_cont_xy_val(_GR_, _DA_(v), _DA_(x), _DA_(y), _DA_(a), s, o);
	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void mgl_cont_val_(uintptr_t *gr, uintptr_t *v, uintptr_t *a, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_cont_val(_GR_, _DA_(v), _DA_(a), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void mgl_cont_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *a, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_cont_xy(_GR_, _DA_(x), _DA_(y), _DA_(a), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void mgl_cont_(uintptr_t *gr, uintptr_t *a, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_cont(_GR_, _DA_(a), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
//
//	ContF series
//
//-----------------------------------------------------------------------------
long mgl_add_pnt(HMGL gr, float d, HCDT x, HCDT y, HCDT z, long i1, long j1, long i2, long j2, float c, bool edge)
{
	long res=-1;
	if(edge || (d>0 && d<1))
	{
		mglPoint p,u,v;
		p = mglPoint(x->v(i1,j1)*(1-d)+x->v(i2,j2)*d,
					 y->v(i1,j1)*(1-d)+y->v(i2,j2)*d,
					 z->v(i1,j1)*(1-d)+z->v(i2,j2)*d);
		u = mglPoint(x->dvx(i1,j1)*(1-d)+x->dvx(i2,j2)*d,
					 y->dvx(i1,j1)*(1-d)+y->dvx(i2,j2)*d,
					 z->dvx(i1,j1)*(1-d)+z->dvx(i2,j2)*d);
		v = mglPoint(x->dvy(i1,j1)*(1-d)+x->dvy(i2,j2)*d,
					 y->dvy(i1,j1)*(1-d)+y->dvy(i2,j2)*d,
					 z->dvy(i1,j1)*(1-d)+z->dvy(i2,j2)*d);
		res = gr->AddPnt(p,c,u^v);
	}
	return res;
}
//-----------------------------------------------------------------------------
void mgl_add_range(HMGL gr, HCDT a, HCDT x, HCDT y, HCDT z, long i1, long j1, long di, long dj, float c, long &u1, long &u2, long ak, float v1, float v2)
{
	long i2=i1+di, j2=j1+dj;

	float f1 = a->v(i1,j1,ak),	f2 = a->v(i2,j2,ak), d1, d2;
	d1 = mgl_d(v1,f1,f2);
	u1 = mgl_add_pnt(gr,d1,x,y,z,i1,j1,i2,j2,c,false);
	d2 = mgl_d(v2,f1,f2);
	u2 = mgl_add_pnt(gr,d2,x,y,z,i1,j1,i2,j2,c,false);
	if(d1>d2)	{	j2=u1;	u1=u2;	u2=j2;	}
}
//-----------------------------------------------------------------------------
void mgl_add_edges(HMGL gr, HCDT a, HCDT x, HCDT y, HCDT z, long i1, long j1, long di, long dj, float c, long &u1, long &u2, long ak, float v1, float v2)
{
	long i2=i1+di, j2=j1+dj;
	u1 = u2 = -1;

	float f1 = a->v(i1,j1,ak),	f2 = a->v(i2,j2,ak);
	if(f1<=v2 && f1>=v1)
		u1 = mgl_add_pnt(gr,0,x,y,z,i1,j1,i2,j2,c,true);
	if(f2<=v2 && f2>=v1)
		u2 = mgl_add_pnt(gr,1,x,y,z,i1,j1,i2,j2,c,true);
}
//-----------------------------------------------------------------------------
void mgl_contf_gen(HMGL gr, float v1, float v2, HCDT a, HCDT x, HCDT y, HCDT z, float c, long ak)
{
	long n=a->GetNx(), m=a->GetNy();
	if(n<2 || m<2 || x->GetNx()*x->GetNy()!=n*m || y->GetNx()*y->GetNy()!=n*m || z->GetNx()*z->GetNy()!=n*m)
	{	gr->SetWarn(mglWarnDim,"ContFGen");	return;	}

	register long i,j;
	gr->Reserve(n*m);
	long *kk = new long[4*n], l1,l2, r1,r2, t1,t2, u1,u2, b1,b2, d1,d2, p[8],num;
	memset(kk,-1,2*n*sizeof(long));
	for(i=0;i<n-1;i++)	// add intersection points for first line
	{
		mgl_add_range(gr,a,x,y,z, i,0,1,0, c,u1,u2, ak,v1,v2);
		kk[4*i]=u1;		kk[4*i+1]=u2;
		mgl_add_edges(gr,a,x,y,z, i,0,1,0, c,d1,d2, ak,v1,v2);
		kk[4*i+2]=d1;		kk[4*i+3]=d2;
	}
	for(j=1;j<m;j++)	// add intersection points
	{
		mgl_add_range(gr,a,x,y,z, 0,j-1,0,1, c,r1,r2, ak,v1,v2);
		for(i=0;i<n-1;i++)
		{
			l1 = r1;		l2 = r2;	num=0;
			t1 = kk[4*i];	t2 = kk[4*i+1];
			b1 = kk[4*i+2];	b2 = kk[4*i+3];
			// right edge
			mgl_add_range(gr,a,x,y,z, i+1,j-1,0,1, c,r1,r2, ak,v1,v2);
			// top edge
			mgl_add_range(gr,a,x,y,z, i,j,1,0, c,u1,u2, ak,v1,v2);
			kk[4*i]=u1;		kk[4*i+1]=u2;
			mgl_add_edges(gr,a,x,y,z, i,j,1,0, c,d1,d2, ak,v1,v2);
			kk[4*i+2]=d1;	kk[4*i+3]=d2;
			// collect points
			if(b1>=0)	p[num++] = b1;	if(t1>=0)	p[num++] = t1;
			if(t2>=0)	p[num++] = t2;	if(b2>=0)	p[num++] = b2;
			if(r1>=0)	p[num++] = r1;	if(r2>=0)	p[num++] = r2;
			if(d2>=0)	p[num++] = d2;	if(u2>=0)	p[num++] = u2;
			if(u1>=0)	p[num++] = u1;	if(d1>=0)	p[num++] = d1;
			if(l2>=0)	p[num++] = l2;	if(l1>=0)	p[num++] = l1;
			// draw it
			if(num<3)	continue;
			if(num==4)	gr->quad_plot(p[0],p[1],p[3],p[2]);
			else if(num==3)	gr->trig_plot(p[0],p[1],p[2]);
			else if(num==5)
			{
				gr->quad_plot(p[0],p[1],p[3],p[2]);
				gr->trig_plot(p[0],p[3],p[4]);
			}
			else if(num==6)
			{
				gr->quad_plot(p[0],p[1],p[3],p[2]);
				gr->quad_plot(p[0],p[3],p[5],p[4]);
			}
		}
	}
	delete []kk;
}
//-----------------------------------------------------------------------------
void mgl_contf_gen(HMGL gr, float v1, float v2, HCDT a, HCDT x, HCDT y, HCDT z, const char *c)
{
	gr->SetPenPal(c);
	mgl_contf_gen(gr,v1,v2,a,x,y,z,gr->CDef,0);
}
//-----------------------------------------------------------------------------
void mgl_contf_xy_val(HMGL gr, HCDT v, HCDT x, HCDT y, HCDT z, const char *sch, const char *opt)
{
	register long i,j,n=z->GetNx(),m=z->GetNy();
	if(x->GetNx()!=n)	{	gr->SetWarn(mglWarnDim,"ContF");	return;	}
	if(n<2 || m<2)		{	gr->SetWarn(mglWarnLow,"ContF");	return;	}
	bool both = x->GetNx()*x->GetNy()==m*n && y->GetNx()*y->GetNy()==m*n;
	if(y->GetNx()!=z->GetNy() && !both)	{	gr->SetWarn(mglWarnDim, "ContF");	return;	}
	gr->SaveState(opt);
	static int cgid=1;	gr->StartGroup("ContF",cgid++);
	long s=gr->AddTexture(sch);

	bool fixed=(sch && strchr(sch,'_'));
	mglData xx, yy, zz(z->GetNx(), z->GetNy());
	if(!both)	// make
	{
		xx.Create(z->GetNx(), z->GetNy());
		yy.Create(z->GetNx(), z->GetNy());
		for(j=0;j<m;j++)	for(i=0;i<n;i++)
		{	xx.a[i+n*j] = x->v(i);	yy.a[i+n*j] = y->v(j);	}
		x = &xx;	y = &yy;
	}
	// x, y -- have the same size z
	float z0, v0;
	for(j=0;j<z->GetNz();j++)	for(i=0;i<v->GetNx()-1;i++)
	{
		v0 = v->v(i);		z0 = fixed ? gr->Min.z : v0;
		if(z->GetNz()>1)
			z0 = gr->Min.z+(gr->Max.z-gr->Min.z)*float(j)/(z->GetNz()-1);
		zz.Fill(z0,z0);
		mgl_contf_gen(gr,v0,v->v(i+1),z,x,y,&zz,gr->GetC(s,v0),j);
	}
	gr->EndGroup();
}
//-----------------------------------------------------------------------------
void mgl_contf_val(HMGL gr, HCDT v, HCDT z, const char *sch, const char *opt)
{
	if(z->GetNx()<2 || z->GetNy()<2)	{	gr->SetWarn(mglWarnLow,"Cont");	return;	}
	gr->SaveState(opt);
	mglData x(z->GetNx(), z->GetNy()), y(z->GetNx(), z->GetNy());
	x.Fill(gr->Min.x,gr->Max.x,'x');
	y.Fill(gr->Min.y,gr->Max.y,'y');
	mgl_contf_xy_val(gr,v,&x,&y,z,sch,0);
}
//-----------------------------------------------------------------------------
void mgl_contf_xy(HMGL gr, HCDT x, HCDT y, HCDT z, const char *sch, const char *opt)
{
	float r = gr->SaveState(opt);
	long Num = isnan(r)?7:long(r+0.5);
	if(Num<1)	{	gr->SetWarn(mglWarnCnt,"Cont");	return;	}
	mglData v(Num);	v.Fill(gr->Min.c, gr->Max.c);
	mgl_contf_xy_val(gr,&v,x,y,z,sch,0);
}
//-----------------------------------------------------------------------------
void mgl_contf(HMGL gr, HCDT z, const char *sch, const char *opt)
{
	float r = gr->SaveState(opt);
	long Num = isnan(r)?7:long(r+0.5);
	if(Num<1)	{	gr->SetWarn(mglWarnCnt,"Cont");	return;	}
	mglData v(Num);	v.Fill(gr->Min.c, gr->Max.c);
	mgl_contf_val(gr,&v,z,sch,0);
}
//-----------------------------------------------------------------------------
void mgl_contf_xy_val_(uintptr_t *gr, uintptr_t *v, uintptr_t *x, uintptr_t *y, uintptr_t *a, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_contf_xy_val(_GR_, _DA_(v), _DA_(x), _DA_(y), _DA_(a), s, o);
	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void mgl_contf_val_(uintptr_t *gr, uintptr_t *v, uintptr_t *a, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_contf_val(_GR_, _DA_(v), _DA_(a), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void mgl_contf_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *a, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_contf_xy(_GR_, _DA_(x), _DA_(y), _DA_(a), s, o);
	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void mgl_contf_(uintptr_t *gr, uintptr_t *a, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_contf(_GR_, _DA_(a), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
//
//	ContD series
//
//-----------------------------------------------------------------------------
int mgl_get_ncol(const char *sch, char *res)
{
	int i,j,n=strlen(sch);
	for(i=j=0;i<n;i++)	if(mglColor(sch[i]).Valid())
	{	if(res)	res[j]=sch[i];	j++;	}
	return j;
}
//-----------------------------------------------------------------------------
void mgl_contd_xy_val(HMGL gr, HCDT v, HCDT x, HCDT y, HCDT z, const char *sch, const char *opt)
{
	register long i,j,n=z->GetNx(),m=z->GetNy();
	if(x->GetNx()!=n)	{	gr->SetWarn(mglWarnDim,"ContD");	return;	}
	if(n<2 || m<2)		{	gr->SetWarn(mglWarnLow,"ContD");	return;	}
	bool both = x->GetNx()*x->GetNy()==m*n && y->GetNx()*y->GetNy()==m*n;
	if(y->GetNx()!=z->GetNy() && !both)	{	gr->SetWarn(mglWarnDim, "ContD");	return;	}
	gr->SaveState(opt);
	static int cgid=1;	gr->StartGroup("ContD",cgid++);
	if(!sch || !sch[0])	sch = MGL_DEF_PAL;
//	char *cc = new char[strlen(sch)];
	long s = gr->AddTexture(sch,1);
	int nc = gr->GetNumPal(s*256);

	bool fixed=(sch && strchr(sch,'_'));
	mglData xx, yy, zz(z->GetNx(), z->GetNy());
	if(!both)	// make
	{
		xx.Create(z->GetNx(), z->GetNy());
		yy.Create(z->GetNx(), z->GetNy());
		for(i=0;i<n;i++)	for(j=0;j<m;j++)
		{	xx.a[i+n*j] = x->v(i);	yy.a[i+n*j] = y->v(j);	}
		x = &xx;	y = &yy;
	}
	// x, y -- have the same size z
	float z0, v0, dc = nc>1 ? 1/(MGL_FLT_EPS*(n-1)) : 0;
	for(j=0;j<z->GetNz();j++)	for(i=0;i<v->GetNx()-1;i++)
	{
		v0 = v->v(i);		z0 = fixed ? gr->Min.z : v0;
		if(z->GetNz()>1)
			z0 = gr->Min.z+(gr->Max.z-gr->Min.z)*float(j)/(z->GetNz()-1);
		zz.Fill(z0,z0);
		mgl_contf_gen(gr,v0,v->v(i+1),z,x,y,&zz,s+i*dc,j);
	}
	gr->EndGroup();
}
//-----------------------------------------------------------------------------
void mgl_contd_val(HMGL gr, HCDT v, HCDT z, const char *sch, const char *opt)
{
	if(z->GetNx()<2 || z->GetNy()<2)	{	gr->SetWarn(mglWarnLow,"ContD");	return;	}
	gr->SaveState(opt);
	mglData x(z->GetNx(), z->GetNy()), y(z->GetNx(), z->GetNy());
	x.Fill(gr->Min.x,gr->Max.x,'x');
	y.Fill(gr->Min.y,gr->Max.y,'y');
	mgl_contd_xy_val(gr,v,&x,&y,z,sch,0);
}
//-----------------------------------------------------------------------------
void mgl_contd_xy(HMGL gr, HCDT x, HCDT y, HCDT z, const char *sch, const char *opt)
{
	if(!sch || !sch[0])	sch = MGL_DEF_PAL;
	gr->SaveState(opt);
	mglData v(mgl_get_ncol(sch,0)+1);
	v.Fill(gr->Min.c, gr->Max.c);
	mgl_contd_xy_val(gr,&v,x,y,z,sch,0);
}
//-----------------------------------------------------------------------------
void mgl_contd(HMGL gr, HCDT z, const char *sch, const char *opt)
{
	if(!sch || !sch[0])	sch = MGL_DEF_PAL;
	gr->SaveState(opt);
	mglData v(mgl_get_ncol(sch,0)+1);
	v.Fill(gr->Min.c, gr->Max.c);
	mgl_contd_val(gr,&v,z,sch,0);
}
//-----------------------------------------------------------------------------
void mgl_contd_xy_val_(uintptr_t *gr, uintptr_t *v, uintptr_t *x, uintptr_t *y, uintptr_t *a, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_contd_xy_val(_GR_, _DA_(v), _DA_(x), _DA_(y), _DA_(a), s, o);
	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void mgl_contd_val_(uintptr_t *gr, uintptr_t *v, uintptr_t *a, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_contd_val(_GR_, _DA_(v), _DA_(a), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void mgl_contd_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *a, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_contd_xy(_GR_, _DA_(x), _DA_(y), _DA_(a), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void mgl_contd_(uintptr_t *gr, uintptr_t *a, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_contd(_GR_, _DA_(a), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
//
//	Cont3 series
//
//-----------------------------------------------------------------------------
struct _mgl_slice	{	mglData x,y,z,a;	};
//-----------------------------------------------------------------------------
void mgl_get_slice(_mgl_slice &s, HCDT x, HCDT y, HCDT z, HCDT a, char dir, float d, bool both)
{
	register long i,j,i0,n=a->GetNx(),m=a->GetNy(),l=a->GetNz(), nx=1,ny=1,p;

	switch(dir)		// îáùàÿ ÷àñòü
	{
	case 'x':	nx = m;	ny = l;	if(d<0)	d = n/2;	break;
	case 'y':	nx = n;	ny = l;	if(d<0)	d = m/2;	break;
	case 'z':	nx = n;	ny = m;	if(d<0)	d = l/2;	break;
	}
	s.x.Create(nx,ny);	s.y.Create(nx,ny);
	s.z.Create(nx,ny);	s.a.Create(nx,ny);
	p = long(d);	d -= p;
	float v;

	if(both)
	{
		switch(dir)
		{
		case 'x':
			for(j=0;j<ny;j++)	for(i=0;i<nx;i++)
			{
				i0 = i+nx*j;
				s.x.a[i0] = x->v(p,i,j)*(1-d) + x->v(p+1,i,j)*d;
				s.y.a[i0] = y->v(p,i,j)*(1-d) + y->v(p+1,i,j)*d;
				s.z.a[i0] = z->v(p,i,j)*(1-d) + z->v(p+1,i,j)*d;
				s.a.a[i0] = a->v(p,i,j)*(1-d) + a->v(p+1,i,j)*d;
			}
			break;
		case 'y':
			for(j=0;j<ny;j++)	for(i=0;i<nx;i++)
			{
				i0 = i+nx*j;
				s.x.a[i0] = x->v(i,p,j)*(1-d) + x->v(i,p+1,j)*d;
				s.y.a[i0] = y->v(i,p,j)*(1-d) + y->v(i,p+1,j)*d;
				s.z.a[i0] = z->v(i,p,j)*(1-d) + z->v(i,p+1,j)*d;
				s.a.a[i0] = a->v(i,p,j)*(1-d) + a->v(i,p+1,j)*d;
			}
			break;
		case 'z':
			for(j=0;j<ny;j++)	for(i=0;i<nx;i++)
			{
				i0 = i+nx*j;
				s.x.a[i0] = x->v(i,j,p)*(1-d) + x->v(i,j,p+1)*d;
				s.y.a[i0] = y->v(i,j,p)*(1-d) + y->v(i,j,p+1)*d;
				s.z.a[i0] = z->v(i,j,p)*(1-d) + z->v(i,j,p+1)*d;
				s.a.a[i0] = a->v(i,j,p)*(1-d) + a->v(i,j,p+1)*d;
			}
			break;
		}
	}
	else	// x, y, z -- vectors
	{
		switch(dir)
		{
		case 'x':
			v = x->v(p)*(1-d)+x->v(p+1)*d;
			for(j=0;j<ny;j++)	for(i=0;i<nx;i++)
			{
				i0 = i+nx*j;	s.x.a[i0] = v;
				s.y.a[i0] = y->v(i);	s.z.a[i0] = z->v(j);
				s.a.a[i0] = a->v(p,i,j)*(1-d) + a->v(p+1,i,j)*d;
			}
			break;
		case 'y':
			v = y->v(p)*(1-d)+y->v(p+1)*d;
			for(j=0;j<ny;j++)	for(i=0;i<nx;i++)
			{
				i0 = i+nx*j;	s.y.a[i0] = v;
				s.x.a[i0] = x->v(i);	s.z.a[i0] = z->v(j);
				s.a.a[i0] = a->v(i,p,j)*(1-d) + a->v(i,p+1,j)*d;
			}
			break;
		case 'z':
			v = z->v(p)*(1-d)+z->v(p+1)*d;
			for(j=0;j<ny;j++)	for(i=0;i<nx;i++)
			{
				i0 = i+nx*j;	s.z.a[i0] = v;
				s.x.a[i0] = x->v(i);	s.y.a[i0] = y->v(j);
				s.a.a[i0] = a->v(i,j,p)*(1-d) + a->v(i,j,p+1)*d;
			}
			break;
		}
	}
}
//-----------------------------------------------------------------------------
void mgl_cont3_xyz_val(HMGL gr, HCDT v, HCDT x, HCDT y, HCDT z, HCDT a, char dir, float sVal, const char *sch, const char *opt)
{
	long n=a->GetNx(),m=a->GetNy(),l=a->GetNz();
	if(n<2 || m<2 || l<2)	{	gr->SetWarn(mglWarnLow,"Cont3");	return;	}
	bool both = x->GetNx()*x->GetNy()*x->GetNz()==n*m*l && y->GetNx()*y->GetNy()*y->GetNz()==n*m*l && z->GetNx()*z->GetNy()*z->GetNz()==n*m*l;
	if(!(both || (x->GetNx()==n && y->GetNx()==m && z->GetNx()==l)))
	{	gr->SetWarn(mglWarnDim,"Cont3");	return;	}
	gr->SaveState(opt);
	static int cgid=1;	gr->StartGroup("Cont3",cgid++);

	bool text=(sch && strchr(sch,'t'));
	long ss=gr->AddTexture(sch);
	gr->SetPenPal(sch);

	_mgl_slice s;
	mgl_get_slice(s,x,y,z,a,dir,sVal,both);
	for(long i=0;i<v->GetNx();i++)
	{
		float v0 = v->v(i);
		mgl_cont_gen(gr,v0,&s.a,&s.x,&s.y,&s.z,gr->GetC(ss,v0),text,0);
	}
	gr->EndGroup();
}
//-----------------------------------------------------------------------------
void mgl_cont3_val(HMGL gr, HCDT v, HCDT a, char dir, float sVal, const char *sch, const char *opt)
{
	if(a->GetNx()<2 || a->GetNy()<2 || a->GetNz()<2)
	{	gr->SetWarn(mglWarnLow,"Cont3");	return;	}
	gr->SaveState(opt);
	mglData x(a->GetNx()), y(a->GetNy()),z(a->GetNz());
	x.Fill(gr->Min.x,gr->Max.x);
	y.Fill(gr->Min.y,gr->Max.y);
	z.Fill(gr->Min.z,gr->Max.z);
	mgl_cont3_xyz_val(gr,v,&x,&y,&z,a,dir,sVal,sch,0);
}
//-----------------------------------------------------------------------------
void mgl_cont3_xyz(HMGL gr, HCDT x, HCDT y, HCDT z, HCDT a, char dir, float sVal, const char *sch, const char *opt)
{
	float r = gr->SaveState(opt);
	long Num = isnan(r)?7:long(r+0.5);
	if(Num<1)	{	gr->SetWarn(mglWarnCnt,"Cont3");	return;	}
	mglData v(Num);
	for(long i=0;i<Num;i++)	v.a[i] = gr->Min.c + (gr->Max.c-gr->Min.c)*float(i+1)/(Num+1);
	mgl_cont3_xyz_val(gr,&v,x,y,z,a,dir,sVal,sch,0);
}
//-----------------------------------------------------------------------------
void mgl_cont3(HMGL gr, HCDT a, char dir, float sVal, const char *sch, const char *opt)
{
	float r = gr->SaveState(opt);
	long Num = isnan(r)?7:long(r+0.5);
	if(Num<1)	{	gr->SetWarn(mglWarnCnt,"Cont3");	return;	}
	mglData v(Num);
	for(long i=0;i<Num;i++)	v.a[i] = gr->Min.c + (gr->Max.c-gr->Min.c)*float(i+1)/(Num+1);
	mgl_cont3_val(gr,&v,a,dir,sVal,sch,0);
}
//-----------------------------------------------------------------------------
void mgl_cont3_xyz_val_(uintptr_t *gr, uintptr_t *v, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a, const char *dir, float *sVal, const char *sch, const char *opt,int,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_cont3_xyz_val(_GR_, _DA_(v), _DA_(x), _DA_(y), _DA_(z), _DA_(a), *dir, *sVal, s, o);
	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void mgl_cont3_val_(uintptr_t *gr, uintptr_t *v, uintptr_t *a, const char *dir, float *sVal, const char *sch, const char *opt,int,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_cont3_val(_GR_, _DA_(v), _DA_(a), *dir, *sVal, s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void mgl_cont3_xyz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a, const char *dir, float *sVal, const char *sch, const char *opt,int,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_cont3_xyz(_GR_, _DA_(x), _DA_(y), _DA_(z), _DA_(a), *dir, *sVal, s, o);
	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void mgl_cont3_(uintptr_t *gr, uintptr_t *a, const char *dir, float *sVal, const char *sch, const char *opt,int,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_cont3(_GR_, _DA_(a), *dir, *sVal, s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
//
//	Dens3 series
//
//-----------------------------------------------------------------------------
void mgl_dens3_xyz(HMGL gr, HCDT x, HCDT y, HCDT z, HCDT a, char dir, float sVal, const char *sch, const char *opt)
{
	long n=a->GetNx(),m=a->GetNy(),l=a->GetNz();
	if(n<2 || m<2 || l<2)	{	gr->SetWarn(mglWarnLow,"Dens3");	return;	}
	bool both = x->GetNx()*x->GetNy()*x->GetNz()==n*m*l && y->GetNx()*y->GetNy()*y->GetNz()==n*m*l && z->GetNx()*z->GetNy()*z->GetNz()==n*m*l;
	if(!(both || (x->GetNx()==n && y->GetNx()==m && z->GetNx()==l)))
	{	gr->SetWarn(mglWarnDim,"Dens3");	return;	}
	gr->SaveState(opt);
	static int cgid=1;	gr->StartGroup("Dens3",cgid++);

	_mgl_slice s;
	mgl_get_slice(s,x,y,z,a,dir,sVal,both);
	mgl_surfc_xy(gr,&s.x,&s.y,&s.z,&s.a,sch,0);
}
//-----------------------------------------------------------------------------
void mgl_dens3(HMGL gr, HCDT a, char dir, float sVal, const char *sch, const char *opt)
{
	if(a->GetNx()<2 || a->GetNy()<2 || a->GetNz()<2)
	{	gr->SetWarn(mglWarnLow,"Dens3");	return;	}
	gr->SaveState(opt);
	mglData x(a->GetNx()), y(a->GetNy()),z(a->GetNz());
	x.Fill(gr->Min.x,gr->Max.x);
	y.Fill(gr->Min.y,gr->Max.y);
	z.Fill(gr->Min.z,gr->Max.z);
	mgl_dens3_xyz(gr,&x,&y,&z,a,dir,sVal,sch,0);
}
//-----------------------------------------------------------------------------
void mgl_dens3_xyz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a, const char *dir, float *sVal, const char *sch, const char *opt,int,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_dens3_xyz(_GR_, _DA_(x), _DA_(y), _DA_(z), _DA_(a), *dir, *sVal, s, o);
	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void mgl_dens3_(uintptr_t *gr, uintptr_t *a, const char *dir, float *sVal, const char *sch, const char *opt,int,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_dens3(_GR_, _DA_(a), *dir, *sVal, s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
//
//	Grid3 series
//
//-----------------------------------------------------------------------------
void mgl_grid3_xyz(HMGL gr, HCDT x, HCDT y, HCDT z, HCDT a, char dir, float sVal, const char *sch, const char *opt)
{
	long n=a->GetNx(),m=a->GetNy(),l=a->GetNz();
	if(n<2 || m<2 || l<2)	{	gr->SetWarn(mglWarnLow,"Grid3");	return;	}
	bool both = x->GetNx()*x->GetNy()*x->GetNz()==n*m*l && y->GetNx()*y->GetNy()*y->GetNz()==n*m*l && z->GetNx()*z->GetNy()*z->GetNz()==n*m*l;
	if(!(both || (x->GetNx()==n && y->GetNx()==m && z->GetNx()==l)))
	{	gr->SetWarn(mglWarnDim,"Grid3");	return;	}
	gr->SaveState(opt);
	static int cgid=1;	gr->StartGroup("Grid3",cgid++);

	_mgl_slice s;
	mgl_get_slice(s,x,y,z,a,dir,sVal,both);
	mgl_mesh_xy(gr,&s.x,&s.y,&s.z,sch,0);
}
//-----------------------------------------------------------------------------
void mgl_grid3(HMGL gr, HCDT a, char dir, float sVal, const char *sch, const char *opt)
{
	if(a->GetNx()<2 || a->GetNy()<2 || a->GetNz()<2)
	{	gr->SetWarn(mglWarnLow,"Grid3");	return;	}
	gr->SaveState(opt);
	mglData x(a->GetNx()), y(a->GetNy()), z(a->GetNz());
	x.Fill(gr->Min.x,gr->Max.x);
	y.Fill(gr->Min.y,gr->Max.y);
	z.Fill(gr->Min.z,gr->Max.z);
	mgl_grid3_xyz(gr,&x,&y,&z,a,dir,sVal,sch,0);
}
//-----------------------------------------------------------------------------
void mgl_grid3_xyz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a, const char *dir, float *sVal, const char *sch, const char *opt,int,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_grid3_xyz(_GR_, _DA_(x), _DA_(y), _DA_(z), _DA_(a), *dir, *sVal, s, o);
	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void mgl_grid3_(uintptr_t *gr, uintptr_t *a, const char *dir, float *sVal, const char *sch, const char *opt,int,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_grid3(_GR_, _DA_(a), *dir, *sVal, s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
//
//	ContF3 series
//
//-----------------------------------------------------------------------------
void mgl_contf3_xyz_val(HMGL gr, HCDT v, HCDT x, HCDT y, HCDT z, HCDT a, char dir, float sVal, const char *sch, const char *opt)
{
	long n=a->GetNx(),m=a->GetNy(),l=a->GetNz();
	if(n<2 || m<2 || l<2)	{	gr->SetWarn(mglWarnLow,"ContF3");	return;	}
	bool both = x->GetNx()*x->GetNy()*x->GetNz()==n*m*l && y->GetNx()*y->GetNy()*y->GetNz()==n*m*l && z->GetNx()*z->GetNy()*z->GetNz()==n*m*l;
	if(!(both || (x->GetNx()==n && y->GetNx()==m && z->GetNx()==l)))
	{	gr->SetWarn(mglWarnDim,"ContF3");	return;	}
	gr->SaveState(opt);
	static int cgid=1;	gr->StartGroup("ContF3",cgid++);

	long ss=gr->AddTexture(sch);
	_mgl_slice s;
	mgl_get_slice(s,x,y,z,a,dir,sVal,both);
	for(long i=0;i<v->GetNx();i++)
	{
		float v0 = v->v(i);
		mgl_contf_gen(gr,v0,v->v(i+1),&s.a,&s.x,&s.y,&s.z,gr->GetC(ss,v0),0);
	}
	gr->EndGroup();
}
//-----------------------------------------------------------------------------
void mgl_contf3_val(HMGL gr, HCDT v, HCDT a, char dir, float sVal, const char *sch, const char *opt)
{
	if(a->GetNx()<2 || a->GetNy()<2 || a->GetNz()<2)
	{	gr->SetWarn(mglWarnLow,"ContF3");	return;	}
	gr->SaveState(opt);
	mglData x(a->GetNx()), y(a->GetNy()),z(a->GetNz());
	x.Fill(gr->Min.x,gr->Max.x);
	y.Fill(gr->Min.y,gr->Max.y);
	z.Fill(gr->Min.z,gr->Max.z);
	mgl_contf3_xyz_val(gr,v,&x,&y,&z,a,dir,sVal,sch,0);
}
//-----------------------------------------------------------------------------
void mgl_contf3_xyz(HMGL gr, HCDT x, HCDT y, HCDT z, HCDT a, char dir, float sVal, const char *sch, const char *opt)
{
	float r = gr->SaveState(opt);
	long Num = isnan(r)?7:long(r+0.5);
	if(Num<1)	{	gr->SetWarn(mglWarnCnt,"ContF3");	return;	}
	mglData v(Num+2);
	for(long i=0;i<Num+2;i++)	v.a[i] = gr->Min.c + (gr->Max.c-gr->Min.c)*float(i)/(Num+1);
	mgl_contf3_xyz_val(gr,&v,x,y,z,a,dir,sVal,sch,0);
}
//-----------------------------------------------------------------------------
void mgl_contf3(HMGL gr, HCDT a, char dir, float sVal, const char *sch, const char *opt)
{
	float r = gr->SaveState(opt);
	long Num = isnan(r)?7:long(r+0.5);
	if(Num<1)	{	gr->SetWarn(mglWarnCnt,"ContF3");	return;	}
	mglData v(Num+2);	v.Fill(gr->Min.c, gr->Max.c);
	mgl_contf3_val(gr,&v,a,dir,sVal,sch,0);
}
//-----------------------------------------------------------------------------
void mgl_contf3_xyz_val_(uintptr_t *gr, uintptr_t *v, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a, const char *dir, float *sVal, const char *sch, const char *opt,int,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_contf3_xyz_val(_GR_, _DA_(v), _DA_(x), _DA_(y), _DA_(z), _DA_(a), *dir, *sVal, s, o);
	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void mgl_contf3_val_(uintptr_t *gr, uintptr_t *v, uintptr_t *a, const char *dir, float *sVal, const char *sch, const char *opt,int,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_contf3_val(_GR_, _DA_(v), _DA_(a), *dir, *sVal, s, o);
	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void mgl_contf3_xyz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a, const char *dir, float *sVal, const char *sch, const char *opt,int,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_contf3_xyz(_GR_, _DA_(x), _DA_(y), _DA_(z), _DA_(a), *dir, *sVal, s, o);
	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void mgl_contf3_(uintptr_t *gr, uintptr_t *a, const char *dir, float *sVal, const char *sch, const char *opt,int,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_contf3(_GR_, _DA_(a), *dir, *sVal, s, o);
	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
//
//	Axial series
//
//-----------------------------------------------------------------------------
long mgl_find_prev(long i, long pc, long *nn)
{
	for(long k=0;k<pc;k++)	if(nn[k]==i)	return k;
	return -1;
}
void mgl_axial_plot(mglBase *gr,long pc, mglPoint *ff, long *nn,char dir,float cc,bool wire)
{
	mglPoint a(0,0,1),b,c,p,q1,q2;
	if(dir=='x')	a = mglPoint(1,0,0);
	if(dir=='y')	a = mglPoint(0,1,0);
	b = !a;	c = a^b;

	register long i,j,k;
	float fi,si,co;
	long p1,p2,p3,p4;
	if(wire)	gr->Reserve(pc*82);	else	gr->Reserve(pc*82);
	for(i=0;i<pc;i++)
	{
		k = mgl_find_prev(i,pc,nn);
		if(nn[i]<0)	continue;
		q1 = k<0 ? ff[nn[i]]-ff[i]  : (ff[nn[i]]-ff[k])*0.5;
		q2 = nn[nn[i]]<0 ? ff[nn[i]]-ff[i]  : (ff[nn[nn[i]]]-ff[i])*0.5;

		p = a*ff[i].y + c*ff[i].x;
		p1 = wire ? gr->AddPnt(p,cc) : gr->AddPnt(p,cc,(a*q1.y + c*q1.x)^b);
		p = a*ff[nn[i]].y + c*ff[nn[i]].x;
		p2 = wire ? gr->AddPnt(p,cc) : gr->AddPnt(p,cc,(a*q2.y + c*q2.x)^b);
		if(wire)	gr->line_plot(p1,p2);

		for(j=1;j<41;j++)
		{
			p3 = p1;	p4 = p2;
			fi = j*M_PI/20;		si = sin(fi);	co = cos(fi);
			p = a*ff[i].y + b*(si*ff[i].x) +  c*(co*ff[i].x);
			p1 = wire ?	gr->AddPnt(p,cc) : gr->AddPnt(p,cc,(a*q1.y + b*(si*q1.x) +  c*(co*q1.x))^(b*co-c*si));
			p = a*ff[nn[i]].y + b*(si*ff[nn[i]].x) +  c*(co*ff[nn[i]].x);
			p2 = wire ?	gr->AddPnt(p,cc) : gr->AddPnt(p,cc,(a*q2.y + b*(si*q2.x) +  c*(co*q2.x))^(b*co-c*si));
			if(wire)
			{	gr->line_plot(p1,p2);	gr->line_plot(p1,p3);
				gr->line_plot(p4,p2);	gr->line_plot(p4,p3);	}
			else	gr->quad_plot(p3,p4,p1,p2);
		}
	}
}
//-----------------------------------------------------------------------------
// NOTE! All data MUST have the same size! Only first slice is used!
void mgl_axial_gen(HMGL gr, float val, HCDT a, HCDT x, HCDT y, float c, char dir,long ak,bool wire)
{
	long n=a->GetNx(), m=a->GetNy();
	if(n<2 || m<2 || x->GetNx()*x->GetNy()!=n*m || y->GetNx()*y->GetNy()!=n*m)
	{	gr->SetWarn(mglWarnDim,"ContGen");	return;	}

	mglPoint *kk = new mglPoint[2*n*m],*pp = new mglPoint[2*n*m],p;
	float d, kx, ky;
	register long i,j,k, pc=0;
	// Usually number of points is much smaller. So, there is no reservation.
//	gr->Reserve(2*n*m);

	// add intersection point of isoline and Y axis
	for(j=0;j<m;j++)	for(i=0;i<n-1;i++)
	{
		d = mgl_d(val,a->v(i,j,ak),a->v(i+1,j,ak));
		if(d>=0 && d<1)
		{
			pp[pc] = mglPoint(x->v(i,j)*(1-d)+x->v(i+1,j)*d,
						y->v(i,j)*(1-d)+y->v(i+1,j)*d);
			kk[pc] = mglPoint(i+d,j);	pc++;
		}
	}
	// add intersection point of isoline and X axis
	for(j=0;j<m-1;j++)	for(i=0;i<n;i++)
	{
		d = mgl_d(val,a->v(i,j,ak),a->v(i,j+1,ak));
		if(d>=0 && d<1)
		{
			pp[pc] = mglPoint(x->v(i,j)*(1-d)+x->v(i,j+1)*d,
						y->v(i,j)*(1-d)+y->v(i,j+1)*d);
			kk[pc] = mglPoint(i,j+d);	pc++;
		}
	}
	// deallocate arrays and finish if no point
	if(pc==0)	{	free(kk);	return;	}
	// allocate arrays for curve
	long *nn = new long[pc], *ff = new long[pc];
	for(i=0;i<pc;i++)	nn[i] = ff[i] = -1;
	// connect points to line
	long i11,i12,i21,i22,j11,j12,j21,j22;
	j=-1;	// current point
	do{
		if(j>=0)
		{
			kx = kk[j].x;	ky = kk[j].y;	i = -1;
			i11 = long(kx+1e-5);	i12 = long(kx-1e-5);
			j11 = long(ky+1e-5);	j12 = long(ky-1e-5);
			for(k=0;k<pc;k++)	// find closest point in grid
			{
				if(k==j || k==ff[j] || ff[k]!=-1)	continue;	// point is marked
				i21 = long(kk[k].x+1e-5);	i22 = long(kk[k].x-1e-5);
				j21 = long(kk[k].y+1e-5);	j22 = long(kk[k].y-1e-5);
				// check if in the same cell
				register bool cond = (i11==i21 || i11==i22 || i12==i21 || i12==i22) &&
						(j11==j21 || j11==j22 || j12==j21 || j12==j22);
				if(cond){	i=k;	break;	}
			}
			if(i<0)	j = -1;	// no free close points
			else			// mark the point
			{	nn[j] = i;	ff[i] = j;	j = nn[i]<0 ? i : -1;	}
		}
		if(j<0)
		{
			for(k=0;k<pc;k++)	if(nn[k]==-1)	// first check edges
			{
				if(kk[k].x==0 || fabs(kk[k].x-n+1)<1e-5 || kk[k].y==0 || fabs(kk[k].y-m+1)<1e-5)
				{	nn[k]=-2;	j = k;	break;	}
			}
			if(j<0)	for(k=0;k<pc;k++)	if(nn[k]==-1)	// or any points inside
			{	j = k;	nn[k]=-2;	break;	}
		}
	}while(j>=0);
	mgl_axial_plot(gr,pc,pp,nn,dir,c,wire);
	delete []kk;	delete []nn;	delete []ff;	delete []pp;
}
//-----------------------------------------------------------------------------
void mgl_axial_xy_val(HMGL gr, HCDT v, HCDT x, HCDT y, HCDT z, const char *sch, const char *opt)
{
	register long i,j,n=z->GetNx(),m=z->GetNy();
	if(x->GetNx()!=n)	{	gr->SetWarn(mglWarnDim,"Axial");	return;	}
	if(n<2 || m<2)		{	gr->SetWarn(mglWarnLow,"Axial");	return;	}
	bool both = x->GetNx()*x->GetNy()==m*n && y->GetNx()*y->GetNy()==m*n;
	if(y->GetNx()!=z->GetNy() && !both)	{	gr->SetWarn(mglWarnDim, "Axial");	return;	}
	gr->SaveState(opt);
	static int cgid=1;	gr->StartGroup("Axial",cgid++);

	long s=gr->AddTexture(sch);
	char dir='y';
	if(sch)
	{
		if(strchr(sch,'x'))	dir = 'x';
		if(strchr(sch,'z'))	dir = 'z';
	}

	mglData xx, yy;
	if(!both)	// make
	{
		xx.Create(z->GetNx(), z->GetNy());
		yy.Create(z->GetNx(), z->GetNy());
		for(j=0;j<m;j++)	for(i=0;i<n;i++)
		{	xx.a[i+n*j] = x->v(i);	yy.a[i+n*j] = y->v(j);	}
		x = &xx;	y = &yy;
	}
	// x, y -- have the same size z
	float v0;
	bool wire = sch && strchr(sch,'#');
	for(j=0;j<z->GetNz();j++)	for(i=0;i<v->GetNx();i++)
	{
		v0 = v->v(i);
		mgl_axial_gen(gr,v0,z,x,y,gr->GetC(s,v0),dir,j,wire);
	}
	gr->EndGroup();
}
//-----------------------------------------------------------------------------
void mgl_axial_val(HMGL gr, HCDT v, HCDT a, const char *sch, const char *opt)
{
	if(a->GetNx()<2 || a->GetNy()<2)	{	gr->SetWarn(mglWarnLow,"Axial");	return;	}
	gr->SaveState(opt);
	mglData x(a->GetNx(),a->GetNy()), y(a->GetNx(),a->GetNy());
	if(gr->Max.x*gr->Min.x>=0)	x.Fill(gr->Min.x,gr->Max.x,'x');
	else	x.Fill(0,gr->Max.x,'x');
	y.Fill(gr->Min.y,gr->Max.y,'y');
	mgl_axial_xy_val(gr,v,&x,&y,a,sch,0);
}
//-----------------------------------------------------------------------------
void mgl_axial_xy(HMGL gr, HCDT x, HCDT y, HCDT a, const char *sch, const char *opt)
{
	float r = gr->SaveState(opt);
	long Num = isnan(r)?3:long(r+0.5);
	if(Num<1)	{	gr->SetWarn(mglWarnCnt,"Axial");	return;	}
	mglData v(Num);
	for(long i=0;i<Num;i++)	v.a[i] = gr->Min.c + (gr->Max.c-gr->Min.c)*float(i+1)/(Num+1);
	mgl_axial_xy_val(gr,&v,x,y,a,sch,0);
}
//-----------------------------------------------------------------------------
void mgl_axial(HMGL gr, HCDT a, const char *sch, const char *opt)
{
	float r = gr->SaveState(opt);
	long Num = isnan(r)?3:long(r+0.5);
	if(Num<1)	{	gr->SetWarn(mglWarnCnt,"Axial");	return;	}
	mglData v(Num);
	for(long i=0;i<Num;i++)	v.a[i] = gr->Min.c + (gr->Max.c-gr->Min.c)*float(i+1)/(Num+1);
	mgl_axial_val(gr,&v,a,sch,0);
}
//-----------------------------------------------------------------------------
void mgl_axial_xy_val_(uintptr_t *gr, uintptr_t *v, uintptr_t *x, uintptr_t *y, uintptr_t *a, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_axial_xy_val(_GR_, _DA_(v), _DA_(x), _DA_(y), _DA_(a), s, o);
	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void mgl_axial_val_(uintptr_t *gr, uintptr_t *v, uintptr_t *a, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_axial_val(_GR_, _DA_(v), _DA_(a), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void mgl_axial_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *a, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_axial_xy(_GR_, _DA_(x), _DA_(y), _DA_(a), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void mgl_axial_(uintptr_t *gr, uintptr_t *a, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_axial(_GR_, _DA_(a), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
//
//		Torus series
//
//-----------------------------------------------------------------------------
void mgl_torus(HMGL gr, HCDT r, HCDT z, const char *sch, const char *opt)
{
	long i,j,n=r->GetNx();
	if(n*r->GetNy()!=z->GetNx()*z->GetNy())	{	gr->SetWarn(mglWarnDim,"Torus");	return;	}
	if(n<2)		{	gr->SetWarn(mglWarnLow,"Torus");	return;	}
	gr->SaveState(opt);
	static int cgid=1;	gr->StartGroup("Torus",cgid++);

	mglPoint *pp = new mglPoint[n];
	long *nn = new long[n];
	long ss=gr->AddTexture(sch);
	char dir='y';
	if(sch)
	{
		if(strchr(sch,'x'))	dir = 'x';
		if(strchr(sch,'z'))	dir = 'z';
	}

	float c = gr->GetC(ss,gr->Min.c);
	for(j=0;j<r->GetNy();j++)
	{
		for(i=0;i<n;i++)
		{
			nn[i] = i<n-1 ? i+1 : -1;
			pp[i] = mglPoint(r->v(i,j), z->v(i,j));
		}
		mgl_axial_plot(gr,n,pp,nn,dir,c,sch && strchr(sch,'#'));
	}
	gr->EndGroup();
	delete []nn;	delete []pp;
}
//-----------------------------------------------------------------------------
void mgl_torus_(uintptr_t *gr, uintptr_t *r, uintptr_t *z, const char *pen, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_torus(_GR_, _DA_(r), _DA_(z), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------