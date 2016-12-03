#include "cv.h"
#include "highgui.h"
#include <iostream>
#include <fstream>
#include "my_func.h"

using namespace std;




int range=0;						//決定讀幾個bits的資料


int main()
{
	//讀入圖檔
  IplImage* pImg=cvLoadImage("left.jpg",1); 
    IplImage* pImg2=cvLoadImage("left.jpg",1);
	CvScalar s1,s2;
	
	
	int d=0,dprime=0;	//原差值與新差值
	int t;				//select_range傳回的值，即lower bound
	int count=0;		//嵌入資料的次數
	int temp;			//暫存最後一次的range值
	char c;				//暫存讀到的bit
	float m,a,b;		//m與新圖的前後pixel value
	bool flag=false;	//判斷是否為最後一次嵌入
	
	float pixel_value_per_block[4];
	float avg_pixel_value[2];
	

	//讀文件檔
	ifstream inf;
	inf.open("secret.txt");
	ofstream off("output.txt");
	

	//建立視窗
    cvNamedWindow("Image",1);
    cvShowImage("Image",pImg);
	
	//轉灰階
	Graylevelimage(pImg);
	Graylevelimage(pImg2);
	
	//開始嵌入資料
	for(int i=0;i<(pImg->height);i+=1)
	{
		for(int j=0;j<(pImg->width);j+=2)
		{
			s1=cvGet2D(pImg2,i,j);
			pixel_value_per_block[0]=s1.val[0];
			s1=cvGet2D(pImg2,i,j+1);
			pixel_value_per_block[1]=s1.val[0];
			//s1=cvGet2D(pImg2,i+1,j);
			//pixel_value_per_block[2]=s1.val[0];
			//s1=cvGet2D(pImg2,i+1,j+1);
			//pixel_value_per_block[3]=s1.val[0];
			avg_pixel_value[0]=pixel_value_per_block[0];
			
			//s2=cvGet2D(pImg2,i,j+2);
			//pixel_value_per_block[0]=s2.val[0];
			//s2=cvGet2D(pImg2,i,j+3);
			//pixel_value_per_block[1]=s2.val[0];
			//s2=cvGet2D(pImg2,i+1,j+2);
			//pixel_value_per_block[2]=s2.val[0];
			//s2=cvGet2D(pImg2,i+1,j+3);
			//pixel_value_per_block[3]=s2.val[0];
			avg_pixel_value[1]=pixel_value_per_block[1];

			printf("加密前:\n%f----%f\n",avg_pixel_value[0],avg_pixel_value[1]);

			d=abs((avg_pixel_value[0])-(avg_pixel_value[1]));
			t=select_range(d);

			for(int k=range-1;k>=0;k--)
			{
				inf.get(c);
				if(c=='1')
					dprime=dprime+root(k,2);
				else if(c=='2')
				{
					dprime/=root((k+1),2);
					printf("%dreg%d",dprime,k);
					temp=(range-1)-k;
					flag=true;
					break;
				}
			}
			
			dprime+=t;
			m=abs(dprime-d);
			
			if( (avg_pixel_value[0]) >= (avg_pixel_value[1]) )
			{
				if(dprime>d)
				{
					a=avg_pixel_value[0]+ceil(m/2);
					b=avg_pixel_value[1]-floor(m/2);
				}
				else
				{
					a=avg_pixel_value[0]-ceil(m/2);
					b=avg_pixel_value[1]+floor(m/2);
				}
			}
			else
			{
				if(dprime>d)
				{
					a=avg_pixel_value[0]-floor(m/2);
					b=avg_pixel_value[1]+ceil(m/2);
				}
				else
				{
					a=avg_pixel_value[0]+ceil(m/2);
					b=avg_pixel_value[1]-floor(m/2);
				}
			}
				
			cvSet2D(pImg2,i,j,cvScalar(a,a,a)); 
			cvSet2D(pImg2,i,j+1,cvScalar(b,b,b)); 

			count++;
			dprime=0;
			if(flag==true)
				goto L1;

		}
	}

	printf("\n");

//------------------------------------------------------------------------------------------
L1://解出資料
	for(int i=0;i<(pImg2->height);i+=1)
	{
		for(int j=0;j<(pImg2->width);j+=2)
		{
			if(count==0)
				goto L2;
			
			s1=cvGet2D(pImg2,i,j);
			s2=cvGet2D(pImg2,i,j+1);

			d=abs((s1.val[0])-(s2.val[0]));
			t=select_range(d);
			
			if(count==1)
			{	
				if(temp==0)
					break;
				else
					range=temp;
			}
			
			dprime=d-t;
			printf("value=%d\n",dprime);

			dprime=bin(dprime);			
			printf("binvalue=%d\n",dprime);

			off.width(range);
			off.fill('0');
			off<<dprime;
			count--;
		}
	}

//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
L2://算PSNR
    long dist, tmp;
    double mse, psnr; 
    dist=0;
    for(int i=0;i<(pImg2->height);i++)
	{
		for(int j=0;j<(pImg2->width);j++)
		{
			s1=cvGet2D(pImg,i,j);
			s2=cvGet2D(pImg2,i,j);

            tmp=(s1.val[0])-(s2.val[0]);
            dist+=tmp*tmp;
		}
	}
    mse=(double)dist/((pImg2->width)*(pImg2->height));
    psnr=log10(255*255/mse)*10;
    printf("PSNR=%8.3f\n",psnr);

//------------------------------------------------------------------------------------------

	cvSaveImage("image_p.pgm", pImg2);
    cvNamedWindow( "image_p", 1 );
    cvShowImage( "image_p", pImg2 );
    cvWaitKey(0);
 
    cvDestroyWindow( "Image" );
	cvDestroyWindow( "image_p" );
    cvReleaseImage( &pImg ); 
	cvReleaseImage( &pImg2 );

	inf.close();
	off.close();
	system("PAUSE");
    
	return 0;
}



int root(int k,int num)
{
	int temp=1;
	for(int a=0;a<k;a++)
		temp=temp*num;
	return temp;
}




int select_range(int i)
{
	if( (i>=0)&&(i<8) )
	{
		range=3;
		return 0;
	}
	else if( (i>=8)&&(i<16) )
	{
		range=3;
		return 8;
	}
	else if( (i>=16)&&(i<32) )
	{
		range=4;
		return 16;
	}
	else if( (i>=32)&&(i<64) )
	{
		range=5;
		return 32;
	}
	else if( (i>=64)&&(i<128) )
	{
		range=6;
		return 64;
	}
	else if( (i>=128)&&(i<256) )
	{
		range=7;
		return 128;
	}
}




int bin(int n)
{
	int count=0;
	int val=0;
	if(n==1)
		return 1;
	else if(n==0)
		return 0;
	else
	{
		while(n!=1)
		{
			val+=(n%2)*root(count,10);
			n=n/2;
			count++;
		}
		val+=root(count,10);
		return val;
	}
}



void Graylevelimage(IplImage* image)
{
	int x,y,r,g,b,h,w,gray;
	h=image->height;
	w=image->width;
	for(x=0;x<h;x++)
	{
		for(y=0;y<w;y++)
		{
			r=((uchar*)(image->imageData+image->widthStep*x))[y*3+2];
			g=((uchar*)(image->imageData+image->widthStep*x))[y*3+1];
			b=((uchar*)(image->imageData+image->widthStep*x))[y*3];
			gray=r*0.299+g*0.587+b*0.114;
			(image->imageData+image->widthStep*x)[y*3+2]=gray;
			(image->imageData+image->widthStep*x)[y*3+1]=gray;
			(image->imageData+image->widthStep*x)[y*3]=gray;
		}
	}
}
