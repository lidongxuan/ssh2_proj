#include "sftp.h"
#include <string>

int main()
{
	std::string ip, username, password;

	ip = "10.0.5.14";
	username = "ldx";
	password = "123";

	int rc = sftp_init(ip, username, password);
	if (rc!=0)
	{
		system("pause");
		return -1;
	}

	std::string file_name = "";

	////�����½��ļ���
	//for (int i = 1; i < 10;i++)
	//{
	//	file_name = std::to_string(i);
	//	rc = sftp_mkdir(file_name);
	//	if (rc != 0)
	//	{
	//		//system("pause");
	//		//return -1;
	//	}
	//}

	////�ϴ����ļ�
	//rc = sftp_upload("D:/lidongxuan/somezip/cuda_8.0.61_windows.exe",  "2.exe");
	//if (rc != 0)
	//{
	//	system("pause");
	//	return -1;
	//}

	////���ش��ļ�
	//rc = sftp_download("1.wmv", "D:/2.wmv");
	//if (rc != 0)
	//{
	//	system("pause");
	//	return -1;
	//}
	//rc = sftp_download("1.zip", "D:/2.zip");
	//if (rc != 0)
	//{
	//	system("pause");
	//	return -1;
	//}

	////�����ϴ�
	//for (int i = 1; i < 100; i++)
	//{
	//	file_name = "D:/lidongxuan/test_image/TEST (" + std::to_string(i) + ").jpg";
	//	rc = sftp_upload(file_name, std::to_string(i) + ".jpg");
	//	if (rc != 0)
	//	{
	//		system("pause");
	//		return -1;
	//	}
	//}

	////��������
	//for (int i = 1; i < 100; i++)
	//{
	//	file_name = "D:/lidongxuan/test_image/" + std::to_string(i) + "_new.jpg";
	//	rc = sftp_download(std::to_string(i) + ".jpg", file_name);
	//	if (rc != 0)
	//	{
	//		system("pause");
	//		return -1;
	//	}
	//}

	sftp_close();

	system("pause");
	return 0;
}