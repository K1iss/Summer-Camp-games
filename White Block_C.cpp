///////////////////////////////////////////////////
// 程序名称：别踩白块儿（双人版） 
// 编译环境：Visual Studio、2013，EasyX 20140321(beta)
//
#undef UNICODE
#undef _UNICODE
#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include <time.h>

#define MAXTASK 5         // 定义游戏需要完成的黑块数量

// 定义宏 __sprintf 自适应 vc6 与 vc2013
#if _MSC_VER > 1200
	#define __sprintf(...) sprintf_s(__VA_ARGS__)
#else
	#define __sprintf sprintf
#endif


// 精确延时函数(可以精确到 1ms，精度 ±1ms)
void HpSleep(int ms)
{
	static clock_t oldclock = clock();		// 静态变量，记录上一次 tick

	oldclock += ms * CLOCKS_PER_SEC / 1000;	// 更新 tick

	if (clock() > oldclock)					// 如果已经超时，无需延时
		oldclock = clock();
	else
		while (clock() < oldclock)			// 延时
			Sleep(1);						// 释放 CPU 控制权，降低 CPU 占用率
}


// 游戏状态常量
enum STATUS{BEGIN,			// 游戏开始
			RUNNING,		// 游戏运行中
			PASSANI,		// 游戏通过的动画
			PASS,			// 游戏通过
			FAILANI,		// 游戏失败的动画
			FAIL };			// 游戏失败


// 游戏者类（每个游戏者都有一个独立的游戏区域）
struct player{
	STATUS	m_status;					// 游戏状态
	char*	m_strName;					// 游戏者名称
	POINT	m_offset;					// 界面的偏移量
	char*	m_keys;						// 按键

	// 任务
	byte	m_Task[MAXTASK];			// 任务列表
	byte	m_iTask;					// 当前需要执行的任务 ID
	int		m_nextTaskY;				// 界面中下一个任务的 Y 坐标

	// 时钟和游戏记录
	clock_t	m_beginClock;				// 游戏开始的时钟计数
	float	m_bestTime;					// 最佳纪录的完成时间
	float	m_lastTime;					// 最后一次的完成时间

	// 控制失败动画的变量
	byte	m_failErrorKey;				// 按错的键的序号（值为 0、1、2、3）
	RECT	m_failRect;					// 按错的键的区域
	int		m_failFrame;				// 失败后的动画的帧计数
};
    
    //创建游戏者
	player player1,player2;

	void PLAYER(char* name, char* keys, int offsetx, int offsety, player *p);		// 模仿构造函数
	void Hit(char key, player *p);												// 处理游戏者按键
	void Draw(player *p);													// 绘制该游戏者的游戏界面

	void Init(player *p);						// 初始化当前游戏者的游戏信息
	void DrawFrame(player *p);					// 绘制游戏界面的外框
	void DrawRow(int baseY, int iTask, player *p);	// 绘制游戏界面中的一行任务
	void DrawPass(player *p);					// 绘制通过游戏后的界面
	void DrawFail(player *p);					// 绘制游戏失败后的界面




	// 进行偏移量计算的绘图函数
	void OutTextXY(int x, int y, LPCTSTR s, player p)							// 在指定位置输出字符串
	{
		outtextxy(p.m_offset.x + x, p.m_offset.y + y, s);
	}
	void OutTextXY(int x, int y, char c, player p)							// 在指定位置输出字符
	{
		outtextxy(p.m_offset.x + x, p.m_offset.y + y, c);
	}
	void Rectangle(int x1, int y1, int x2, int y2, player p)					// 绘制矩形
	{
		rectangle(p.m_offset.x + x1, p.m_offset.y + y1, p.m_offset.x + x2, p.m_offset.y + y2);
	}
	void FillRectangle(int x1, int y1, int x2, int y2, player p)				// 绘制有边框填充矩形
	{
		fillrectangle(p.m_offset.x + x1, p.m_offset.y + y1, p.m_offset.x + x2, p.m_offset.y + y2);
	}
	void SolidRectangle(int x1, int y1, int x2, int y2, player p)				// 绘制无边框填充矩形
	{
		solidrectangle(p.m_offset.x + x1, p.m_offset.y + y1, p.m_offset.x + x2, p.m_offset.y + y2);
	}


//提供信息创建游戏者
void PLAYER(char* name, char* keys, int offsetx, int offsety, player *p)
{
	(*p).m_strName	= name;
	(*p).m_keys		= keys;
	(*p).m_offset.x	= offsetx;
	(*p).m_offset.y	= offsety;

	(*p).m_bestTime	= 99;	// 设置最佳成绩

	Init(p);				// 初始化游戏者

}


// 初始化当前游戏者的游戏信息
void Init(player *p)
{
	// 初始化任务
	for (int i = 0; i < MAXTASK; i++)
		(*p).m_Task[i] = rand() % 4;

	(*p).m_iTask		= 0;			// 从第一个任务开始
	(*p).m_nextTaskY	= 200;			// 设定下一行任务的 Y 坐标，100 是基准，200 表示开始会有下落的动画
	(*p).m_status	= BEGIN;		// 设置游戏初始状态
	(*p).m_failFrame = 0;			// 重置失败后的动画的帧计数

	// 初始化游戏界面
	DrawFrame(p);
}


// 绘制该游戏者的游戏界面
void Draw(player *p)
{
	switch ((*p).m_status)
	{
		case PASSANI:			// 游戏成功后的动画
			if ((*p).m_nextTaskY == 100)
			{
				(*p).m_status = PASS;
				DrawPass(p);
				break;
			}

		case BEGIN:				// 游戏初次开始
		case RUNNING:			// 游戏运行中
		{
			// 如果画面处于静止，直接返回不再重绘
			if ((*p).m_nextTaskY == 100)
				return;

			(*p).m_nextTaskY -= ((*p).m_nextTaskY - 100 + 9) / 10;

			// 绘制完成的任务区
			int rowy = (*p).m_nextTaskY;
			int itask = (*p).m_iTask;
			do
			{
				rowy -= 100;
				itask--;
				DrawRow(rowy, itask, p);
			} while (rowy > 0);

			// 绘制未完成的任务区
			rowy = (*p).m_nextTaskY;
			itask = (*p).m_iTask;
			do
			{
				DrawRow(rowy, itask, p);
				rowy += 100;
				itask++;
			} while (rowy < 400);

			break;
		}

		case FAILANI:			// 游戏失败后的动画
			DrawFail(p);
			break;

		case PASS:				// 游戏通过后的成绩显示
		case FAIL:				// 游戏失败后的成绩显示
			break;
	}
}


// 绘制游戏界面的外框
void DrawFrame(player *p)
{
	// 画外框
	setlinecolor(0xfb9700);                       //外框线条颜色
	Rectangle(0, 0, 243, 464, *p);                 //外框
	setfillcolor(0xeca549);                       //文字背景框颜色
	settextcolor(BLACK);                          //文字颜色
	settextstyle(16, 0, "Verdana");
	setbkmode(TRANSPARENT);

	// 画姓名区
	SolidRectangle(2, 2, 241, 21, *p);             //姓名区背景
	int w = textwidth((*p).m_strName);
	OutTextXY((244 - w) / 2, 4, (*p).m_strName, *p);  //姓名

	// 画成绩区
	SolidRectangle(2, 23, 241, 42, *p);
	char tmp[50];
	__sprintf(tmp, "最好记录：%.3f 秒", (*p).m_bestTime);
	OutTextXY(10, 26, tmp, *p);

	// 2 <= x <= 241, 44 <= y <= 443 为游戏区

	// 画控制区
	SolidRectangle(2, 445, 241, 462, *p);
	for (int i = 0; i < 4; i++)
		OutTextXY(2 + i * 60 + 26, 446, (*p).m_keys[i], *p);
}


// 绘制游戏界面中的一行任务
void DrawRow(int baseY, int iTask, player *p)
{
	int fromY = baseY;				// 任务行的起始 y 坐标
	int toY = baseY + 99;			// 任务行的终止 y 坐标

	// 如果 y 坐标超出显示范围，做调整
	if (fromY < 0) fromY = 0;
	if (toY > 399) toY = 399;

	COLORREF c[4];					// 任务行四个方块的颜色
	if (iTask < 0)
	{
		for (int i = 0; i < 4; i++)
			c[i] = YELLOW;
	}
	else if (iTask >= MAXTASK)
	{
		for (int i = 0; i < 4; i++)
			c[i] = GREEN;
	}
	else
	{
		for (int i = 0; i < 4; i++)
			c[i] = WHITE;
		
		c[(*p).m_Task[iTask]] = (iTask < (*p).m_iTask)? LIGHTGRAY : BLACK;
	}

	// 画任务行的四个方块
	setlinecolor(0xe9dbd6);
	for (int i = 0; i < 4; i++)
	{
		setfillcolor(c[i]);
		FillRectangle(2 + i * 60, 44 + 399 - fromY, 2 + i * 60 + 59, 44 + 399 - toY, *p);
	}

	// 如果是第一行，在方块儿上写“开始”两个字
	if (iTask == 0 && (*p).m_iTask == 0)
	{
		int w = textwidth("开始");
		int h = textheight("开始");
		int x = 2 + (*p).m_Task[iTask] * 60 + (60 - w) / 2;
		int y = 44 + 399 - 99 - fromY + (100 - h) / 2;
		settextcolor(WHITE);
		settextstyle(16, 0, "Verdana");
		OutTextXY(x, y, "开始", *p);
	}
}


// 绘制通过游戏后的界面
void DrawPass(player *p)
{
	// 绘制成功的背景
	setfillcolor(GREEN);
	SolidRectangle(2, 44, 241, 443, *p);

	// 输出"成功"
	settextcolor(WHITE);
	settextstyle(60, 0, "Verdana");
	int w = textwidth("成功");
	OutTextXY((244 - w) / 2, 100, "成功", *p);

	// 输出成绩
	char tmp[100];
	settextstyle(32, 0, "Verdana");
	__sprintf(tmp, "成绩：%.3f 秒", (*p).m_lastTime);
	w = textwidth(tmp);
	OutTextXY((244 - w) / 2, 200, tmp ,*p);
	__sprintf(tmp, "速度：%.3f/s", MAXTASK / (*p).m_lastTime);
	OutTextXY((244 - w) / 2, 240, tmp ,*p);

	// 输出重新开始的提示
	settextstyle(16, 0, "Verdana");
	w = textwidth("按任意控制键重新开始");
	OutTextXY((244 - w) / 2, 400, "按任意控制键重新开始" ,*p);
}


// 绘制游戏失败后的界面
void DrawFail(player *p)
{
	if ((*p).m_failFrame == 0)
	{	// 初始化，计算闪烁效果的区域
		(*p).m_failRect.left		= 3 + (*p).m_failErrorKey * 60;
		(*p).m_failRect.right	= (*p).m_failRect.left + 57;
		(*p).m_failRect.bottom	= (*p).m_nextTaskY + 1;
		(*p).m_failRect.top		= (*p).m_nextTaskY + 98;

		if ((*p).m_failRect.top > 398) (*p).m_failRect.top = 398;
		(*p).m_failRect.bottom	= 44 + 399 - (*p).m_failRect.bottom;
		(*p).m_failRect.top		= 44 + 399 - (*p).m_failRect.top;
	}

	if ((*p).m_failFrame < 60)
	{	// 实现闪烁效果
		setfillcolor((((*p).m_failFrame / 6) % 2 == 0) ? RED : LIGHTRED);
		SolidRectangle((*p).m_failRect.left, (*p).m_failRect.bottom, (*p).m_failRect.right, (*p).m_failRect.top, (*p));
		(*p).m_failFrame++;
	}
	else
	{
		// 改变游戏状态
		(*p).m_status = FAIL;

		// 绘制失败的背景
		setfillcolor(RED);
		SolidRectangle(2, 44, 241, 443 ,*p);

		// 输出"失败"
		settextcolor(WHITE);
		settextstyle(60, 0, "Verdana");
		int w = textwidth("失败");
		OutTextXY((244 - w) / 2, 100, "失败", *p);

		// 输出历史成绩
		settextstyle(20, 0, "Verdana");
		char tmp[100];
		__sprintf(tmp, "历史最好成绩：%.3f 秒", (*p).m_bestTime);
		w = textwidth(tmp);
		OutTextXY((244 - w) / 2, 200, tmp, *p);

		// 输出重新开始的提示
		settextstyle(16, 0, "Verdana");
		w = textwidth("按任意控制键重新开始");
		OutTextXY((244 - w) / 2, 400, "按任意控制键重新开始", *p);
	}
}


// 处理游戏者按键
void Hit(char key, player *p)
{
	switch ((*p).m_status)
	{
		case BEGIN:				// 游戏初次开始
			if (strchr((*p).m_keys, key) != NULL)           //strchr(s,c)函数：查找字符串s中首次出现字符c的位置
			{
				(*p).m_beginClock = clock();				// 记录游戏开始时的时钟
				(*p).m_status = RUNNING;					// 改变游戏状态
			}

		case RUNNING:			// 游戏运行中
		{
			char* pdest = strchr((*p).m_keys, key);
			byte pos;
			if (pdest != NULL)						// 判断是否是当前游戏者按键
			{
				pos = pdest - (*p).m_keys;				// 计算按键对应的位置

				if (pos == (*p).m_Task[(*p).m_iTask])			// 判断按键是否正确
				{
					// 按键正确
					(*p).m_iTask++;
					(*p).m_nextTaskY += 100;

					if ((*p).m_iTask == MAXTASK)			// 如果完成了全部任务
					{
						// 计算完成时间
						clock_t t = clock();
						(*p).m_lastTime = ((float)(clock() - (*p).m_beginClock)) / CLOCKS_PER_SEC;

						// 更新最好记录
						if ((*p).m_lastTime < (*p).m_bestTime)
							(*p).m_bestTime = (*p).m_lastTime;

						// 将最后一条任务滚动出屏幕
						(*p).m_iTask++;
						(*p).m_nextTaskY += 100;
						(*p).m_status = PASSANI;
					}
				}
				else
				{
					// 按键失败
					(*p).m_failErrorKey = pos;
					(*p).m_status = FAILANI;
				}
			}

			break;
		}

		case PASSANI:			// 游戏成功后的动画
		case FAILANI:			// 游戏失败后的动画
			break;

		case PASS:				// 游戏通过后的成绩显示
		case FAIL:				// 游戏失败后的成绩显示
			if (strchr((*p).m_keys, key) != NULL)
				Init(p);
			break;
	}
}


// 程序入口主函数
void main()
{

	initgraph(640, 480);					// 创建绘图窗口
	srand((unsigned)time(NULL));			// 设置随机函数种子

	setbkcolor(0x01bbfb);
	cleardevice();

	//创建游戏者
	PLAYER("A","asdf",38,8,&player1);
	PLAYER("B","jkl;",320,8,&player2);

	char c = 0;

	while (c != 27)                         //按Exc键退出程序
	{
		while (_kbhit())					// 判断是否有按键
		{
			// 按键处理
			c = _getch();
			Hit(c,&player1);
			Hit(c,&player2);
		}

		// 绘制游戏场景
		Draw(&player1);
		Draw(&player2);

		// 延时
		HpSleep(16);
	}

	// 结束游戏
	closegraph();							// 关闭绘图窗口
}