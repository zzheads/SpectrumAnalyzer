# include <stdio.h>
# include <string.h>
# include <mem.h>
# include <stdlib.h>
# include <dos.h>

int prot();

void main()                      // Создает защифрованный файл с дампом
	{                              // БИОС для защиты копир. пр-м на др.машины
	 int		j,r1,r2;
	 unsigned	mem[512],i,tmp;
	 FILE 		*fprot;

	 fprot=fopen("anlzr.lp","wb");
	 for(j=5;j>=1;j--)	// считываем не весь биос (8к), т.к. замечено, что
		{                 // при работе из под Виндоус меняются значения в
											//последнем килобайте fe00:03f1 - fe00:03f4

			srand(j*4729); 		/* инициирование случ.чисел и чтение из BIOS */
		 for(i=0;i<512;i++)	mem[i]=peek(0xfe00,j*1024+i*2);
		 for(i=0;i<512;i++)	/* рандомизация массива */
				{
				 r1=rand()/64;	r2=rand()/64;
				 tmp=mem[r1];	mem[r1]=mem[r2];	mem[r2]=tmp;
				 }
			fwrite(mem,sizeof(mem),1,fprot);
			}
	 fclose(fprot);
	 if(prot()!=0)	puts("\nInstallation breakdone !!\n");
		else 	puts("\nOK Installation\n");
	 }


int prot()                             // Проверяет машину на соответствие
	{                                    // записанной в файле информации
	 int		j,r1,r2;
	 unsigned	mem[512],fl[512],i,tmp;
	 FILE 		*fprot;

	 fprot=fopen("anlzr.lp","rb");
	 if(fprot==NULL)
		{puts("\nНемогу открыть файл");
			return(-1);
			}
	 for(j=5;j>=1;j--)
		{
			srand(j*4729); 		/* инициирование случ.чисел и чтение из BIOS */
		 for(i=0;i<512;i++)	mem[i]=peek(0xfe00,j*1024+i*2);
		for(i=0;i<512;i++)	/* рандомизация массива */
				{
				 r1=rand()/64;	r2=rand()/64;
         tmp=mem[r1];	mem[r1]=mem[r2];	mem[r2]=tmp;
				 }
		if(fread(fl,sizeof(fl),1,fprot)!=1)	/* чтение из файла */
				{puts("\nФайл не читается");
				 fclose(fprot);
				 return(-1);
				 }
			if(memcmp(mem,fl,sizeof(mem))!=0)	/* сравнение */
				{
				 fclose(fprot);
				 return(-1);       /* при различиях возврат -1, успех 0 */
				 }
			}
	 fclose(fprot);
	 return(0);
	 }
