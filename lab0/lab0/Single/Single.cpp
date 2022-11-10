// Single.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#include <iostream> 
#include <cstdio> 
#include <cstring> 
#include <map>
#include <chrono>
using namespace std;

double algebraicComplement(int** arr, int i, int j, int size);
double determinant(int** arr, int size);
void inputMatrix(int** arr, int size);
void outputMatrix(int** arr, int size);
void outputResult(map<int, double>, int size);
int main()
{
	map<int, double> results_map;
	int size;
	/*std::cout << "Input matrix size" << endl;
	std::cin >> size;

	int** arr = new int* [size];
	int** algebraicComplements = new int* [size];
	for (int i = 0; i < size; i++)
		arr[i] = new int[size];
	inputMatrix(arr, size);*/
	size = 5;
    int** arr = new int* [5];
	arr[0] = new int[5]{ 11, 234, -543, 234, 234 };
	arr[1] = new int[5]{ 134, 234, 432, 123, 234 };
	arr[2] = new int[5]{ 1, 234, 4324, 234, 13 };
	arr[3] = new int[5]{ 32, 234, 444, -234, 234 };
	arr[4] = new int[5]{ 1, 234, 434, 234, 234 };
	//size = 10;
	//int** arr = new int* [10];
	//arr[0] = new int[10]{ 11, 234, -543, 234, 234, 234, 234, 234, 24, 432 };
	//arr[1] = new int[10]{ 134, 234, 432, 123, 234, 234, 234, 234, 34, 432 };
	//arr[2] = new int[10]{ 1, 234, 4324, 234, 13, 12, 342, 41, 432, 432 };
	//arr[3] = new int[10]{ 32, 234, 444, -234, 234, 234, 13, 234, 432, 432 };
	//arr[4] = new int[10]{ 1, 234, 434, 234, 234, 234, 960, 742, 432, 432 };
	//arr[5] = new int[10]{ 1, 234, 434, 234, 234, 234, 234, 234, 432, 432 };
	//arr[6] = new int[10]{ 231, 234, 4324, 554, 234, 234, -234, 234, 432, 432 };
	//arr[7] = new int[10]{ 1, 234, 4324, 234, 234, -234, 644, 244, 432, 411 };
	//arr[8] = new int[10]{ 1, 234, -141, 234, 234, 234, 234, 234, 432, 432 };
	//arr[9] = new int[10]{ 213, 234, 4324, 234, 234, 234, 234, 234, 432, 432 };
	outputMatrix(arr, size);
	auto start_time = std::chrono::steady_clock::now();
	int id = 0;
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			results_map[id] = algebraicComplement(arr, i, j, size);
			id = id + 1;
		}
	}
	auto end_time = std::chrono::steady_clock::now();
	auto elapsed_ns = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
	std::cout << "elapsed time:" << elapsed_ns.count() << " ms" << endl;
	outputResult(results_map, size);
	cout << "press any key to exit" << endl;
	int nothing;
	cin >> nothing;
	return 0;
}

double algebraicComplement(int** arr, int aci, int acj, int size)
{
	int** temp = new int* [size - 1];
	for (int i = 0; i < size - 1; i++)
		temp[i] = new int[size - 1];
	int s = 0, s1 = 0;
	for (int i = 0;i < size;++i)
	{
		if (i != aci)
		{
			s1 = 0;
			for (int j = 0;j < size;++j)
				if (j != acj)
				{
					temp[s][s1] = arr[i][j];
					s1++;
				}
			s++;
		}
	}

	return pow(-1., aci + acj) * determinant(temp, size - 1);
}

double determinant(int** arr, int size)
{
	int i, j;
	double det = 0;       //переменная определителя
	int** matr;         //указатель
	if (size == 1)     // 1-е условие , размер 1
	{
		det = arr[0][0];
	}
	else if (size == 2)    // 2-е условие , размер 2
	{
		det = arr[0][0] * arr[1][1] - arr[0][1] * arr[1][0];    //
	}
	else
	{
		matr = new int* [size - 1]; //создание динамического массива
		for (i = 0;i < size;++i)
		{
			for (j = 0;j < size - 1;++j)
			{
				if (j < i)
				{
					matr[j] = arr[j];
				}
				else
					matr[j] = arr[j + 1];
			}
			det += pow(-1., (i + j)) * determinant(matr, size - 1) * arr[i][size - 1];    //подсчеты
		}
		delete[] matr;  //удаляем массив
	}
	return det; //возвращаем значение определителя
}


void inputMatrix(int** arr, int size)
{
	int i, j;

	for (i = 0; i < size; i++)
	{
		for (j = 0; j < size; j++)
		{
			std::cout << "input [" << i << "," << j << "]" << endl;
			std::cin >> arr[i][j];
		}
	}
}

void outputMatrix(int** arr, int size)
{
	int i, j;
	for (i = 0;i < size;i++)
	{
		for (j = 0;j < size;j++)
		{
			std::cout << arr[i][j] << " ";
		}
		std::cout << endl;
	}
	std::cout << endl;
}

void outputResult(map<int, double> results, int size)
{
	int totalSize = size * size;
	int row = 0;
	map <int, double> ::iterator it = results.begin();
	for (int i = 0; it != results.end(); it++, i++)
	{
		std::cout << it->second << " ";
		if (++row == size) {
			std::cout << endl;
			row = 0;
		}
	}
	std::cout << endl;
}