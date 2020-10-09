﻿#include <iostream>//для использования стандарных операторов типо cout
#include <conio.h>      //для использования getch()
#pragma comment(lib, "ws2_32")//включаем библиотеку для сокетов
#include <winsock2.h> //подключаем библеотеку винсокета
#include <Windows.h> 
#include <Shellapi.h>
#include <cstdio>
#include <TlHelp32.h>

#pragma warning(disable:4996)

using namespace std;//прямой доступ к операторам типо std::cout<<endl;

//глобальные переменные

//------------

int main(void)
{
	system("chcp 1251 > text");//ставим подержку русской кодировки
//--------------
	std::cout << "    С Е Р В Е Р  >>>" << endl << endl;
	while (1) // постоянная работа сервера
	{
		int i_error = 0;//будет хранить код ошибки

		/********************************************
		0. настройка библиотеки Ws2_32.dll*/
		//В случае успеха WSAStartup возвращает 0; иначе - код ошибки. 
		WSADATA wsaData;//определяем переменную
		i_error = WSAStartup(MAKEWORD(2, 2), &wsaData);//настраиваем
		if (i_error)//проверка на успешность настройки
		{

		std::cout << "   ОШИБКА! " << WSAGetLastError() << "; 2й метод: " << i_error << endl;

		}
		else
		{
			std::cout << "   библиотека Ws2_32.dll - успешно настроена." << endl;
		}

		/********************************************
		1. создание серверного сокета*/
		//В случае успеха socket возвращает номер сокет-дескриптора. иначе -1
		SOCKET SSock;//создаем прототип
		SSock = socket(
			AF_INET,//используется при создании Интернет-приложений
			SOCK_STREAM,//потоковый
			IPPROTO_TCP//ТСР - для потоковых
		);
		if (SSock == -1)
		{
			std::cout << "   ОШИБКА! - сокет не создан!" << endl;
		}
		else 
		{ 
			std::cout << "   Сокет успешно создан." << endl; 
		}

		/********************************************
		2. Привязка к локальным именам*/
		//необходимо сокет связать с IP-адресом компьютера.
		struct sockaddr_in  dest_addr;//создаем протатип(dest_addr), структуры sockaddr_in
		dest_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
		dest_addr.sin_port = htons(3000); //порт
		dest_addr.sin_family = AF_INET;//при создании Интернет-приложений
		//INADDR_ANY сервер работает НА машине, с любым ip
		/*если мы захотим фактически задать IP-адрес
		Мы должны использовать функцию inet_addr(). inet_addr ("129.42.12.241")*/
		i_error = 0;//очистка переменной для ошибок
		i_error = bind( //В случае успеха bind возвращает 0, в противном случае - "-1"
				SSock,
				(SOCKADDR*)&dest_addr,//addr - указатель на структуру sockaddr_in
				sizeof(dest_addr) //размер структуры.
			          );
		if (i_error == -1) 
		{
			std::cout << "   ОШИБКА! сокет не связан с IP-адресом компьютера!" << endl;
		}
		else 
		{ 
			std::cout << "   Сокет успешно связан с IP-адресом компьютера." << endl;
		}

		/********************************************
		3. Инициализация процесса «прослушивания» порта*/
		//приложение-сервер будет следить за всеми подключениями на заданный порт
		std::cout << "   Инициализация процесса «прослушивания» порта" << endl;
		listen(
			SSock,
			3 //сколько запросов могут стоять в очереди
		);
	
		/********************************************
		4. подключение клиента - Получение дескриптора клиентского соединения*/
		//сервер создает новый сокет и связывает его с ассоциацией, 
		//эквивалентной 'слушающему сокету'
		cout << "   Ожидание клиента\n";
		SOCKET ClientSock;//создаем прототип сокета для подключающегося клиента
		struct sockaddr_in addr_klienta;//создаем прототип структуры для подключающегося клиента
		//Если функция accept выполнилась успешно - возвращает новый сокет 
		//для установленного соединения, в противном случае  INVALID_SOCKET
		int size_client_addr = sizeof(addr_klienta);//размер структуры для клиента
		ClientSock = accept(
			SSock,
			(sockaddr*)&addr_klienta, //адрес структуры (!)sockaddr_in 
			//куда будет помещена информация о клиенте.
			&size_client_addr // Длинна адреса
			               );
		/*
			Метод Socket.Accept извлекает из очереди первый запрос и
			возвращает новый объект Socket,
			который можно использовать для коммуникационного взаимодействия с клиентом.
		*/
		/*
			accept в addr будет возвращена информация о клиенте.
			Нам будет важна только:
			addr.sin_port – порт с которого было подключение
			addr.sin_addr – ip клиента в двоичном формате. ;
		*/

		if (ClientSock == INVALID_SOCKET) std::cout << "   ОШИБКА! подключения клиента" << endl;
		else std::cout << "   Клиент успешно подключен" << endl;

		while (1) // Для поддержки нескольких команд
		{
			/********************************************
			Передача-чтение данных*/
			//чтение
			char Messege_Server[1024];
			if (recv(//чтение
				ClientSock,//сокет
				Messege_Server,//буфер куда поподут данные
				1024,//размер буфера чтения - сколько считать
				0//делаем ее аналогичной как функция read
				) > 0)
			{
					std::cout << "   Получене команда: " << Messege_Server << endl;

					/*********************************************
					Обработка команд клиента*/
				switch (Messege_Server[0])
				{
					/*********************************************
					Открытие тотал командера*/
					case 'O':
					{
						STARTUPINFO si = { sizeof(STARTUPINFO) };

						si.cb = sizeof(si);
						si.dwFlags = STARTF_USESHOWWINDOW;
						si.wShowWindow = SW_HIDE;
						PROCESS_INFORMATION pi;

						CreateProcess(L"C:\\totalcmd\\TOTALCMD64.EXE", NULL, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
					}
					/*********************************************
					Закрытие тотал командера*/
					case 'C':
					{

							break;
						}
					/*********************************************
					просмотр системеного времени*/
					case 'T':
					{

							break;
						}
					/*********************************************
					изменение системного времени*/
					case 'M':
					{


							break;
						}
					/*********************************************
					Обработка команд клиента*/
					case 'h':
					{
							char Messege_To_Opened[1024] = "\n\n   Сервер поддерживает команды:\n\n   OPEN_TCOMMANDER  для открытия Total Commander\n   CLOSE_TCOMMANDER  для закрытия Total Commander\n   TIME  для отображения системного времени\n   MAKE_TIME  для смены системного времени\n";

							send(
								ClientSock,
								Messege_To_Opened,
								sizeof(Messege_To_Opened),
								0
							);

							cout << "   клиенту дано описание команд\n";

							break;
						}
						/*********************************************
						Отсутсвие команд*/
					default:
					{
						char Messege_To_Error[1024] = "   Команда не найдена";
						cout << "   Команда не найдена" << endl; //вывод сообщения серверу
						send(
						ClientSock,
						Messege_To_Error,
						sizeof(Messege_To_Error),
						0
						);
						break; //выход из кейса
					}
				}
			}
			    else
				{
					cout << "\n   Клиент отключился" << endl;
					break;
				}
		}
		/********************************************
		Закрытие сокета*/
		closesocket(SSock);//закрытие
		WSACleanup();// выгружаем WinSock
	}
//----------------
    std::cout << "\n\nДля завершения программы нажмите любую клавишу." << endl;
    getch();//ждем нажатие клавиши :)
    return 0;
}


