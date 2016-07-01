// 五子棋.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "五子棋.h"
#include<stdio.h>
#include<cmath>
#include <tchar.h>
#include<string>
#include<cstring>
#include <sstream>
#include <mmsystem.h>  
#include<fstream>
#pragma comment(lib, "winmm.lib")  
#define MAX_LOADSTRING 100

const int comElo=1000;
int playerElo=1000;
int winNum=0;
int tieNum=0;
int loseNum=0;
int playerRank=6;
char playername[50];

class InfoBase{
public:
   virtual void readInfo()=0;
   virtual void saveInfo()=0;
   virtual int CDECL MessageBoxPrintf (TCHAR *szCaption,TCHAR *szFormat,...)=0;
};
class InfoHandle:public InfoBase{
public:
	 TCHAR szBuffer [1024];
	 va_list pArgList;
	void setrank(){
if(playerElo<875){
			playerRank=9;
		}
			if(playerElo>=875&&playerElo<925){
			playerRank=8;
		}
	if(playerElo>=925&&playerElo<975){
			playerRank=7;
		}
		if(playerElo>=975&&playerElo<1025){
			playerRank=6;
		}
		if(playerElo>=1025&&playerElo<1075){
			playerRank=5;
		}
		if(playerElo>=1075&&playerElo<1125){
			playerRank=4;
		}
		if(playerElo>=1125&&playerElo<1175){
			playerRank=3;
		}
		if(playerElo>=1175&&playerElo<1225){
			playerRank=2;
		}
		if(playerElo>=1225){
			playerRank=1;
		}
	}
	void ELO(double a){
		double e=10*(a-(1.0/(1+pow(10,-double(playerElo-comElo)/400.0))));
		if(e<1&&e>0){
			e=1;
		}
		playerElo+=int(e);
		setrank();
		
	}
	
	void saveInfo(){
		std::fstream saver(".\\source\\game.dat",std::ios::out|std::ios::binary);
		saver.write((char*)&winNum,sizeof(winNum));
		saver.write((char*)&tieNum,sizeof(tieNum));
		saver.write((char*)&loseNum,sizeof(loseNum));
		saver.write((char*)&playerElo,sizeof(playerElo));
		saver.write((char*)&playerRank,sizeof(playerRank));
		saver.close();
	}
	void readInfo(){
		std::fstream load(".\\source\\game.dat",std::ios::in|std::ios::binary);
		load.read((char*)&winNum,sizeof(winNum));
		load.read((char*)&tieNum,sizeof(tieNum));
		load.read((char*)&loseNum,sizeof(loseNum));
		load.read((char*)&playerElo,sizeof(playerElo));
		load.read((char*)&playerRank,sizeof(playerRank));
	}
int CDECL MessageBoxPrintf(TCHAR * szCaption,TCHAR * szFormat, ...)
	{
	     va_start(pArgList,szFormat);
	     _vsntprintf(szBuffer,sizeof(szBuffer)/sizeof(TCHAR),szFormat,pArgList);
	     va_end(pArgList); 
	     return MessageBox(NULL, szBuffer, szCaption, MB_YESNO|MB_ICONINFORMATION|MB_SYSTEMMODAL);
	}
};

// 全局变量:
class text;
HINSTANCE hInst;								
TCHAR szTitle[MAX_LOADSTRING];					
TCHAR szWindowClass[MAX_LOADSTRING];			
HDC hdc,mdc,bufdc,foo,blankdc,tempdc;
HWND hWnd;
DWORD tPre,tNow;
std::wstring wstr;
InfoHandle handle;

int board[15][15];
bool playerWinMethods[15][15][572];
bool computerWinMethods[15][15][572];
int win[2][572];
int num[2];
bool turn,over;
bool computerWhite=false;
bool firstMove=true;
int winner;
bool inMenu=true;
bool refresh=false;

class text{
public: 
HFONT      hFont ;
LOGFONT    lf ;

int myDrawText(TCHAR *text, int x, int y, int color)   
{   
	 lf.lfHeight         = - 40;
        lf.lfWidth          = 0 ;
        lf.lfEscapement     = 0 ;
        lf.lfOrientation      = 0 ;
        lf.lfWeight         = 5;
        lf.lfItalic           = 0 ;
        lf.lfUnderline       = 0 ;
        lf.lfStrikeOut       = 0 ;
        lf.lfCharSet        = DEFAULT_CHARSET ;
        lf.lfOutPrecision    = 0 ;
        lf.lfClipPrecision    = 0 ;
        lf.lfQuality         = 0 ;
        lf.lfPitchAndFamily  = 0 ;
        lstrcpy (lf.lfFaceName, _T("华文行楷")) ;   
		SetBkMode(hdc, TRANSPARENT);  
        hFont = CreateFontIndirect (&lf) ;
		SelectObject (hdc,hFont) ;
	SetTextColor(hdc, color);   
    TextOut(hdc, x, y, text, lstrlen(text));  
    return 1;   
}  

int myDrawText(TCHAR *text1,TCHAR *text2)   
{   InfoHandle temp;
	temp.MessageBoxPrintf(text1,text2);
    return 1;   
}  
bool multiToUnicode(const std::string& multiText, std::wstring& unicodeText)
{ 
    int size=::MultiByteToWideChar(CP_ACP,0,multiText.c_str(),-1,NULL,0);
    if (0==size){
        return false;
    }
    wchar_t* wszBuffer=new wchar_t[size+1];
    ::ZeroMemory(wszBuffer,(size+1)*sizeof(wchar_t));
    if(0==::MultiByteToWideChar(CP_ACP,0,multiText.c_str(),-1,wszBuffer,size+1))
    {
        delete[] wszBuffer;
        return false;
    }     
    unicodeText=wszBuffer;
    delete[] wszBuffer;
    return true;
}
};

BOOL CALLBACK SetChildFont(HWND hWnd,LPARAM lParam)
{
	SendMessage(hWnd, WM_SETFONT,(WPARAM)lParam,TRUE);
	return TRUE;
}

text myText;


class GUIandAI{
	public:
	HBITMAP chess[2];
    HBITMAP tile,bmp,menu,blank;

	void MyPaint(HDC hdc);
	void restart();
	void menuPaint(HDC hdc);
	void InitGame();
	void ComputerTurn();
	BOOL operator <<(WCHAR *sound){
		return(PlaySound(sound, NULL, SND_FILENAME | SND_ASYNC)); 
	}
};
GUIandAI gui;
void GUIandAI::menuPaint(HDC hdc){
	if(refresh==true){
		BitBlt(mdc,0,0,1000,750,blankdc,0,0,SRCCOPY);
	}
}
void GUIandAI::MyPaint(HDC hdc){
	SelectObject(mdc,tile);
	
	refresh=false;
	char*str;
	if(over){
		switch(winner){
		case 0:
			gui<<_T(".\\source\\win.wav");  
			break;
		case 1:
			
			gui<<_T(".\\source\\lose.wav");  
			break;
		case 2:
			gui<<_T(".\\source\\tie.wav"); 
			break;
		}

	}
	else if(!turn){
		
		if(firstMove==false){
		gui.ComputerTurn();
		}
	}
	else{
		
	}
	
	for(int i=0;i<15;i++){
		for(int j=0;j<15;j++){
			if(computerWhite==true){
if(board[i][j]==0){
	SelectObject(bufdc,gui.chess[0]);
	BitBlt(mdc,i*50,j*50,50,50,bufdc,50,0,SRCAND);
	BitBlt(mdc,i*50,j*50,50,50,bufdc,0,0,SRCPAINT);
}
else if(board[i][j]==1){
	SelectObject(bufdc,gui.chess[1]);
	BitBlt(mdc,i*50,j*50,50,50,bufdc,50,0,SRCAND);
	BitBlt(mdc,i*50,j*50,50,50,bufdc,0,0,SRCPAINT);
}
			}
			if(computerWhite==false){
if(board[i][j]==0){
	SelectObject(bufdc,gui.chess[1]);
	BitBlt(mdc,i*50,j*50,50,50,bufdc,50,0,SRCAND);
	BitBlt(mdc,i*50,j*50,50,50,bufdc,0,0,SRCPAINT);
}
else if(board[i][j]==1){
	SelectObject(bufdc,gui.chess[0]);
	BitBlt(mdc,i*50,j*50,50,50,bufdc,50,0,SRCAND);
	BitBlt(mdc,i*50,j*50,50,50,bufdc,0,0,SRCPAINT);
}
			}
		}
	}

	BitBlt(hdc,0,0,1000,750,mdc,0,0,SRCCOPY);
	myText.myDrawText(TEXT("山僧对棋坐"),755, 230, 000000) ;
	myText.myDrawText(TEXT("局上竹阴清"),775, 280, 000000) ;
	tPre=GetTickCount();
}
void GUIandAI::restart(){
	BitBlt(hdc,0,0,1000,750,foo,0,0,SRCCOPY);
}
void GUIandAI::InitGame(){
	int i,j,k;
	int count=0;
	computerWhite=false;
	firstMove=true;
	over=false;
	num[0]=num[1]=0;
	for(int i=0;i<572;i++){
		win[0][i]=0;
		win[1][i]=0;
	}
	for(int i=0;i<15;i++){
		for(int j=0;j<15;j++){
			board[i][j]=2;
		}
	}

	for(i=0;i<15;i++){
		for(j=0;j<11;j++){
			for(k=0;k<5;k++){
				playerWinMethods[i][j+k][count]=true;
				computerWinMethods[i][j+k][count]=true;
			}
			count++;
		}
	}
	for(i=0;i<15;i++){
		for(j=0;j<11;j++){
			for(k=0;k<5;k++){
				playerWinMethods[j+k][i][count]=true;
				computerWinMethods[j+k][i][count]=true;
			}
			count++;
		}
	}
	for(i=0;i<11;i++){
		for(j=0;j<11;j++){
			for(k=0;k<5;k++){
				playerWinMethods[j+k][i+k][count]=true;
				computerWinMethods[j+k][i+k][count]=true;
			}
			count++;
		}
	}
	for(i=0;i<11;i++){
		for(j=14;j>=4;j--){
			for(k=0;k<5;k++){
				playerWinMethods[j-k][i+k][count]=true;
				computerWinMethods[j-k][i+k][count]=true;
			}
			count++;
		}
	}
	srand(GetTickCount());
	if(inMenu==false){
	if(rand()%2==0){
		turn =true;
		firstMove=false;
		computerWhite=true;
	}
	else{
		turn =false;
		computerWhite=false;
		gui.ComputerTurn();
	}
	}
}

void GUIandAI::ComputerTurn(){
	if(computerWhite==false&&firstMove==true){
		board[7][7]=1;
	num[1]++;
		for(int j=0;j<572;j++){
			if(computerWinMethods[7][7][j]){
				win[1][j]++;
				playerWinMethods[7][7][j]=false;
				win[0][j]=7;
			}
		}
		turn =true;
		firstMove=false;
		::InvalidateRect(hWnd, NULL, TRUE);
		return ;
	}
	int grades[2][15][15];
	int m,n,i,max=0;
	int u=8,v=8;
	for(m=0;m<15;m++){
		for(n=0;n<15;n++){
			grades[0][m][n]=0;
			grades[1][m][n]=0;
			if(board[m][n]==2){
				for(i=0;i<572;i++){
					if(playerWinMethods[m][n][i]&&win[0][i]!=7){
						switch(win[0][i]){
						case 0:
							grades[0][m][n]+=1;break;
						case 1:
							grades[0][m][n]+=220;break;
						case 2:
							grades[0][m][n]+=420;break;
						case 3:
							grades[0][m][n]+=2500;break;
						case 4:
							grades[0][m][n]+=30000;
							break;
						}
					}
					if(computerWinMethods[m][n][i]&&win[1][i]!=7){
						switch(win[1][i]){
						case 0:
							grades[1][m][n]+=1;break;
						case 1:
							grades[1][m][n]+=220;break;
						case 2:
							grades[1][m][n]+=420;break;
						case 3:
							grades[1][m][n]+=4000;break;
						case 4:
							grades[1][m][n]+=100000;
							break;
						}
					}
				}
				if(max==0){
					u=m;
					v=n;
				}
				if(grades[0][m][n]>max){
					max=grades[0][m][n];
					u=m;
					v=n;
				}
				else if(grades[0][m][n]==max){
					if(grades[1][m][n]>grades[1][u][v]){
						u=m;
						v=n;
					}

				}
				if(grades[1][m][n]>max){
					max=grades[1][m][n];
					u=m;
					v=n;
				}
				else if(grades[1][m][n]==max){
					if(grades[0][m][n]>grades[0][u][v]){
						u=m;
						v=n;
					}

				}
			}
		}
	}
	board[u][v]=1;
	num[1]++;
	if((num[0]==112&&num[1]==113)||(num[1]==112&&num[0]==113)){
		winner=2;
		over=true;
		tieNum++;
		handle.saveInfo();handle.ELO(0.5);
		gui.MyPaint(hdc);
		if(handle.MessageBoxPrintf (TEXT("五子棋---Powered by wuch15"),TEXT("         不分胜负，本局平局！\n您的棋力：%d    段位：%d级\n胜利：%d  平局：%d  败北：%d"), playerElo,playerRank,winNum,tieNum,loseNum)){
		inMenu=true;
		refresh=true;
		//gui<<_T("tie.wav");
		::InvalidateRect(hWnd, NULL, TRUE);
	}
	}
	else{
		for(i=0;i<572;i++){
			if(computerWinMethods[u][v][i]){
				win[1][i]++;
				playerWinMethods[u][v][i]=false;
				win[0][i]=7;
				if(win[1][i]==5){
					winner =1;
					over =true;
					loseNum++;
					handle.saveInfo();handle.ELO(0);
					gui.MyPaint(hdc);
					handle.MessageBoxPrintf (TEXT("五子棋---Powered by wuch15"),TEXT("         抱歉，您本局不敌电脑！\n您的棋力：%d    段位：%d级\n胜利：%d  平局：%d  败北：%d"), playerElo,playerRank,winNum,tieNum,loseNum);
					inMenu=true;
					refresh=true;
					//gui<<_T("lose.wav");
					::InvalidateRect(hWnd, NULL, TRUE);
					
				}
			}
		}

	}
	turn =true;
	::InvalidateRect(hWnd, NULL, TRUE);
}

//部分函数声明
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: 在此放置代码。
	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MY, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY));

	// 主消息循环:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}

// 注册窗口类。

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_MY);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

	return RegisterClassEx(&wcex);
}


//实例句柄，创建主窗口

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	
int rowNum,colNum;
int i;
   hInst = hInstance; // 将实例句柄存储在全局变量中

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }
   MoveWindow(hWnd,0,0,1000,807,true);
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   hdc=GetDC(hWnd);
   mdc=CreateCompatibleDC(hdc);
   bufdc=CreateCompatibleDC(hdc);
   foo=CreateCompatibleDC(hdc);
   blankdc=CreateCompatibleDC(hdc);
   tempdc=CreateCompatibleDC(hdc);
   gui.tile=CreateCompatibleBitmap(hdc,1000,750);
   handle.readInfo();
   handle.setrank();
   //bmp=CreateCompatibleBitmap(hdc,1000,750);
   gui.menu=(HBITMAP)LoadImage(NULL,L".\\source\\menu.bmp",IMAGE_BITMAP,1000,750,LR_LOADFROMFILE); 
   gui.blank=(HBITMAP)LoadImage(NULL,L".\\source\\blank.bmp",IMAGE_BITMAP,1000,750,LR_LOADFROMFILE); 
   gui.tile=(HBITMAP)LoadImage(NULL,L".\\source\\chessboard.bmp",IMAGE_BITMAP,1000,750,LR_LOADFROMFILE);  
   
   SelectObject(foo,gui.menu);
    SelectObject(blankdc,gui.blank);
   BitBlt(hdc,0,0,1000,750,foo,0,0,SRCCOPY);
    gui.chess[0]=(HBITMAP)LoadImage(NULL,L".\\source\\black.bmp",IMAGE_BITMAP,100,50,LR_LOADFROMFILE);
    gui.chess[1]=(HBITMAP)LoadImage(NULL,L".\\source\\white.bmp",IMAGE_BITMAP,100,50,LR_LOADFROMFILE);
	gui.InitGame();
	//MyPaint(hdc);
   return TRUE;
}

//主窗口消息

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	int x,y,m,n,i;
	PAINTSTRUCT ps;

	switch (message)
	{
	
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_KEYDOWN:
		switch(wParam){
	case VK_ESCAPE:
		PostQuitMessage(0);
		break;
		}

	case WM_LBUTTONDOWN:
		if(inMenu==false){
		::InvalidateRect(hWnd, NULL, TRUE);
		gui.MyPaint(hdc);
		
		if(!over){
			if(turn){
				x=LOWORD(lParam);
				y=HIWORD(lParam);
				if(x>10&&x<740&&y>10&&y<740){
					m=x/50;
					n=y/50;
					if(board[m][n]==2){
						board[m][n]=0;
						num[0]++;

						if((num[0]==112&&num[1]==113)||(num[1]==112&&num[0]==113)){
							winner=2;

							handle.saveInfo();
							tieNum++;

							over=true;handle.ELO(0.5);
							gui.MyPaint(hdc);
							inMenu=true;
							refresh=true;
							::InvalidateRect(hWnd, NULL, TRUE);
							
								
						//if(winner==2)
						//{gui<<_T("tie.wav");}
							if(handle.MessageBoxPrintf (TEXT("五子棋---Powered by wuch15"),TEXT("         不分胜负，本局平局！\n您的棋力：%d    段位：%d级\n胜利：%d  平局：%d  败北：%d"), playerElo,playerRank,winNum,tieNum,loseNum)){
									
									
								::InvalidateRect(hWnd, NULL, TRUE);}
						}
						else{
							for(i=0;i<572;i++){
								if(playerWinMethods[m][n][i]){
									win[0][i]++;
									computerWinMethods[m][n][i]=false;
									win[1][i]=7;

								

								if(win[0][i]==5){
									winNum++;
									handle.saveInfo();
									winner=0;
									over=true;handle.ELO(1);
									gui.MyPaint(hdc);
							inMenu=true;
							refresh=true;
							::InvalidateRect(hWnd, NULL, TRUE);
							//if(winner==0)
						//{gui<<_T("win.wav");}
									if(handle.MessageBoxPrintf (TEXT("五子棋---Powered by wuch15"),TEXT("         恭喜您本局获得胜利！\n您的棋力：%d    段位：%d级\n胜利：%d  平局：%d  败北：%d"), playerElo,playerRank,winNum,tieNum,loseNum)){
										
										//gui.MyPaint(hdc);
										//gui.menuPaint(hdc);
										//::InvalidateRect(hWnd, NULL, TRUE);
									}
								}
								}
							}
							
						}
						turn =false;
						gui<<_T(".\\source\\Stone.wav");
					
				}
				}

			}
		}
		
		::InvalidateRect(hWnd, NULL, TRUE);
		gui.menuPaint(hdc);
		
		}
		if(inMenu==true){
			int xx=LOWORD(lParam);
			int yy=HIWORD(lParam);
			if(xx>=180&&xx<=470&&yy>=330&&yy<=380){
				inMenu=false;
				gui.InitGame();
				::InvalidateRect(hWnd, NULL, TRUE);
			}
			if(xx>=180&&xx<=470&&yy>=435&&yy<=485){
				handle.saveInfo();
	handle.MessageBoxPrintf (TEXT("五子棋---Powered by wuch15"),TEXT("             游戏信息：\n您的棋力：%d    段位：%d级\n胜利：%d  平局：%d  败北：%d"), playerElo,playerRank,winNum,tieNum,loseNum);

		
			}
			if(xx>=180&&xx<=470&&yy>=540&&yy<=590){
				int result=myText.myDrawText(L"五子棋---Powered by wuch15",L"谢谢您的使用，再见");
					exit(0);
				//::MessageBox(NULL,L"                 谢谢使用，再见",L"五子棋---Powered by wuch15",MB_OK);
				
			}
		}
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		if(inMenu==false){
			gui.menuPaint(hdc);
		gui.MyPaint(hdc);
		}
		if(inMenu==true){
		 gui.restart();
		}
		// TODO: 在此添加任意绘图代码...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		DeleteDC(mdc);
		DeleteDC(bufdc);
		DeleteObject(gui.chess[0]);
		DeleteObject(gui.chess[1]);
		ReleaseDC(hWnd,hdc);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}



