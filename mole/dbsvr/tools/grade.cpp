
int grade[]={0,15,45,90,150,225,315,420,540,675,825,990,
	1170,1365,1575,1800,2040,2295,2565,2850,3150,3465,
	3795,4140,4500,4875,5265,5670,6090,6525,6975,7440,
	7920,8415,8925,9450,9990,10545,11115,11700,12300,
	12915,13545,14190,14850,15525,16215,16920,17640,
	18375,19125,19890,20670,21465,22275,23100,23940,24795,
	25665,26550,27450,28365,29295,30240,31200,32175,33165,
	34170,35190,36225,37275,38340,39420,40515,41625,42750,
	43890,45045,46215,47400,48600,49815,51045,52290,53550,
	54825,56115,57420,58740,60075,61425,62790,64170,
	65565,66975,68400,69840,71295,72765,74250,75750,1000000000 };
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
using namespace std;
int get_grade(int exp){
	pair< int*, int* > range;	
	int* it;	
	range=equal_range(grade,grade+102, exp );
	it=range.first;
	if (it<grade+102){
		if (*it== exp ){
			return  it-grade ;
		}else{
			return  it-grade-1 ;
		}
	}else{
	 	return	101;
	}
}
int
main ( int argc, char *argv[] )
{

		int exp;
		FILE *fp_in=fopen("f.in","r");
		FILE *fp_out=fopen("f.out","w");
		while ( !feof( fp_in ))  {
			fscanf(fp_in,"%d\n", &exp);
			fprintf(fp_out,"%d\n", get_grade(exp));
		}
		fclose(fp_in);
		fclose(fp_out);
		return 0;
}				/* ----------  end of function main  ---------- */
