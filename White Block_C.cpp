///////////////////////////////////////////////////
// �������ƣ���Ȱ׿����˫�˰棩 
// ���뻷����Visual Studio��2013��EasyX 20140321(beta)
//
#undef UNICODE
#undef _UNICODE
#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include <time.h>

#define MAXTASK 5         // ������Ϸ��Ҫ��ɵĺڿ�����

// ����� __sprintf ����Ӧ vc6 �� vc2013
#if _MSC_VER > 1200
	#define __sprintf(...) sprintf_s(__VA_ARGS__)
#else
	#define __sprintf sprintf
#endif


// ��ȷ��ʱ����(���Ծ�ȷ�� 1ms������ ��1ms)
void HpSleep(int ms)
{
	static clock_t oldclock = clock();		// ��̬��������¼��һ�� tick

	oldclock += ms * CLOCKS_PER_SEC / 1000;	// ���� tick

	if (clock() > oldclock)					// ����Ѿ���ʱ��������ʱ
		oldclock = clock();
	else
		while (clock() < oldclock)			// ��ʱ
			Sleep(1);						// �ͷ� CPU ����Ȩ������ CPU ռ����
}


// ��Ϸ״̬����
enum STATUS{BEGIN,			// ��Ϸ��ʼ
			RUNNING,		// ��Ϸ������
			PASSANI,		// ��Ϸͨ���Ķ���
			PASS,			// ��Ϸͨ��
			FAILANI,		// ��Ϸʧ�ܵĶ���
			FAIL };			// ��Ϸʧ��


// ��Ϸ���ࣨÿ����Ϸ�߶���һ����������Ϸ����
struct player{
	STATUS	m_status;					// ��Ϸ״̬
	char*	m_strName;					// ��Ϸ������
	POINT	m_offset;					// �����ƫ����
	char*	m_keys;						// ����

	// ����
	byte	m_Task[MAXTASK];			// �����б�
	byte	m_iTask;					// ��ǰ��Ҫִ�е����� ID
	int		m_nextTaskY;				// ��������һ������� Y ����

	// ʱ�Ӻ���Ϸ��¼
	clock_t	m_beginClock;				// ��Ϸ��ʼ��ʱ�Ӽ���
	float	m_bestTime;					// ��Ѽ�¼�����ʱ��
	float	m_lastTime;					// ���һ�ε����ʱ��

	// ����ʧ�ܶ����ı���
	byte	m_failErrorKey;				// ����ļ�����ţ�ֵΪ 0��1��2��3��
	RECT	m_failRect;					// ����ļ�������
	int		m_failFrame;				// ʧ�ܺ�Ķ�����֡����
};
    
    //������Ϸ��
	player player1,player2;

	void PLAYER(char* name, char* keys, int offsetx, int offsety, player *p);		// ģ�¹��캯��
	void Hit(char key, player *p);												// ������Ϸ�߰���
	void Draw(player *p);													// ���Ƹ���Ϸ�ߵ���Ϸ����

	void Init(player *p);						// ��ʼ����ǰ��Ϸ�ߵ���Ϸ��Ϣ
	void DrawFrame(player *p);					// ������Ϸ��������
	void DrawRow(int baseY, int iTask, player *p);	// ������Ϸ�����е�һ������
	void DrawPass(player *p);					// ����ͨ����Ϸ��Ľ���
	void DrawFail(player *p);					// ������Ϸʧ�ܺ�Ľ���




	// ����ƫ��������Ļ�ͼ����
	void OutTextXY(int x, int y, LPCTSTR s, player p)							// ��ָ��λ������ַ���
	{
		outtextxy(p.m_offset.x + x, p.m_offset.y + y, s);
	}
	void OutTextXY(int x, int y, char c, player p)							// ��ָ��λ������ַ�
	{
		outtextxy(p.m_offset.x + x, p.m_offset.y + y, c);
	}
	void Rectangle(int x1, int y1, int x2, int y2, player p)					// ���ƾ���
	{
		rectangle(p.m_offset.x + x1, p.m_offset.y + y1, p.m_offset.x + x2, p.m_offset.y + y2);
	}
	void FillRectangle(int x1, int y1, int x2, int y2, player p)				// �����б߿�������
	{
		fillrectangle(p.m_offset.x + x1, p.m_offset.y + y1, p.m_offset.x + x2, p.m_offset.y + y2);
	}
	void SolidRectangle(int x1, int y1, int x2, int y2, player p)				// �����ޱ߿�������
	{
		solidrectangle(p.m_offset.x + x1, p.m_offset.y + y1, p.m_offset.x + x2, p.m_offset.y + y2);
	}


//�ṩ��Ϣ������Ϸ��
void PLAYER(char* name, char* keys, int offsetx, int offsety, player *p)
{
	(*p).m_strName	= name;
	(*p).m_keys		= keys;
	(*p).m_offset.x	= offsetx;
	(*p).m_offset.y	= offsety;

	(*p).m_bestTime	= 99;	// ������ѳɼ�

	Init(p);				// ��ʼ����Ϸ��

}


// ��ʼ����ǰ��Ϸ�ߵ���Ϸ��Ϣ
void Init(player *p)
{
	// ��ʼ������
	for (int i = 0; i < MAXTASK; i++)
		(*p).m_Task[i] = rand() % 4;

	(*p).m_iTask		= 0;			// �ӵ�һ������ʼ
	(*p).m_nextTaskY	= 200;			// �趨��һ������� Y ���꣬100 �ǻ�׼��200 ��ʾ��ʼ��������Ķ���
	(*p).m_status	= BEGIN;		// ������Ϸ��ʼ״̬
	(*p).m_failFrame = 0;			// ����ʧ�ܺ�Ķ�����֡����

	// ��ʼ����Ϸ����
	DrawFrame(p);
}


// ���Ƹ���Ϸ�ߵ���Ϸ����
void Draw(player *p)
{
	switch ((*p).m_status)
	{
		case PASSANI:			// ��Ϸ�ɹ���Ķ���
			if ((*p).m_nextTaskY == 100)
			{
				(*p).m_status = PASS;
				DrawPass(p);
				break;
			}

		case BEGIN:				// ��Ϸ���ο�ʼ
		case RUNNING:			// ��Ϸ������
		{
			// ������洦�ھ�ֹ��ֱ�ӷ��ز����ػ�
			if ((*p).m_nextTaskY == 100)
				return;

			(*p).m_nextTaskY -= ((*p).m_nextTaskY - 100 + 9) / 10;

			// ������ɵ�������
			int rowy = (*p).m_nextTaskY;
			int itask = (*p).m_iTask;
			do
			{
				rowy -= 100;
				itask--;
				DrawRow(rowy, itask, p);
			} while (rowy > 0);

			// ����δ��ɵ�������
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

		case FAILANI:			// ��Ϸʧ�ܺ�Ķ���
			DrawFail(p);
			break;

		case PASS:				// ��Ϸͨ����ĳɼ���ʾ
		case FAIL:				// ��Ϸʧ�ܺ�ĳɼ���ʾ
			break;
	}
}


// ������Ϸ��������
void DrawFrame(player *p)
{
	// �����
	setlinecolor(0xfb9700);                       //���������ɫ
	Rectangle(0, 0, 243, 464, *p);                 //���
	setfillcolor(0xeca549);                       //���ֱ�������ɫ
	settextcolor(BLACK);                          //������ɫ
	settextstyle(16, 0, "Verdana");
	setbkmode(TRANSPARENT);

	// ��������
	SolidRectangle(2, 2, 241, 21, *p);             //����������
	int w = textwidth((*p).m_strName);
	OutTextXY((244 - w) / 2, 4, (*p).m_strName, *p);  //����

	// ���ɼ���
	SolidRectangle(2, 23, 241, 42, *p);
	char tmp[50];
	__sprintf(tmp, "��ü�¼��%.3f ��", (*p).m_bestTime);
	OutTextXY(10, 26, tmp, *p);

	// 2 <= x <= 241, 44 <= y <= 443 Ϊ��Ϸ��

	// ��������
	SolidRectangle(2, 445, 241, 462, *p);
	for (int i = 0; i < 4; i++)
		OutTextXY(2 + i * 60 + 26, 446, (*p).m_keys[i], *p);
}


// ������Ϸ�����е�һ������
void DrawRow(int baseY, int iTask, player *p)
{
	int fromY = baseY;				// �����е���ʼ y ����
	int toY = baseY + 99;			// �����е���ֹ y ����

	// ��� y ���곬����ʾ��Χ��������
	if (fromY < 0) fromY = 0;
	if (toY > 399) toY = 399;

	COLORREF c[4];					// �������ĸ��������ɫ
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

	// �������е��ĸ�����
	setlinecolor(0xe9dbd6);
	for (int i = 0; i < 4; i++)
	{
		setfillcolor(c[i]);
		FillRectangle(2 + i * 60, 44 + 399 - fromY, 2 + i * 60 + 59, 44 + 399 - toY, *p);
	}

	// ����ǵ�һ�У��ڷ������д����ʼ��������
	if (iTask == 0 && (*p).m_iTask == 0)
	{
		int w = textwidth("��ʼ");
		int h = textheight("��ʼ");
		int x = 2 + (*p).m_Task[iTask] * 60 + (60 - w) / 2;
		int y = 44 + 399 - 99 - fromY + (100 - h) / 2;
		settextcolor(WHITE);
		settextstyle(16, 0, "Verdana");
		OutTextXY(x, y, "��ʼ", *p);
	}
}


// ����ͨ����Ϸ��Ľ���
void DrawPass(player *p)
{
	// ���Ƴɹ��ı���
	setfillcolor(GREEN);
	SolidRectangle(2, 44, 241, 443, *p);

	// ���"�ɹ�"
	settextcolor(WHITE);
	settextstyle(60, 0, "Verdana");
	int w = textwidth("�ɹ�");
	OutTextXY((244 - w) / 2, 100, "�ɹ�", *p);

	// ����ɼ�
	char tmp[100];
	settextstyle(32, 0, "Verdana");
	__sprintf(tmp, "�ɼ���%.3f ��", (*p).m_lastTime);
	w = textwidth(tmp);
	OutTextXY((244 - w) / 2, 200, tmp ,*p);
	__sprintf(tmp, "�ٶȣ�%.3f/s", MAXTASK / (*p).m_lastTime);
	OutTextXY((244 - w) / 2, 240, tmp ,*p);

	// ������¿�ʼ����ʾ
	settextstyle(16, 0, "Verdana");
	w = textwidth("��������Ƽ����¿�ʼ");
	OutTextXY((244 - w) / 2, 400, "��������Ƽ����¿�ʼ" ,*p);
}


// ������Ϸʧ�ܺ�Ľ���
void DrawFail(player *p)
{
	if ((*p).m_failFrame == 0)
	{	// ��ʼ����������˸Ч��������
		(*p).m_failRect.left		= 3 + (*p).m_failErrorKey * 60;
		(*p).m_failRect.right	= (*p).m_failRect.left + 57;
		(*p).m_failRect.bottom	= (*p).m_nextTaskY + 1;
		(*p).m_failRect.top		= (*p).m_nextTaskY + 98;

		if ((*p).m_failRect.top > 398) (*p).m_failRect.top = 398;
		(*p).m_failRect.bottom	= 44 + 399 - (*p).m_failRect.bottom;
		(*p).m_failRect.top		= 44 + 399 - (*p).m_failRect.top;
	}

	if ((*p).m_failFrame < 60)
	{	// ʵ����˸Ч��
		setfillcolor((((*p).m_failFrame / 6) % 2 == 0) ? RED : LIGHTRED);
		SolidRectangle((*p).m_failRect.left, (*p).m_failRect.bottom, (*p).m_failRect.right, (*p).m_failRect.top, (*p));
		(*p).m_failFrame++;
	}
	else
	{
		// �ı���Ϸ״̬
		(*p).m_status = FAIL;

		// ����ʧ�ܵı���
		setfillcolor(RED);
		SolidRectangle(2, 44, 241, 443 ,*p);

		// ���"ʧ��"
		settextcolor(WHITE);
		settextstyle(60, 0, "Verdana");
		int w = textwidth("ʧ��");
		OutTextXY((244 - w) / 2, 100, "ʧ��", *p);

		// �����ʷ�ɼ�
		settextstyle(20, 0, "Verdana");
		char tmp[100];
		__sprintf(tmp, "��ʷ��óɼ���%.3f ��", (*p).m_bestTime);
		w = textwidth(tmp);
		OutTextXY((244 - w) / 2, 200, tmp, *p);

		// ������¿�ʼ����ʾ
		settextstyle(16, 0, "Verdana");
		w = textwidth("��������Ƽ����¿�ʼ");
		OutTextXY((244 - w) / 2, 400, "��������Ƽ����¿�ʼ", *p);
	}
}


// ������Ϸ�߰���
void Hit(char key, player *p)
{
	switch ((*p).m_status)
	{
		case BEGIN:				// ��Ϸ���ο�ʼ
			if (strchr((*p).m_keys, key) != NULL)           //strchr(s,c)�����������ַ���s���״γ����ַ�c��λ��
			{
				(*p).m_beginClock = clock();				// ��¼��Ϸ��ʼʱ��ʱ��
				(*p).m_status = RUNNING;					// �ı���Ϸ״̬
			}

		case RUNNING:			// ��Ϸ������
		{
			char* pdest = strchr((*p).m_keys, key);
			byte pos;
			if (pdest != NULL)						// �ж��Ƿ��ǵ�ǰ��Ϸ�߰���
			{
				pos = pdest - (*p).m_keys;				// ���㰴����Ӧ��λ��

				if (pos == (*p).m_Task[(*p).m_iTask])			// �жϰ����Ƿ���ȷ
				{
					// ������ȷ
					(*p).m_iTask++;
					(*p).m_nextTaskY += 100;

					if ((*p).m_iTask == MAXTASK)			// ��������ȫ������
					{
						// �������ʱ��
						clock_t t = clock();
						(*p).m_lastTime = ((float)(clock() - (*p).m_beginClock)) / CLOCKS_PER_SEC;

						// ������ü�¼
						if ((*p).m_lastTime < (*p).m_bestTime)
							(*p).m_bestTime = (*p).m_lastTime;

						// �����һ�������������Ļ
						(*p).m_iTask++;
						(*p).m_nextTaskY += 100;
						(*p).m_status = PASSANI;
					}
				}
				else
				{
					// ����ʧ��
					(*p).m_failErrorKey = pos;
					(*p).m_status = FAILANI;
				}
			}

			break;
		}

		case PASSANI:			// ��Ϸ�ɹ���Ķ���
		case FAILANI:			// ��Ϸʧ�ܺ�Ķ���
			break;

		case PASS:				// ��Ϸͨ����ĳɼ���ʾ
		case FAIL:				// ��Ϸʧ�ܺ�ĳɼ���ʾ
			if (strchr((*p).m_keys, key) != NULL)
				Init(p);
			break;
	}
}


// �������������
void main()
{

	initgraph(640, 480);					// ������ͼ����
	srand((unsigned)time(NULL));			// ���������������

	setbkcolor(0x01bbfb);
	cleardevice();

	//������Ϸ��
	PLAYER("A","asdf",38,8,&player1);
	PLAYER("B","jkl;",320,8,&player2);

	char c = 0;

	while (c != 27)                         //��Exc���˳�����
	{
		while (_kbhit())					// �ж��Ƿ��а���
		{
			// ��������
			c = _getch();
			Hit(c,&player1);
			Hit(c,&player2);
		}

		// ������Ϸ����
		Draw(&player1);
		Draw(&player2);

		// ��ʱ
		HpSleep(16);
	}

	// ������Ϸ
	closegraph();							// �رջ�ͼ����
}