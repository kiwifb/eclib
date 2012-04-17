// THTCONST.CC:  Test program for Silverman & CPS height bounds  
//////////////////////////////////////////////////////////////////////////
//
// Copyright 1990-2012 John Cremona
// 
// This file is part of the mwrank package.
// 
// mwrank is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2 of the License, or (at your
// option) any later version.
// 
// mwrank is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
// 
// You should have received a copy of the GNU General Public License
// along with mwrank; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
// 
//////////////////////////////////////////////////////////////////////////
//
#include <eclib/points.h>  // from qcurves library
#include <eclib/sieve_search.h>
#include <eclib/cperiods.h>
#include <eclib/elog.h>
#include <eclib/egr.h>
#include <eclib/htconst.h>


bigint a1,a2,a3,a4,a6;
Curve C;
Curvedata CD;
vector<Point> plist;

const double margin = 0.01;

int getcurve(void)
{
  //  cout << "Enter curve coefficients a1,a2,a3,a4,a6: " << endl;
  cin >> C;
  //  cout<<C<<endl;
  if(C.isnull()) return 0;  // quitting condition
  //  cin >> a1 >> a2 >> a3 >> a4 >> a6;
  //  C = Curve(a1,a2,a3,a4,a6);
  CD = Curvedata(C,0);     // "1" means minimise
  //  cout<<CD<<endl;
  return 1;
}

int main()
{
  set_precision(30);
  initprimes(string("PRIMES").c_str(),0);
  cin.flags( cin.flags() | ios::dec );  //force decimal input (bug fix)

  long nsilverbetter=0, ncpsbetter=0, n=0;
  double silvertotal=0, cpstotal=0, cps2total=0;
  long kkmintotal=0;
  while (getcurve())
    {
      cout<<endl;
      if(C!=(Curve)CD)
	{
	  cout<<"Input curve "<<C<<", ";
	  cout<<"Working with minimal curve "<<CD<<":\t";
	}
      else 
	cout << "Curve "<< C <<":\t";

      double silver = silverman_bound(CD);
      cout << "Silverman bound = " << silver << "\t";
      double cps = cps_bound(CD);
      cout << "CPS bound = " << cps << "\n";
      double egr_bound = egr_height_constant(CD);
      cout << "egr bound = " << egr_bound << "\n";
      bigint k;
      Curvedata CD2=opt_x_shift(CD,k);
      double cps2 = cps_bound(CD2);
      cout << "Shift by "<<k<<": "<< (Curve)CD2 <<":\t";
      cout << "CPS bound 2 = " << cps2 << "\n";
      bigint kmin=k; double cpsmin=cps2; long kkmin=0;

      Curvedata CDup=CD2;
      CDup.transform(BIGINT(1),BIGINT(0),BIGINT(0));
      double cpsup = cps_bound(CDup);
      cout << "Shift by "<<(k+1)<<" (up by 1): "<< (Curve)CDup <<":\t";
      cout << "CPS bound = " << cpsup << "\n";

      Curvedata CDdown=CD2;
      CDdown.transform(BIGINT(-1),BIGINT(0),BIGINT(0));
      double cpsdown = cps_bound(CDdown);
      cout << "Shift by "<<(k-1)<<" (down by 1): "<< (Curve)CDdown <<":\t";
      cout << "CPS bound = " << cpsdown << "\n";
      
      long kk=0, kd=0;
      if(cpsup<cps2-margin) 
	{cout<<"Up looks better\n";  kd=+1;}
      else
	if(cpsdown<cps2-margin) 
	  {cout<<"Down looks better\n";  kd=-1;}

      if(kd==0)
	{
	  kkmintotal+=kkmin;
	  cout<<"No significant improvement (margin of "<<margin<<")!\n"; 
	  cout << "Best shift by "<<k<<": ";
	  cout << "Best CPS bound = " << cps2 << "\n";
	}
      else
	{
	  double cpsk, lastcps=cps2;
	  for(kk=kd; ; kk+=kd)
	    {
	      Curvedata CDk=CD2;
	      CDk.transform(BIGINT(kk),BIGINT(0),BIGINT(0));
	      cpsk = cps_bound(CDk);
	      
	      cout << "Shift by "<<(k+kk)<<" (relative "<<kk<<"): "<< (Curve)CDk <<":\t";
	      cout << "CPS bound = " << cpsk << "\n";
	      if(lastcps<cpsk+margin)
		{
		  cout<<"not getting significantly better, stopping."<<endl;
		  break;
		}
	      lastcps=cpsk;
	      if(cpsk<cpsmin) {cpsmin=cpsk; kmin=k+kk; kkmin=kk;}
	    }
	  cps2=cpsmin; kkmintotal+=kkmin;
	  cout << "Best shift by "<<k+kkmin<<": ";
	  cout << "Best CPS bound = " << cps2 << "\n";
	}
      if (silver<cps) nsilverbetter++; else ncpsbetter++;
      silvertotal+=silver;
      cpstotal+=cps;
      cps2total+=cps2;
      n++;
      if(n%100==0) 
	cout<<"So far, CPS is better for "<<ncpsbetter
	    <<" curves out of "<<n<<endl;
//       double best = height_constant(CD);
//       cout << "Best = " << best << "\n";
      
    }
  cout<<"Overall, CPS is better for "<<ncpsbetter
      <<" curves out of "<<n<<endl;
  double rate = (100.0*ncpsbetter)/n;
  cout << "For " << rate << "% of curves the new bound is better."<<endl;
  cout << "Average difference (silverman-cps) = "<<(silvertotal-cpstotal)/n<<endl;
  cout << "Average difference (silverman-cps2) = "<<(silvertotal-cps2total)/n<<endl;
  cout << "Silverman average = "<<silvertotal/n<<endl;
  cout << "CPS 1 average = "<<cpstotal/n<<endl;
  cout << "CPS 2 average = "<<cps2total/n<<endl;
  cout << "Average offset = "<<(double)kkmintotal/n << endl;
}