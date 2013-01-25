/* Copyright 1999-2000 (C) John Moyer */
/* assume 32 bit ints */
/* mailto:jrm@rsok.com */
/* http://www.rsok.com/~jrm/ */
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdint.h>	/* C99 */
#include <string.h>
#include <math.h>

#define STEP_CONST 128

int32_t small_primes[343] = {
	2,3,5,7,11,13,17,19,23,29,
	31,37,41,43,47,53,59,61,67,71,
	73,79,83,89,97,101,103,107,109,113,
	127,131,137,139,149,151,157,163,167,173,
	179,181,191,193,197,199,211,223,227,229,
	233,239,241,251,257,263,269,271,277,281,
	283,293,307,311,313,317,331,337,347,349,
	353,359,367,373,379,383,389,397,401,409,
	419,421,431,433,439,443,449,457,461,463,
	467,479,487,491,499,503,509,521,523,541,
	547,557,563,569,571,577,587,593,599,601,
	607,613,617,619,631,641,643,647,653,659,
	661,673,677,683,691,701,709,719,727,733,
	739,743,751,757,761,769,773,787,797,809,
	811,821,823,827,829,839,853,857,859,863,
	877,881,883,887,907,911,919,929,937,941,
	947,953,967,971,977,983,991,997,1009,1013,
	1019,1021,1031,1033,1039,1049,1051,1061,1063,1069,
	1087,1091,1093,1097,1103,1109,1117,1123,1129,1151,
	1153,1163,1171,1181,1187,1193,1201,1213,1217,1223,
	1229,1231,1237,1249,1259,1277,1279,1283,1289,1291,
	1297,1301,1303,1307,1319,1321,1327,1361,1367,1373,
	1381,1399,1409,1423,1427,1429,1433,1439,1447,1451,
	1453,1459,1471,1481,1483,1487,1489,1493,1499,1511,
	1523,1531,1543,1549,1553,1559,1567,1571,1579,1583,
	1597,1601,1607,1609,1613,1619,1621,1627,1637,1657,
	1663,1667,1669,1693,1697,1699,1709,1721,1723,1733,
	1741,1747,1753,1759,1777,1783,1787,1789,1801,1811,
	1823,1831,1847,1861,1867,1871,1873,1877,1879,1889,
	1901,1907,1913,1931,1933,1949,1951,1973,1979,1987,
	1993,1997,1999,2003,2011,2017,2027,2029,2039,2053,
	2063,2069,2081,2083,2087,2089,2099,2111,2113,2129,
	2131,2137,2141,2143,2153,2161,2179,2203,2207,2213,
	2221,2237,2239,2243,2251,2267,2269,2273,2281,2287,
	2293,2297,2309
};

/* expand scheme from 30 to 2310
   30 == 2*3*5 and 8 == (2-1)*(3-1)*(5-1)
   2*3*5*7*11 == 2310
   1*2*4*6*10 ==  480 bits == 60 bytes ==> prime or not prime for 
   38.5 integers per byte

   In each 2310 integers, for N >= 1, the numbers that might be prime are:
   N*2310+1,
   N*2310+13,
   N*2310+17,
   .
   .
   .
   N*2310+13*13,
   N*2310+13*17,
   .
   .
   .
   N*2310+2309
   */


uint32_t maskbits[32] = 
{
	0x00000001, 0x00000002, 0x00000004, 0x00000008, 
	0x00000010, 0x00000020, 0x00000040, 0x00000080,
	0x00000100, 0x00000200, 0x00000400, 0x00000800, 
	0x00001000, 0x00002000, 0x00004000, 0x00008000,
	0x00010000, 0x00020000, 0x00040000, 0x00080000, 
	0x00100000, 0x00200000, 0x00400000, 0x00800000,
	0x01000000, 0x02000000, 0x04000000, 0x08000000, 
	0x10000000, 0x20000000, 0x40000000, 0x80000000
};

/*
   value of mask_bit_index is bit corresponding to integer modulo 2310
   mask_bit_index[i] is zero if this is integer that could not possibly
   be prime or bit number from 1 to 480 if it could be prime.
   */
uint32_t mask_bit_index[2310];

/* bit is one to 480 */
void clear_one_mask_bit(uint32_t *sieve_array, uint32_t bit)
{
	int32_t k;

	k = (bit - 1) >> 5 ;				/* divide by 32 */
	sieve_array[k] &= ~(maskbits[(bit - 1) & 31]);	/* modulo 32 */
#ifdef DEBUG
	printf("sieve_array[%"PRIuMAX"]=0x%08x, ~(maskbits[(%d - 1) & 31]=0x%08x\n",
			k, sieve_array[k], bit, ~(maskbits[(bit - 1) & 31]));
#endif
}

/* bit is one to 480 */
int test_one_mask_bit(uint32_t *sieve_array, uint32_t bit)
{
	int32_t k;

	k = (bit - 1) >> 5 ;				/* divide by 32 */
	return (sieve_array[k] & (maskbits[(bit - 1) & 31]));	/* modulo 32 */
}


extern char *optarg;
extern int optind, opterr, optopt;
int getopt (int argc, char *const argv[], const char *opts);


int main(int argc, char *argv[])
{
	uint32_t b;
	uint64_t s;
	uint64_t k, j, ii;
	uint64_t i;
	uint32_t bit_index = 0;
	uint64_t indx;
	uint64_t start, stop;
	int32_t c;
	uint32_t *list;
	FILE *fp;
	uint64_t stop_here, t_stop_here;
	char ifnam[2048];
	int errflag = 0;
	int write_flag = 0;

	strcpy(ifnam, "primes.dat");

	while ((c = getopt(argc, argv, "s:e:f:n")) != EOF)
	{
		switch(c)
		{
			case 's':
				start = strtoull(optarg,NULL,0);
				break;
			case 'e':
				stop = strtoull(optarg,NULL,0);
				s = (strtoull(optarg,NULL, 0) +2309)/2310 * 60 +60; /* bytes required */
				break;
			case 'f':
				strncpy(ifnam, optarg, sizeof(ifnam) -1);
				ifnam[sizeof(ifnam) -1] = '\0';
				write_flag = 1;
				break;
			case 'n':
				write_flag = 0;
				break;
			default:
				errflag++;
				break;
		}
	}

	if ( (errflag != 0) || ( argc > optind) || (argc < 5) || (stop < start))
	{
		fprintf(stderr,"Usage: %s -s start -e end [-f file]\n", argv[0]);
		fprintf(stderr,"Start and end must be positive integers\n");
		fprintf(stderr,
				"If a file name is given, primes will be saved to file as 480bits/2310integers.\n");
		if (stop < start)
			fprintf(stderr,"end must be greater than start\n");
		return -1;
	}


	/* print the first few small primes if they were requested */
	for ( i = 0 ; i < 343 ; i ++)
	{
		if ( small_primes[i] > stop )
			return 0;	/* return to OS if nothing more to do */
		if ( small_primes[i] >= start )
			fprintf(stdout,"%12d\n",small_primes[i]);
	}



	fprintf(stderr,"attempting to malloc %"PRIuMAX" bytes\n", s);
	list = malloc(s);
	if ( list == NULL )
	{
		fprintf(stderr, "Could not allocate %"PRIuMAX" bytes of memory\n", s);
		exit(1);
	}

	memset(list,0xff,s);


	j = 5;

	/* create array mapping 2310 integers to 480 bits */
	mask_bit_index[0] = bit_index++;
	mask_bit_index[1] = bit_index++;

	for ( ii = 2; ii < 2310 ; ii++ )
	{
		while (small_primes[j] < ii )
			j++;
		/* if modulo any of the prime factors of 2310, then could not be prime */
		if ( ii == small_primes[j] || 
				((ii%2)!=0 && (ii%3)!=0 && (ii%5)!=0 && (ii%7)!=0 && (ii%11)!=0))
		{
			mask_bit_index[ii] = bit_index++;
		}
		else
			mask_bit_index[ii] = 0;
#ifdef DEBUG
		printf("mask_bit_index[%"PRIuMAX"]=%d, j=%"PRIuMAX"\n", ii, mask_bit_index[ii], j);
#endif
	}


	indx = 0L;
	/* only 53 bits here, not 64 */
	stop_here = (uint64_t) (sqrt((stop+2309.0)/2310.0 *2310.0) + 0.5);

	for ( k = 0 ; k*2310 < stop ; k+=STEP_CONST )
	{
		/* no need to sieve numbers larger than this range */
		t_stop_here = (k+STEP_CONST)*2310ULL;
		if ( stop_here < t_stop_here )
			t_stop_here = stop_here;
		for( i = 13 ; i <= t_stop_here ; i+=2 )
		{
			/* start with 13 since multiples of 2,3,5,7,11 are handled by the storage method */
			/* increment by 2 for odd numbers only */

			/*    if ( (i >= ((k+STEP_CONST)* 2310)))
				  break;	/* no need to sieve numbers larger than this range, do next k */

			b = i % 2310;

			/* i could not possibly be prime if remainder is 2,3,4,7,11 */
			if ( mask_bit_index[b] == 0
					|| (i < k*2310
						&& (test_one_mask_bit(&list[i/2310 *15], mask_bit_index[b]))==0 )
			   )
				continue;	/* or this one already marked so it is not a prime */
			/* */
			if ( k == 0 )
				indx = i*i;
			else
				indx = (k*2310) - (k*2310)%i +i;
			if ( (indx & 1) == 0 )
				indx += i;
			/* start with i*i since any integer < i has already been sieved */
			/* add 2 * i to avoid even numbers and mark all multiples of this prime */
			for ( ; indx < (k+STEP_CONST)*2310 && indx <= (stop+2309)/2310*2310
					; indx +=(i+i))
			{
				b = indx % 2310;		/* modulo 2310 */
				if ( mask_bit_index[b] != 0 )
				{
					clear_one_mask_bit(&list[indx/2310 *15],mask_bit_index[b]);
#ifdef DEBUG
					printf("indx = %lu, mask_bit_index[%d]=%d\n", indx, b, mask_bit_index[b]);
#endif
				}
			} /* for indx */
		} /* for i */

		if ( start < (k+STEP_CONST)*2310 ) /* are there some to print now? */
		{
			if ( k*2310 < start )
				i = start;
			else
				i = k*2310;
			if ( (i & 1) == 0 )
				i++;	/* force it to be odd */
			if ( 2311 >= i )
				i = 2311;
			for ( ; i <= stop && i < (k+STEP_CONST)*2310; i+=2 )
			{
				b = i % 2310;
				if ( mask_bit_index[b] != 0 && i >= start &&
						(test_one_mask_bit(&list[i/2310 *15], mask_bit_index[b]))!=0 )
					/*        fprintf(stdout,"%Lu\n",i); */
					fprintf(stdout,"%"PRIuMAX"\n",i);
			} /* for i */
		}
	} /* for k */

	if ( write_flag )
	{
		if ( (fp = fopen(ifnam,"wb")) == NULL )
		{
			perror(ifnam);
			return 1;
		}
		i = fwrite( list, 1L, s, fp );
		if ( i != s )
			/*    fprintf(stderr,"write error: i=%Lu, s=%lu\n",i,s); */
			fprintf(stderr,"write error: i=%"PRIuMAX", s=%"PRIuMAX"\n",i,s);
	}

	return 0;
}
