//这是鼠标控制的版本
#include <stdlib.h>
#include <graphics.h>
#include <stdio.h>
#include <conio.h>
#include"logical.h"
//棋子操作部分n
//最多翻转多少个子

//判定结束
int IsFinish(int**game,int n,int x,int y,int colour)//判断是否结束
{
	if(x==-1)return 1;
	else return 0;
}
//翻转操作
void turn(int**game,int n,int x,int y,int colour)//翻转棋子
{
	int runx,runy,flag;
	game[x][y]=colour;
	for(flag=1,runx=x-1;runx>=0;runx--)//上
	{
		if(game[runx][y]==0){flag=0;break;}
		if(game[runx][y]==colour)break;
	}
	if(runx!=-1&&flag==1)for(;runx<x;runx++)if(game[runx][y]!=0)game[runx][y]=colour;
	for(flag=1,runx=x+1;runx<n;runx++)//下
	{
		if(game[runx][y]==0){flag=0;break;}
		if(game[runx][y]==colour)break;
	}
	if(runx!=n&&flag==1)for(;runx>x;runx--)if(game[runx][y]!=0)game[runx][y]=colour;
	for(flag=1,runy=y-1;runy>=0;runy--)//左
	{
		if(game[x][runy]==0){flag=0;break;}
		if(game[x][runy]==colour)break;
	}
	if(runy!=-1&&flag==1)for(;runy<y;runy++)if(game[x][runy]!=0)game[x][runy]=colour;
	for(flag=1,runy=y+1;runy<n;runy++)//右
	{
		if(game[x][runy]==0){flag=0;break;}
		if(game[x][runy]==colour)break;
	}
	if(runy!=n&&flag==1)for(;runy>y;runy--)if(game[x][runy]!=0)game[x][runy]=colour;
	for(flag=1,runx=x-1,runy=y-1;runx>=0&&runy>=0;runx--,runy--)//左上
	{
		if(game[runx][runy]==0){flag=0;break;}
		if(game[runx][runy]==colour)break;
	}
	if(runx!=-1&&runy!=-1&&flag==1)for(;runx<x;runx++,runy++)if(game[runx][runy]!=0)game[runx][runy]=colour;
	for(flag=1,runx=x+1,runy=y+1;runx<n&&runy<n;runx++,runy++)//右下
	{
		if(game[runx][runy]==0){flag=0;break;}
		if(game[runx][runy]==colour)break;
	}
	if(runx!=n&&runy!=n&&flag==1)for(;runx>x;runx--,runy--)if(game[runx][runy]!=0)game[runx][runy]=colour;
	for(flag=1,runx=x-1,runy=y+1;runx>=0&&runy<n;runx--,runy++)//右上
	{
		if(game[runx][runy]==0){flag=0;break;}
		if(game[runx][runy]==colour)break;
	}
	if(runx!=-1&&runy!=n&&flag==1)for(;runx<x;runx++,runy--)if(game[runx][runy]!=0)game[runx][runy]=colour;
	for(flag=1,runx=x+1,runy=y-1;runx<n&&runy>=0;runx++,runy--)//左下
	{
		if(game[runx][runy]==0){flag=0;break;}
		if(game[runx][runy]==colour)break;
	}
	if(runx!=n&&runy!=-1&&flag==1)for(;runx>x;runx--,runy++)if(game[runx][runy]!=0)game[runx][runy]=colour;
}
//初始化
int**GameInit(int n)//初始化
{
	int**p;
	p=(int**)malloc(n*sizeof(int*));
	for(int i=0;i<n;i++)
	{
		p[i]=(int*)malloc(n*sizeof(int));
		for(int j=0;j<n;j++)
		{
			p[i][j]=0;
		}

	}
	p[(n+1)/2][(n+1)/2]=1;
	p[(n-1)/2][(n-1)/2]=1;
	p[(n+1)/2][(n-1)/2]=-1;
	p[(n-1)/2][(n+1)/2]=-1;
	return p;
}
//求局部最优解
void Find(int**game, int n, int*x_,int*y_,int step,int colour)//局部最优解
{
	int runx,runy,max=0;
	for(runx=0;runx<n;runx++)
	{
		for(runy=0;runy<n;runy++)
		{
			if(game[runx][runy]!=0)continue;
			else if(max<HowMany(game,n,runx,runy,colour))
			{
				max=HowMany(game,n,runx,runy,colour);
				*x_=runx;*y_=runy;
			}
		}
	  }
	if(max==0)*x_=-1;
}
//计算赢家
int count(int**game, int n)
{
	int count=0,flag;
	for(int i=0;i<n;i++){
		for(int j=0;j<n;j++){
			if(game[i][j]==1)count++;
		}
	}
	if(count==n*n)return 0;
	else if(count>n*n)return 1;
	else return -1;
}


//绘图部分
// 包含黑白棋绘图所需函数

//绘制图片框架
void DrawFrame(void)
{
	initgraph(960, 720);
	setbkcolor(0xf0ffff);
	setbkmode(TRANSPARENT);
	cleardevice();

	setlinecolor(BLACK);
	rectangle(160,20,800,700);
	setfillcolor(0xed9564);
	solidrectangle(161,21,799,59);
	settextcolor(BLACK);
	settextstyle(32, 0,_T("Verdana"));
	outtextxy(165,23,_T("player 1"));
	outtextxy(690,23,_T("player 2"));
    setfillcolor(BLACK);
	solidcircle(290,40,14);
	setfillcolor(WHITE);
	solidcircle(665,40,14);

	line(161,60,799,60);
	setfillcolor(0x32cd32);
	solidrectangle(161,61,799,699);
	for(int i=1;i<8;i++)
	{
		line(161,60+80*i,799,60+80*i);
		line(160+80*i,60,160+80*i,699);
	}
}
//绘制回合指示
void DrawTurn(int player)
{
	if(player==1)
	{
		setfillcolor(RED);
		POINT pts1[]={{330,40},{360,30},{360,50}};
		solidpolygon(pts1,3);
		setfillcolor(0xed9564);
		POINT pts2[]={{610,40},{580,30},{580,50}};
		solidpolygon(pts2,3);
	}
	if(player==-1)
	{
		setfillcolor(RED);
		POINT pts1[]={{610,40},{580,30},{580,50}};
		solidpolygon(pts1,3);
		setfillcolor(0xed9564);
		POINT pts2[]={{330,40},{360,30},{360,50}};
		solidpolygon(pts2,3);
	}
}
//绘制棋子
void DrawCircle(int x,int y, int player)
{
	if(player==1)setfillcolor(BLACK);
	if(player==-1)setfillcolor(WHITE);
	solidcircle(120+80*x,20+80*y,32);
}
//淡化（写的有点蠢）
void Darken(DWORD* pMem)
{
for(int i = 0; i < 960*720; i++)
{
pMem[i] = RGB(
(GetRValue(pMem[i])+510)/3 ,
(GetGValue(pMem[i])+510)/3 ,
(GetBValue(pMem[i])+510)/3 );
}
}
//绘制结果
void DrawResult(int winner)
{
	Darken(GetImageBuffer());
	FlushBatchDraw();
	settextstyle(80,0,_T("Vernana"));
	if(winner==1)outtextxy(225,350,_T("player 1 win!"));
	if(winner==-1)outtextxy(180,350,_T("player 2 win!"));
}
//将所有内容更新
void DrawAll(int**game,int n){
	int i,j;
	for(i=0;i<n;i++){
		for(j=0;j<n;j++){
			if(game[i][j]==0);
			else DrawCircle(j+1,i+1,game[i][j]);
		}
	}
}
//落子无效
void DrawError(void){
	settextcolor(RED);
	settextstyle(30,0,_T("Vernana"));
	outtextxy(400,30,_T("Move in Vain"));
}

//鼠标模式部分
void GetClickPoint(int*x_, int*y_){
	//拿到鼠标单击坐标
	int x,y,flag=0;
	MOUSEMSG mouse;
	while(1){
		while(1){
			mouse=GetMouseMsg();
			if(	mouse.uMsg==WM_LBUTTONUP)break;
		}
		x=mouse.x;y=mouse.y;
		if(x>160&&x<800&&y>60&&y<700){
			*y_=((int)x-160)/((800-160)/8)+1;
			*x_=((int)y-60)/((700-60)/8)+1;
			break;
	}
	}
	}
	//计算属于哪个格点
//主函数
void main(void)
{
	int**game;
	int n=8,x=0,y=0,colour=1,flag,step=0;
	game=GameInit(n);
	DrawFrame();
	DrawAll(game,n);
	while(1){
		//eve
		/*Find(game,n,&x,&y,0,colour);
		if(x==-1)break;
		turn(game,n,x,y,colour);
		print(game,n);
		colour=-colour;*/
		//pve
		/*Find(game,n,&x,&y,0,1);
		if(x==-1){printf("你输了\n");break;}
		printf("请输入落子点\n");//人机模式
		scanf("%d %d",&y,&x);
		turn(game,n,x-1,y-1,1);
		print(game,n);
		Find(game,n,&x,&y,0,-1);
		if(x==-1){printf("你赢了\n");break;}
		turn(game,n,x,y,-1);
		print(game,n);*/

		//pve
		if(step==n*n){
			flag=count(game,n);
			DrawResult(flag);getchar();break;
		}//若下满则清算
		else{
			Find(game,n,&x,&y,step,colour);
			if(x==-1){DrawResult(-colour);getchar();break;}//无字可落，出结果
			else{
				DrawTurn(colour);
				while(1){
					//scanf_s("%d %d",&y,&x);//直接输入法
					GetClickPoint(&x,&y);
					if(HowMany(game,n,x-1,y-1,colour)==0||game[x-1][y-1]!=0)
						DrawError();
					else {solidrectangle(400,21,580,59);break;}
				}//错误落子判定
				turn(game,n,x-1,y-1,colour);
				DrawAll(game,n);
				step++;
				colour=-colour;
			}
		}
	}
	getchar();
}