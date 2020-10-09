#include <iostream>
#include <conio.h>    
#pragma comment(lib, "ws2_32")
#include <winsock2.h> 
#include <Windows.h> 
#include <Shellapi.h>
#include <cstdio>
#include <TlHelp32.h>

#pragma warning(disable:4996)

using namespace std;

int main(void)
{
	system("chcp 1251 > text");//ставим подержку русской кодировки

	std::cout << "    С Е Р В Е Р  >>>" << endl << endl;
	while (1) // Для постоянной работы сервера
	{
		int i_error = 0;//будет хранить код ошибки

		/********************************************
		0. настройка библиотеки Ws2_32.dll*/
		//В случае успеха WSAStartup возвращает 0; иначе - код ошибки. 
		WSADATA wsaData;//определяем переменную
		i_error = WSAStartup(MAKEWORD(2, 2), &wsaData);

		if (i_error)
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
		SOCKET SSock;// Создаем сокета
		SSock = socket(
			AF_INET,//используется при создании Интернет-приложений
			SOCK_STREAM,//потоковый
			IPPROTO_TCP);//ТСР - для потоковых
	   
		if (SSock == -1)
		{
			std::cout << "   ОШИБКА! - сокет не создан!" << endl;
		}
		else 
		{ 
			std::cout << "   Сокет успешно создан." << endl; 
		}

		/********************************************
		Привязка к локальным именам*/
		struct sockaddr_in  dest_addr;
		dest_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
		dest_addr.sin_port = htons(3000); 
		dest_addr.sin_family = AF_INET;
		i_error = 0;
		//В случае успеха bind возвращает 0, в противном случае - "-1"
		i_error = bind( 
				SSock,
				(SOCKADDR*)&dest_addr,
				sizeof(dest_addr)
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
		4.Подключение клиента - Получение дескриптора клиентского соединения*/
		cout << "   Ожидание клиента\n";
		SOCKET ClientSock;
		struct sockaddr_in addr_klienta;
		/*Если функция accept выполнилась успешно - возвращает новый сокет 
		для установленного соединения, в противном случае  INVALID_SOCKET*/
		int size_client_addr = sizeof(addr_klienta);
		ClientSock = accept(
			SSock,
			(sockaddr*)&addr_klienta, //адрес структуры (!)sockaddr_in 
			//куда будет помещена информация о клиенте.
			&size_client_addr); // Длинна адреса

		if (ClientSock == INVALID_SOCKET)
		{
			std::cout << "   ОШИБКА! подключения клиента" << endl;
		}
		else
		{
			std::cout << "   Клиент успешно подключен" << endl;
		}

		while (1) // Для поддержки нескольких команд
		{
			/********************************************
			Передача-чтение данных*/
			//чтение
			char Messege_Server[1024];
			if (recv(
				ClientSock,
				Messege_Server,
				1024,
				0) > 0)
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
						
						if (!ShellExecute(0, L"Open", L"C:\\totalcmd\\TOTALCMD64.EXE", NULL, NULL, SW_RESTORE)) 
						{
							char Msg_to_Client[1024] = "\nОшибка,нельзя открыть total commander";
							send(
								ClientSock,
								Msg_to_Client,
								sizeof(Msg_to_Client),
								0);
							
							cout << "   По запросу клиента, нельзя открыть total commander";
						}
						else 
						{

							char Msg_to_Client[1024] = "\nToal commander открыт";
							send(
								ClientSock,
								Msg_to_Client,
								sizeof(Msg_to_Client),
								0);
							
							cout << "   По запросу клиента,открыт total commander";

						}

						break;
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
						SYSTEMTIME new_time;

						GetLocalTime(&new_time);

						new_time.wHour = 15;
						new_time.wMinute = 05;

						if (SetSystemTime(&new_time) != 0)
						{
							char Msg_to_Client[1024] = "\nСистемное время изменено";
							send(
								ClientSock,
								Msg_to_Client,
								sizeof(Msg_to_Client),
								0);

							cout << "   По запросу клиента, изменено системное время";
						}
						else
						{

							char Msg_to_Client[1024] = "\nОшибка,нет прав администратора";
							send(
								ClientSock,
								Msg_to_Client,
								sizeof(Msg_to_Client),
								0);

							cout << "   Ошибка,нет прав администратора";

						}
					}

					/*********************************************
					Обработка команд клиента*/
					case 'h':
					{
							char Messege_To_Opened[1024] = "\n\n   Сервер поддерживает команды:\n\n   OPEN_TCOMMANDER  для открытия Total Commander\n   CLOSE_TCOMMANDER  для закрытия Total Commander\n   TIME  для отображения системного времени\n   MAKE_TIME  для смены системного времени\n";

							send(
								ClientSock,
								Messege_To_Opened,
								sizeof(Messege_To_Opened),0);
							
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
						0);
						break; 
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


