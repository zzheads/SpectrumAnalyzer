#include <stdio.h>            /*программа вывода в файл нескольких       */
#include <stdlib.h>						/* файлов по маске (можно задать несколько */
#include <process.h>					/* масок. Перед файлом вставляется его имя */
#include <dos.h>							/* подчеркивается 79 символами = и затем */
#include <dir.h>							/* пустая строка. Длина строки входного файла*/
#include <string.h>						/* не более 255. В выходном файле строки */
																			/* длиннее 79 символов переносятся */
void main (int argc, char **argv)     /* причем остаток перенесенной строки */
{                                     /* выравнивается по правому краю */
FILE *soursefile, *destfile;          /* табуляция заменяется 2 пробелами */
char	userchar,buf[512],buftmp[82];   /* в конце файла вставляется перевод */
struct ffblk ffblk;                   /* формата */
int	done,i,j,n,strlengs;


strcpy (buftmp+79,"\n");

if (argc < 3)
	{puts("\ninput arguments in command string:");
	puts("\n FILEMERGE sourse_filenames_mask...    destination_filename");
	puts("\nAll text sourse_filenames_mask (one, or more mask) files add to");
	puts("destination_filename. Before any file inset strings with, filename,");
	puts("underburs and new line. After any file insert new page code (012).");
	puts("All string, longer then 79 charakters, print on any lines, and");
	puts("end of string margin to right side.\n");
	return;
	}
destfile=fopen(argv[argc-1],"r");
if(destfile!=NULL)
	{fclose(destfile);
	printf("\n\nFile %s is exist, overwrite?\n",argv[argc-1]);
	userchar = getchar();
	if(userchar != 'Y' && userchar != 'y')	abort();
	}
destfile = fopen(argv[argc-1],"wt");
if(destfile == NULL)
	{puts("\nOpening file is problem\n");
	abort();
	}
printf("\nOutput file is: %s\n\n",argv[argc-1]);
for(i=1;i<argc-1;i++)
	{done = findfirst(argv[i],&ffblk,0);
	while(!done)
		{soursefile = fopen(ffblk.ff_name,"rt");
		if(soursefile == NULL)
			{printf("\nProblem in opening file %s\n",ffblk.ff_name);
			abort();
			}
		fputs(ffblk.ff_name,destfile);
		fputs("\n===============================================================================\n\n",destfile);
		while(fgets(buf,255,soursefile) != NULL)	/* заменяем табуляцию пробелом */
			{for(n=0;buf[n]!=0;n++)   if(buf[n]=='\t')	buf[n]=' ';
			strlengs = strlen(buf);
			if(strlengs>79)
				{for (j=0; j<(strlengs-79); j+=79)
					{strncpy (buftmp,buf+j,79);
					fputs (buftmp,destfile);
					}
				for(; j>(strlengs-79); j--)	buf[j-1]=' ';
				fputs(buf+j+1,destfile);
				}
			else	fputs(buf,destfile);
			}
		fputs("\n\f\n",destfile);
		fclose(soursefile);
		printf("\nAdd %s",ffblk.ff_name);
		done = findnext(&ffblk);
		}
	}
	fclose(destfile);
}