#include <graphics.h>
#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>

void main(void)
{
int	gd=CGA, gm=CGAHI, errorcode, size;		//CGA графика с высоким разрешением
void far	*image;
FILE *file;

initgraph (&gd,&gm,"e:\\bc\\bgi\\");
errorcode = graphresult();

if (errorcode != grOk)  /* an error occurred */
	{
	 printf("Graphics error: %s\n", grapherrormsg(errorcode));
	 printf("Press any key to halt:");
	 getchar();
	 exit(-1);             /* return with error code */
	}

//setbkcolor (1);										// выбираем цвета
setcolor (BLACK);
																			// рисуем картинку
bar(0,0,100,100);
line(0,0,10,0);
line(1,1,7,1);
line(2,2,8,2);
line(3,3,5,3);
line(9,3,10,3);
line(4,4,4,4);
line(11,4,12,4);
//line(10,5,11,5);

getchar();
size = imagesize(0,0,12,4);
																			 // записываем ее в файл .img
image = farcalloc (1,size);
getimage(0,0,12,4,image);

file = fopen ("scur07.img","wb");
fwrite (image,size,1,file);
fclose(file);
farfree(image);

closegraph();
}

