# include <stdio.h>
# include <string.h>
# include <mem.h>
# include <stdlib.h>
# include <dos.h>

int prot();

void main()                      // ������� ����஢���� 䠩� � ������
	{                              // ���� ��� ����� �����. ��-� �� ��.��設�
	 int		j,r1,r2;
	 unsigned	mem[512],i,tmp;
	 FILE 		*fprot;

	 fprot=fopen("anlzr.lp","wb");
	 for(j=5;j>=1;j--)	// ���뢠�� �� ���� ���� (8�), �.�. ����祭�, ��
		{                 // �� ࠡ�� �� ��� ������� �������� ���祭�� �
											//��᫥���� �������� fe00:03f1 - fe00:03f4

			srand(j*4729); 		/* ���樨஢���� ���.�ᥫ � �⥭�� �� BIOS */
		 for(i=0;i<512;i++)	mem[i]=peek(0xfe00,j*1024+i*2);
		 for(i=0;i<512;i++)	/* ࠭�������� ���ᨢ� */
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


int prot()                             // �஢���� ��設� �� ᮮ⢥��⢨�
	{                                    // ����ᠭ��� � 䠩�� ���ଠ樨
	 int		j,r1,r2;
	 unsigned	mem[512],fl[512],i,tmp;
	 FILE 		*fprot;

	 fprot=fopen("anlzr.lp","rb");
	 if(fprot==NULL)
		{puts("\n������ ������ 䠩�");
			return(-1);
			}
	 for(j=5;j>=1;j--)
		{
			srand(j*4729); 		/* ���樨஢���� ���.�ᥫ � �⥭�� �� BIOS */
		 for(i=0;i<512;i++)	mem[i]=peek(0xfe00,j*1024+i*2);
		for(i=0;i<512;i++)	/* ࠭�������� ���ᨢ� */
				{
				 r1=rand()/64;	r2=rand()/64;
         tmp=mem[r1];	mem[r1]=mem[r2];	mem[r2]=tmp;
				 }
		if(fread(fl,sizeof(fl),1,fprot)!=1)	/* �⥭�� �� 䠩�� */
				{puts("\n���� �� �⠥���");
				 fclose(fprot);
				 return(-1);
				 }
			if(memcmp(mem,fl,sizeof(mem))!=0)	/* �ࠢ����� */
				{
				 fclose(fprot);
				 return(-1);       /* �� ࠧ����� ������ -1, �ᯥ� 0 */
				 }
			}
	 fclose(fprot);
	 return(0);
	 }
