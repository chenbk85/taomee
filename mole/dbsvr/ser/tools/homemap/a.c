#define  Bin0000 0
#define  Bin0001 1
#define  Bin0010 2
#define  Bin0011 3
#define  Bin0100 4
#define  Bin0101 5
#define  Bin0110 6
#define  Bin0111 7
#define  Bin1000 8
#define  Bin1001 9
#define  Bin1010 A
#define  Bin1011 B
#define  Bin1100 C
#define  Bin1101 D
#define  Bin1110 E
#define  Bin1111 F
#define  HEX_MKI(x,y) 0x##x##y
//如果直接定义 #define  HEX_MK(x,y) 0x##x##y  ,是不行的。 而是需要下面的定义。
#define  HEX_MK(x,y) HEX_MKI(x,y)  
#define  BIN_MK_4(x1,x2,x3,x4)  Bin##x1##x2##x3##x4  
#define  BIN_2(x,y)  HEX_MK(Bin##x,Bin##y)
#define  BIN_8(x1,x2,x3,x4,y1,y2,y3,y4)  \
    HEX_MK( BIN_MK_4(x1,x2,x3,x4), BIN_MK_4(y1,y2,y3,y4)  )
int
main ( int argc, char *argv[] )
{
	unsigned char v;	
	v=BIN_8(1,0,1,1,1,1,0,1);
	v=BIN_2(1011, 0000);
	return 0;
}	
